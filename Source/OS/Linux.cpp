



#ifdef LaINUX
#	pragma warning(disable: 4996)
#	include "OS.h"
#	include "Input.h"
#	include "../Utils/Point2D.h"
#	include <stdio.h>
#	include <pthread.h>
#	include <cstdlib>
#	include <cstring>
#	define XK_MISCELLANY
#include <pthread.h>

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
#	include "Image.h"
#	include "Layer.h"
#	include "../Encoding/URI.h"
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
		void OpenTerminal(std::string Title, int maxlines) {
			
			
			int fd1[2];
			pipe(fd1);
			if(!fork()) {
				dup2(fd1[0], 1);
				close(fd1[0]);
				close(fd1[1]);

				execlp("xconsole","xconsole","-file","/dev/stdout",(char *)NULL);
				exit(0);
			}
			else {
				dup2(fd1[1], 1);
				close(fd1[0]);
				close(fd1[1]);
			}
			
			
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

			
			static int WaitForMapNotify(Display *d, XEvent *e, char *arg) {
				return (e->type == MapNotify) && (e->xmap.window == (Window)arg);
			}
			
			static int WaitForSelectionNotify(Display *d, XEvent *e, char *arg) {
				return (e->type == SelectionNotify);
			}
			
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
						 if (event.xclient.message_type == XA_PROTOCOLS 
							&& event.xclient.format == 32 
							&& event.xclient.data.l[0] == (long)WM_DELETE_WINDOW) 
						{ 
							bool allow;
							allow=true;
							window::Closing(allow);
							if(allow) {
								ReleaseAll();
								window::Destroyed();
								break;
							}
						}
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
					
					case FocusIn:
						window::Activated();
						break;
						
					case FocusOut:
						ReleaseAll();
						window::Deactivated();
						break;
						
					case KeyPress:
						key=XLookupKeysym(&event.xkey,0);
						
						switch(key) {
							case XK_Shift_L:
								keyboard::Modifier::isAlternate=false;
								ProcessKeyDown(key);
								keyboard::Modifier::Add(keyboard::Modifier::Shift);
								break;
								
							case XK_Shift_R:
								keyboard::Modifier::isAlternate=true;
								ProcessKeyDown(XK_Shift_L);
								keyboard::Modifier::Add(keyboard::Modifier::Shift);
								break;
								
							case XK_Control_L:
								keyboard::Modifier::isAlternate=false;
								ProcessKeyDown(key);
								keyboard::Modifier::Add(keyboard::Modifier::Ctrl);
								break;
								
							case XK_Control_R:
								keyboard::Modifier::isAlternate=true;
								ProcessKeyDown(XK_Control_L);
								keyboard::Modifier::Add(keyboard::Modifier::Ctrl);
								break;
								
							case XK_Alt_L:
								keyboard::Modifier::isAlternate=false;
								ProcessKeyDown(key);
								keyboard::Modifier::Add(keyboard::Modifier::Alt);
								break;
								
							case XK_Alt_R:
								keyboard::Modifier::isAlternate=true;
								ProcessKeyDown(XK_Alt_L);
								keyboard::Modifier::Add(keyboard::Modifier::Alt);
								break;
								
							case XK_Super_L:
								keyboard::Modifier::isAlternate=false;
								ProcessKeyDown(key);
								keyboard::Modifier::Add(keyboard::Modifier::Super);
								break;
								
							case XK_Super_R:
								keyboard::Modifier::isAlternate=true;
								ProcessKeyDown(XK_Super_L);
								keyboard::Modifier::Add(keyboard::Modifier::Super);
								break;
								
							default:
								char buffer[2];
								int nchars;
								
								ProcessKeyDown(key);

								nchars = XLookupString(
									&(event.xkey),
									buffer,
									2,  /* buffer size */
									(KeySym*)&key,
									NULL 
								);
								
								if ((nchars == 1 || 
									(int)key==keyboard::KeyCodes::Enter) && 
									(int)key!=keyboard::KeyCodes::Delete)
									ProcessChar(key);
								break;
						}
						break;
					case KeyRelease:
						key=XLookupKeysym(&event.xkey,0);
						bool pkey;
						pkey=false;
						
						if(XEventsQueued(display, QueuedAfterReading)) {
							XEvent nextevent;
							XPeekEvent(display, &nextevent);
							if(nextevent.type == KeyPress && nextevent.xkey.time == event.xkey.time && 
								nextevent.xkey.keycode == event.xkey.keycode) {
								
								if((int)key==keyboard::KeyCodes::Delete || 
									(int)key==keyboard::KeyCodes::Escape ||
									(int)key==keyboard::KeyCodes::Up ||
									(int)key==keyboard::KeyCodes::Down ||
									(int)key==keyboard::KeyCodes::Left ||
									(int)key==keyboard::KeyCodes::PageUp ||
									(int)key==keyboard::KeyCodes::PageDown ||
									(int)key==keyboard::KeyCodes::Right) {
									
									ProcessKeyDown(key);
									
								}
								else {
									char buffer[2];
									int nchars;
									
									nchars = XLookupString(
										&(event.xkey),
										buffer,
										2,  /* buffer size */
										(KeySym*)&key,
										NULL 
									);
									
									if (nchars == 1 || (int)key==keyboard::KeyCodes::Enter)
										ProcessChar(key);
								}
								XNextEvent(display, &nextevent);
							
								break;
							}
							else {
								pkey=true;
							}
						}
						else {
							pkey=true;
						}
						
						if(pkey) {
							switch(key) {
								case XK_Shift_L:
									keyboard::Modifier::isAlternate=false;
									ProcessKeyUp(key);
									keyboard::Modifier::Remove(keyboard::Modifier::Shift);
									break;
									
								case XK_Shift_R:
									keyboard::Modifier::isAlternate=true;
									ProcessKeyUp(XK_Shift_L);
									keyboard::Modifier::Remove(keyboard::Modifier::Shift);
									break;
									
								case XK_Control_L:
									keyboard::Modifier::isAlternate=false;
									ProcessKeyUp(key);
									keyboard::Modifier::Remove(keyboard::Modifier::Ctrl);
									break;
									
								case XK_Control_R:
									keyboard::Modifier::isAlternate=true;
									ProcessKeyUp(XK_Control_L);
									keyboard::Modifier::Remove(keyboard::Modifier::Ctrl);
									break;
									
								case XK_Alt_L:
									keyboard::Modifier::isAlternate=false;
									ProcessKeyUp(key);
									keyboard::Modifier::Remove(keyboard::Modifier::Alt);
									break;
									
								case XK_Alt_R:
									keyboard::Modifier::isAlternate=true;
									ProcessKeyUp(XK_Alt_L);
									keyboard::Modifier::Remove(keyboard::Modifier::Alt);
									break;
									
								case XK_Super_L:
									keyboard::Modifier::isAlternate=false;
									ProcessKeyUp(key);
									keyboard::Modifier::Remove(keyboard::Modifier::Super);
									break;
									
								case XK_Super_R:
									keyboard::Modifier::isAlternate=true;
									ProcessKeyUp(XK_Super_L);
									keyboard::Modifier::Remove(keyboard::Modifier::Super);
									break;
									
								default:
									ProcessKeyUp(key);
									break;
							}
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
			utils::EventChain<> Activated	("WindowActivated" );
			utils::EventChain<> Deactivated ("WindowDectivated");
			utils::EventChain<> Destroyed	("WindowDestroyed" );
			utils::EventChain<Empty, bool&> Closing	("WindowClosing" );

		//TODO Private
			Point cursorlocation=Point(0,0);
			
			//!Show
			WindowHandle CreateWindow(std::string Name, std::string Title, os::IconHandle Icon, int Left, int Top, int Width, int Height, int BitDepth, bool Show, bool &FullScreen) {
				XSetWindowAttributes attributes; 
				int screen;
				int depth; 
				XEvent event;
				
				screen = DefaultScreen(display);
				depth  = DefaultDepth(display,screen); 
				attributes.event_mask = 
					StructureNotifyMask | 
					KeyPressMask |
					KeyReleaseMask |
					ButtonPressMask |
					ButtonReleaseMask|
					FocusChangeMask|
					SubstructureRedirectMask
				;
				Colormap cmap = XCreateColormap(display, RootWindow(display, screen), visual, AllocNone);
				attributes.colormap = cmap;
				windowhandle= XCreateWindow(display, XRootWindow(display,screen),
                   Left,Top, Width,Height,0, depth, InputOutput, visual,
                   CWEventMask,&attributes);	
				
				char data[1]={0};
				XColor dummy;
				Pixmap blank = XCreateBitmapFromData (display, windowhandle, data, 1, 1);
				emptycursor = XCreatePixmapCursor(display, blank, blank, &dummy, &dummy, 0, 0);
				XFreePixmap (display, blank);
				
				XClassHint *classhint=XAllocClassHint();
				classhint->res_name=(char*)malloc(Name.length()+1);
				strcpy(classhint->res_name, Name.c_str());
				classhint->res_class=(char*)malloc(Name.length()+1);
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
				
				XSetWMProtocols(display, windowhandle, &WM_DELETE_WINDOW, 1);

				if(Icon) {
					Atom net_wm_icon = XInternAtom(display, "_NET_WM_ICON", False);
					Atom cardinal = XInternAtom(display, "CARDINAL", False);
					XChangeProperty(display, windowhandle, net_wm_icon, cardinal , 32, PropModeReplace, (const unsigned char*)Icon, 2+((unsigned*)Icon)[0]*((unsigned*)Icon)[1]);
					XSync(display, 1);
				}

				
				if(Show) {
					XMapWindow(display,windowhandle);
					XIfEvent(display, &event, system::WaitForMapNotify, (char*)windowhandle);
				}
				
				system::XdndInit();
		
				
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


		std::string GetClipboardText() {
			Window owner=XGetSelectionOwner(display, XA_CLIPBOARD);
			if(!owner)
				return "";
			
			XEvent event;
			
			XConvertSelection (display, XA_CLIPBOARD, XA_STRING, None, windowhandle, CurrentTime);
			XFlush(display);
			
			XIfEvent(display, &event, system::WaitForSelectionNotify, (char*)windowhandle);
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

		std::string GetAppDataPath() {
			return getenv("HOME");
		}
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