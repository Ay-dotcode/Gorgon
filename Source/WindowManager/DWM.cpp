#include <map>

#include "../WindowManager.h"
#include "../Window.h"

#define WINDOWS_LEAN_AND_MEAN

#include <windows.h>

#	undef CreateWindow
#	undef Rectangle

namespace Gorgon {

	/// @cond INTERNAL
	namespace internal {

		struct windowdata {
			HWND handle = 0;

			LRESULT Proc(UINT message, WPARAM wParam, LPARAM lParam) {
				return DefWindowProc(handle, message, wParam, lParam);
			}

			static std::map<HWND, windowdata*> mapping;
		};

		std::map<HWND, windowdata*> windowdata::mapping;
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
		/// @endcond

		Geometry::Rectangle GetUsableScreenRegion(int monitor) {
			RECT rect;

			SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);

			return Geometry::Bounds{rect.left, rect.top, rect.right, rect.bottom};
		}

		void Initialize() {
			defaultcursor=LoadCursor(NULL, IDC_ARROW);
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
	}

	Window::Window(Geometry::Rectangle rect, const std::string &name, const std::string &title, bool visible, bool useoutermetrics) :
		data(new internal::windowdata) { 
		windows.Add(this);

		WNDCLASSEX windclass;
		HINSTANCE instance=GetModuleHandle(NULL);

		HWND hwnd;

		///*Creating window class
		windclass.cbClsExtra=0;
		windclass.cbSize=sizeof(WNDCLASSEX);
		windclass.cbWndExtra=0;
		windclass.hbrBackground=(HBRUSH)16;
		windclass.hCursor=LoadCursor(NULL, NULL);
		windclass.hInstance=instance;
		windclass.lpfnWndProc=WndProc;
		windclass.lpszClassName=name.c_str();
		windclass.lpszMenuName=NULL;
		windclass.hIcon=(HICON)0;
		windclass.hIconSm=(HICON)0;
		windclass.style=3;
		ATOM ret=RegisterClassEx(&windclass);

		if(ret==0) {
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

		if(!useoutermetrics) {
			rect.Width += (wi.rcWindow.right-wi.rcWindow.left) - (wi.rcClient.right-wi.rcClient.left);
			rect.Height+= (wi.rcWindow.bottom-wi.rcWindow.top) - (wi.rcClient.bottom-wi.rcClient.top);
		}

		if(rect.TopLeft()==automaticplacement) {
			rect.Move( (WindowManager::GetUsableScreenRegion()-rect.GetSize()).Center() );
		}

		SetWindowPos(hwnd, 0, rect.X, rect.Y, rect.Width, rect.Height, 0);

		if(visible)
			::ShowWindow(hwnd, 1);

		UpdateWindow(hwnd);

		data->handle=hwnd;
	}
	
	Window::Window(const std::string &name, const std::string &title) : data(new internal::windowdata) {
		
	}
	
	Window::~Window() {
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

	void Window::ResizeOuter(const Geometry::Size &size) {
		SetWindowPos(data->handle, 0, 0, 0, size.Width, size.Height, SWP_NOMOVE | SWP_NOZORDER);
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
		WINDOWINFO wi;
		wi.cbSize=sizeof(wi);

		GetWindowInfo(data->handle, &wi);

		ResizeOuter({
			size.Width + (wi.rcWindow.right-wi.rcWindow.left) - (wi.rcClient.right-wi.rcClient.left),
			size.Height+ (wi.rcWindow.bottom-wi.rcWindow.top) - (wi.rcClient.bottom-wi.rcClient.top)
		});
	}

	void Window::Move(const Geometry::Point &location) {
		SetWindowPos(data->handle, 0, location.X, location.Y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	}

	void Window::processmessages() {
		// handled by operating system
	}


} 