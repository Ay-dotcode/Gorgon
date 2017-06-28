#include <map>

#include "../WindowManager.h"
#include "../Window.h"
#include "../Input.h"
#include "../OS.h"
#include "../Any.h"

#define WINDOWS_LEAN_AND_MEAN

#include "../GL/OpenGL.h"
#include "../Graphics.h"

#include "../Graphics/Layer.h"
#include "../Geometry/Margin.h"

#include "../Input/DnD.h"

#include "../Containers/Vector.h"
#include "../Encoding/PNG.h"
#include "../Encoding/JPEG.h"
#include "../IO/MemoryStream.h"

#include <windows.h>
#include <ShlObj.h>
#include "../Utils/ScopeGuard.h"

#	undef CreateWindow
#	undef Rectangle

namespace Gorgon {

	namespace OS {
		void winslashtonormal(std::string &);
		void normalslashtowin(std::string &s);
	}

	namespace WindowManager {
		Geometry::Point GetMousePosition(Gorgon::internal::windowdata *wind);

		class GGEDropTarget : public IDropTarget {
		public:
			GGEDropTarget(Window *wind) : m_lRefCount(0), parent(wind) {
			}

			// IUnknown implementation
			HRESULT __stdcall QueryInterface(REFIID iid, void ** ppvObject) { return S_OK; }
			ULONG   __stdcall AddRef(void) { return ++m_lRefCount; }
			ULONG   __stdcall Release(void) { return --m_lRefCount; }

			// IDropTarget implementation
			HRESULT __stdcall DragEnter(IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect) {
				// does the data object contain data we want?
				FORMATETC fileformat ={CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
				FORMATETC textformat ={CF_TEXT, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};

				if(pDataObject->QueryGetData(&fileformat)==S_OK) {
					islocal = parent->IsLocalPointer();
					if(islocal)
						parent->SwitchToWMPointers();

					auto data=new FileData();

					STGMEDIUM stgmed;
					std::string name;
					pDataObject->GetData(&fileformat, &stgmed);
					DROPFILES *fdata=(DROPFILES*)GlobalLock(stgmed.hGlobal);


					wchar_t *widetext=(wchar_t*)((char*)(fdata)+fdata->pFiles);

					while(widetext[0]) {
						name.resize(wcslen(widetext));

						wcstombs(&name[0], widetext, wcslen(widetext)+1);
						OS::winslashtonormal(name);
						data->AddFile(name);

						widetext+=wcslen(widetext)+1;
					}

					GlobalUnlock(stgmed.hGlobal);
					ReleaseStgMedium(&stgmed);


					auto &drag = Input::PrepareDrag();
					drag.AssumeData(*data);
                    Input::GetDragOperation().MarkAsOS();
					Input::GetDragOperation().DataReady();
					Input::StartDrag();

					*pdwEffect = DROPEFFECT_MOVE;
				}
				
				if(pDataObject->QueryGetData(&textformat) == S_OK) {
					islocal = parent->IsLocalPointer();
					if(islocal)
						parent->SwitchToWMPointers();

					STGMEDIUM stgmed;
					std::string textdata;
					pDataObject->GetData(&textformat, &stgmed);
					char *text=(char*)GlobalLock(stgmed.hGlobal);

					textdata=text;

                    if(Input::IsDragging()) {
                        auto &drag = Input::GetDragOperation();
                        drag.AddTextData(textdata);
                    }
                    else {
                        Input::BeginDrag(textdata);
                        Input::GetDragOperation().MarkAsOS();
                        Input::GetDragOperation().DataReady();
                    }

					GlobalUnlock(stgmed.hGlobal);
					ReleaseStgMedium(&stgmed);

					*pdwEffect = DROPEFFECT_COPY;
				}
				else {
					*pdwEffect = DROPEFFECT_NONE;
				}

				return S_OK;
			}

			HRESULT __stdcall DragOver(DWORD grfKeyState, POINTL pt, DWORD * pdwEffect) {
				if(!Input::IsDragging() || !Input::GetDragOperation().HasTarget()) { 
					*pdwEffect = DROPEFFECT_NONE;
				}
				else {
					auto &drag = Input::GetDragOperation();

					if(drag.HasData(Resource::GID::File)) {
						auto &data=dynamic_cast<FileData&>(drag.GetData(Resource::GID::File));

						if(data.Action==data.Move)
							*pdwEffect=DROPEFFECT_MOVE;
						else
							*pdwEffect=DROPEFFECT_COPY;
					}
					else if(drag.HasData(Resource::GID::Text)) {
						*pdwEffect=DROPEFFECT_COPY;
					}
					else {
						*pdwEffect=DROPEFFECT_NONE;
					}
				}

				return S_OK;
			}

			HRESULT __stdcall DragLeave(void) {
				Input::CancelDrag();

				if(islocal)
					parent->SwitchToLocalPointers();

				return S_OK;
			}
			HRESULT __stdcall Drop(IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect) {
				Input::Drop(parent->GetMouseLocation());

				if(islocal)
					parent->SwitchToLocalPointers();

				return S_OK;
			}

			virtual ~GGEDropTarget() {}

		//private:
			void dragfinished() { }

			long   m_lRefCount;
			Window *parent;

			bool islocal = false;
		};

		///@cond internal
		UINT cf_png, cf_g_bmp, cf_html, cf_urilist;
		std::vector<HGLOBAL> clipbuffers;

		template<class T_>
		std::shared_ptr<CopyFreeAny> make_clipboarddata(T_ data) {
			auto a = &MakeCopyFreeAny(std::move(data));
			return std::shared_ptr<CopyFreeAny>{a};
		}

		struct clipboardentry {
			UINT type;
			std::shared_ptr<CopyFreeAny> data;

			bool operator ==(const clipboardentry &other) const {
				return type == other.type;
			}
		};

		std::vector<clipboardentry> clipboard_entries;
		///@endcond
	}

	/// @cond INTERNAL
	namespace internal {

		struct windowdata {
			windowdata(Window &parent) : parent(&parent), target(&parent) { }

			HWND handle = 0;
			Window *parent;
			HGLRC context=0;
			HDC device_context=0;
			Geometry::Margin chrome = {0, 0};
			bool pointerdisplayed = true;
			bool min = false;

			WindowManager::GGEDropTarget target;

			std::map<Input::Key, ConsumableEvent<Window, Input::Key, bool>::Token> handlers;

			Input::Keyboard::Key maposkey(WPARAM wParam, LPARAM lParam) {
				switch(wParam) {
					case VK_SHIFT://Input::Keyboard::
						//if(lParam)
						return Input::Keyboard::Shift;
					case VK_CONTROL:
						return Input::Keyboard::Control;
					case VK_MENU:
						return Input::Keyboard::Alt;
					case VK_LWIN:
						return Input::Keyboard::Meta;

					case VK_HOME:
						return Input::Keyboard::Home;
					case VK_END:
						return Input::Keyboard::End;
					case VK_INSERT:
						return Input::Keyboard::Insert;
					case VK_DELETE:
						return Input::Keyboard::Delete;
					case VK_PRIOR:
						return Input::Keyboard::PageUp;
					case VK_NEXT:
						return Input::Keyboard::PageDown;

					case VK_LEFT:
						return Input::Keyboard::Left;
					case VK_UP:
						return Input::Keyboard::Up;
					case VK_RIGHT:
						return Input::Keyboard::Right;
					case VK_DOWN:
						return Input::Keyboard::Down;

					case VK_SNAPSHOT:
						return Input::Keyboard::PrintScreen;
					case VK_PAUSE:
						return Input::Keyboard::Pause;

					case VK_APPS:
						return Input::Keyboard::Menu;

					case VK_CAPITAL:
						return Input::Keyboard::CapsLock;

					case VK_RETURN: //Input::Keyboard::Numpad_Enter
						return Input::Keyboard::Enter;
					case VK_TAB:
						return Input::Keyboard::Tab;
					case VK_BACK:
						return Input::Keyboard::Backspace;
					case VK_SPACE:
						return Input::Keyboard::Space;
					case VK_ESCAPE:
						return Input::Keyboard::Escape;

					case VK_F1:
						return Input::Keyboard::F1;
					case VK_F2:
						return Input::Keyboard::F2;
					case VK_F3:
						return Input::Keyboard::F3;
					case VK_F4:
						return Input::Keyboard::F4;
					case VK_F5:
						return Input::Keyboard::F5;
					case VK_F6:
						return Input::Keyboard::F6;
					case VK_F7:
						return Input::Keyboard::F7;
					case VK_F8:
						return Input::Keyboard::F8;
					case VK_F9:
						return Input::Keyboard::F9;
					case VK_F10:
						return Input::Keyboard::F10;
					case VK_F11:
						return Input::Keyboard::F11;
					case VK_F12:
						return Input::Keyboard::F12;

					case 'A':
						return Input::Keyboard::A;
					case 'B':
						return Input::Keyboard::B;
					case 'C':
						return Input::Keyboard::C;
					case 'D':
						return Input::Keyboard::D;
					case 'E':
						return Input::Keyboard::E;
					case 'F':
						return Input::Keyboard::F;
					case 'G':
						return Input::Keyboard::G;
					case 'H':
						return Input::Keyboard::H;
					case 'I':
						return Input::Keyboard::I;
					case 'J':
						return Input::Keyboard::J;
					case 'K':
						return Input::Keyboard::K;
					case 'L':
						return Input::Keyboard::L;
					case 'M':
						return Input::Keyboard::M;
					case 'N':
						return Input::Keyboard::N;
					case 'O':
						return Input::Keyboard::O;
					case 'P':
						return Input::Keyboard::P;
					case 'Q':
						return Input::Keyboard::Q;
					case 'R':
						return Input::Keyboard::R;
					case 'S':
						return Input::Keyboard::S;
					case 'T':
						return Input::Keyboard::T;
					case 'U':
						return Input::Keyboard::U;
					case 'V':
						return Input::Keyboard::V;
					case 'W':
						return Input::Keyboard::W;
					case 'X':
						return Input::Keyboard::X;
					case 'Y':
						return Input::Keyboard::Y;
					case 'Z':
						return Input::Keyboard::Z;

					case '1':
						return Input::Keyboard::Number_1;
					case '2':
						return Input::Keyboard::Number_2;
					case '3':
						return Input::Keyboard::Number_3;
					case '4':
						return Input::Keyboard::Number_4;
					case '5':
						return Input::Keyboard::Number_5;
					case '6':
						return Input::Keyboard::Number_6;
					case '7':
						return Input::Keyboard::Number_7;
					case '8':
						return Input::Keyboard::Number_8;
					case '9':
						return Input::Keyboard::Number_9;
					case '0':
						return Input::Keyboard::Number_0;

					case VK_NUMPAD1:
						return Input::Keyboard::Numpad_1;
					case VK_NUMPAD2:
						return Input::Keyboard::Numpad_2;
					case VK_NUMPAD3:
						return Input::Keyboard::Numpad_3;
					case VK_NUMPAD4:
						return Input::Keyboard::Numpad_4;
					case VK_NUMPAD5:
						return Input::Keyboard::Numpad_5;
					case VK_NUMPAD6:
						return Input::Keyboard::Numpad_6;
					case VK_NUMPAD7:
						return Input::Keyboard::Numpad_7;
					case VK_NUMPAD8:
						return Input::Keyboard::Numpad_8;
					case VK_NUMPAD9:
						return Input::Keyboard::Numpad_9;
					case VK_ADD:
						return Input::Keyboard::Numpad_Plus;
					case VK_SUBTRACT:
						return Input::Keyboard::Numpad_Minus;
					case VK_MULTIPLY:
						return Input::Keyboard::Numpad_Mult;
					case VK_DIVIDE:
						return Input::Keyboard::Numpad_Div;
					case VK_DECIMAL:
						return Input::Keyboard::Numpad_Decimal;
					case VK_NUMLOCK:
						return Input::Keyboard::Numlock;
				}
			}

			LRESULT Proc(UINT message, WPARAM wParam, LPARAM lParam) {

				switch(message) {

					case WM_ACTIVATE: {
						if(LOWORD(wParam)) {
							parent->FocusedEvent();
						}
						else {
							//ReleaseAll();
							parent->LostFocusEvent();
						}
					} //Activate
					break;


					case WM_ERASEBKGND:
						return true;

					case WM_CLOSE: {
						bool allow;
						allow=true;
						parent->ClosingEvent(allow);
						if(allow)
							return DefWindowProc(handle, message, wParam, lParam);
					}
					break;

					case WM_DESTROYCLIPBOARD:
						WindowManager::clipbuffers.empty();
						WindowManager::clipboard_entries.clear();

						break;

					case WM_RENDERFORMAT: {
						auto type = (UINT)wParam;

						for(auto &e : WindowManager::clipboard_entries) {
							if(e.type == type) {
								if(e.type == WindowManager::cf_png) {
									std::vector<Byte> data;
									Encoding::Png.Encode(e.data->GetData<Containers::Image>(), data);
									WindowManager::clipbuffers.push_back(GlobalAlloc(GMEM_DDESHARE, data.size()));
									auto clipbuffer = WindowManager::clipbuffers.back();

									auto buff = GlobalLock(clipbuffer);
									memcpy(buff, &data[0], data.size());
									GlobalUnlock(clipbuffer);

									SetClipboardData(type, clipbuffer);
									break;
								}
								else if(e.type == CF_DIB) {
									std::ostringstream data;
									e.data->GetData<Containers::Image>().ExportBMP(data, false, true);

									WindowManager::clipbuffers.push_back(GlobalAlloc(GMEM_DDESHARE, data.str().size()));
									auto clipbuffer = WindowManager::clipbuffers.back();

									auto buff = GlobalLock(clipbuffer);
									memcpy(buff, &data.str()[0], data.str().size());
									GlobalUnlock(clipbuffer);

									SetClipboardData(type, clipbuffer);
									break;
								}
								else if(e.type == WindowManager::cf_g_bmp) {
									std::ostringstream data;
									Containers::Image &img = e.data->GetData<Containers::Image>();

									WindowManager::clipbuffers.push_back(GlobalAlloc(GMEM_DDESHARE, img.GetTotalSize()+4*3));
									auto clipbuffer = WindowManager::clipbuffers.back();

									auto buff = GlobalLock(clipbuffer);
									((int32_t*)buff)[0] = img.GetWidth();
									((int32_t*)buff)[1] = img.GetHeight();
									((int32_t*)buff)[2] = (int32_t)img.GetMode();

									memcpy((Byte*)buff+3*4, img.RawData(), img.GetTotalSize());

									GlobalUnlock(clipbuffer);

									SetClipboardData(type, clipbuffer);
									break;
								}
							}
						}
					}
					break;

					case WM_SYSCOMMAND:
						if(wParam == SC_MINIMIZE) {
							parent->MinimizedEvent();
							min = true;
						}
						else if(wParam == SC_RESTORE && min) {
							parent->RestoredEvent();
							min = false;
						}
						break;

					case WM_DESTROY:
						//ReleaseAll();
						parent->DestroyedEvent();
						break;

					case WM_LBUTTONDOWN: {
						int x=lParam%0x10000;
						int y=lParam>>16;

						parent->mouse_down({x, y}, Input::Mouse::Button::Left);
					}
					break;

					case WM_SIZE: {
						RECT rect;
						GetClientRect(parent->data->handle, &rect);

						parent->activatecontext();
						parent->Layer::Resize({rect.right-rect.left, rect.bottom-rect.top});
						GL::Resize({rect.right-rect.left, rect.bottom-rect.top});
					}

					case WM_LBUTTONUP: {
						int x=lParam%0x10000;
						int y=lParam>>16;

						parent->mouse_up({x, y}, Input::Mouse::Button::Left);
					}
					break;

					case WM_RBUTTONDOWN: {
						int x=lParam%0x10000;
						int y=lParam>>16;

						parent->mouse_down({x, y}, Input::Mouse::Button::Right);
					}
					break;

					case WM_RBUTTONUP: {
						int x=lParam%0x10000;
						int y=lParam>>16;

						parent->mouse_up({x, y}, Input::Mouse::Button::Right);
					}
					break;

					case WM_MBUTTONDOWN: {
						int x=lParam%0x10000;
						int y=lParam>>16;

						parent->mouse_down({x, y}, Input::Mouse::Button::Middle);
					}
					break;

					case WM_MBUTTONUP: {
						int x=lParam%0x10000;
						int y=lParam>>16;

						parent->mouse_up({x, y}, Input::Mouse::Button::Middle);
					}
					break;

					case WM_MOVE: 
						parent->MovedEvent();
						break;
					

					case WM_XBUTTONDOWN: {
						int x=lParam%0x10000;
						int y=lParam>>16;

						auto btn = Input::Mouse::Button::None;

						switch(GET_XBUTTON_WPARAM(wParam)) {
						case 1:
							btn = Input::Mouse::Button::X1;
							break;
						case 2:
							btn = Input::Mouse::Button::X2;
							break;
						}

						parent->mouse_down({x, y}, btn);
					}
					break;

					case WM_XBUTTONUP: {
						int x=lParam%0x10000;
						int y=lParam>>16;

						auto btn = Input::Mouse::Button::None;

						switch(GET_XBUTTON_WPARAM(wParam)) {
							case 1:
								btn = Input::Mouse::Button::X1;
								break;
							case 2:
								btn = Input::Mouse::Button::X2;
								break;
						}

						parent->mouse_up({x, y}, btn);
					}
					break;

					case WM_MOUSEWHEEL:
						parent->mouse_event(Input::Mouse::EventType::Scroll_Vert, parent->mouselocation, Input::Mouse::Button::None, std::round(GET_WHEEL_DELTA_WPARAM(wParam)/120.f*16)/16);
						break;

					case WM_MOUSEHWHEEL:
						parent->mouse_event(Input::Mouse::EventType::Scroll_Hor, parent->mouselocation, Input::Mouse::Button::None, std::round(GET_WHEEL_DELTA_WPARAM(wParam)/120.f*16)/16);
						break;
                        
                    case WM_GESTURE: {
                        GESTUREINFO gi;  
    
                        ZeroMemory(&gi, sizeof(GESTUREINFO));
                        
                        gi.cbSize = sizeof(GESTUREINFO);

                        BOOL bResult  = GetGestureInfo((HGESTUREINFO)lParam, &gi);
                        
                        if(gi.dwID == GID_ZOOM) {
                            //todo test
                            parent->mouse_event(Input::Mouse::EventType::Zoom, parent->mouselocation, Input::Mouse::Button::None, (float)gi.ullArguments);
						}
                        else if(gi.dwID == GID_ROTATE) {
                            parent->mouse_event(Input::Mouse::EventType::Zoom, parent->mouselocation, Input::Mouse::Button::None, (float)GID_ROTATE_ANGLE_FROM_ARGUMENT(gi.ullArguments));
                        }
                    }
                    break;

					case WM_SYSKEYDOWN:
					case WM_KEYDOWN: {
						if(lParam&1<<30) {
							if(handlers.count(wParam)>0 && handlers[wParam]!=ConsumableEvent<Window, Input::Key, bool>::EmptyToken) {
								parent->KeyEvent.FireFor(handlers[wParam], wParam, true);
							}

							return 0;
						}
						
						switch(wParam) {
						case VK_CONTROL:
							Input::Keyboard::CurrentModifier.Add(Input::Keyboard::Modifier::Ctrl);
							break;
						case VK_SHIFT:
							Input::Keyboard::CurrentModifier.Add(Input::Keyboard::Modifier::Shift);
							break;
						case VK_LWIN:
							Input::Keyboard::CurrentModifier.Add(Input::Keyboard::Modifier::Meta);
							break;
						case VK_RWIN:
							Input::Keyboard::CurrentModifier.Add(Input::Keyboard::Modifier::Meta);
							break;
						}

						auto token=parent->KeyEvent(wParam, true);
						if(token!=ConsumableEvent<Window, Input::Key, bool>::EmptyToken) {
							handlers[wParam]=token;
							return 0;
						}
					} //Keydown
					return 0;


					case WM_SYSKEYUP:
					case WM_KEYUP: {

						switch(wParam) {
						case VK_CONTROL:
							Input::Keyboard::CurrentModifier.Remove(Input::Keyboard::Modifier::Ctrl);
							break;
						case VK_SHIFT:
							Input::Keyboard::CurrentModifier.Remove(Input::Keyboard::Modifier::Shift);
							break;
						case VK_LWIN:
							Input::Keyboard::CurrentModifier.Remove(Input::Keyboard::Modifier::Meta);
							break;
						case VK_RWIN:
							Input::Keyboard::CurrentModifier.Remove(Input::Keyboard::Modifier::Meta);
							break;
						}

						if(handlers.count(wParam)>0 && handlers[wParam]!=ConsumableEvent<Window, Input::Key, bool>::EmptyToken) {
							parent->KeyEvent.FireFor(handlers[wParam], wParam, false);
							handlers[wParam]=ConsumableEvent<Window, Input::Key, bool>::EmptyToken;
						}
						else {
							parent->KeyEvent(wParam, false);
						}
						
					} //Keyup
					return 0;


					case WM_CHAR:
						if(handlers.count(wParam)==0 || handlers[wParam]==ConsumableEvent<Window, Input::Key, bool>::EmptyToken) {
							if(wParam==8 || wParam==127 || wParam==27) return 0;

							parent->CharacterEvent(wParam);
						}
						else {
							parent->KeyEvent.FireFor(handlers[wParam], wParam, true);
						}
						return 0;

					default:
						//std::cout<<std::hex<<message<<": "<<wParam<<", "<<lParam<<std::endl;
						break;

				}

				return DefWindowProc(handle, message, wParam, lParam);
			}

			static std::map<HWND, windowdata*> mapping;
		};

		std::map<HWND, windowdata*> windowdata::mapping;

		bool ishandled(HWND hwnd, Input::Key key) { 
			if(!windowdata::mapping[hwnd])  return false;

			return windowdata::mapping[hwnd]->handlers.count(key) && windowdata::mapping[hwnd]->handlers[key]!=ConsumableEvent<Window, Input::Key, bool>::EmptyToken;
		}
	}

	LRESULT __stdcall WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
		if(internal::windowdata::mapping.count(hWnd)) {
			return internal::windowdata::mapping[hWnd]->Proc(message, wParam, lParam);
		}
		else {
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	/// @endcond

	namespace WindowManager {

		/// @cond INTERNAL
		HCURSOR defaultcursor;
		extern intptr_t context;

		Geometry::Point GetMousePosition(Gorgon::internal::windowdata *wind) {
			POINT pnt;
			RECT winrect;
			Geometry::Point ret;
			GetWindowRect((HWND)wind->handle, &winrect);
			GetCursorPos(&pnt);

			ret.X=pnt.x-(wind->chrome.Left+winrect.left);
			ret.Y=pnt.y-(wind->chrome.Top+winrect.top);

			return ret;
		}

		namespace internal {
			struct monitordata {
				int index = -1;
				HMONITOR handle;

				static BOOL CALLBACK MonitorEnumProc(
					_In_ HMONITOR hMonitor,
					_In_ HDC      hdcMonitor,
					_In_ LPRECT   lprcMonitor,
					_In_ LPARAM   dwData
					);

				~monitordata() {}
			};

			struct icondata {
				HICON icon = 0;
			};

			void switchcontext(Gorgon::internal::windowdata &data) {
				if(context==reinterpret_cast<intptr_t>(&data)) return;
				wglMakeCurrent(data.device_context, data.context);
				context=reinterpret_cast<intptr_t>(&data);
			}

			void finalizerender(Gorgon::internal::windowdata &data) {
				SwapBuffers(data.device_context);
			}

			Gorgon::internal::windowdata *getdata(const Window &w) {
				return w.data;
			}

		}
		/// @endcond

		//Modified from https://social.msdn.microsoft.com/Forums/en-US/41f3fa1c-d7cd-4ba6-a3bf-a36f16641e37/conversion-from-multibyte-to-unicode-character-set?forum=vcgeneral
		std::string MByteToUnicode(const std::string &multiByteStr) {
			// Get the required size of the buffer that receives the Unicode string. 
			DWORD minSize;
			minSize = MultiByteToWideChar(CP_UTF8, 0, multiByteStr.c_str(), -1, NULL, 0);

			std::string ret;
			ret.resize(minSize*2);

			// Convert string from multi-byte to Unicode.
			MultiByteToWideChar(CP_UTF8, 0, multiByteStr.c_str(), -1, (LPWSTR)&ret[0], minSize);

			return ret;
		}

		//https://social.msdn.microsoft.com/Forums/en-US/41f3fa1c-d7cd-4ba6-a3bf-a36f16641e37/conversion-from-multibyte-to-unicode-character-set?forum=vcgeneral
		std::string UnicodeToMByte(LPWSTR unicodeStr) {
			// Get the required size of the buffer that receives the multiByte string. 
			DWORD minSize;
			minSize = WideCharToMultiByte(CP_UTF8, NULL, unicodeStr, -1, NULL, 0, NULL, FALSE);

			std::string ret;
			ret.resize(minSize);
			
			// Convert string from Unicode to multi-byte.
			WideCharToMultiByte(CP_UTF8, NULL, unicodeStr, -1, &ret[0], minSize, NULL, FALSE);
			return ret;
		}

		void init() {
			defaultcursor=LoadCursor(NULL, IDC_ARROW);

			Monitor::Refresh(true);

			cf_png = RegisterClipboardFormat("PNG");
			cf_urilist = RegisterClipboardFormat("URIList");
			cf_g_bmp = RegisterClipboardFormat("[Gorgon]Bitmap");
			cf_html = RegisterClipboardFormat("HTML Format");
		}

		///this function opens the clipboard but not closes it, you must ensure the clipboard
		///is closed, otherwise, clipboard for the entire system will be locked down until the
		///application is closed.
		std::vector<int> getclipboardformats() {
			std::vector<int> ret;

			Utils::ScopeGuard g(&CloseClipboard);

			if(!OpenClipboard(NULL))
				return ret;

			int format = 0;

			while(true) {
				format = EnumClipboardFormats(format);

				if(!format)
					break;

				ret.push_back(format);
			}

			return ret;
		}

		std::vector<Resource::GID::Type> GetClipboardFormats() {
			int format = 0;

			std::vector<Resource::GID::Type> ret;

			auto formats = getclipboardformats();

			for(auto format : formats) {				
				if(format == cf_png || 
				   format == cf_g_bmp||
				   format == CF_DIB||
				   format == CF_DIBV5
				  ) {
					Containers::PushBackUnique(ret, Resource::GID::Image_Data);
				}
				else if(format == cf_html) {
					Containers::PushBackUnique(ret, Resource::GID::HTML);
				}
				else if(format == CF_HDROP) {
					Containers::PushBackUnique(ret, Resource::GID::FileList);
					Containers::PushBackUnique(ret, Resource::GID::URIList);
				}
				else if(format == cf_urilist) {
					Containers::PushBackUnique(ret, Resource::GID::URIList);
				}
				else if(format == CF_TEXT || format == CF_UNICODETEXT) {
					Containers::PushBackUnique(ret, Resource::GID::Text);
				}
				else {
					std::cout<<"Unknown type: ";
					switch(format) {
						case CF_TEXT: std::cout<<"CF_TEXT"<<std::endl; break;
						case CF_BITMAP: std::cout<<"CF_BITMAP"<<std::endl; break;
						case CF_METAFILEPICT: std::cout<<"CF_METAFILEPICT"<<std::endl; break;
						case CF_SYLK: std::cout<<"CF_SYLK"<<std::endl; break;
						case CF_DIF: std::cout<<"CF_DIF"<<std::endl; break;
						case CF_TIFF: std::cout<<"CF_TIFF"<<std::endl; break;
						case CF_OEMTEXT: std::cout<<"CF_OEMTEXT"<<std::endl; break;
						case CF_DIB: std::cout<<"CF_DIB"<<std::endl; break;
						case CF_PALETTE: std::cout<<"CF_PALETTE"<<std::endl; break;
						case CF_PENDATA: std::cout<<"CF_PENDATA"<<std::endl; break;
						case CF_RIFF: std::cout<<"CF_RIFF"<<std::endl; break;
						case CF_WAVE: std::cout<<"CF_WAVE"<<std::endl; break;
						case CF_UNICODETEXT: std::cout<<"CF_UNICODETEXT"<<std::endl; break;
						case CF_ENHMETAFILE: std::cout<<"CF_ENHMETAFILE"<<std::endl; break;
						case CF_HDROP: std::cout<<"CF_HDROP"<<std::endl; break;
						case CF_LOCALE: std::cout<<"CF_LOCALE"<<std::endl; break;
						case CF_DIBV5: std::cout<<"CF_DIBV5"<<std::endl; break;
						case CF_MAX: std::cout<<"CF_MAX"<<std::endl; break;
						case CF_OWNERDISPLAY: std::cout<<"CF_OWNERDISPLAY"<<std::endl; break;
						case CF_DSPTEXT: std::cout<<"CF_DSPTEXT"<<std::endl; break;
						case CF_DSPBITMAP: std::cout<<"CF_DSPBITMAP"<<std::endl; break;
						case CF_DSPMETAFILEPICT: std::cout<<"CF_DSPMETAFILEPICT"<<std::endl; break;
						case CF_DSPENHMETAFILE: std::cout<<"CF_DSPENHMETAFILE"<<std::endl; break;
						default: {
							char name[256];
							int l = GetClipboardFormatName(format, name, 254);
							name[l] = 0;
							std::cout<<name<<std::endl;
						}
					}
				}
			}

			CloseClipboard();

			return ret;
		}

		void SetClipboardText(const std::string &text, Resource::GID::Type type, bool unicode, bool append) {
			if(OpenClipboard(NULL)) {

				Utils::ScopeGuard g(&CloseClipboard);

				if(!append) {
					EmptyClipboard();
					clipboard_entries.clear();
				}

				if(type == Resource::GID::Text) {
					char * buffer;
					clipbuffers.push_back(GlobalAlloc(GMEM_DDESHARE, text.length()+1));
					auto clipbuffer = clipbuffers.back();

					buffer = (char*)GlobalLock(clipbuffer);
					strncpy_s(buffer, text.length()+1, text.c_str(), text.length());
					buffer[text.length()] = 0;
					GlobalUnlock(clipbuffer);

					if(unicode) {
						auto unicode = MByteToUnicode(text);

						char *cbu;
						clipbuffers.push_back(GlobalAlloc(GMEM_DDESHARE, unicode.length()));
						auto clipbuffer = clipbuffers.back();

						cbu = (char*)GlobalLock(clipbuffer);
						memcpy(cbu, unicode.c_str(), unicode.length());
						GlobalUnlock(clipbuffer);

						SetClipboardData(CF_UNICODETEXT, cbu);
					}

					SetClipboardData(CF_TEXT, clipbuffer);
				}
				else if(type == Resource::GID::HTML) {
					auto htmltxt = 
						"Version:0.9\r\n"
						"StartHTML:00000097\r\n"
						"EndHTML:"+String::PadStart(String::From(161+text.length()), 8, '0')+"\r\n"
						"StartFragment:00000129\r\n"
						"EndFragment:"+String::PadStart(String::From(129+text.length()), 8, '0')+"\r\n"
						"<html><body><!--StartFragment-->"+text+"<!--EndFragment--></body></html>";

					char * buffer;
					clipbuffers.push_back(GlobalAlloc(GMEM_DDESHARE, htmltxt.length()+1));
					auto clipbuffer = clipbuffers.back();

					buffer = (char*)GlobalLock(clipbuffer);
					memcpy(buffer, htmltxt.c_str(), htmltxt.length());
					buffer[htmltxt.length()] = 0;
					GlobalUnlock(clipbuffer);

					SetClipboardData(cf_html, clipbuffer);
				}
			}
		}
		
		std::string GetClipboardText(Resource::GID::Type type) {
			HANDLE clip;
			if(OpenClipboard(NULL)) {

				Utils::ScopeGuard g(&CloseClipboard);

				bool unicode = false;
				if(type == Resource::GID::Text) {
					unicode = true;

					clip = GetClipboardData(CF_UNICODETEXT);

					if(clip == nullptr) {
						unicode = false;
						clip = GetClipboardData(CF_TEXT);
					}
				}
				else if(type == Resource::GID::HTML) {
					clip = GetClipboardData(cf_html);
				}

				if(clip == nullptr)
					return "";

				if(unicode) {
					auto s = UnicodeToMByte((LPWSTR)clip);
					return s;
				}
				else {
					std::string s = (char*)clip;
					if(type == Resource::GID::HTML && s.length() > 11) {
						if(s.substr(0, 11) != "Version:0.9" && s.substr(0, 11) != "Version:1.0") goto bail;

						auto p = s.find("StartFragment:");

						if(p==s.npos) goto bail;

						auto p2 = s.find("EndFragment:");

						if(p2==s.npos) goto bail;

						auto start = String::To<unsigned long>(s.substr(p+14, s.find_first_of('\n', p)));

						if(start <= p2)
							goto bail;

						auto end = String::To<unsigned long>(s.substr(p2+12, s.find_first_of('\n', p2)));

						if(end <= p2)
							goto bail;

						s = s.substr(start, end-start);
					}

					bail:
					return s;
				}
			}
			else {
				return "";
			}
		}

		std::vector<std::string> GetClipboardList(Resource::GID::Type type) {
			std::vector<std::string> ret;

			if(!OpenClipboard(NULL))
				return ret;

			Utils::ScopeGuard g(&CloseClipboard);


			if(type == Resource::GID::FileList) {
				auto clip = GetClipboardData(CF_HDROP);

				if(clip != nullptr) {

					DROPFILES *fdata=(DROPFILES*)GlobalLock(clip);

					wchar_t *widetext=(wchar_t*)((char*)(fdata)+fdata->pFiles);

					std::string name;
					while(widetext[0]) {
						name.resize(wcslen(widetext));

						wcstombs(&name[0], widetext, wcslen(widetext)+1);
						OS::winslashtonormal(name);
						ret.push_back(name);

						widetext+=wcslen(widetext)+1;
					}
				}
			}
			else if(type == Resource::GID::URIList) {
				auto clip = GetClipboardData(cf_urilist);

				if(clip != nullptr) {
					std::string data = (char*)GlobalLock(clip);
					int last = 0;

					for(int i=0; i<(int)data.length(); i++) {
						if(data[i] == '\n') {
							if(i-last > 0) {
								ret.push_back(data.substr(last, i-last));
								last = i + 1;
							}
							else 
								last++;
						}
						else if(data[i] == '\r') {
							ret.push_back(data.substr(last, i-last));
							last = i + 1;
						}
					}
					if(last<(int)data.length()) {
						ret.push_back(data.substr(last, data.length()-last));
					}

					return ret;
				}

				clip = GetClipboardData(CF_HDROP);

				if(clip != nullptr) {

					DROPFILES *fdata=(DROPFILES*)GlobalLock(clip);

					wchar_t *widetext=(wchar_t*)((char*)(fdata)+fdata->pFiles);

					std::string name;
					while(widetext[0]) {
						name.resize(wcslen(widetext));

						wcstombs(&name[0], widetext, wcslen(widetext)+1);
						OS::winslashtonormal(name);
						ret.push_back("file://" + name);

						widetext+=wcslen(widetext)+1;
					}
				}
			}

			return ret;
		}

		void SetClipboardList(std::vector<std::string> list, Resource::GID::Type type, bool append) {
			if(OpenClipboard(NULL)) {

				Utils::ScopeGuard g(&CloseClipboard);

				if(!append) {
					EmptyClipboard();
					clipboard_entries.clear();
				}

				if(type == Resource::GID::FileList) {
					size_t len;


					len = 0;
					for(const auto &e : list)
						len += e.length() + 9;

					clipbuffers.push_back(GlobalAlloc(GMEM_DDESHARE, len+1));
					auto uri = clipbuffers.back();
					char *str = (char*)GlobalLock(uri);
					bool first = true;
					for(auto &e : list) {
						if(!first) {
							*str++ = '\r';
							*str++ = '\n';
						}

						memcpy(str, "file://", 7);
						str += 7;
						memcpy(str, &e[0], e.length());
						str += e.length();

						first = false;
					}
					*str = 0;
					GlobalUnlock(uri);
					SetClipboardData(cf_urilist, uri);

					len = 0;
					for(const auto &e : list)
						len += e.length() + 1;

					clipbuffers.push_back(GlobalAlloc(GMEM_DDESHARE, len*2+sizeof(DROPFILES)+2));
					auto clipbuffer = clipbuffers.back();

					DROPFILES *files = (DROPFILES *)GlobalLock(clipbuffer);
					files->fNC = false;
					files->fWide = true;
					files->pt.x = 0;
					files->pt.y = 0;
					files->pFiles = sizeof(DROPFILES);

					wchar_t *buffer = (wchar_t*)((char*)(files)+files->pFiles);
					for(auto &e : list) {
						OS::normalslashtowin(e);
						auto s = MByteToUnicode(e);
						memcpy(buffer, &s[0], s.length());
						buffer += s.length()/2;
					}
					buffer[0] = 0;
					buffer[1] = 0;
					GlobalUnlock(clipbuffer);

					SetClipboardData(CF_HDROP, clipbuffer);
				}
				else if(type == Resource::GID::URIList) {
					size_t len;


					len = 0;
					for(const auto &e : list)
						len += e.length() + 2;

					clipbuffers.push_back(GlobalAlloc(GMEM_DDESHARE, len+1));
					auto uri = clipbuffers.back();
					char *str = (char*)GlobalLock(uri);
					bool first = true;
					for(auto &e : list) {
						if(!first) {
							*str++ = '\r';
							*str++ = '\n';
						}

						memcpy(str, &e[0], e.length());
						str += e.length();

						first = false;
					}
					*str = 0;
					GlobalUnlock(uri);
					SetClipboardData(cf_urilist, uri);
				}
			}
		}

		Containers::Image GetClipboardBitmap() {
			Containers::Image ret;

			auto owner = GetClipboardOwner();

			for(const auto &w : Window::Windows) {
				auto d = internal::getdata(w);
				if(d && d->handle == owner) {
					//we own the data, no need to get it from clipboard
					for(auto &e : clipboard_entries) {
						if(e.type == cf_png) {
							ret = e.data->GetData<Containers::Image>().Duplicate();
							return ret;
						}
					}
				}
			}
			
			if(!OpenClipboard(NULL)) return ret;

			Utils::ScopeGuard g(&CloseClipboard);

			auto clip = GetClipboardData(cf_g_bmp);

			if(clip) {
				auto sz = GlobalSize(clip);
				Byte *data = (Byte*)GlobalLock(clip);
				int w = ((uint32_t*)data)[0], h = ((uint32_t*)data)[1];
				Graphics::ColorMode mode = (Graphics::ColorMode)((uint32_t*)data)[2];

				ret.Resize({w, h}, mode);
				memcpy(ret.RawData(), data+3*4, ret.GetTotalSize());

				GlobalUnlock(clip);
				CloseClipboard();
				return ret;
			}

			clip = GetClipboardData(cf_png);

			if(clip) {
				auto sz = GlobalSize(clip);
				Byte *data = (Byte*)GlobalLock(clip);
				Encoding::Png.Decode(data, sz, ret);
				GlobalUnlock(clip);
				CloseClipboard();
				return ret;
			}

			clip = GetClipboardData(CF_DIBV5);

			if(clip) {
				auto sz = GlobalSize(clip);
				char *data = (char*)GlobalLock(clip);
				IO::MemoryInputStream ms(data, data+sz);
				ret.ImportBMP(ms, true);
				GlobalUnlock(clip);
				CloseClipboard();
				return ret;
			}

			clip = GetClipboardData(CF_DIB);

			if(clip) {
				auto sz = GlobalSize(clip);
				char *data = (char*)GlobalLock(clip);
				IO::MemoryInputStream ms(data, data+sz);
				ret.ImportBMP(ms, true);
				GlobalUnlock(clip);
				CloseClipboard();
				return ret;
			}

			return ret;
		}

		void SetClipboardBitmap(Containers::Image img, bool append) {
			HWND wind = NULL;

			for(const auto &w : Window::Windows) {
				auto d = internal::getdata(w);
				if(d && d->handle) {
					wind = d->handle;
					break;
				}
			}

			if(!OpenClipboard(wind)) return;

			Utils::ScopeGuard g(&CloseClipboard);

			if(!append) {
				EmptyClipboard();
				clipboard_entries.clear();
			}

			auto mode = img.GetMode();


			auto data = make_clipboarddata(std::move(img));

			clipboard_entries.push_back({cf_g_bmp, data});
			clipboard_entries.push_back({cf_png, data});
			clipboard_entries.push_back({CF_DIB, data});

			SetClipboardData(cf_g_bmp, nullptr);
			SetClipboardData(cf_png, nullptr);
			SetClipboardData(CF_DIB, nullptr);
			//jpg does not work on windows

			//add file data later
		}

		//Monitor Related
		namespace internal {
			BOOL CALLBACK monitordata::MonitorEnumProc(
				_In_ HMONITOR hMonitor,
				_In_ HDC      hdcMonitor,
				_In_ LPRECT   lprcMonitor,
				_In_ LPARAM   dwData
				) 
			{
				MONITORINFOEX mi;
				mi.cbSize=sizeof(MONITORINFOEX);
				GetMonitorInfo(hMonitor, &mi);
				auto mon=new Monitor();
				mon->data->handle=hMonitor;
				mon->name=mi.szDevice;
				mon->area={mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right-mi.rcMonitor.left, mi.rcMonitor.bottom-mi.rcMonitor.top};
				mon->usable ={mi.rcWork.left, mi.rcWork.top, mi.rcWork.right, mi.rcWork.bottom};
				mon->isprimary=(mi.dwFlags&MONITORINFOF_PRIMARY)!=0;
				if(mon->isprimary) {
					Monitor::primary=mon;
					Monitor::monitors.Insert(mon, 0);
				}
				else {
					Monitor::monitors.Add(mon);
				}
				return true;
			}
		}
		Monitor::Monitor() {
			data = new internal::monitordata;
		}

		Monitor::~Monitor() {
			delete data;
		}

		void Monitor::Refresh(bool force) {
			monitors.Destroy();
			primary=nullptr;
			EnumDisplayMonitors(nullptr, nullptr, internal::monitordata::MonitorEnumProc, 0);
		}

		bool Monitor::IsChangeEventSupported() {
			return false;
		}

		Icon::Icon(const Containers::Image &image) {
			data = new internal::icondata;

            data->icon = 0;
            FromImage(image);
		}
		
		Icon::Icon() {
			data = new internal::icondata;
            data->icon = 0;
        }
        
        Icon::Icon(Icon &&icon) {
            data = new internal::icondata;
            std::swap(data, icon.data);
        }
        
        Icon &Icon::operator =(Icon &&icon) {
            Destroy();
            
            std::swap(data, icon.data);
            return *this;
        }
        
        void Icon::FromImage(const Containers::Image &image) {
			LONG dwWidth, dwHeight;
			BITMAPV5HEADER bi;
			HBITMAP hBitmap;
			void *lpBits;
			HICON hAlphaIcon = NULL;

			dwWidth  = image.GetWidth(); 
			dwHeight = image.GetHeight();

			ZeroMemory(&bi, sizeof(BITMAPV5HEADER));
			bi.bV5Size           = sizeof(BITMAPV5HEADER);
			bi.bV5Width           = dwWidth;
			bi.bV5Height          = -dwHeight;
			bi.bV5Planes = 1;
			bi.bV5BitCount = 32;
			bi.bV5Compression = BI_BITFIELDS;
			// The following mask specification specifies a supported 32 BPP
			// alpha format for Windows XP.
			bi.bV5RedMask   =  0x00FF0000;
			bi.bV5GreenMask =  0x0000FF00;
			bi.bV5BlueMask  =  0x000000FF;
			bi.bV5AlphaMask =  0xFF000000;

			HDC hdc;
			hdc = GetDC(NULL);

			// Create the DIB section with an alpha channel.
			hBitmap = CreateDIBSection(hdc, (BITMAPINFO *)&bi, DIB_RGB_COLORS,
				(void **)&lpBits, NULL, (DWORD)0);



			// Create an empty mask bitmap.
			HBITMAP hMonoBitmap = CreateBitmap(dwWidth, dwHeight, 1, 1, NULL);

			image.CopyToBGRABuffer((Byte*)lpBits);

			ICONINFO ii;
			ii.fIcon = TRUE;  // Change fIcon to TRUE to create an alpha icon
			ii.xHotspot = 0;
			ii.yHotspot = 0;
			ii.hbmMask = hMonoBitmap;
			ii.hbmColor = hBitmap;

			// Create the alpha cursor with the alpha DIB section.
			hAlphaIcon = CreateIconIndirect(&ii);

			DeleteObject(hBitmap);
			DeleteObject(hMonoBitmap);

			data->icon = hAlphaIcon;
        }
        
        void Icon::Destroy() {
            if(data->icon) {
                DeleteObject(data->icon);
                data->icon = 0;
            }
        }

		Icon::~Icon() {
            Destroy();
            
			delete data;
		}

		Event<> Monitor::ChangedEvent;
		Containers::Collection<Monitor> Monitor::monitors;
		Monitor *Monitor::primary=nullptr;
	}

	Window::Window(const WindowManager::Monitor &monitor, Geometry::Rectangle rect, const std::string &name, const std::string &title, bool allowresize, bool visible) :
		data(new internal::windowdata(*this)) { 
		windows.Add(this);
        pointerlayer = new Graphics::Layer;
        Add(pointerlayer);

		this->name = name;
		this->allowresize = allowresize;

		WNDCLASSEX windclass;
		HINSTANCE instance=GetModuleHandle(NULL);

		HWND hwnd;

		//Creating window class
		windclass.cbClsExtra = 0;
		windclass.cbSize = sizeof(WNDCLASSEX);
		windclass.cbWndExtra = 0;
		windclass.hbrBackground = (HBRUSH)16;
		windclass.hCursor = LoadCursor(NULL, NULL);
		windclass.hInstance = instance;
		windclass.lpfnWndProc = WndProc;
		windclass.lpszClassName = name.c_str();
		windclass.lpszMenuName = NULL;
		windclass.hIcon = (HICON)0;
		windclass.hIconSm = (HICON)0;
		windclass.style = 3;
		ATOM ret = RegisterClassEx(&windclass);

		if(ret == 0) {
			throw std::runtime_error("Cannot create window");
		}
		if(allowresize) {
			hwnd=CreateWindowExA(
				WS_EX_APPWINDOW | WS_EX_WINDOWEDGE, 
				name.c_str(), title.c_str(), 
				WS_TILEDWINDOW,
				CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, instance, NULL
			);
		}
		else {
			hwnd=CreateWindowExA(
				WS_EX_APPWINDOW | WS_EX_WINDOWEDGE,
				name.c_str(), title.c_str(),
				WS_MINIMIZEBOX | WS_SYSMENU | WS_CLIPSIBLINGS |WS_CLIPCHILDREN,
				CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, instance, NULL
			);
		}

		if(hwnd==INVALID_HANDLE_VALUE) {
			throw std::runtime_error("Cannot create window");
		}

		WINDOWINFO wi;
		wi.cbSize=sizeof(wi);

		GetWindowInfo(hwnd, &wi);

		auto size=rect.GetSize();

		data->chrome  = Geometry::Margin(wi.rcClient.left-wi.rcWindow.left, wi.rcClient.top-wi.rcWindow.top, wi.rcWindow.right-wi.rcClient.right, wi.rcWindow.bottom-wi.rcClient.bottom);
		rect += data->chrome.Total();

		if(rect.TopLeft()==automaticplacement) {
			rect.Move( (monitor.GetUsable()-rect.GetSize()).Center() );
		}

		SetWindowPos(hwnd, 0, rect.X, rect.Y, rect.Width, rect.Height, 0);

		if(visible)
			::ShowWindow(hwnd, 1);

		UpdateWindow(hwnd);

		data->handle=hwnd;
		data->device_context = GetDC(data->handle);
		internal::windowdata::mapping[hwnd]=data;

		PostMessage(hwnd, WM_ACTIVATE, -1, -1);

		Layer::Resize(size);

		createglcontext();
		glsize = size;

		OleInitialize(NULL);
		RegisterDragDrop(hwnd, &data->target);
	}

	Window::Window(const FullscreenTag &, const WindowManager::Monitor &monitor, const std::string &name, const std::string &title) : data(new internal::windowdata(*this)) {
		windows.Add(this);
        pointerlayer = new Graphics::Layer;
        Add(pointerlayer);

		this->name = name;

		WNDCLASSEX windclass;
		HINSTANCE instance;
		HWND hwnd;
		MSG msg;
		bool visible = true;
		ZeroMemory(&msg, sizeof(MSG));

		instance = GetModuleHandle(NULL);
		
		windclass.cbClsExtra = 0;
		windclass.cbSize = sizeof(WNDCLASSEX);
		windclass.cbWndExtra = 0;
		windclass.hbrBackground = (HBRUSH)16;
		windclass.hCursor = LoadCursor(NULL, NULL);
		windclass.hInstance = instance;
		windclass.lpfnWndProc = WndProc;
		windclass.lpszClassName = name.c_str();
		windclass.lpszMenuName = NULL;
		windclass.hIcon = (HICON)0;
		windclass.hIconSm = (HICON)0;
		windclass.style = CS_HREDRAW | CS_VREDRAW;
		windclass.cbSize = (unsigned int)sizeof(WNDCLASSEX);

		RegisterClassExA(&windclass);

		hwnd = CreateWindowEx(WS_EX_APPWINDOW, 
			name.c_str(), title.c_str(), 
			WS_OVERLAPPED | WS_POPUP, 0, 0, 
			(int)GetSystemMetrics(SM_CXSCREEN),
			(int)GetSystemMetrics(SM_CYSCREEN), 
			NULL, NULL, instance, NULL);

		if (hwnd == INVALID_HANDLE_VALUE) {
			throw std::runtime_error("Cannot create window");
		}

		SetWindowPos(hwnd, 0, monitor.GetLocation().X, monitor.GetLocation().Y, monitor.GetSize().Width, monitor.GetSize().Height, 0);
		if(visible)
			::ShowWindow(hwnd, SW_SHOW);

		UpdateWindow(hwnd);

		data->handle = hwnd;
		data->device_context = GetDC(data->handle);
		internal::windowdata::mapping[hwnd] = data;

		PostMessage(hwnd, WM_ACTIVATE, -1, -1);

		Layer::Resize({monitor.GetSize().Width, monitor.GetSize().Height});

		createglcontext();
		glsize = monitor.GetSize();

		OleInitialize(NULL);
		RegisterDragDrop(hwnd, &data->target);
	}

	void Window::Destroy() {
        if(data) {
            DestroyWindow(data->handle);
            internal::windowdata::mapping[data->handle]=nullptr;
        }
        
		windows.Remove(this);
        
		delete data;
        data = nullptr;

        delete pointerlayer;
        pointerlayer = nullptr;
	}

	void Window::Show() {
		ShowWindow(data->handle, SW_SHOW);
		SetActiveWindow(data->handle);
		ShowWindow(data->handle, SW_SHOW);
	}

	void Window::Hide() {
		ShowWindow(data->handle, SW_HIDE);
	}
	
	void Window::HidePointer() {
        if(iswmpointer) {
            if(data->pointerdisplayed) {
                data->pointerdisplayed=false;
                SetCursor(NULL);
                ShowCursor(false);
			
				SetCursor(NULL);
				ShowCursor(false);
			}
        }
        else {
            pointerlayer->Clear();
            pointerlayer->Hide();
        }
		showptr = false;
	}

	void Window::ShowPointer() {
        if(iswmpointer) {
            if(!data->pointerdisplayed) {
				data->pointerdisplayed=true;
				ShowCursor(true);
				SetCursor((HCURSOR)WindowManager::defaultcursor);

				ShowCursor(true);
				SetCursor((HCURSOR)WindowManager::defaultcursor);
			}
        }
        else {
            pointerlayer->Show();
        }

		showptr = true;
	}

	void Window::Resize(const Geometry::Size &size) {
		Layer::Resize(size);

		WINDOWINFO wi;
		wi.cbSize=sizeof(wi);

		GetWindowInfo(data->handle, &wi);

		SetWindowPos(data->handle, 0, 0, 0, 
			size.Width + (wi.rcWindow.right-wi.rcWindow.left) - (wi.rcClient.right-wi.rcClient.left), 
			size.Height+ (wi.rcWindow.bottom-wi.rcWindow.top) - (wi.rcClient.bottom-wi.rcClient.top), 
			SWP_NOMOVE | SWP_NOZORDER
		);
		activatecontext();
		GL::Resize(size);
	}

	void Window::Move(const Geometry::Point &location) {
		SetWindowPos(data->handle, 0, location.X, location.Y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	}

	void Window::processmessages() {
		POINT p;
		HWND handle;

		GetCursorPos(&p);

		handle=WindowFromPoint(p);

		mouselocation = WindowManager::GetMousePosition(data);
		if(handle == data->handle)
			mouse_location();
	}

	void Window::createglcontext() {
		static	PIXELFORMATDESCRIPTOR pfd=	// pfd Tells Windows How We Want Things To Be
		{
			sizeof(PIXELFORMATDESCRIPTOR),	// Size Of This Pixel Format Descriptor
			1,								// Version Number
			PFD_DRAW_TO_WINDOW |			// Format Must Support Window
			PFD_SUPPORT_OPENGL |			// Format Must Support OpenGL
			PFD_DOUBLEBUFFER,				// Must Support Double Buffering
			PFD_TYPE_RGBA,					// Request An RGBA Format
			32,								// Select Our Color Depth
			0, 0, 0, 0, 0, 0,				// Color Bits Ignored
			0,								// No Alpha Buffer
			0,								// Shift Bit Ignored
			0,								// No Accumulation Buffer
			0, 0, 0, 0,						// Accumulation Bits Ignored
			16,								// 16Bit Z-Buffer (Depth Buffer)
			0,								// No Stencil Buffer
			0,								// No Auxiliary Buffer
			PFD_MAIN_PLANE,					// Main Drawing Layer
			0,								// Reserved
			0, 0, 0							// Layer Masks Ignored
		};

		int PixelFormat=ChoosePixelFormat(data->device_context, &pfd);
		SetPixelFormat(data->device_context, PixelFormat, &pfd);

		data->context = wglCreateContext(data->device_context);
		WindowManager::internal::switchcontext(*data);

		if(data->context==NULL) {
			OS::DisplayMessage("Context creation failed");
			exit(0);
		}

		Graphics::Initialize();

		GL::SetupContext(bounds.GetSize());
	}

	void Window::Close() {
		DestroyWindow(data->handle);
		data->handle = 0;
	}

	void Window::SetTitle(const std::string &title) {
		SetWindowText(data->handle, title.c_str());
	}

	std::string Window::GetTitle() const {
		std::string ret;
		ret.resize(256);

		auto len = GetWindowText(data->handle, &ret[0], 256);
		ret.resize(len);

		return ret;
	}

	bool Window::IsClosed() const {
		return data->handle == 0;
	}

	Geometry::Bounds Window::GetExteriorBounds() const {
		RECT rect;

		GetWindowRect(data->handle, &rect);

		return {(int)rect.left, (int)rect.top, (int)rect.right, (int)rect.bottom};
	}

	void Window::Focus() {
		SetFocus(data->handle);
	}

	bool Window::IsFocused() const {
		return GetForegroundWindow() == data->handle;
	}

	void Window::Minimize() {
		ShowWindow(data->handle, SW_MINIMIZE);
		data->min = true;
		MinimizedEvent();
	}

	void Window::Maximize() {
		ShowWindow(data->handle, SW_MAXIMIZE);

		RECT rect;
		GetClientRect(data->handle, &rect);

		Layer::Resize({int(rect.right-rect.left), int(rect.bottom-rect.top)});
		activatecontext();
		GL::Resize({int(rect.right-rect.left), int(rect.bottom-rect.top)});
	}

	void Window::Restore() {
		ShowWindow(data->handle, SW_RESTORE);
		if(data->min) {
			data->min = false;
			RestoredEvent();
		}
		else {
            RECT rect;
            GetClientRect(data->handle, &rect);

            Layer::Resize({int(rect.right-rect.left), int(rect.bottom-rect.top)});
			activatecontext();
			GL::Resize({int(rect.right-rect.left), int(rect.bottom-rect.top)});
        }
	}

	bool Window::IsMinimized() const {
		return IsIconic(data->handle) != 0;
	}

	bool Window::IsMaximized() const {
		WINDOWPLACEMENT p;
		p.length = sizeof(p);

		GetWindowPlacement(data->handle, &p);

		return p.showCmd == SW_MAXIMIZE;
	}

	void Window::AllowResize() {
		auto s = GetWindowLong(data->handle, GWL_STYLE);
		s |= WS_SIZEBOX;
		s |= WS_MAXIMIZEBOX;
		SetWindowLong(data->handle, GWL_STYLE, s);

		allowresize = true;
	}

	void Window::PreventResize() {
		auto s = GetWindowLong(data->handle, GWL_STYLE);
		s &= ~WS_SIZEBOX;
		s &= ~WS_MAXIMIZEBOX;
		SetWindowLong(data->handle, GWL_STYLE, s);

		allowresize = false;
	}

	void Window::SetIcon(const WindowManager::Icon &icon) {
        if(icon.data->icon)
            SetClassLong(data->handle, GCL_HICON, (LONG)icon.data->icon);
	}

	void Window::updatedataowner() {
		if(data) {
			data->parent = this;
			data->target.parent = this;
		}
	}
}
