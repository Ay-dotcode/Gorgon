#include <map>

#include "../WindowManager.h"
#include "../Window.h"
#include "../Input.h"
#include "../OS.h"

#define WINDOWS_LEAN_AND_MEAN

#include "../GL/OpenGL.h"
#include "../Graphics.h"

#include <windows.h>

#	undef CreateWindow
#	undef Rectangle

namespace Gorgon {

	/// @cond INTERNAL
	namespace internal {

		struct windowdata {
			windowdata(Window &parent) : parent(parent) { }

			HWND handle = 0;
			Window &parent;
			HGLRC context=0;
			HDC device_context=0;

			std::map<Input::Key, ConsumableEvent<Window, Input::Key, bool>::Token> handlers;

			LRESULT Proc(UINT message, WPARAM wParam, LPARAM lParam) {
				switch(message) {

					case WM_ACTIVATE: {
						if(LOWORD(wParam)) {
							parent.ActivatedEvent();
						}
						else {
							//ReleaseAll();
							parent.DeactivatedEvent();
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


					case WM_DESTROY:
						//ReleaseAll();
						parent.DestroyedEvent();
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
							Keyboard::CurrentModifier.Add(Keyboard::Modifier::Ctrl);
							break;
						case VK_SHIFT:
							Keyboard::CurrentModifier.Add(Keyboard::Modifier::Shift);
							break;
						case VK_LWIN:
							Keyboard::CurrentModifier.Add(Keyboard::Modifier::Meta);
							break;
						case VK_RWIN:
							Keyboard::CurrentModifier.Add(Keyboard::Modifier::Meta);
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
							Keyboard::CurrentModifier.Remove(Keyboard::Modifier::Ctrl);
							break;
						case VK_SHIFT:
							Keyboard::CurrentModifier.Remove(Keyboard::Modifier::Shift);
							break;
						case VK_LWIN:
							Keyboard::CurrentModifier.Remove(Keyboard::Modifier::Meta);
							break;
						case VK_RWIN:
							Keyboard::CurrentModifier.Remove(Keyboard::Modifier::Meta);
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

			void switchcontext(Gorgon::internal::windowdata &data) {
				wglMakeCurrent(data.device_context, data.context);
				context=reinterpret_cast<intptr_t>(&data);
			}

			void finalizerender(Gorgon::internal::windowdata &data) {
				glFinish();
				glFlush();
				SwapBuffers(data.device_context);
			}
		}
		/// @endcond

		Geometry::Rectangle GetUsableScreenRegion(int monitor) {
			RECT rect;

			SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);

			return Geometry::Bounds{rect.left, rect.top, rect.right, rect.bottom};
		}

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
				mon->size={mi.rcMonitor.right-mi.rcMonitor.left, mi.rcMonitor.bottom-mi.rcMonitor.top};
				mon->location={mi.rcMonitor.left, mi.rcMonitor.top};
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

		Event<> Monitor::ChangedEvent;
		Containers::Collection<Monitor> Monitor::monitors;
		Monitor *Monitor::primary=nullptr;
	}

	Window::Window(Geometry::Rectangle rect, const std::string &name, const std::string &title, bool visible) :
		data(new internal::windowdata(*this)) { 
		windows.Add(this);

		WNDCLASSEX windclass;
		HINSTANCE instance=GetModuleHandle(NULL);

		HWND hwnd;

		///*Creating window class
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

		hwnd=CreateWindowExA(
			WS_EX_APPWINDOW | WS_EX_WINDOWEDGE, 
			name.c_str(), title.c_str(), 
			WS_MINIMIZEBOX | WS_SYSMENU | WS_CLIPSIBLINGS |WS_CLIPCHILDREN, 
			CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, instance, NULL
		);

		if(hwnd==INVALID_HANDLE_VALUE) {
			throw std::runtime_error("Cannot create window");
		}

		WINDOWINFO wi;
		wi.cbSize=sizeof(wi);

		GetWindowInfo(hwnd, &wi);

		auto size=rect.GetSize();

		rect.Width += (wi.rcWindow.right-wi.rcWindow.left) - (wi.rcClient.right-wi.rcClient.left);
		rect.Height+= (wi.rcWindow.bottom-wi.rcWindow.top) - (wi.rcClient.bottom-wi.rcClient.top);

		if(rect.TopLeft()==automaticplacement) {
			rect.Move( (WindowManager::GetUsableScreenRegion()-rect.GetSize()).Center() );
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
	}

	Window::Window(const FullscreenTag &, const std::string &name, const std::string &title) : data(new internal::windowdata(*this)) {
		windows.Add(this);

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

		SetWindowPos(hwnd, 0, 0, 0, (int)GetSystemMetrics(SM_CXSCREEN), (int)GetSystemMetrics(SM_CYSCREEN), 0);
		if(visible)
			::ShowWindow(hwnd, SW_SHOW);

		UpdateWindow(hwnd);

		data->handle = hwnd;
		data->device_context = GetDC(data->handle);
		internal::windowdata::mapping[hwnd] = data;

		PostMessage(hwnd, WM_ACTIVATE, -1, -1);

		Layer::Resize({(int)GetSystemMetrics(SM_CXSCREEN), (int)GetSystemMetrics(SM_CYSCREEN)});

		createglcontext();
	}
	
	Window::~Window() {
		DestroyWindow(data->handle);
		internal::windowdata::mapping[data->handle]=nullptr;
		windows.Remove(this);
		delete data;
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
		if(WindowManager::pointerdisplayed) {
			WindowManager::pointerdisplayed=false;
			SetCursor(NULL);
			ShowCursor(false);
		}
	}

	void Window::ShowPointer() {
		if(!WindowManager::pointerdisplayed) {
			WindowManager::pointerdisplayed=true;
			ShowCursor(true);
			SetCursor((HCURSOR)WindowManager::defaultcursor);
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
	}

	void Window::Move(const Geometry::Point &location) {
		SetWindowPos(data->handle, 0, location.X, location.Y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	}

	void Window::processmessages() {
		// handled by operating system
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

		GL::SetupContext(bounds.GetSize());

		Graphics::Initialize();		
	}
} 
