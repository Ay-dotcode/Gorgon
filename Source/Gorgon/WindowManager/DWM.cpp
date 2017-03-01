#include <map>

#include "../WindowManager.h"
#include "../Window.h"
#include "../Input.h"
#include "../OS.h"

#define WINDOWS_LEAN_AND_MEAN

#include "../GL/OpenGL.h"
#include "../Graphics.h"

#include "../Graphics/Layer.h"

#include <windows.h>
#include "../Geometry/Margin.h"

#	undef CreateWindow
#	undef Rectangle

namespace Gorgon {

	namespace WindowManager {
		Geometry::Point GetMousePosition(Gorgon::internal::windowdata *wind);
	}

	/// @cond INTERNAL
	namespace internal {

		struct windowdata {
			windowdata(Window &parent) : parent(parent) { }

			HWND handle = 0;
			Window &parent;
			HGLRC context=0;
			HDC device_context=0;
			Geometry::Margin chrome = {0, 0};
			bool min = false;

			std::map<Input::Key, ConsumableEvent<Window, Input::Key, bool>::Token> handlers;

			LRESULT Proc(UINT message, WPARAM wParam, LPARAM lParam) {

				switch(message) {

					case WM_ACTIVATE: {
						if(LOWORD(wParam)) {
							parent.FocusedEvent();
						}
						else {
							//ReleaseAll();
							parent.LostFocusEvent();
						}
					} //Activate
					break;


					case WM_ERASEBKGND:
						return true;

					case WM_CLOSE: {
						bool allow;
						allow=true;
						parent.ClosingEvent(allow);
						if(allow)
							return DefWindowProc(handle, message, wParam, lParam);
					}
					break;


					case WM_SYSCOMMAND:
						if(wParam == SC_MINIMIZE) {
							parent.MinimizedEvent();
							min = true;
						}
						else if(wParam == SC_RESTORE && min) {
							parent.RestoredEvent();
							min = false;
						}
						break;

					case WM_DESTROY:
						//ReleaseAll();
						parent.DestroyedEvent();
						break;

					case WM_LBUTTONDOWN: {
						int x=lParam%0x10000;
						int y=lParam>>16;

						parent.mouse_down({x, y}, Input::Mouse::Button::Left);
					}
					break;

					case WM_SIZE: {
						RECT rect;
						GetClientRect(parent.data->handle, &rect);

						parent.activatecontext();
						parent.Layer::Resize({rect.right-rect.left, rect.bottom-rect.top});
						GL::Resize({rect.right-rect.left, rect.bottom-rect.top});
					}

					case WM_LBUTTONUP: {
						int x=lParam%0x10000;
						int y=lParam>>16;

						parent.mouse_up({x, y}, Input::Mouse::Button::Left);
					}
					break;

					case WM_RBUTTONDOWN: {
						int x=lParam%0x10000;
						int y=lParam>>16;

						parent.mouse_down({x, y}, Input::Mouse::Button::Right);
					}
					break;

					case WM_RBUTTONUP: {
						int x=lParam%0x10000;
						int y=lParam>>16;

						parent.mouse_up({x, y}, Input::Mouse::Button::Right);
					}
					break;

					case WM_MBUTTONDOWN: {
						int x=lParam%0x10000;
						int y=lParam>>16;

						parent.mouse_down({x, y}, Input::Mouse::Button::Middle);
					}
					break;

					case WM_MBUTTONUP: {
						int x=lParam%0x10000;
						int y=lParam>>16;

						parent.mouse_up({x, y}, Input::Mouse::Button::Middle);
					}
					break;

					case WM_MOVE: 
						parent.MovedEvent();
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

						parent.mouse_down({x, y}, btn);
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

						parent.mouse_up({x, y}, btn);
					}
					break;

					case WM_MOUSEWHEEL:
						parent.mouse_event(Input::Mouse::EventType::Scroll_Vert, parent.mouselocation, Input::Mouse::Button::None, std::round(GET_WHEEL_DELTA_WPARAM(wParam)/120.f*16)/16);
						break;

					case WM_MOUSEHWHEEL:
						parent.mouse_event(Input::Mouse::EventType::Scroll_Hor, parent.mouselocation, Input::Mouse::Button::None, std::round(GET_WHEEL_DELTA_WPARAM(wParam)/120.f*16)/16);
						break;
                        
                    case WM_GESTURE: {
                        GESTUREINFO gi;  
    
                        ZeroMemory(&gi, sizeof(GESTUREINFO));
                        
                        gi.cbSize = sizeof(GESTUREINFO);

                        BOOL bResult  = GetGestureInfo((HGESTUREINFO)lParam, &gi);
                        
                        if(gi.dwID == GID_ZOOM) {
                            //todo test
                            parent.mouse_event(Input::Mouse::EventType::Zoom, parent.mouselocation, Input::Mouse::Button::None, (float)gi.ullArguments);
						}
                        else if(gi.dwID == GID_ROTATE) {
                            parent.mouse_event(Input::Mouse::EventType::Zoom, parent.mouselocation, Input::Mouse::Button::None, (float)GID_ROTATE_ANGLE_FROM_ARGUMENT(gi.ullArguments));
                        }
                    }
                    break;

					case WM_SYSKEYDOWN:
					case WM_KEYDOWN: {
						if(lParam&1<<30) {
							if(handlers.count(wParam)>0 && handlers[wParam]!=ConsumableEvent<Window, Input::Key, bool>::EmptyToken) {
								parent.KeyEvent.FireFor(handlers[wParam], wParam, true);
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

						auto token=parent.KeyEvent(wParam, true);
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
							parent.KeyEvent.FireFor(handlers[wParam], wParam, false);
							handlers[wParam]=ConsumableEvent<Window, Input::Key, bool>::EmptyToken;
						}
						else {
							parent.KeyEvent(wParam, false);
						}
						
					} //Keyup
					return 0;


					case WM_CHAR:
						if(handlers.count(wParam)==0 || handlers[wParam]==ConsumableEvent<Window, Input::Key, bool>::EmptyToken) {
							if(wParam==8 || wParam==127 || wParam==27) return 0;

							parent.CharacterEvent(wParam);
						}
						else {
							parent.KeyEvent.FireFor(handlers[wParam], wParam, true);
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
		bool pointerdisplayed;
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
		}
		/// @endcond


		void Initialize() {
			defaultcursor=LoadCursor(NULL, IDC_ARROW);

			Monitor::Refresh(true);
		}

		void SetClipboardText(const std::string &text) {
			if(OpenClipboard(NULL)) {
				HGLOBAL clipbuffer;
				char * buffer;
				EmptyClipboard();
				clipbuffer = GlobalAlloc(GMEM_DDESHARE, text.length()+1);
				buffer = (char*)GlobalLock(clipbuffer);
				strcpy_s(buffer, text.length()+1, text.c_str());
				GlobalUnlock(clipbuffer);
				SetClipboardData(CF_TEXT, clipbuffer);
				CloseClipboard();
			}
		}
		
		std::string GetClipboardText() {
			HANDLE clip;
			if(OpenClipboard(NULL)) {
				clip = GetClipboardData(CF_TEXT);
				if(clip == nullptr)
					return "";

				return std::string((char*)clip);
			}
			else {
				return "";
			}
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
	}

	Window::~Window() {
		DestroyWindow(data->handle);
		internal::windowdata::mapping[data->handle]=nullptr;
		windows.Remove(this);
		delete data;
        delete pointerlayer;
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
            if(WindowManager::pointerdisplayed) {
                WindowManager::pointerdisplayed=false;
                SetCursor(NULL);
                ShowCursor(false);
            }
        }
        else {
            pointerlayer->Clear();
            pointerlayer->Hide();
        }
	}

	void Window::ShowPointer() {
        if(iswmpointer) {
            if(!WindowManager::pointerdisplayed) {
                WindowManager::pointerdisplayed=true;
                ShowCursor(true);
                SetCursor((HCURSOR)WindowManager::defaultcursor);
            }
        }
        else {
            pointerlayer->Show();
        }
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
}
