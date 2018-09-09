#include "../OS.h"
#include "../Filesystem.h"
#include "../Window.h"

#include <string.h>
#include <fstream>

#include <sys/utsname.h>
#include <unistd.h>
#include <fcntl.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>

namespace Gorgon { namespace OS {

	std::string GetEnvVar(const std::string &var) {
		auto ret=getenv(var.c_str());
		if(!ret)
			return "";
		else
			return ret;
	}
		
	namespace User {
		
		std::string GetUsername() {
			struct passwd *p=getpwnam(getlogin());
			
			return p->pw_name;
		}

		std::string GetName() {
			struct passwd *p=getpwnam(getlogin());
			
			int n = strcspn(p->pw_gecos, ",");

			return std::string(p->pw_gecos, n);
		}

		std::string GetDocumentsPath() {
			std::string s=GetEnvVar("HOME");
			if(Filesystem::IsDirectory(s+"/Documents"))
				return s+"/Documents";
			else
				return s;
		}

		std::string GetHomePath() {
			return GetEnvVar("HOME");
		}

		std::string GetDataPath() {
			return GetEnvVar("HOME");
		}
		
		bool IsAdmin() {
			return getuid() == 0;
		}
	}
	
	void Initialize() {
	}

	std::string GetName() {
		
		FILE *p = popen("lsb_release -d", "r");
		
		if(p!=nullptr) {
			int len;
			std::string line;
			char buf[16];
			
			while( (len = fread(buf, 1, 16, p)) > 0 ) {
				line.insert(line.length(), buf, len);
			}
			
			if(line!="") {			
				String::Extract(line, ':');
				
				return String::TrimStart(line);
			}
		}
		
		std::ifstream issuefile("/etc/issue");
		
		if(issuefile.is_open()) {
			std::string line;
			std::getline(issuefile, line);
			
			if(line.find_first_of('\\') != line.npos) {
				return String::Extract(line, '\\');
			}
			else 
				return line;
		}
		
		return "Linux";
	}
	
	void OpenTerminal() {		
	}

	void DisplayMessage(const std::string &message) {
		std::system( std::string("xmessage -center \""+message+"\"").c_str() );	
	}

	std::string GetAppDataPath() {
		return "/usr/share";
	}

	std::string GetAppSettingPath() {
		return "/etc";
	}
	
	bool Start(const std::string &name, const std::vector<std::string> &args) {
		int execpipe[2];
		if(pipe(execpipe)) {
			return false;
		}

		if(fcntl(execpipe[1], F_SETFD, fcntl(execpipe[1], F_GETFD) | FD_CLOEXEC)==-1) {
			close(execpipe[0]);
			close(execpipe[1]);

			return false;
		}
		
		int f=fork();
		
		if(f==-1) {
			close(execpipe[0]);
			close(execpipe[1]);

			return false;
		}
		
		if(f==0) {
			close(execpipe[0]);

			//build args
			char *v[args.size()+2];
			int arg=1;
			v[0]=(char*)malloc(name.length()+1);
			strcpy(v[0], Filesystem::GetFilename(name).c_str());

			for(auto &s : args) {
				v[arg] = (char*)malloc(s.length()+1);
				strcpy(v[arg], s.c_str());
				arg++;
			}
			v[args.size()+1]=nullptr;

			//if path is given, from current directory
			if(name.find_first_of('/')!=name.npos) {
				execv(name.c_str(), v);
			}
			else {
				execvp(name.c_str(), v);
			}

			//only arrives here if there is an error
			write(execpipe[1], &errno, sizeof(errno));
			close(execpipe[1]);
			exit(-1);
		}
		else {
			close(execpipe[1]);
			int childErrno;

			//check if execution is successful
			if(read(execpipe[0], &childErrno, sizeof(childErrno)) == sizeof(childErrno)) {
				close(execpipe[0]);

				return false;
			}
			close(execpipe[0]);
			
			return true;
		}
	}
	
	#include <ext/stdio_filebuf.h>
	
	/// This variant of start enables reading output of the application, buf is returned,
	/// ownership lies with the caller of the function
	bool Start(const std::string &name, std::streambuf *&buf, const std::vector<std::string> &args) {
        buf = nullptr;
        
		int execpipe[2];
		if(pipe(execpipe)) {
			return false;
            
		}int outpipe[2];
		if(pipe(outpipe)) {
			return false;
		}

		if(fcntl(execpipe[1], F_SETFD, fcntl(execpipe[1], F_GETFD) | FD_CLOEXEC) == -1) {
			close(execpipe[0]);
			close(execpipe[1]);

			return false;
		}
		
		int f=fork();
		
		if(f==-1) {
			close(execpipe[0]);
			close(execpipe[1]);
			close(outpipe[0]);
			close(outpipe[1]);

			return false;
		}
		
		if(f==0) {
			close(execpipe[0]);
			close(outpipe[0]);
            dup2(outpipe[1], 1);

			//build args
			char *v[args.size()+2];
			int arg=1;
			v[0]=(char*)malloc(name.length()+1);
			strcpy(v[0], Filesystem::GetFilename(name).c_str());

			for(auto &s : args) {
				v[arg] = (char*)malloc(s.length()+1);
				strcpy(v[arg], s.c_str());
				arg++;
			}
			v[args.size()+1]=nullptr;

			//if path is given, from current directory
			if(name.find_first_of('/')!=name.npos) {
				execv(name.c_str(), v);
			}
			else {
				execvp(name.c_str(), v);
			}

			//only arrives here if there is an error
			write(execpipe[1], &errno, sizeof(errno));
			close(execpipe[1]);
			exit(-1);
		}
		else {
			close(execpipe[1]);
			close(outpipe[1]);
			int childErrno;

			//check if execution is successful
			if(read(execpipe[0], &childErrno, sizeof(childErrno)) == sizeof(childErrno)) {
				close(execpipe[0]);
                close(outpipe[0]);

				return false;
			}
			close(execpipe[0]);
            
            buf = new __gnu_cxx::stdio_filebuf<char>(outpipe[0], std::ios::in);
			
			return true;
		}
	}
	
	bool Open(const std::string &file) {
		if(!Start("xdg-open", {file})) {
			if(GetEnvVar("XDG_CURRENT_DESKTOP")=="KDE") {
				return Start("kde-open", {file});
			}
			else {
				return Start("gnome-open", {file});
			}
		}
		
		return true;
	}

	void processmessages() {
		for(auto &w : Window::Windows) {
			w.processmessages();
		}
	}

} }



#ifdef LaINUX

static const int None=0;
	

	extern int Application(std::vector<std::string> &arguments);

	int main(int argc, char *argv[]) {
		std::vector<std::string> arguments;

		for(int i=0;i<argc;i++)
			arguments.push_back(argv[i]);

		return Application(arguments);
	}

	namespace gge { 
		namespace os {
		Display * display=NULL;
		Window  windowhandle=0;
		Visual *visual;
		Cursor emptycursor;
		Atom XA_CLIPBOARD;
		Atom XA_TIMESTAMP;
		Atom XA_TARGETS;
		Atom XA_PROTOCOLS;
		Atom WM_DELETE_WINDOW; 
		std::string copiedtext;
		
		void Quit(int ret) {
			Cleanup();
			exit(0);
		}
		void Cleanup() {
			XDestroyWindow(display, windowhandle);
			windowhandle=0;
		}


		namespace system {
			CursorHandle defaultcursor;
			bool pointerdisplayed;
			InputLayer *inlayer;
			gge::input::mouse::Event::Target *dragobject;
			

			//BEGIN XDND
		
			Atom XdndAware;
			Atom XdndSelection;
			Atom XdndEnter;
			Atom XdndFinished;
			Atom XdndStatus;
			Atom XdndPosition;
			Atom XdndLeave;
			Atom XdndDrop;
			Atom XdndActionCopy;
			Atom XdndActionMove;
			Atom XdndTypeList;
			Atom XA_Filelist;
			
			void finishdrag() {
				if(dynamic_cast<gge::input::mouse::FileListDragData*>(&gge::input::mouse::GetDraggedObject())) {
					auto &data=dynamic_cast<gge::input::mouse::FileListDragData&>(gge::input::mouse::GetDraggedObject());

					delete &data;
				}
			}

			void XdndInit()
			{
				if (XdndAware)
					return;

				XdndAware         = XInternAtom(display, "XdndAware",         0);
				XdndSelection     = XInternAtom(display, "XdndSelection",     0);
				XdndStatus			= XInternAtom(display, "XdndStatus",     	  0);
				XdndTypeList		= XInternAtom(display, "XdndTypeList",      0);
				XdndEnter         = XInternAtom(display, "XdndEnter",         0);
				XdndFinished      = XInternAtom(display, "XdndFinished",      0);
				XdndPosition      = XInternAtom(display, "XdndPosition",      0);
				XdndLeave         = XInternAtom(display, "XdndLeave",         0);
				XdndDrop          = XInternAtom(display, "XdndDrop",          0);
				XdndActionCopy    = XInternAtom(display, "XdndActionCopy",    0);
				XdndActionMove    = XInternAtom(display, "XdndActionMove",    0);
				XA_Filelist			= XInternAtom(display, "text/uri-list",	  0);
				
				inlayer=new InputLayer();
				dragobject=&inlayer->MouseEvents.RegisterLambda([&]() -> bool {
					finishdrag();
					return true;
				}, utils::Bounds2D(0,0,10000,10000), gge::input::mouse::Event::DragCanceled | gge::input::mouse::Event::DragAccepted);
				
				int version=4;
				
				XChangeProperty(display, windowhandle, XdndAware, XA_ATOM, 32, PropModeReplace, (unsigned char *)&version, 1);
			}


		
			//END

			
			
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
			std::string GetAtomName(Atom a)
			{
				if(a == (int)None)
					return "None";
				else
					return XGetAtomName(display, a);
			}

			void ProcessMessage() {
				XEvent event;
				
				while(XEventsQueued(display, QueuedAfterReading)) {
					XNextEvent(display, &event);
					unsigned key;
					switch(event.type) {
					case ClientMessage:
						
						else if(event.xclient.message_type==XdndEnter) {
							unsigned long len, bytes, dummy;
							unsigned char *data=NULL;
							Atom type;
							int format;
								
							XGetWindowProperty(display, event.xclient.data.l[0], XdndTypeList, 0, 0, 0, AnyPropertyType, &type, &format, &len, &bytes, &data);
							XGetWindowProperty (display, event.xclient.data.l[0], 
												XdndTypeList, 0,bytes,0,
												AnyPropertyType, &type, &format,
												&len, &dummy, &data);
							Atom*atoms=(Atom*)data;
							for(int i=0;i<(int)bytes/4;i++) {
								if(atoms[i]==XA_Filelist) {
									gge::Pointers.Hide();
									auto dragdata=new gge::input::mouse::FileListDragData();
									gge::input::mouse::BeginDrag(*dragdata, *dragobject);
									return;
								}
								else if(atoms[i]==XA_STRING) {
									gge::Pointers.Hide();
									auto dragdata=new gge::input::mouse::TextDragData();
									gge::input::mouse::BeginDrag(*dragdata, *dragobject);
									return;
								}
							}
						}
						else if(event.xclient.message_type==XdndPosition) {
							XClientMessageEvent m;
							memset(&m, sizeof(m), 0);
							m.type = ClientMessage;
							m.display = event.xclient.display;
							m.window = event.xclient.data.l[0];
							m.message_type = XdndStatus;
							m.format=32;
							m.data.l[0] = windowhandle;
							m.data.l[1] = 1;//gge::input::mouse::HasDragTarget() && gge::input::mouse::IsDragging();
							m.data.l[2] = 0;
							m.data.l[3] = 0;
							
							if(gge::input::mouse::HasDragTarget() && dynamic_cast<gge::input::mouse::FileListDragData*>(&gge::input::mouse::GetDraggedObject())) {
								auto &data=dynamic_cast<gge::input::mouse::FileListDragData&>(gge::input::mouse::GetDraggedObject());
								if(data.GetAction()==data.Move)
									m.data.l[4] = XdndActionMove;
								else
									m.data.l[4] = XdndActionCopy;
							}
							else if(gge::input::mouse::HasDragTarget() && dynamic_cast<gge::input::mouse::TextDragData*>(&gge::input::mouse::GetDraggedObject())) {
								m.data.l[4] = XdndActionCopy;
							}
							else {
								m.data.l[4] = None;
							}

							XSendEvent(display, event.xclient.data.l[0], False, NoEventMask, (XEvent*)&m);
							XFlush(display);
							gge::Pointers.Hide();
						}
						else if(event.xclient.message_type == XdndLeave) {
							gge::input::mouse::CancelDrag();
							gge::Pointers.Show();
						}
						else if(event.xclient.message_type == XdndDrop) {
							if(0 && !gge::input::mouse::HasDragTarget()) {
								XClientMessageEvent m;
								memset(&m, sizeof(m), 0);
								m.type = ClientMessage;
								m.display = event.xclient.display;
								m.window = event.xclient.data.l[0];
								m.message_type = XdndFinished;
								m.format=32;
								m.data.l[0] = windowhandle;
								m.data.l[1] = 0;
								m.data.l[2] = None; //Failed.
								XSendEvent(display, event.xclient.data.l[0], False, NoEventMask, (XEvent*)&m);
								
								return;
							}
							
							
							XEvent ev;

							
							XConvertSelection(display, XdndSelection, XA_Filelist, XA_PRIMARY, windowhandle, event.xclient.data.l[0]);
							XFlush(display);
							
							XIfEvent(display, &ev, system::WaitForSelectionNotify, (char*)windowhandle);
							if (ev.xselection.property != (unsigned)None && dynamic_cast<gge::input::mouse::FileListDragData*>(&gge::input::mouse::GetDraggedObject())) {
								Atom type;
								unsigned long len, bytes, dummy;
								unsigned char *data;
								int format;
								
								auto &dragdata=dynamic_cast<gge::input::mouse::FileListDragData&>(gge::input::mouse::GetDraggedObject());


								XGetWindowProperty(display, windowhandle, ev.xselection.property, 0, 0, 0, AnyPropertyType, &type, &format, &len, &bytes, &data);

								if(bytes) {

									XGetWindowProperty (display, windowhandle, 
											ev.xselection.property, 0,bytes,0,
											AnyPropertyType, &type, &format,
											&len, &dummy, &data);
									
									
									int p=0;
									for(int i=0;i<(int)len+1;i++) {
										if(i==(int)len || (char)data[i]=='\n') {
											if(i-p>1) {
												std::string s((char*)(data+p), (int)(i-p));
												if(s.length()>6 && s.substr(0, 7)=="file://") 
													s=s.substr(7);
												if(s[s.length()-1]=='\r')
													s.resize(s.length()-1);
												
												dragdata.data.push_back(encoding::UriDecode(s));
												p=i+1;
											}
										}
									}
									
									XFree(data);
								}
								
								XClientMessageEvent m;
								memset(&m, sizeof(m), 0);
								m.type = ClientMessage;
								m.display = display;
								m.window = event.xclient.data.l[0];
								m.message_type = XdndFinished;
								m.format=32;
								m.data.l[0] = windowhandle;
								m.data.l[1] = 1;
								m.data.l[2] = XdndActionCopy; //We only ever copy.

								//Reply that all is well.
								XSendEvent(display, event.xclient.data.l[0], False, NoEventMask, (XEvent*)&m);
							}
							else if(dynamic_cast<gge::input::mouse::TextDragData*>(&gge::input::mouse::GetDraggedObject())) {
								XConvertSelection(display, XdndSelection, XA_STRING, XA_PRIMARY, windowhandle, event.xclient.data.l[0]);
								XFlush(display);
								
								
								XIfEvent(display, &ev, system::WaitForSelectionNotify, (char*)windowhandle);
								if (ev.xselection.property != (unsigned)None) {
									Atom type;
									unsigned long len, bytes, dummy;
									unsigned char *data;
									int format;
									
									auto &dragdata=dynamic_cast<gge::input::mouse::TextDragData&>(gge::input::mouse::GetDraggedObject());


									XGetWindowProperty(display, windowhandle, ev.xselection.property, 0, 0, 0, AnyPropertyType, &type, &format, &len, &bytes, &data);

									if(bytes) {

										XGetWindowProperty (display, windowhandle, 
												ev.xselection.property, 0,bytes,0,
												AnyPropertyType, &type, &format,
												&len, &dummy, &data);
										
										std::string tmp((char*)data, bytes);
										XFree(data);
										
										dragdata.data=tmp;
									}
									
									XClientMessageEvent m;
									memset(&m, sizeof(m), 0);
									m.type = ClientMessage;
									m.display = display;
									m.window = event.xclient.data.l[0];
									m.message_type = XdndFinished;
									m.format=32;
									m.data.l[0] = windowhandle;
									m.data.l[1] = 1;
									m.data.l[2] = XdndActionCopy; //We only ever copy.

									//Reply that all is well.
									XSendEvent(display, event.xclient.data.l[0], False, NoEventMask, (XEvent*)&m);
								}
							}

							gge::input::mouse::DropDrag();
							gge::Pointers.Show();
						}
						else {
							//std::cout<<"E"<<event.xclient.message_type<<std::endl;
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
						break;
						//std::cout<<event.type<<std::endl;
					}
				}
				
			}
		}
		namespace window {

		//TODO Private
			Point cursorlocation=Point(0,0);
			
		}
		IconHandle IconFromImage(graphics::ImageData &image) {
			unsigned *img=new unsigned[2+image.GetWidth()*image.GetHeight()];
			
			img[0]=image.GetWidth();
			img[1]=image.GetHeight();
			
			int is=image.GetWidth()*image.GetHeight();
			int *id=(int*)image.RawData();
			for(int i=0;i<is;i++)
				img[i+2]=id[i];
			
			return (IconHandle)img;
		}
		namespace input {

		}


	namespace input { namespace keyboard {
		const Key KeyCodes::Shift	= XK_Shift_L;
		const Key KeyCodes::Control	= XK_Control_L;
		const Key KeyCodes::Alt	= XK_Alt_L;
		const Key KeyCodes::Super	= XK_Super_L;

		const Key KeyCodes::Home	= XK_Home;
		const Key KeyCodes::End	= XK_End;
		const Key KeyCodes::Insert	= XK_Insert;
		const Key KeyCodes::Delete	= XK_Delete;
		const Key KeyCodes::PageUp	= XK_Page_Up;
		const Key KeyCodes::PageDown	= XK_Page_Down;

		const Key KeyCodes::Left	= XK_Left;
		const Key KeyCodes::Up	= XK_Up;
		const Key KeyCodes::Right	= XK_Right;
		const Key KeyCodes::Down	= XK_Down;

		const Key KeyCodes::PrintScreen=XK_Print;
		const Key KeyCodes::Pause	= XK_Pause;

		const Key KeyCodes::CapsLock	= XK_Caps_Lock;
		const Key KeyCodes::NumLock	= XK_Num_Lock;

		const Key KeyCodes::Enter	= XK_Return;
		const Key KeyCodes::Backspace	= XK_BackSpace;
		const Key KeyCodes::Escape	= XK_Escape;
		const Key KeyCodes::Tab	= XK_Tab;
		const Key KeyCodes::Space	= 0x20;

		const Key KeyCodes::F1	= XK_F1;
		const Key KeyCodes::F2	= XK_F2;
		const Key KeyCodes::F3	= XK_F3;
		const Key KeyCodes::F4	= XK_F4;
		const Key KeyCodes::F5	= XK_F5;
		const Key KeyCodes::F6	= XK_F6;
		const Key KeyCodes::F7	= XK_F7;
		const Key KeyCodes::F8	= XK_F8;
		const Key KeyCodes::F9	= XK_F9;
		const Key KeyCodes::F10	= XK_F10;
		const Key KeyCodes::F11	= XK_F11;
		const Key KeyCodes::F12	= XK_F12;

	} }

}


#endif
