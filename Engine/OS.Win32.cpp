#ifdef WIN32
#	pragma warning(disable: 4996)
#	include "OS.h"
#	include "input.h"
#	include "Multimedia.h"
#	include "..\Utils\Point2D.h"
#	include <stdio.h>


	using namespace gge::utils;
	using namespace gge::input;
	using namespace gge::input::system;

//#	define WINVER 0x0500
//#	define _WIN32_WINNT 0x0500
#	include <windows.h>
#	include <shlobj.h>
#	include <OleIdl.h>
#	include <sys\stat.h>
#	include <io.h>
#	include "Pointer.h"
#	include "Input.h"

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

#	undef CreateDirectory
#	undef DeleteFile

	//extern "C" {
	//	__declspec(dllimport) unsigned long __stdcall timeGetTime(void);
	//}

	HINSTANCE Instance;

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

		void DisplayMessage(const char *Title, const char *Text) {
			MessageBox(NULL,Text,Title,0);
		}
		void Quit(int ret) {
			PostQuitMessage(ret);
			CloseWindow((HWND)Main.getWindow());
			quiting=true;
			exit(ret);
		}
		void Initialize() {
			system::pointerdisplayed=true;
		}
		void Sleep(int ms) {
			::Sleep(ms);
		}
		unsigned int GetTime() { return timeGetTime(); }

		void RunInNewThread(int(threadfncall *fn)(void *), void *data) {
			DWORD threadid;
			CreateThread(NULL, 0, (unsigned long (__stdcall *)(void *))fn, data, 0, &threadid);
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
			utils::EventChain<> Activated	("WindowActivated" );
			utils::EventChain<> Deactivated ("WindowDectivated");
			utils::EventChain<> Destroyed	("WindowDestroyed" );
			utils::EventChain<Empty, bool&> Closing	("WindowClosing" );


		//TODO Private
			POINT overhead;
			HWND curwin;
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
				case WM_LBUTTONDBLCLK:
					{
						int x=lParam%0x10000;
						int y=lParam>>16;
						
						ProcessMouseDblClick(mouse::Event::Left,x,y);
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
				case WM_RBUTTONDBLCLK:
					{
						int x=(int)lParam%0x10000;
						int y=(int)lParam>>16;
						
						ProcessMouseDblClick(mouse::Event::Right,x,y);
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
				case WM_MBUTTONDBLCLK:
					{
						int x=lParam%0x10000;
						int y=lParam>>16;
						
						ProcessMouseDblClick(mouse::Event::Middle,x,y);
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
				case WM_XBUTTONDBLCLK:
					{
						int x=lParam%0x10000;
						int y=lParam>>16;
						
						switch(GET_XBUTTON_WPARAM(wParam)) {
						case 1:
							ProcessMouseDblClick(mouse::Event::X1,x,y);
							break;
						case 2:
							ProcessMouseDblClick(mouse::Event::X2,x,y);
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
					/*if(lParam&1<<24)
						AddModifier(&KeyboardModifier,KEYB_MOD_ALTERNATIVE);
					else
						RemoveModifier(&KeyboardModifier,KEYB_MOD_ALTERNATIVE);*/

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
					Destroyed();
					break;
				default:
					return DefWindowProc(hWnd, message, wParam, lParam);
				}
				return 0;
			}

			WindowHandle CreateWindow(string Name, string Title, os::IconHandle Icon, int Left, int Top, int Width, int Height, int BitDepth, bool &FullScreen) {
				WNDCLASSEX windclass;

				HWND ret;

				///*Creating window class
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

				OleInitialize(NULL);
				RegisterDragDrop(ret, new system::GGEDropTarget());

				return (WindowHandle)ret;
			}

			void MoveWindow(WindowHandle h, utils::Point p) {
				SetWindowPos((HWND)h, 0, p.x,p.y, 0,0, SWP_NOSIZE | SWP_NOZORDER);
			}

			void ResizeWindow(WindowHandle h,utils::Size size) {
				RECT cr,wr;
				GetClientRect((HWND)h,&cr);
				GetWindowRect((HWND)h,&wr);

				SetWindowPos((HWND)h, 0, 0,0,
					size.Width + ( (wr.right-wr.left) - (cr.right-cr.left) ),
					size.Height + ( (wr.bottom-wr.top) - (cr.bottom-cr.top) ), SWP_NOMOVE | SWP_NOZORDER);
			}

			utils::Rectangle UsableScreenMetrics(int Monitor) {
				utils::Rectangle r;

				RECT rect;

				SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);

				r.Left=rect.left;
				r.Top=rect.left;
				r.SetRight(rect.right);
				r.SetBottom(rect.bottom);

				return r;
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
		IconHandle IconFromResource(int ID) {
			return (gge::os::IconHandle)LoadIcon(Instance, (LPCTSTR)ID);
		}
		namespace input {
			utils::Point getMousePosition(WindowHandle Window)
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

		std::string GetClipbardText() {
			HANDLE clip;
			if (OpenClipboard(NULL)) 
				clip = GetClipboardData(CF_TEXT);

			return std::string((char*) clip);
		}

		void SetClipboardText(const std::string &text) {
			if(OpenClipboard(NULL))
			{
				HGLOBAL clipbuffer;
				char * buffer;
				EmptyClipboard();
				clipbuffer = GlobalAlloc(GMEM_DDESHARE, text.length()+1);
				buffer = (char*)GlobalLock(clipbuffer);
				strcpy_s(buffer, text.length()+1, text.c_str());
				GlobalUnlock(clipbuffer);
				SetClipboardData(CF_TEXT,clipbuffer);
				CloseClipboard();
			}
		}

		namespace user {
			std::string GetDocumentsPath() {
				CHAR my_documents[MAX_PATH];
				my_documents[0]=0;

				HRESULT result = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents);

				return my_documents;
			}

			std::string GetUsername() {
				CHAR username[256];
				username[0]=0;

				DWORD s=256;
				GetUserName(username, &s);

				return username;
			}
		}

		namespace filesystem {
			bool CreateDirectory(const std::string &name) {
				auto pos=name.length();

				pos=name.find_last_of("\\/",std::string::npos);

				if(!IsDirectoryExists(name.substr(0,pos)))
					CreateDirectory(name.substr(0,pos));

				CreateDirectoryA(name.c_str(), NULL);

				return IsDirectoryExists(name);
			}

			osdirenum::osdirenum() : search_handle(NULL), valid(false) {
				data=new WIN32_FIND_DATAA();
			}

			osdirenum::~osdirenum() {
				delete data;
			}

			DirectoryIterator::DirectoryIterator(const std::string &dir, const std::string &pattern/* ="*" */) {
				std::string src=dir;
				if(src[src.length()-1]!='\\') src+="\\";
				src+=pattern;

				dirinfo.search_handle=FindFirstFileA(src.c_str(), dirinfo.data);

				if(dirinfo.search_handle != INVALID_HANDLE_VALUE) {
					dirinfo.valid=true;
					current=dirinfo.data->cFileName;
				}
			}

			DirectoryIterator::DirectoryIterator(const DirectoryIterator &it ) {
				dirinfo.search_handle=it.dirinfo.search_handle;
				memcpy(dirinfo.data, it.dirinfo.data, sizeof WIN32_FIND_DATAA);
				dirinfo.valid=true;
				current=it.current;
			}

			DirectoryIterator::DirectoryIterator() {
				dirinfo.search_handle=NULL;
				dirinfo.valid=false;
				current="";
			}

			void DirectoryIterator::Next() {
				if(!dirinfo.valid) return;

				if (FindNextFileA (dirinfo.search_handle, dirinfo.data) == FALSE) {
					dirinfo.valid=false;
					FindClose (dirinfo.search_handle);
					dirinfo.search_handle = INVALID_HANDLE_VALUE;

					current="";
				}
				else {
					current=dirinfo.data->cFileName;
				}
			}

			bool DirectoryIterator::IsValid() const {
				return dirinfo.valid;
			}

			bool IsDirectoryExists(const std::string &Filename) {
				if ( _access(Filename.c_str(), 0) )
					return false;

				struct stat status;

				stat( Filename.c_str(), &status );

				if(status.st_mode & S_IFDIR)
					return true;
				else
					return false;
			}

			bool IsFileExists(const std::string &Filename) {
				if ( _access(Filename.c_str(), 0) )
					return false;

				struct stat status;

				stat( Filename.c_str(), &status );

				if(status.st_mode & S_IFREG)
					return true;
				else
					return false;
			}

			void DeleteFile(const std::string &Filename) {
				remove(Filename.c_str());
			}
		}

		std::string GetAppDataPath() {
			CHAR my_documents[MAX_PATH];
			my_documents[0]=0;

			HRESULT result = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents);

			return my_documents;
		}
	} 

	namespace input {
		const int keyboard::KeyCodes::Shift		= VK_SHIFT;
		const int keyboard::KeyCodes::Control	= VK_CONTROL;
		const int keyboard::KeyCodes::Alt		= VK_MENU;
		const int keyboard::KeyCodes::Super		= VK_LWIN;

		const int keyboard::KeyCodes::Home		= VK_HOME;
		const int keyboard::KeyCodes::End		= VK_END;
		const int keyboard::KeyCodes::Insert	= VK_INSERT;
		const int keyboard::KeyCodes::Delete	= VK_DELETE;
		const int keyboard::KeyCodes::PageUp	= VK_PRIOR;
		const int keyboard::KeyCodes::PageDown	= VK_NEXT;

		const int keyboard::KeyCodes::Left		= VK_LEFT;
		const int keyboard::KeyCodes::Up		= VK_UP;
		const int keyboard::KeyCodes::Right		= VK_RIGHT;
		const int keyboard::KeyCodes::Down		= VK_DOWN;

		const int keyboard::KeyCodes::PrintScreen=VK_SNAPSHOT;
		const int keyboard::KeyCodes::Pause		= VK_PAUSE;

		const int keyboard::KeyCodes::CapsLock	= VK_CAPITAL;
		const int keyboard::KeyCodes::NumLock	= VK_NUMLOCK;

		const int keyboard::KeyCodes::Enter		= VK_RETURN;
		const int keyboard::KeyCodes::Backspace	= VK_BACK;
		const int keyboard::KeyCodes::Escape	= VK_ESCAPE;
		const int keyboard::KeyCodes::Tab		= VK_TAB;
		const int keyboard::KeyCodes::Space		= VK_SPACE;

		const int keyboard::KeyCodes::F1		= VK_F1;
		const int keyboard::KeyCodes::F2		= VK_F2;
		const int keyboard::KeyCodes::F3		= VK_F3;
		const int keyboard::KeyCodes::F4		= VK_F4;
		const int keyboard::KeyCodes::F5		= VK_F5;
		const int keyboard::KeyCodes::F6		= VK_F6;
		const int keyboard::KeyCodes::F7		= VK_F7;
		const int keyboard::KeyCodes::F8		= VK_F8;
		const int keyboard::KeyCodes::F9		= VK_F9;
		const int keyboard::KeyCodes::F10		= VK_F10;
		const int keyboard::KeyCodes::F11		= VK_F11;
		const int keyboard::KeyCodes::F12		= VK_F12;

	}

}


#endif