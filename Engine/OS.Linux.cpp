#ifdef LINUX
#	pragma warning(disable: 4996)
#	include "OS.h"
#	include "Input.h"
#	include "../Utils/Point2D.h"
#	include <stdio.h>
#	include <pthread.h>
#	define XK_MISCELLANY

	using namespace gge::utils;
	using namespace gge::input;
	using namespace gge::input::system;

#	include <sys/stat.h>
#	include <sys/types.h>
#	include <dirent.h>
#	include "Pointer.h"
#	include "Input.h"
#	include <X11/keysymdef.h>
#	include <X11/X.h>
#	include <X11/Xlib.h>
#	include <X11/Xutil.h>
#	include <X11/Xatom.h>
#	include <sys/time.h>
#	include <time.h>
#	include <stdlib.h>
#	include <unistd.h>
#	include <pwd.h>
#	include <regex>
#	undef None
static const int None=0;
	
	pthread_attr_t common_thread_attr;


	extern int Application(std::vector<std::string> &arguments);

	int main(int argc, char *argv[]) {
		std::vector<std::string> arguments;

		for(int i=0;i<argc;i++)
			arguments.push_back(argv[i]);

		return Application(arguments);
	}

	namespace gge { namespace os {
		Display * display=NULL;
		Window  windowhandle=0;
		Cursor emptycursor;
		Atom XA_CLIPBOARD;
		Atom XA_TIMESTAMP;
		Atom XA_TARGETS;
		std::string copiedtext;
		
		void DisplayMessage(const char *Title, const char *Text) {
			std::system( (std::string("xmessage -center \"")+Title+"\n"+Text+"\"").c_str() );
		}
		void Quit(int ret) {
			Cleanup();
			exit(0);
		}
		void Cleanup() {
			XDestroyWindow(display, windowhandle);
			windowhandle=0;
		}
		void Initialize() {
			system::pointerdisplayed=true;
			pthread_attr_init(&common_thread_attr);
			pthread_attr_setdetachstate(&common_thread_attr, PTHREAD_CREATE_DETACHED);
			display = XOpenDisplay(NULL); 
			emptycursor=None;
			XA_CLIPBOARD=XInternAtom(display, "CLIPBOARD", 1);
			XA_TIMESTAMP=XInternAtom(display, "TIMESTAMP", 1);
			XA_TARGETS = XInternAtom (display, "TARGETS", 0);
		}
		void Sleep(int ms) {
			usleep(ms*1000);
		}
		unsigned int GetTime() { 
			timeval tv;
			gettimeofday(&tv, 0 );
			return tv.tv_usec/1000+tv.tv_sec*1000;
		}

		void RunInNewThread(int(threadfncall *fn)(void *), void *data) {
			pthread_t threadid;
			pthread_create(&threadid, &common_thread_attr, (void*(*)(void *))fn, data);
		}

		namespace system {
			CursorHandle defaultcursor;
			bool pointerdisplayed;
			gge::input::mouse::Event::Type X11ButtonCodeToGGE(unsigned btn) {
				using namespace gge::input::mouse;
				switch(btn) {
				case 1:
					return Event::Left;
				case 2:
					return Event::Middle;
				case 3:
					return Event::Right;
				case 8:
					return Event::X1;
				case 9:
					return Event::X2;
				default:
					return Event::None;
				}
			}
			void ProcessMessage() {
				//TODO: modifier keys, window events
				
				XEvent event;
				
				while(XEventsQueued(display, QueuedAfterReading)) {
					XNextEvent(display, &event);
					unsigned key;
					switch(event.type) {
					case KeyPress:
						key=XLookupKeysym(&event.xkey,0);
						ProcessKeyDown(key);
						ProcessChar(key);
						break;
					case KeyRelease:
						key=XLookupKeysym(&event.xkey,0);
						
						if(XEventsQueued(display, QueuedAfterReading)) {
							XEvent nextevent;
							XPeekEvent(display, &nextevent);
							if(nextevent.type == KeyPress && nextevent.xkey.time == event.xkey.time && 
								nextevent.xkey.keycode == event.xkey.keycode) {
								ProcessChar(key);
								XNextEvent(display, &nextevent);
							}
							else {
								ProcessKeyUp(key);
							}
						}
						else {
							ProcessKeyUp(key);
						}
						
						break;
					case ButtonPress:
						if(event.xbutton.button==4) {
							ProcessVScroll(1, event.xbutton.x, event.xbutton.y);
						}
						else if(event.xbutton.button==5) {
							ProcessVScroll(-1, event.xbutton.x, event.xbutton.y);
						}
						else {
							ProcessMouseDown(X11ButtonCodeToGGE(event.xbutton.button), event.xbutton.x, event.xbutton.y);
						}
						break;
					case ButtonRelease:
						if(event.xbutton.button!=4 && event.xbutton.button!=5) {
							ProcessMouseClick(X11ButtonCodeToGGE(event.xbutton.button), event.xbutton.x, event.xbutton.y);
							ProcessMouseUp(X11ButtonCodeToGGE(event.xbutton.button), event.xbutton.x, event.xbutton.y);
						}
						break;
					case SelectionRequest:
						XEvent respond;
						
						if(event.xselectionrequest.target==XA_STRING && event.xselectionrequest.selection==XA_CLIPBOARD) {
						
							XChangeProperty (display,
								event.xselectionrequest.requestor,
								event.xselectionrequest.property,
								XA_STRING,
								8,
								PropModeReplace,
								(unsigned char*) &copiedtext[0],
								(int)copiedtext.length());
							respond.xselection.property=event.xselectionrequest.property;
						}
						else if(event.xselectionrequest.target==XA_TARGETS && event.xselectionrequest.selection==XA_CLIPBOARD) {
							Atom supported[]={XA_STRING};
							XChangeProperty (display,
								event.xselectionrequest.requestor,
								event.xselectionrequest.property,
								XA_TARGETS,
								8,
								PropModeReplace,
								(unsigned char *)(&supported),
								sizeof(supported)
							);
						}
						else {
							respond.xselection.property= None;
						}
						
						respond.xselection.type= SelectionNotify;
						respond.xselection.display= event.xselectionrequest.display;
						respond.xselection.requestor= event.xselectionrequest.requestor;
						respond.xselection.selection=event.xselectionrequest.selection;
						respond.xselection.target= event.xselectionrequest.target;
						respond.xselection.time = event.xselectionrequest.time;
						XSendEvent (display, event.xselectionrequest.requestor,0,0,&respond);
						XFlush (display);
						break;

					default:
						std::cout<<event.type<<std::endl;
					}
				}
				
			}
		}
		namespace window {
			utils::EventChain<> Activated	("WindowActivated" );
			utils::EventChain<> Deactivated ("WindowDectivated");
			utils::EventChain<> Destroyed	("WindowDestroyed" );
			utils::EventChain<Empty, bool&> Closing	("WindowClosing" );


		//TODO Private
			Point cursorlocation=Point(0,0);
			
			static int WaitForMapNotify(Display *d, XEvent *e, char *arg) {
				return (e->type == MapNotify) && (e->xmap.window == (Window)arg);
			}
			
			static int WaitForSelectionNotify(Display *d, XEvent *e, char *arg) {
				return (e->type == SelectionNotify);
			}

			WindowHandle CreateWindow(string Name, string Title, os::IconHandle Icon, int Left, int Top, int Width, int Height, int BitDepth, bool &FullScreen) {
				//TODO: handle icon
				XSetWindowAttributes attributes; 
				Visual *visual; 
				int screen;
				int depth; 
				XEvent event;
				
				screen = DefaultScreen(display);
				visual = DefaultVisual(display,screen); 
				depth  = DefaultDepth(display,screen); 
				attributes.background_pixel      = XWhitePixel(display,screen); 
				attributes.border_pixel          = XBlackPixel(display,screen); 
				attributes.event_mask = 
					StructureNotifyMask | 
					KeyPressMask |
					KeyReleaseMask |
					ButtonPressMask |
					ButtonReleaseMask
				;
				Colormap cmap = XCreateColormap(display, RootWindow(display, screen), visual, AllocNone);
				attributes.colormap = cmap;
				windowhandle= XCreateWindow(display, XRootWindow(display,screen),
                   Left,Top, Width,Height,0, depth, InputOutput, visual,
                   CWBorderPixel | CWEventMask | CWColormap,&attributes);	
				
				char data[1]={0};
				XColor dummy;
				Pixmap blank = XCreateBitmapFromData (display, windowhandle, data, 1, 1);
				emptycursor = XCreatePixmapCursor(display, blank, blank, &dummy, &dummy, 0, 0);
				XFreePixmap (display, blank);
				
				XClassHint *classhint=XAllocClassHint();
				classhint->res_name=(char*)malloc(Name.length());
				strcpy(classhint->res_name, Name.c_str());
				classhint->res_class=(char*)malloc(Name.length());
				strcpy(classhint->res_class, Name.c_str());
				XSetClassHint(display, windowhandle, classhint);
				XFree(classhint);
				
				XStoreName(display, windowhandle, (char*)Title.c_str());
				
				XSizeHints *sizehints=XAllocSizeHints();
				sizehints->min_width=Width;
				sizehints->max_width=Width;
				sizehints->min_height=Height;
				sizehints->max_height=Height;
				sizehints->flags=PMinSize | PMaxSize;
				XSetWMNormalHints(display, windowhandle, sizehints);		
				XFree(sizehints);

				
				XMapWindow(display,windowhandle);
				XIfEvent(display, &event, WaitForMapNotify, (char*)windowhandle);
		
				return (WindowHandle)windowhandle;
			}

			void MoveWindow(WindowHandle h, utils::Point p) {
				XMoveWindow(display, h, p.x, p.y);
				XFlush(display);
			}

			void ResizeWindow(WindowHandle h,utils::Size size) {
				XSizeHints *sizehints=XAllocSizeHints();
				sizehints->min_width=size.Width;
				sizehints->max_width=size.Width;
				sizehints->min_height=size.Height;
				sizehints->max_height=size.Height;
				sizehints->flags=PMinSize | PMaxSize;
				XSetWMNormalHints(display, windowhandle, sizehints);		
				XFree(sizehints);

				XResizeWindow(display, h, size.Width, size.Height);
				XFlush(display);
			}

			utils::Rectangle UsableScreenMetrics(int Monitor) {
				utils::Rectangle r(0,0,1000,1000);

				Screen  *screen  = XScreenOfDisplay(display, Monitor);
				r.Width =XWidthOfScreen(screen);
				r.Height=XHeightOfScreen(screen);

				return r;
			}

		}
	
		void ShowPointer() {
			if(!system::pointerdisplayed) {
				system::pointerdisplayed=true;
				XDefineCursor(display,windowhandle,None);
			}
		}
		void HidePointer() {
			if(system::pointerdisplayed) {
				system::pointerdisplayed=false;
				XDefineCursor(display, windowhandle, emptycursor);
			}
		}
		IconHandle IconFromResource(int ID) {
			//TODO: will be replaced
			return (gge::os::IconHandle)0;
		}
		namespace input {
			utils::Point GetMousePosition(WindowHandle Window)
			{
				utils::Point ret;
				
				int rx,ry;
				::Window w;
				unsigned mask;
				
				XQueryPointer(display, windowhandle, &w,
                &w, &rx, &ry, &ret.x, &ret.y,
                &mask);

				return ret;
			}

		}

		int CurrentHour() {
			time_t rawtime;
			struct tm * timeinfo;

			time ( &rawtime );
			timeinfo = localtime ( &rawtime );	
			
			return timeinfo->tm_hour;
		}
		int CurrentMinute() {
			time_t rawtime;
			struct tm * timeinfo;

			time ( &rawtime );
			timeinfo = localtime ( &rawtime );	
			
			return timeinfo->tm_min;
		}
		int CurrentSecond() {
			time_t rawtime;
			struct tm * timeinfo;

			time ( &rawtime );
			timeinfo = localtime ( &rawtime );	
			
			return timeinfo->tm_sec;
		}
		int CurrentMillisecond() {
			struct timeval  tv;
			gettimeofday(&tv, NULL);
			
			return tv.tv_usec / 1000;
		}
		int CurrentWeekday() {
			time_t rawtime;
			struct tm * timeinfo;

			time ( &rawtime );
			timeinfo = localtime ( &rawtime );	
			
			return (timeinfo->tm_wday+1)%7;
		}
		int CurrentDay() {
			time_t rawtime;
			struct tm * timeinfo;

			time ( &rawtime );
			timeinfo = localtime ( &rawtime );	
			
			return timeinfo->tm_mday;
		}
		int CurrentMonth() {
			time_t rawtime;
			struct tm * timeinfo;

			time ( &rawtime );
			timeinfo = localtime ( &rawtime );	
			
			return timeinfo->tm_mon;
		}
		int CurrentYear() {
			time_t rawtime;
			struct tm * timeinfo;

			time ( &rawtime );
			timeinfo = localtime ( &rawtime );	
			
			return timeinfo->tm_year+1900;
		}


		std::string GetClipboardText() {
			Window owner=XGetSelectionOwner(display, XA_CLIPBOARD);
			if(!owner)
				return "";
			
			XEvent event;
			
			XConvertSelection (display, XA_CLIPBOARD, XA_STRING, None, windowhandle, CurrentTime);
			XFlush(display);
			
			XIfEvent(display, &event, window::WaitForSelectionNotify, (char*)windowhandle);
			if (event.xselection.property == XA_STRING) {
				Atom type;
				unsigned long len, bytes, dummy;
				unsigned char *data;
				int format;

				XGetWindowProperty(display, windowhandle, XA_STRING, 0, 0, 0, AnyPropertyType, &type, &format, &len, &bytes, &data);
				
				if(bytes) {
					XGetWindowProperty (display, windowhandle, 
							XA_STRING, 0,bytes,0,
							AnyPropertyType, &type, &format,
							&len, &dummy, &data);
					
					std::string tmp((char*)data, bytes);
					XFree(data);

					return tmp;
				}
				
			}
			
			return "";
		}

		void SetClipboardText(const std::string &text) {
			XSetSelectionOwner (display, XA_CLIPBOARD, windowhandle, CurrentTime);
			copiedtext=text;
			XFlush(display);
		}

		namespace user {
			std::string GetDocumentsPath() {
				return getenv("HOME");
			}

			std::string GetUsername() {
				struct passwd *p=getpwnam(getlogin());
				
				int n = strcspn(p->pw_gecos, ",");

				return std::string(p->pw_gecos, n);
			}
		}

		namespace filesystem {
			bool CreateDirectory(const std::string &name) {
				auto pos=name.length();

				pos=name.find_last_of("\\/",std::string::npos);

				if(!IsDirectoryExists(name.substr(0,pos)))
					CreateDirectory(name.substr(0,pos));

				mkdir(name.c_str(), 0755);

				return IsDirectoryExists(name);

				return true;
			}

			osdirenum::osdirenum() : dp() {
			}
			
			osdirenum::~osdirenum() {
				if(dp)
					closedir((DIR*)dp);
			}
			
			std::string escape_regexp(const std::string &str) {
				static const char shouldbeescaped[]="\\/().[],?*+-|";
				std::string ret=str;
				int pos=0;
				while( (pos=ret.find_first_of(shouldbeescaped,pos)) != std::string::npos) {
					ret=ret.insert(pos, "\\");
					pos+=2;
				}
				
				return ret;
			}

			DirectoryIterator::DirectoryIterator(const std::string &dir, const std::string &pattern/* ="*" */) {
				dirinfo.dp  	 = opendir(dir.c_str());
				if(pattern!="*") {
					dirinfo.pattern = escape_regexp(pattern);
					int pos=0;
					while( (pos=dirinfo.pattern.find_first_of("*?", pos)) != std::string::npos) {
						dirinfo.pattern[pos-1]='.';
						if(dirinfo.pattern[pos]=='?') {
							dirinfo.pattern.erase(pos, pos+1);
							pos--;
						}
						pos++;
					}
				}
				Next();
			}

			DirectoryIterator::DirectoryIterator(const DirectoryIterator &it ) {
				dirinfo.dp=it.dirinfo.dp;
				dirinfo.pattern=it.dirinfo.pattern;
				current=it.current;
			}

			DirectoryIterator::DirectoryIterator() {
			}

			void DirectoryIterator::Next() {
				struct dirent *ent;
				ent = readdir((DIR*)dirinfo.dp);
				if(ent==NULL)
					current="";
				else
					current=ent->d_name;
				
				if(current=="." || current=="..")
					Next();
				
				if(dirinfo.pattern!="" && current!="") {
					std::regex r(dirinfo.pattern);
					if(!std::regex_match(current.begin(), current.end(), r))
						Next();
				}
			}

			bool DirectoryIterator::IsValid() const {
				return current!="";
			}

			bool IsDirectoryExists(const std::string &Filename) {
				if ( access(Filename.c_str(), 0) )
					return false;

				struct stat status;

				stat( Filename.c_str(), &status );

				if(status.st_mode & S_IFDIR)
					return true;
				else
					return false;
			}

			bool IsFileExists(const std::string &Filename) {
				if ( access(Filename.c_str(), 0) )
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
			
			DirectoryIterator EndOfDirectory;
		}

		std::string GetAppDataPath() {
			return getenv("HOME");
		}
	} 

	namespace input {
		const int keyboard::KeyCodes::Shift	= XK_Shift_L;
		const int keyboard::KeyCodes::Control	= XK_Control_L;
		const int keyboard::KeyCodes::Alt	= XK_Alt_L;
		const int keyboard::KeyCodes::Super	= XK_Super_L;

		const int keyboard::KeyCodes::Home	= XK_Home;
		const int keyboard::KeyCodes::End	= XK_End;
		const int keyboard::KeyCodes::Insert	= XK_Insert;
		const int keyboard::KeyCodes::Delete	= XK_Delete;
		const int keyboard::KeyCodes::PageUp	= XK_Page_Up;
		const int keyboard::KeyCodes::PageDown	= XK_Page_Down;

		const int keyboard::KeyCodes::Left	= XK_Left;
		const int keyboard::KeyCodes::Up	= XK_Up;
		const int keyboard::KeyCodes::Right	= XK_Right;
		const int keyboard::KeyCodes::Down	= XK_Down;

		const int keyboard::KeyCodes::PrintScreen=XK_Print;
		const int keyboard::KeyCodes::Pause	= XK_Pause;

		const int keyboard::KeyCodes::CapsLock	= XK_Caps_Lock;
		const int keyboard::KeyCodes::NumLock	= XK_Num_Lock;

		const int keyboard::KeyCodes::Enter	= XK_Return;
		const int keyboard::KeyCodes::Backspace	= XK_BackSpace;
		const int keyboard::KeyCodes::Escape	= XK_Escape;
		const int keyboard::KeyCodes::Tab	= XK_Tab;
		const int keyboard::KeyCodes::Space	= 0x20;

		const int keyboard::KeyCodes::F1	= XK_F1;
		const int keyboard::KeyCodes::F2	= XK_F2;
		const int keyboard::KeyCodes::F3	= XK_F3;
		const int keyboard::KeyCodes::F4	= XK_F4;
		const int keyboard::KeyCodes::F5	= XK_F5;
		const int keyboard::KeyCodes::F6	= XK_F6;
		const int keyboard::KeyCodes::F7	= XK_F7;
		const int keyboard::KeyCodes::F8	= XK_F8;
		const int keyboard::KeyCodes::F9	= XK_F9;
		const int keyboard::KeyCodes::F10	= XK_F10;
		const int keyboard::KeyCodes::F11	= XK_F11;
		const int keyboard::KeyCodes::F12	= XK_F12;

	}

}


#endif