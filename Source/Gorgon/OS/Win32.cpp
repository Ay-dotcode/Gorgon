
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>

#include "../OS.h"
#include "../Main.h"
#include "../Window.h"
#include "../Input.h"

#include "../Filesystem.h"


#define WINDOWS_LEAN_AND_MEAN
#define SECURITY_WIN32

#include <windows.h>
#include <Shlobj.h>
#include <Security.h>
#include <Secext.h>

#include <LM.h>
#include <LMaccess.h>

#ifndef WM_MOUSEWHEEL
#	define WM_MOUSEWHEEL					0x020A
#	define GET_WHEEL_DELTA_WPARAM(wParam)  ((short)HIWORD(wParam))
#endif


#ifndef WM_XBUTTONDOWN
#	define WM_XBUTTONDOWN                  0x020B
#	define WM_XBUTTONUP                    0x020C
#	define WM_XBUTTONDBLCLK                0x020D

#	define GET_XBUTTON_WPARAM(wParam)      (HIWORD(wParam))
#endif


#ifndef WM_MOUSEHWHEEL
#	define WM_MOUSEHWHEEL					0x020E
#endif

#undef GetName

namespace Gorgon { 
	namespace internal { bool ishandled(HWND hwnd, Input::Key key); }
	namespace OS {
	
	void Initialize() {
	}

	namespace User {
		std::string GetUsername() {
			CHAR username[256];
			username[0]=0;

			DWORD s=256;
			GetUserName(username, &s);

			return username;
		}

		std::string GetName() {
			CHAR name[256];
			name[0]=0;

			DWORD s=256;
			GetUserNameEx(NameDisplay, name, &s);

			return name;
		}

		std::string GetDocumentsPath() {
			CHAR my_documents[MAX_PATH];
			my_documents[0]=0;

			SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents);

			std::string s=my_documents;

			return Filesystem::Canonical(my_documents);
		}

		std::string GetHomePath() {
			CHAR profile[MAX_PATH];
			profile[0]=0;

			SHGetFolderPath(NULL, CSIDL_PROFILE, NULL, SHGFP_TYPE_CURRENT, profile);

			std::string s=profile;

			return Filesystem::Canonical(profile);
		}

		std::string GetDataPath() {
			CHAR path[MAX_PATH];
			path[0]=0;

			SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, path);

			std::string s=path;

			return Filesystem::Canonical(path);
		}
		
		bool IsAdmin() {
			bool result;
			DWORD rc;
			wchar_t user_name[256];
			
			USER_INFO_1 *info;
			DWORD size = sizeof( user_name );
			GetUserNameW( user_name, &size);
			rc = NetUserGetInfo( NULL, user_name, 1, (byte **) &info );
			
			if ( rc != NERR_Success )
					return false;
			
			result = info->usri1_priv == USER_PRIV_ADMIN;
			NetApiBufferFree( info );
			
			return result;
		}
	}

	void OpenTerminal() {
		int hConHandle;

		long lStdHandle;

		CONSOLE_SCREEN_BUFFER_INFO coninfo;

		FILE *fp;

		// allocate a console for this app

		AllocConsole();

		// set the screen buffer to be big enough to let us scroll text

		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),

			&coninfo);

		coninfo.dwSize.Y = 1024;

		SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE),

			coninfo.dwSize);

		// redirect unbuffered STDOUT to the console

		lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);

		hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

		fp = _fdopen(hConHandle, "w");

		*stdout = *fp;

		setvbuf(stdout, NULL, _IONBF, 0);

		// redirect unbuffered STDIN to the console

		lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);

		hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

		fp = _fdopen(hConHandle, "r");

		*stdin = *fp;

		setvbuf(stdin, NULL, _IONBF, 0);

		// redirect unbuffered STDERR to the console

		lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);

		hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

		fp = _fdopen(hConHandle, "w");

		*stderr = *fp;

		setvbuf(stderr, NULL, _IONBF, 0);


		// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog 

		// point to console as well

		std::ios::sync_with_stdio();
	}

	std::string GetName() {
		OSVERSIONINFO os ={};
		os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
#pragma warning(push)
#pragma warning(disable:4996)
		GetVersionEx(&os);
#pragma warning(pop)

		if(os.dwMajorVersion == 5) {
			switch(os.dwMinorVersion) {
			case 0:
				return "Windows 2000";
			case 1:
				return "Windows XP";
			case 2:
				return "Windows XP SP2";
			}
		}
		else if(os.dwMajorVersion == 6) {
			switch(os.dwMinorVersion) {
			case 0:
				return "Windows Vista";
			case 1:
				return "Windows 7";
			case 2:
				return "Windows 8";
			case 3:
				return "Windows 8.1";
			}
		}
		else if(os.dwMajorVersion == 10) {
			return "Windows 10";
		}

		return "Windows";
	}

	void DisplayMessage(const std::string &message) {
		MessageBox(NULL, message.c_str(), GetSystemName().c_str(), 0);
	}

	std::string GetAppDataPath() {
		CHAR path[MAX_PATH];
		path[0]=0;

		SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, SHGFP_TYPE_CURRENT, path);

		std::string s=path;

		return Filesystem::Canonical(path);
	}

	std::string GetAppSettingPath() {
		return GetAppDataPath();
	}

	void processmessages() {
		MSG msg;
		while(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
			GetMessage(&msg, NULL, 0, 0);

			DispatchMessage(&msg);

			if(msg.message!=WM_KEYDOWN || !internal::ishandled(msg.hwnd, msg.wParam)) {
				TranslateMessage(&msg);
			}
		}
	}

	bool Start(const std::string &name, const std::vector<std::string> &args) {
		STARTUPINFO si;
		memset(&si, 0, sizeof(si));

		PROCESS_INFORMATION pi;

		bool usepath=name.find_first_of("/")==name.npos;
		int size=0;
		size=name.length()+3;
		if(usepath) size*=2;
		for(auto &arg : args) {
			size+=arg.size()+3;
		}

		//build command line
		char *cmd=new char[size];
		int current=0;

		if(usepath) {
			//application to run
			cmd[current++]='"';
			strcpy(cmd+current, name.c_str());
			current+=name.size();
			cmd[current++]='"';
			cmd[current++]=' ';
		}

		//application name as first arg
		cmd[current++]='"';
		strcpy(cmd, name.c_str());
		current+=name.size();
		cmd[current++]='"';
		cmd[current++]=' ';

		//arguments
		for(auto arg : args) {
			cmd[current++]='"';
			strcpy(cmd, arg.c_str());
			current+=arg.size();
			cmd[current++]='"';
			cmd[current++]=' ';
		}
		cmd[size]='\0';

		bool ret;

		if(usepath) {
			ret=CreateProcess(nullptr, cmd, nullptr, nullptr, false, 0, nullptr, nullptr, &si, &pi)!=0;
		}
		else {
			ret=CreateProcess(name.c_str(), cmd, nullptr, nullptr, false, 0, nullptr, nullptr, &si, &pi)!=0;
		}

		if(ret) {
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);

			return true;
		}
		else {
			return false;
		}
	}
	
	bool Open(const std::string &file) {
		return (int)ShellExecute(nullptr, "open", file.c_str(), nullptr, nullptr, SW_SHOWNORMAL)>32;
	}

} }
/*	HINSTANCE Instance;

	extern int Application(std::vector<std::string> &arguments);

	int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
		Instance=hInstance;

		std::vector<std::string> arguments;

		for(int i=0;i<__argc;i++)
			arguments.push_back(__argv[i]);

		return Application(arguments);
	}

	namespace gge { namespace os {
		bool quiting=false;


		void Quit(int ret) {
			PostQuitMessage(ret);
			CloseWindow((HWND)Main.GetWindow());
			quiting=true;
			exit(ret);
		}
		void Cleanup() {
			CloseWindow((HWND)Main.GetWindow());
			quiting=true;
		}
		void Initialize() {
			system::pointerdisplayed=true;
		}

		namespace system { 
			class mutex_data { 
			public: 
				HANDLE mutex; 
				mutex_data() {
					mutex=CreateMutex(NULL,0,NULL);
				}
				~mutex_data() {
					CloseHandle(mutex);
				}
			}; 
		}


		namespace system {
			CursorHandle defaultcursor;
			bool pointerdisplayed;
			
			class GGEDropTarget : public IDropTarget
			{
			public:
				GGEDropTarget() : m_lRefCount(0) {
					object=&inlayer.MouseEvents.RegisterLambda([&]() -> bool {
						this->dragfinished();
						return true;
					}, utils::Bounds2D(0,0,10000,10000), gge::input::mouse::Event::DragCanceled | gge::input::mouse::Event::DragAccepted);
				}

				// IUnknown implementation
				HRESULT __stdcall QueryInterface (REFIID iid, void ** ppvObject) { return S_OK; }
				ULONG   __stdcall AddRef (void) {return ++m_lRefCount;}
				ULONG   __stdcall Release (void) {return --m_lRefCount;}

				// IDropTarget implementation
				HRESULT __stdcall DragEnter(IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect) {
					// does the dataobject contain data we want?
					FORMATETC fileformat = { CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
					FORMATETC textformat = { CF_TEXT, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

					if(pDataObject->QueryGetData(&fileformat)==S_OK) {
						gge::Pointers.Hide();
						auto data=new gge::input::mouse::FileListDragData();

						STGMEDIUM stgmed;
						std::string name;
						pDataObject->GetData(&fileformat, &stgmed);
						DROPFILES *fdata=(DROPFILES*)GlobalLock(stgmed.hGlobal);


						wchar_t *widetext=(wchar_t*)((char*)(fdata)+fdata->pFiles);

						while(widetext[0]) {
							name.resize(wcslen(widetext));

							wcstombs(&name[0], widetext, wcslen(widetext)+1);
							winslashtonormal(name);
							data->data.push_back(name);

							widetext+=wcslen(widetext)+1;
						}

						GlobalUnlock(stgmed.hGlobal);
						ReleaseStgMedium(&stgmed);


						gge::input::mouse::BeginDrag(*data, *object);
						*pdwEffect = DROPEFFECT_MOVE;
					}
					else if(pDataObject->QueryGetData(&textformat) == S_OK) {
						gge::Pointers.Hide();
						auto data=new gge::input::mouse::TextDragData();

						STGMEDIUM stgmed;
						std::string name;
						pDataObject->GetData(&textformat, &stgmed);
						char *text=(char*)GlobalLock(stgmed.hGlobal);

						name=text;

						data->data=name;


						GlobalUnlock(stgmed.hGlobal);
						ReleaseStgMedium(&stgmed);


						gge::input::mouse::BeginDrag(*data, *object);
						*pdwEffect = DROPEFFECT_COPY;
					}
					else {
						*pdwEffect = DROPEFFECT_NONE;
					}

					return S_OK;
				}
				HRESULT __stdcall DragOver(DWORD grfKeyState, POINTL pt, DWORD * pdwEffect){
					if(gge::input::mouse::HasDragTarget() && dynamic_cast<gge::input::mouse::FileListDragData*>(&gge::input::mouse::GetDraggedObject())) {
						auto &data=dynamic_cast<gge::input::mouse::FileListDragData&>(gge::input::mouse::GetDraggedObject());

						if(data.GetAction()==data.Move)
							*pdwEffect=DROPEFFECT_MOVE;
						else
							*pdwEffect=DROPEFFECT_COPY;
					}
					else if(gge::input::mouse::HasDragTarget() && dynamic_cast<gge::input::mouse::TextDragData*>(&gge::input::mouse::GetDraggedObject())) {
						*pdwEffect=DROPEFFECT_COPY;
					}
					else {
						*pdwEffect=DROPEFFECT_NONE;
					}

					return S_OK;
				}
				HRESULT __stdcall DragLeave(void) {
					gge::input::mouse::CancelDrag();
					gge::Pointers.Show();
					return S_OK;
				}
				HRESULT __stdcall Drop(IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect){
					gge::input::mouse::DropDrag();
					gge::Pointers.Show();

					return S_OK;
				}

				// Constructor
				~GGEDropTarget() {}

			private:
				void dragfinished() {
					if(dynamic_cast<gge::input::mouse::FileListDragData*>(&gge::input::mouse::GetDraggedObject())) {
						auto &data=dynamic_cast<gge::input::mouse::FileListDragData&>(gge::input::mouse::GetDraggedObject());

						delete &data;
					}
				}

				// Private member variables
				long   m_lRefCount;
				gge::input::mouse::Event::Target *object;
				InputLayer inlayer;


			};
		}
			

		namespace window {
			Point cursorlocation=Point(0,0);

			LRESULT __stdcall WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
			{
				if(quiting)
					return NULL;

				switch(message)
				{
				case WM_ACTIVATE:
					if(LOWORD(wParam))  {
						Activated();
					}
					else {
						ReleaseAll();
						Deactivated();
					}
					break;
				case WM_ERASEBKGND:
					return true;
				case WM_CLOSE:
					bool allow;
					allow=true;
					Closing(allow);
					if(allow)
						return DefWindowProc(hWnd, message, wParam, lParam);

					break;
				case WM_LBUTTONDOWN:
					{
						int x=lParam%0x10000;
						int y=lParam>>16;
						
						ProcessMouseDown(mouse::Event::Left,x,y);
					}
					break;
				case WM_LBUTTONUP:
					{
						int x=lParam%0x10000;
						int y=lParam>>16;
						
						ProcessMouseClick(mouse::Event::Left,x,y);
						ProcessMouseUp(mouse::Event::Left,x,y);
					}
					break;

				case WM_RBUTTONDOWN:
					{
						int x=lParam%0x10000;
						int y=lParam>>16;
						
						ProcessMouseDown(mouse::Event::Right,x,y);
					}
					break;
				case WM_RBUTTONUP:
					{
						int x=lParam%0x10000;
						int y=lParam>>16;
						
						ProcessMouseClick(mouse::Event::Right,x,y);
						ProcessMouseUp(mouse::Event::Right,x,y);
					}
					break;

				case WM_MBUTTONDOWN:
					{
						int x=lParam%0x10000;
						int y=lParam>>16;
						
						ProcessMouseDown(mouse::Event::Middle,x,y);
					}
					break;
				case WM_MBUTTONUP:
					{
						int x=lParam%0x10000;
						int y=lParam>>16;
						
						ProcessMouseClick(mouse::Event::Middle,x,y);
						ProcessMouseUp(mouse::Event::Middle,x,y);
					}
					break;

				case WM_XBUTTONDOWN:
					{
						int x=lParam%0x10000;
						int y=lParam>>16;
						
						switch(GET_XBUTTON_WPARAM(wParam)) {
						case 1:
							ProcessMouseDown(mouse::Event::X1,x,y);
							break;
						case 2:
							ProcessMouseDown(mouse::Event::X2,x,y);
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
							ProcessMouseClick(mouse::Event::X1,x,y);
							ProcessMouseUp(mouse::Event::X1,x,y);
							break;
						case 2:
							ProcessMouseClick(mouse::Event::X2,x,y);
							ProcessMouseUp(mouse::Event::X2,x,y);
							break;
						}
					}
					break;

				case WM_MOUSEWHEEL:
					{
						int x=lParam%0x10000;
						int y=lParam>>16;
						
						ProcessVScroll(GET_WHEEL_DELTA_WPARAM(wParam)/120,cursorlocation.x,cursorlocation.y);
					}
					break;
				case WM_MOUSEHWHEEL:
					{
						int x=lParam%0x10000;
						int y=lParam>>16;
						
						ProcessHScroll(GET_WHEEL_DELTA_WPARAM(wParam)/120,cursorlocation.x,cursorlocation.y);
					}
					break;

				case WM_KEYDOWN:
					if(lParam&1<<24) //Ctrl & Alt
						keyboard::Modifier::isAlternate=true;
					else
						keyboard::Modifier::isAlternate=false;

					if(lParam==0x360001) //Shift
						keyboard::Modifier::isAlternate=true;

					if(wParam==VK_RWIN) { //Win
						keyboard::Modifier::isAlternate=true;
						wParam=VK_LWIN;
					}

					ProcessKeyDown(wParam);
					keyboard::Modifier::isAlternate=false;

					switch(wParam) {
					case VK_CONTROL:
						keyboard::Modifier::Add(keyboard::Modifier::Ctrl);
						break;
					case VK_SHIFT:
						keyboard::Modifier::Add(keyboard::Modifier::Shift);
						break;
					case VK_LWIN:
						keyboard::Modifier::Add(keyboard::Modifier::Super);
						break;
					case VK_RWIN:
						keyboard::Modifier::Add(keyboard::Modifier::Super);
						break;
					}
						
					break; 
				case WM_KEYUP:
					if(lParam&1<<24)
						keyboard::Modifier::isAlternate=true;
					else
						keyboard::Modifier::isAlternate=false;

					if(lParam==0x360001)
						keyboard::Modifier::isAlternate=true;

					if(wParam==VK_RWIN) {
						keyboard::Modifier::isAlternate=true;
						wParam=VK_LWIN;
					}

					ProcessKeyUp(wParam);
					keyboard::Modifier::isAlternate=false;

					switch(wParam) {
					case VK_CONTROL:
						keyboard::Modifier::Remove(keyboard::Modifier::Ctrl);
						break;
					case VK_SHIFT:
						keyboard::Modifier::Remove(keyboard::Modifier::Shift);
						break;
					case VK_MENU:
						keyboard::Modifier::Remove(keyboard::Modifier::Alt);
						break;
					case VK_LWIN:
						keyboard::Modifier::Remove(keyboard::Modifier::Super);
						break;
					case VK_RWIN:
						keyboard::Modifier::Remove(keyboard::Modifier::Super);
						break;
					}
					
					break; 
				case WM_CHAR:
					if(lParam&1<<24)
						AddModifier(&KeyboardModifier,KEYB_MOD_ALTERNATIVE);
					else
						RemoveModifier(&KeyboardModifier,KEYB_MOD_ALTERNATIVE);

					ProcessChar(wParam);
					break; 
				case WM_SYSKEYDOWN:
					keyboard::Modifier::Add(keyboard::Modifier::Alt);
					break; 
				case WM_SYSKEYUP:
					keyboard::Modifier::Remove(keyboard::Modifier::Alt);
					break; 
				case WM_SYSCHAR:
					keyboard::Modifier::Add(keyboard::Modifier::Alt);
					ProcessChar(wParam);
					keyboard::Modifier::Remove(keyboard::Modifier::Alt);
					break;
				case WM_DESTROY:
					ReleaseAll();
					Destroyed();
					break;
				default:
					return DefWindowProc(hWnd, message, wParam, lParam);
				}
				return 0;
			}

			WindowHandle CreateWindow(std::string Name, std::string Title, os::IconHandle Icon, int Left, int Top, int Width, int Height, int BitDepth, bool Show, bool &FullScreen) {
				WNDCLASSEX windclass;

				HWND ret;

				//Creating window class
				windclass.cbClsExtra=0;
				windclass.cbSize=sizeof(WNDCLASSEX);
				windclass.cbWndExtra=0;
				windclass.hbrBackground=(HBRUSH)16;
				windclass.hCursor=LoadCursor(NULL, NULL);
				windclass.hInstance=Instance;
				windclass.lpfnWndProc=WndProc;
				windclass.lpszClassName=Name.c_str();
				windclass.lpszMenuName=NULL;
				windclass.hIcon=(HICON)Icon;
				windclass.hIconSm=(HICON)Icon;
				windclass.style=3;
				RegisterClassEx(&windclass);

				//Setting fullscreen parameters
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

				//Creating window
				if(!FullScreen)
					ret=CreateWindowExA(WS_EX_APPWINDOW | WS_EX_WINDOWEDGE, Name.c_str(), Title.c_str(), WS_MINIMIZEBOX | WS_SYSMENU | WS_CLIPSIBLINGS |WS_CLIPCHILDREN ,CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, (HINSTANCE)Instance, NULL);
				else																							   
					ret=CreateWindowA(Name.c_str(), Title.c_str(), WS_POPUP, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, (HINSTANCE)Instance, NULL);

				//Adjusting window size and position
				if(Show)
					::ShowWindow(ret,1);

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

				OleInitialize(NULL);
				RegisterDragDrop(ret, new system::GGEDropTarget());

				return (WindowHandle)ret;
			}


		IconHandle IconFromImage(graphics::ImageData &image) {
			DWORD dwWidth, dwHeight;
			BITMAPV5HEADER bi;
			HBITMAP hBitmap;
			void *lpBits;
			DWORD x,y;
			HICON hAlphaIcon = NULL;

			dwWidth  = image.GetWidth();  // width of cursor
			dwHeight = image.GetHeight();  // height of cursor

			ZeroMemory(&bi,sizeof(BITMAPV5HEADER));
			bi.bV5Size           = sizeof(BITMAPV5HEADER);
			bi.bV5Width           = dwWidth;
			bi.bV5Height          = dwHeight;
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
			HBITMAP hMonoBitmap = CreateBitmap(dwWidth,dwHeight,1,1,NULL);

			// Set the alpha values for each pixel in the cursor so that
			// the complete cursor is semi-transparent.
			DWORD *lpdwPixel;
			lpdwPixel = (DWORD *)lpBits;
			DWORD *source=(DWORD *)image.RawData();
			for (y=0;y<dwHeight;y++)
			{
				for (x=0;x<dwWidth;x++) {
					// Clear the alpha bits
					*lpdwPixel = source[x+(dwHeight-y-1)*dwWidth];

					lpdwPixel++;
				}
			}

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

			return (IconHandle)hAlphaIcon;
		}
		namespace input {
			utils::Point GetMousePosition(WindowHandle Window)
			{
				POINT pnt;
				RECT winrect;
				utils::Point ret;
				GetWindowRect((HWND)Window, &winrect);
				GetCursorPos(&pnt);

				ret.x=pnt.x-(window::overhead.x+winrect.left);
				ret.y=pnt.y-(window::overhead.y+winrect.top);

				os::window::cursorlocation=ret;

				return ret;
			}

		}


		namespace user {
		}


	} 

	namespace input {
		const keyboard::Key keyboard::KeyCodes::Shift		= VK_SHIFT;
		const keyboard::Key keyboard::KeyCodes::Control	= VK_CONTROL;
		const keyboard::Key keyboard::KeyCodes::Alt		= VK_MENU;
		const keyboard::Key keyboard::KeyCodes::Super		= VK_LWIN;

		const keyboard::Key keyboard::KeyCodes::Home		= VK_HOME;
		const keyboard::Key keyboard::KeyCodes::End		= VK_END;
		const keyboard::Key keyboard::KeyCodes::Insert	= VK_INSERT;
		const keyboard::Key keyboard::KeyCodes::Delete	= VK_DELETE;
		const keyboard::Key keyboard::KeyCodes::PageUp	= VK_PRIOR;
		const keyboard::Key keyboard::KeyCodes::PageDown	= VK_NEXT;

		const keyboard::Key keyboard::KeyCodes::Left		= VK_LEFT;
		const keyboard::Key keyboard::KeyCodes::Up		= VK_UP;
		const keyboard::Key keyboard::KeyCodes::Right		= VK_RIGHT;
		const keyboard::Key keyboard::KeyCodes::Down		= VK_DOWN;

		const keyboard::Key keyboard::KeyCodes::PrintScreen=VK_SNAPSHOT;
		const keyboard::Key keyboard::KeyCodes::Pause		= VK_PAUSE;

		const keyboard::Key keyboard::KeyCodes::CapsLock	= VK_CAPITAL;
		const keyboard::Key keyboard::KeyCodes::NumLock	= VK_NUMLOCK;

		const keyboard::Key keyboard::KeyCodes::Enter		= VK_RETURN;
		const keyboard::Key keyboard::KeyCodes::Backspace	= VK_BACK;
		const keyboard::Key keyboard::KeyCodes::Escape	= VK_ESCAPE;
		const keyboard::Key keyboard::KeyCodes::Tab		= VK_TAB;
		const keyboard::Key keyboard::KeyCodes::Space		= VK_SPACE;

		const keyboard::Key keyboard::KeyCodes::F1		= VK_F1;
		const keyboard::Key keyboard::KeyCodes::F2		= VK_F2;
		const keyboard::Key keyboard::KeyCodes::F3		= VK_F3;
		const keyboard::Key keyboard::KeyCodes::F4		= VK_F4;
		const keyboard::Key keyboard::KeyCodes::F5		= VK_F5;
		const keyboard::Key keyboard::KeyCodes::F6		= VK_F6;
		const keyboard::Key keyboard::KeyCodes::F7		= VK_F7;
		const keyboard::Key keyboard::KeyCodes::F8		= VK_F8;
		const keyboard::Key keyboard::KeyCodes::F9		= VK_F9;
		const keyboard::Key keyboard::KeyCodes::F10		= VK_F10;
		const keyboard::Key keyboard::KeyCodes::F11		= VK_F11;
		const keyboard::Key keyboard::KeyCodes::F12		= VK_F12;

	}

}


#endif
*/