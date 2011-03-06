#include "OS.h"
#include "input.h"
#include "Multimedia.h"

#ifdef WIN32

#	define WINVER 0x0500
#	define _WIN32_WINNT 0x0500
#	include <windows.h>
#	undef CreateWindow
#	undef Rectangle

#	ifndef WM_MOUSEWHEEL
#		define WM_MOUSEWHEEL					0x020A
#		define GET_WHEEL_DELTA_WPARAM(wParam)  ((short)HIWORD(wParam))
#	endif


#	ifndef WM_XBUTTONDOWN
#		define WM_XBUTTONDOWN                  0x020B
#		define WM_XBUTTONUP                    0x020C
#		define WM_XBUTTONDBLCLK                0x020D

#		define GET_XBUTTON_WPARAM(wParam)      (HIWORD(wParam))
#endif


#	ifndef WM_MOUSEHWHEEL
#		define WM_MOUSEHWHEEL					0x020E
#	endif

	namespace gge { namespace os {
		void DisplayMessage(const char *Title, const char *Text) {
			MessageBox(NULL,Text,Title,0);
		}
		void Quit(int ret) {
			PostQuitMessage(ret);
			exit(ret);
		}
		void Initialize() {
			system::pointerdisplayed=true;
		}
		void gge::os::Sleep(int ms) {
			::Sleep(ms);
		}


		namespace system {
			CursorHandle defaultcursor;
			bool pointerdisplayed;
			void ProcessMessage() {
			MSG msg;
			if( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) ) {
				GetMessage(&msg, NULL, 0, 0 );

				// Translate and dispatch the message
				TranslateMessage( &msg ); 
				DispatchMessage( &msg );
			}
		}
		}
		namespace window {
			EventChain<Empty, empty_event_params> Activated("WindowActivated",NULL);
			EventChain<Empty, empty_event_params> Deactivated("WindowDectivated",NULL);
			EventChain<Empty, empty_event_params> Destroyed("WindowDestroyed",NULL);


			//!Private
			POINT overhead;
			HWND curwin;

			LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
			{
				switch(message)
				{
				case WM_ACTIVATE:
					if(LOWORD(wParam))  {
						Activated();
					}
					else {
						Deactivated();
					}
					break;
				case WM_ERASEBKGND:
					return true;
				case WM_LBUTTONDOWN:
					{
						int x=lParam%0x10000;
						int y=lParam>>16;
						
						ProcessMouseDown(1,x,y);
					}
					break;
				case WM_LBUTTONUP:
					{
						int x=lParam%0x10000;
						int y=lParam>>16;
						
						ProcessMouseUp(1,x,y);
						ProcessMouseClick(1,x,y);
					}
					break;
				case WM_LBUTTONDBLCLK:
					{
						int x=lParam%0x10000;
						int y=lParam>>16;
						
						ProcessMouseDblClick(1,x,y);
					}
					break;

				case WM_RBUTTONDOWN:
					{
						int x=lParam%0x10000;
						int y=lParam>>16;
						
						ProcessMouseDown(2,x,y);
					}
					break;
				case WM_RBUTTONUP:
					{
						int x=lParam%0x10000;
						int y=lParam>>16;
						
						ProcessMouseUp(2,x,y);
						ProcessMouseClick(2,x,y);
					}
					break;
				case WM_RBUTTONDBLCLK:
					{
						int x=(int)lParam%0x10000;
						int y=(int)lParam>>16;
						
						ProcessMouseDblClick(2,x,y);
					}
					break;

				case WM_MBUTTONDOWN:
					{
						int x=lParam%0x10000;
						int y=lParam>>16;
						
						ProcessMouseDown(4,x,y);
					}
					break;
				case WM_MBUTTONUP:
					{
						int x=lParam%0x10000;
						int y=lParam>>16;
						
						ProcessMouseUp(4,x,y);
						ProcessMouseClick(4,x,y);
					}
					break;
				case WM_MBUTTONDBLCLK:
					{
						int x=lParam%0x10000;
						int y=lParam>>16;
						
						ProcessMouseDblClick(4,x,y);
					}
					break;

				case WM_XBUTTONDOWN:
					{
						int x=lParam%0x10000;
						int y=lParam>>16;
						
						switch(GET_XBUTTON_WPARAM(wParam)) {
						case 1:
							ProcessMouseDown(101,x,y);
							break;
						case 2:
							ProcessMouseDown(102,x,y);
							break;
						}
					}
					break;
				case WM_XBUTTONUP:
					{
						int x=lParam%0x10000;
						int y=lParam>>16;
						
						
						switch(GET_XBUTTON_WPARAM(wParam)) {
						case 1:
							ProcessMouseUp(101,x,y);
							ProcessMouseClick(101,x,y);
							break;
						case 2:
							ProcessMouseUp(102,x,y);
							ProcessMouseClick(102,x,y);
							break;
						}
					}
					break;
				case WM_XBUTTONDBLCLK:
					{
						int x=lParam%0x10000;
						int y=lParam>>16;
						
						switch(GET_XBUTTON_WPARAM(wParam)) {
						case 1:
							ProcessMouseDblClick(101,x,y);
							break;
						case 2:
							ProcessMouseDblClick(102,x,y);
							break;
						}
					}
					break;

				case WM_MOUSEWHEEL:
					{
						int x=lParam%0x10000;
						int y=lParam>>16;
						
						ProcessVScroll(GET_WHEEL_DELTA_WPARAM(wParam)/120,x-overhead.x,y-overhead.y);
					}
					break;
				case WM_MOUSEHWHEEL:
					{
						int x=lParam%0x10000;
						int y=lParam>>16;
						
						ProcessHScroll(GET_WHEEL_DELTA_WPARAM(wParam)/120,x-overhead.x,y-overhead.y);
					}
					break;

				case WM_KEYDOWN:
					if(lParam&1<<24)
						KeyboardModifier::isAlternate=true;
					else
						KeyboardModifier::isAlternate=false;

					if(lParam==0x360001)
						KeyboardModifier::Add(KeyboardModifier::Alternate);


					switch(wParam) {
					case VK_CONTROL:
						KeyboardModifier::Add(KeyboardModifier::Ctrl);
						break;
					case VK_SHIFT:
						KeyboardModifier::Add(KeyboardModifier::Shift);
						break;
					case VK_MENU:
						KeyboardModifier::Add(KeyboardModifier::Alt);
						break;
					case VK_LWIN:
						KeyboardModifier::Remove(KeyboardModifier::Alternate);
						KeyboardModifier::Add(KeyboardModifier::Win);
						break;
					case VK_RWIN:
						KeyboardModifier::Add(KeyboardModifier::Alternate);
						KeyboardModifier::Add(KeyboardModifier::Win);
						break;
					default:
						KeyboardModifier::Remove(KeyboardModifier::Alternate);
					}
					ProcessKeyDown(wParam);
						
					break; 
				case WM_KEYUP:
					if(lParam&1<<24)
						KeyboardModifier::isAlternate=true;
					else
						KeyboardModifier::isAlternate=false;

					ProcessKeyUp(wParam);
					switch(wParam) {
					case VK_CONTROL:
						KeyboardModifier::Remove(KeyboardModifier::Ctrl);
						break;
					case VK_SHIFT:
						KeyboardModifier::Remove(KeyboardModifier::Shift);
						break;
					case VK_MENU:
						KeyboardModifier::Remove(KeyboardModifier::Alt);
						break;
					case VK_LWIN:
						KeyboardModifier::Remove(KeyboardModifier::Alternate);
						KeyboardModifier::Remove(KeyboardModifier::Win);
						break;
					case VK_RWIN:
						KeyboardModifier::Remove(KeyboardModifier::Alternate);
						KeyboardModifier::Remove(KeyboardModifier::Win);
						break;
					}
						
					break; 
				case WM_CHAR:
					/*if(lParam&1<<24)
						AddModifier(&KeyboardModifier,KEYB_MOD_ALTERNATIVE);
					else
						RemoveModifier(&KeyboardModifier,KEYB_MOD_ALTERNATIVE);*/

					ProcessChar(wParam);
					break; 
				case WM_SYSKEYDOWN:
					KeyboardModifier::Add(KeyboardModifier::Alt);
					break; 
				case WM_SYSKEYUP:
					KeyboardModifier::Remove(KeyboardModifier::Alt);
					break; 
				case WM_SYSCHAR:
					KeyboardModifier::Add(KeyboardModifier::Alt);
					ProcessChar(wParam);
					KeyboardModifier::Remove(KeyboardModifier::Alt);
					break;
				case WM_VIDEO_NOTIFY:
					if(lParam) {
						Multimedia* vid=((Multimedia*)lParam);
						vid->ProcessMsg();
					}
					break;
				case WM_DESTROY:
					Destroyed();
					break;
				default:
					return DefWindowProc(hWnd, message, wParam, lParam);
				}
				return 0;
			}

			WindowHandle CreateWindow(string Name, string Title, os::IconHandle Icon, os::InstanceHandle Instance, int Left, int Top, int Width, int Height, int BitDepth, bool &FullScreen) {
				WNDCLASSEX windclass;

				HWND ret;

				///*Creating window class
				windclass.cbClsExtra=0;
				windclass.cbSize=sizeof(WNDCLASSEX);
				windclass.cbWndExtra=0;
				windclass.hbrBackground=(HBRUSH)16;
				windclass.hCursor=LoadCursor(NULL, NULL);
				windclass.hInstance=(HINSTANCE)Instance;
				windclass.lpfnWndProc=WndProc;
				windclass.lpszClassName=Name.c_str();
				windclass.lpszMenuName=NULL;
				windclass.hIcon=(HICON)Icon;
				windclass.hIconSm=(HICON)Icon;
				windclass.style=3;
				RegisterClassEx(&windclass);

				///*Setting fullscreen parameters
				if (FullScreen) {
					DEVMODE dmScreenSettings;									// device mode
					memset(&dmScreenSettings,0,sizeof(dmScreenSettings));
					dmScreenSettings.dmSize       = sizeof(dmScreenSettings); 
					dmScreenSettings.dmPelsWidth  = Width;						// screen width
					dmScreenSettings.dmPelsHeight = Height;						// screen height
					dmScreenSettings.dmBitsPerPel = BitDepth;					// bits per pixel
					dmScreenSettings.dmFields	  = DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;
					if(ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN)!= DISP_CHANGE_SUCCESSFUL) {
						MessageBox(NULL,"Cannot run in fullscreen, reverting to windowed mode\nTam ekran modu çalýþmamakta, pencerede gösterim modu seçildi","Initialize / Baþlangýç",0);
						FullScreen=false;
					}
				}

				///*Creating window
				if(!FullScreen)
					ret=CreateWindowExA(WS_EX_APPWINDOW | WS_EX_WINDOWEDGE, Name.c_str(), Title.c_str(), WS_MINIMIZEBOX | WS_SYSMENU | WS_CLIPSIBLINGS |WS_CLIPCHILDREN ,CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, (HINSTANCE)Instance, NULL);
				else																							   
					ret=CreateWindowA(Name.c_str(), Title.c_str(), WS_POPUP, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, (HINSTANCE)Instance, NULL);

				///*Adjusting window size and position
				ShowWindow(ret,1);
				if(!FullScreen) {
					RECT cr,wr;
					GetClientRect(ret,&cr);
					GetWindowRect(ret,&wr);

					SetWindowPos(ret, 0, 0, 0, 
						Width + ( (wr.right-wr.left) - (cr.right-cr.left) ),
						Height + ( (wr.bottom-wr.top) - (cr.bottom-cr.top) ),0);
				}
				else
					SetWindowPos(ret, 0, Left, Top, Width, Height, 0);

				UpdateWindow(ret);
				curwin=ret;

				HCURSOR cursor=LoadCursor(NULL, IDC_ARROW);
				system::defaultcursor=(CursorHandle)cursor;

				overhead.x=0;
				overhead.y=0;

				if(!FullScreen)
					ClientToScreen(ret,&overhead);

				if(!FullScreen)
					SetWindowPos(ret, 0, Left, Top, 0, 0,SWP_NOSIZE | SWP_NOZORDER);


				return (WindowHandle)ret;
			}


		}
	
		void ShowPointer() {
			if(!system::pointerdisplayed) {
				system::pointerdisplayed=true;
				ShowCursor(true);
				SetCursor((HCURSOR)system::defaultcursor);
			}
		}
		void HidePointer() {
			if(system::pointerdisplayed) {
				system::pointerdisplayed=false;
				SetCursor(NULL);
				ShowCursor(false);
			}
		}
		namespace input {
			Point getMousePosition(WindowHandle Window) {
				POINT pnt;
				RECT winrect;
				Point ret;
				GetWindowRect((HWND)Window, &winrect);
				GetCursorPos(&pnt);

				ret.x=pnt.x-(window::overhead.x+winrect.left);
				ret.y=pnt.y-(window::overhead.y+winrect.top);

				return ret;
			}

		}
	} }


#endif