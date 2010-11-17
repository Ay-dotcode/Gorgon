#include "OS.h"
#include "input.h"
#include "Multimedia.h"

#ifdef WIN32

#define WINVER 0x0500
#define _WIN32_WINNT 0x0500
#include <windows.h>
#undef CreateWindow
#undef Rectangle

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL					0x020A
#define GET_WHEEL_DELTA_WPARAM(wParam)  ((short)HIWORD(wParam))
#endif


#ifndef WM_XBUTTONDOWN
#define WM_XBUTTONDOWN                  0x020B
#define WM_XBUTTONUP                    0x020C
#define WM_XBUTTONDBLCLK                0x020D

#define GET_XBUTTON_WPARAM(wParam)      (HIWORD(wParam))
#endif


#ifndef WM_MOUSEHWHEEL
#define WM_MOUSEHWHEEL					0x020E
#endif

namespace gge {

	EventChain<Empty, empty_event_params> WindowActivateEvent("WindowActivate",NULL);
	EventChain<Empty, empty_event_params> WindowDeactivateEvent("WindowDectivate",NULL);
	EventChain<Empty, empty_event_params> WindowDestroyedEvent("WindowDestroyed",NULL);

	CursorHandle defaultcursor;
	bool ospointerdisplayed;

	POINT windowoverhead;
	HWND curwin;

	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch(message)
		{
		case WM_ACTIVATE:
			if(LOWORD(wParam))  {
				WindowActivateEvent();
			}
			else {
				WindowDeactivateEvent();
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
				int x=lParam%0x10000;
				int y=lParam>>16;
				
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
				
				ProcessVScroll(GET_WHEEL_DELTA_WPARAM(wParam)/120,x-windowoverhead.x,y-windowoverhead.y);
			}
			break;
		case WM_MOUSEHWHEEL:
			{
				int x=lParam%0x10000;
				int y=lParam>>16;
				
				ProcessHScroll(GET_WHEEL_DELTA_WPARAM(wParam)/120,x-windowoverhead.x,y-windowoverhead.y);
			}
			break;

		case WM_KEYDOWN:
			if(lParam&1<<24)
				isAlternateKey=true;
			else
				isAlternateKey=false;

			if(lParam==0x360001)
				AddModifier(KeyboardModifiers,KEYB_MOD_ALTERNATIVE);


			switch(wParam) {
			case VK_CONTROL:
				AddModifier(KeyboardModifiers,KEYB_MOD_CTRL);
				break;
			case VK_SHIFT:
				AddModifier(KeyboardModifiers,KEYB_MOD_SHIFT);
				break;
			case VK_MENU:
				AddModifier(KeyboardModifiers,KEYB_MOD_ALT);
				break;
			case VK_LWIN:
				RemoveModifier(KeyboardModifiers,KEYB_MOD_ALTERNATIVE);
				AddModifier(KeyboardModifiers,KEYB_MOD_WIN);
				break;
			case VK_RWIN:
				AddModifier(KeyboardModifiers,KEYB_MOD_ALTERNATIVE);
				AddModifier(KeyboardModifiers,KEYB_MOD_WIN);
				break;
			default:
				RemoveModifier(KeyboardModifiers,KEYB_MOD_ALTERNATIVE);
			}
			ProcessKeyDown(wParam);
				
			break; 
		case WM_KEYUP:
			if(lParam&1<<24)
				isAlternateKey=true;
			else
				isAlternateKey=false;

			ProcessKeyUp(wParam);
			switch(wParam) {
			case VK_CONTROL:
				RemoveModifier(KeyboardModifiers,KEYB_MOD_CTRL);
				break;
			case VK_SHIFT:
				RemoveModifier(KeyboardModifiers,KEYB_MOD_SHIFT);
				break;
			case VK_MENU:
				RemoveModifier(KeyboardModifiers,KEYB_MOD_ALT);
				break;
			case VK_LWIN:
				RemoveModifier(KeyboardModifiers,KEYB_MOD_ALTERNATIVE);
				RemoveModifier(KeyboardModifiers,KEYB_MOD_WIN);
				break;
			case VK_RWIN:
				RemoveModifier(KeyboardModifiers,KEYB_MOD_ALTERNATIVE);
				RemoveModifier(KeyboardModifiers,KEYB_MOD_WIN);
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
			AddModifier(KeyboardModifiers,KEYB_MOD_ALT);
			break; 
		case WM_SYSKEYUP:
			RemoveModifier(KeyboardModifiers,KEYB_MOD_ALT);
			break; 
		case WM_SYSCHAR:
			AddModifier(KeyboardModifiers,KEYB_MOD_ALT);
			ProcessChar(wParam);
			RemoveModifier(KeyboardModifiers,KEYB_MOD_ALT);
			break;
		case WM_VIDEO_NOTIFY:
			if(lParam) {
				Multimedia* vid=((Multimedia*)lParam);
				vid->ProcessMsg();
			}
			break;
		case WM_DESTROY:
			WindowDestroyedEvent();
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		return 0;
	}

	WindowHandle CreateWin(const char *Name, const char *Title, IconHandle Icon, InstanceHandle Instance, int Left, int Top, int Width, int Height, int BitDepth, bool &FullScreen) {
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
		windclass.lpszClassName=Name;
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
			ret=CreateWindowEx(WS_EX_APPWINDOW | WS_EX_WINDOWEDGE, Name, Title, WS_MINIMIZEBOX | WS_SYSMENU | WS_CLIPSIBLINGS |WS_CLIPCHILDREN ,CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, (HINSTANCE)Instance, NULL);
		else																							   
			ret=CreateWindowA(Name, Title, WS_POPUP, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, (HINSTANCE)Instance, NULL);

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
		defaultcursor=(CursorHandle)cursor;

		windowoverhead.x=0;
		windowoverhead.y=0;

		if(!FullScreen)
			ClientToScreen(ret,&windowoverhead);

		if(!FullScreen)
			SetWindowPos(ret, 0, Left, Top, 0, 0,SWP_NOSIZE | SWP_NOZORDER);


		return (WindowHandle)ret;
	}

	void ProcessMessage() {
		MSG msg;
		if( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) ) {
			GetMessage(&msg, NULL, 0, 0 );

			// Translate and dispatch the message
			TranslateMessage( &msg ); 
			DispatchMessage( &msg );
		}
	}
	void DisplayMessage(char *Title, char *Text) {
		MessageBox(NULL,Text,Title,0);
	}
	void ShowOSPointer() {
		if(!ospointerdisplayed) {
			ospointerdisplayed=true;
			ShowCursor(true);
			SetCursor((HCURSOR)defaultcursor);
		}
	}
	void HideOSPointer() {
		if(ospointerdisplayed) {
			ospointerdisplayed=false;
			SetCursor(NULL);
			ShowCursor(false);
		}
	}
	void Quit(int ret) {
		PostQuitMessage(ret);
		exit(ret);
	}
	Point getMousePosition(WindowHandle Window) {
		POINT pnt;
		RECT winrect;
		Point ret;
		GetWindowRect((HWND)Window, &winrect);
		GetCursorPos(&pnt);

		ret.x=pnt.x-(windowoverhead.x+winrect.left);
		ret.y=pnt.y-(windowoverhead.y+winrect.top);

		return ret;
	}
	void InitializeOS() {
		ospointerdisplayed=true;
	}
}
#endif