#include "../WindowManager.h"
#include <thread>
#include <Gorgon/Geometry/Margins.h>
#include <limits.h>
#include "../Window.h"
#include "../Time.h"
#include "../OS.h"
#include "../Graphics.h"
#include "../Utils/Assert.h"


#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xrandr.h>
#include <X11/extensions/Xinerama.h>
#include <string.h>
#include <unistd.h>

#include <GL/glx.h>

#undef None


namespace Gorgon { 

	namespace internal {
	
		struct windowdata {
			::Window handle = 0;
			bool pointerdisplayed=true;
			bool move=false;
			Geometry::Point moveto;
			bool ismapped=false;
			GLXContext context=0;
			
			std::map<Input::Key, ConsumableEvent<Window, Input::Key, bool>::Token> handlers;
		};
		
        static int None = 0;
	}

	namespace WindowManager {
		
		/// @cond INTERNAL
		/// X11 display information
		Display *display = nullptr;
		
		/// Depends on monitor, might be moved
		Visual  *visual  = nullptr;

		/// Blank cursor to remove WindowManager cursor
		Cursor blank_cursor;
		
		/// XRandr extension to query physical monitors
		bool xrandr = false;
		
		/// Xinerama extension to query physical monitors, this is for legacy systems
		bool xinerama = false;
		
		/// Copied text
		std::string copiedtext;
		
		///@{ X11 atoms for various data identifiers
		Atom XA_CLIPBOARD;
		Atom XA_TIMESTAMP;
		Atom XA_TARGETS;
		Atom XA_PROTOCOLS;
		Atom WM_DELETE_WINDOW;
		Atom XA_STRING;
		Atom XA_ATOM;
		Atom XA_CARDINAL;
		Atom XA_NET_FRAME_EXTENTS;
		Atom XA_NET_WORKAREA;
		Atom XA_NET_REQUEST_FRAME_EXTENTS;
        Atom XA_NET_WM_STATE;
        Atom XA_NET_WM_STATE_FULLSCREEN;
        Atom XA_STRUT;
		///@}
			
		///@{ waits for specific events
		static int waitfor_mapnotify(Display *d, XEvent *e, char *arg) {
			return (e->type == MapNotify) && (e->xmap.window == (::Window)arg);
		}
		
		static int waitfor_propertynotify(Display *d, XEvent *e, char *arg) {
			return (e->type == PropertyNotify) && (e->xproperty.window == (::Window)arg);
		}
		
		static int waitfor_selectionnotify(Display *d, XEvent *e, char *arg) {
			return (e->type == SelectionNotify);
		}
		namespace internal {
			Gorgon::internal::windowdata *getdata(const Window &w) {
				return w.data;
			}
			
			struct monitordata {
				int index = -1;
				RROutput out = 0;
				
				~monitordata() {
				}
			};
			
			intptr_t context;

			void switchcontext(Gorgon::internal::windowdata &data) {
				glXMakeCurrent(WindowManager::display, data.handle, data.context);
				context=reinterpret_cast<intptr_t>(&data);
			}

			void finalizerender(Gorgon::internal::windowdata &data) {
				glFinish();
				glFlush();
				glXSwapBuffers(WindowManager::display, data.handle);
				XFlush(WindowManager::display);
			}
		}
		///@}
		
		template<class T_>
		T_ GetX4Prop(Atom atom, ::Window w, const T_ &def) {
			Atom actual_type;
			int actual_format;
			unsigned long item_count;
			unsigned long bytes_left;
			
			Byte *data;

			int status = XGetWindowProperty(
				display,
				w,
				atom,
				0, 4, 0, AnyPropertyType,
				&actual_type, &actual_format,
				&item_count, &bytes_left,
				&data
			);
			
			if(status!=Success) return def;
			
			if(item_count<4 || actual_format!=32) return def;
			
			int32_t *cardinals=reinterpret_cast<int32_t*>(data);
			
			T_ ret={
				cardinals[0],
				cardinals[1],
				cardinals[2],
				cardinals[3]
			};
			
			XFree(data);
			
			return ret;
		}
		/// @endcond
		
		void Initialize() {
			//get display
			display = XOpenDisplay(NULL);
			visual = XDefaultVisualOfScreen(DefaultScreenOfDisplay(display));
			
			//query atoms
			XA_CLIPBOARD=XInternAtom(display, "CLIPBOARD", 1);
			XA_TIMESTAMP=XInternAtom(display, "TIMESTAMP", 1);
			XA_TARGETS  =XInternAtom (display, "TARGETS", 0);
			XA_PROTOCOLS=XInternAtom(display, "WM_PROTOCOLS", 0);
			XA_STRING   =XInternAtom(display, "STRING", 0);
			XA_CARDINAL =XInternAtom(display, "CARDINAL", 0);
			XA_ATOM     =XInternAtom(display, "ATOM", 0);
			WM_DELETE_WINDOW = XInternAtom(display, "WM_DELETE_WINDOW", 0);
			XA_NET_FRAME_EXTENTS = XInternAtom(display, "_NET_FRAME_EXTENTS", 0);
			XA_NET_WORKAREA = XInternAtom(display, "_NET_WORKAREA", 0);
			XA_NET_REQUEST_FRAME_EXTENTS = XInternAtom(display, "_NET_REQUEST_FRAME_EXTENTS", 0);
            
            XA_NET_WM_STATE = XInternAtom(display, "_NET_WM_STATE", False);
            XA_NET_WM_STATE_FULLSCREEN = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", False);
            XA_STRUT = XInternAtom(display, "_NET_WM_STRUT_PARTIAL", False);
			

			char data[1]={0};
			XColor dummy;
			Pixmap blank = XCreateBitmapFromData (display, DefaultRootWindow(display), data, 1, 1);
			blank_cursor = XCreatePixmapCursor(display, blank, blank, &dummy, &dummy, 0, 0);
			XFreePixmap (display, blank);
			
			//detect extensions
			int eventbase, errorbase;
			xrandr=(bool)XRRQueryExtension(display, &eventbase, &errorbase);
			
			xinerama=(bool)XineramaQueryExtension(display, &eventbase, &errorbase);
			xinerama=xinerama && (bool)XineramaIsActive(display);
			
			Monitor::Refresh(true);
		}

		//Clipboard related
		
		std::string GetClipboardText() {
			::Window owner=XGetSelectionOwner(display, XA_CLIPBOARD);
			if(!owner)
				return "";
			
			::Window windowhandle=0;
			for(auto &w : Window::Windows) {
				auto data=internal::getdata(w);
				if(data && data->handle) {
					windowhandle=data->handle;
					break;
				}
			}
			if(windowhandle==0) {
#ifdef NDEBUG
				return "";
#else
				throw std::runtime_error("Cannot copy without a window, if necessary create a hidden window");
#endif
			}
			
			XEvent event;
			
			XConvertSelection (display, XA_CLIPBOARD, XA_STRING, Gorgon::internal::None, windowhandle, CurrentTime);
			XFlush(display);
			
			XIfEvent(display, &event, waitfor_selectionnotify, (char*)windowhandle);
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
			::Window windowhandle=0;
			for(auto &w : Window::Windows) {
				auto data=internal::getdata(w);
				if(data && data->handle) {
					windowhandle=data->handle;
					break;
				}
			}
			if(windowhandle==0) {
#ifdef NDEBUG
			return;
#else
				throw std::runtime_error("Cannot copy without a window, if necessary create a hidden window");
#endif
			}

			XSetSelectionOwner (display, XA_CLIPBOARD, windowhandle, Time::GetTime());
			copiedtext=text;
			XFlush(display);
		}
	
		//Monitor Related
		
		Monitor::Monitor() {
			data = new internal::monitordata;
		}
		
		Monitor::~Monitor() {
			delete data;
		}
		
		void addpadding(const Monitor *monitor, int l, int t, int r, int b) {
            Monitor *mon = nullptr;
            
            for(auto &mon2 : Monitor::monitors) {
                if(&mon2 == monitor)
                    mon = &mon2;
            }
            
            if(mon) {
                mon->usable.Left  += l;
                mon->usable.Top   += t;
                mon->usable.Right -= r;
                mon->usable.Bottom-= b;
            }
        }

        static void fixmonitorworkarea(int parent = 0, int x = 0, int y = 0) {
            ::Window* children, w;
            
            unsigned int child_count;
            
            if(parent == 0)
                parent=XDefaultRootWindow(display);

            XQueryTree(display, parent, &w, &w, &children, &child_count);
            
            for(int i=0; i<child_count; i++) {
                Atom actual_type;
                int actual_format;
                unsigned long item_count;
                unsigned long bytes_left;
                Byte *data;
                
                XWindowAttributes xwa;
                XGetWindowAttributes(display, children[i], &xwa);
                
                int status = XGetWindowProperty(
                    display, children[i], XA_STRUT, 
                    0, 12 * 4, 0,
                    XA_CARDINAL, &actual_type, &actual_format,
                    &item_count, &bytes_left, &data
                );
                
                if(status == Success && item_count) {
                    int32_t *cardinals=reinterpret_cast<int32_t*>(data);
                    
                    auto monitor = Monitor::FromLocation({int(x+xwa.x), int(y+xwa.y)});
                    
                    if(monitor) {
                        addpadding(monitor, cardinals[0], cardinals[2], cardinals[1], cardinals[3]);
                    }
                    
                    XFree(data);
                }
                
                fixmonitorworkarea(children[i], x+xwa.x, y+xwa.y);
            }
        }
		
		void Monitor::Refresh(bool force) {
			//check if change is needed or forced
			
			monitors.Destroy();
			Monitor::primary=nullptr;
			
			if(xrandr) {
				auto root=XDefaultRootWindow(display);
				XRRScreenResources* sr = XRRGetScreenResources(display, root);
				RROutput primary = XRRGetOutputPrimary(display, root);
				
				if(!sr) {
					goto failsafe;
				}
				
				XRRCrtcInfo*   ci = nullptr;
				XRROutputInfo* oi = nullptr;
				int ind = 0;
				try {
					for(int i=0; i<sr->ncrtc; i++) {
						ci = XRRGetCrtcInfo(display, sr, sr->crtcs[i]);
						
						for(int j=0; j<ci->noutput; j++) {
							oi = XRRGetOutputInfo(display, sr, ci->outputs[j]);
							
							if(oi->connection==0) {
								auto monitor=new Monitor();
								monitor->data->index=ind++;
								monitor->data->out=ci->outputs[j];
								monitor->area={(int)ci->x, (int)ci->y, (int)ci->width, (int)ci->height};
                                monitor->usable = monitor->area;
								monitor->isprimary=(ci->outputs[j]==primary);
								if(monitor->IsPrimary()) Monitor::primary=monitor;
								monitor->name=oi->name;
								if(monitor->IsPrimary())
									monitors.Insert(monitor, 0);
								else
									monitors.Add(monitor);
							}
							
							XRRFreeOutputInfo(oi);
							oi=nullptr;
						}
						
						XRRFreeCrtcInfo(ci);
						ci=nullptr;
					}
				}
				catch(...) {
					XRRFreeScreenResources(sr);
					if(ci) {
						XRRFreeCrtcInfo(ci);
					}
					if(oi) {
						XRRFreeOutputInfo(oi);
					}
					throw;
				}
				
				XRRFreeScreenResources(sr);
				
				if(monitors.GetCount()) {
					if(Monitor::primary==nullptr) {
						Monitor::primary=monitors.First().CurrentPtr();
					}
					
					fixmonitorworkarea();
					return;
				}
			}
			
failsafe: //this should use X11 screen as monitor
			Geometry::Rectangle rect;
			rect.X=0;
			rect.Y=0;
			
			Screen  *screen  = XScreenOfDisplay(display, 0);
			rect.Width       = XWidthOfScreen(screen);
			rect.Height      = XHeightOfScreen(screen);
			
            auto &monitor = *new Monitor();
            monitors.Add(monitor);
            
            monitor.name = "Default";
            monitor.data->index = -1;
            monitor.data->out   = -1;
            monitor.area = rect;
            monitor.usable = rect;
            
            Monitor::primary = &monitor;
            fixmonitorworkarea();
		}
		
		    
        Geometry::Point GetMousePosition(Gorgon::internal::windowdata *wind) {
            Geometry::Point ret;
            
            int rx,ry;
            ::Window root, child;
            unsigned mask;
            
            if(XQueryPointer(display, wind->handle, &root, &child, &rx, &ry, &ret.X, &ret.Y, &mask) == False) {
                ret.X = -1;
                ret.Y = -1;
            }

            return ret;
        }

		
		bool Monitor::IsChangeEventSupported() {
			return false;//xrandr;
		}
	
		Event<> Monitor::ChangedEvent;
		Containers::Collection<Monitor> Monitor::monitors;
		Monitor *Monitor::primary=nullptr;
	}
	
	Window::Window(const WindowManager::Monitor &monitor, Geometry::Rectangle rect, const std::string &name, const std::string &title, bool visible) : 
	data(new internal::windowdata) {
		
#ifndef NDEBUG
		ASSERT(WindowManager::display, "Window manager system is not initialized.");
#endif
		
		windows.Add(this);
		
		//using defaults
		int screen = DefaultScreen(WindowManager::display);
		int depth  = DefaultDepth(WindowManager::display,screen);
		
		//adjust atrributes
		XSetWindowAttributes attributes;
		
		attributes.event_mask = 
					StructureNotifyMask |    //move resize
					KeyPressMask |           //keyboard
					KeyReleaseMask |
					ButtonPressMask |        //mouse
					ButtonReleaseMask|    
					FocusChangeMask|         //activate/deactivate
					EnterWindowMask |
					LeaveWindowMask |
					SubstructureRedirectMask //??
		;
		
		bool autoplaced=false;
		if(rect.TopLeft()==automaticplacement) {
			rect.Move( (monitor.GetUsable()-rect.GetSize()).Center() );
			autoplaced=true;
		}
		
		auto rootwin=XRootWindow(WindowManager::display,screen);
		
		data->handle = XCreateWindow(WindowManager::display, 
			rootwin,
			rect.X,rect.Y, rect.Width,rect.Height,
			0, depth, InputOutput, 
			WindowManager::visual, CWEventMask, &attributes
		);
		
		XClassHint *classhint=XAllocClassHint();
		classhint->res_name=(char*)malloc(name.length()+1);
		strcpy(classhint->res_name, name.c_str());
		classhint->res_class=(char*)malloc(name.length()+1);
		strcpy(classhint->res_class, name.c_str());
		XSetClassHint(WindowManager::display, data->handle, classhint);
		XFree(classhint);
	
		XStoreName(WindowManager::display, data->handle, (char*)title.c_str());

		XSizeHints *sizehints=XAllocSizeHints();
		sizehints->min_width=rect.Width;
		sizehints->max_width=rect.Width;
		sizehints->min_height=rect.Height;
		sizehints->max_height=rect.Height;
		sizehints->flags=PMinSize | PMaxSize | PWinGravity;
        if(autoplaced)
            sizehints->win_gravity = CenterGravity;
        
		XSetWMNormalHints(WindowManager::display, data->handle, sizehints);		
		XFree(sizehints);
		
		XSetWMProtocols(WindowManager::display, data->handle, &WindowManager::WM_DELETE_WINDOW, 1);
		
		if(visible) {
			XEvent event;
			
			XFlush(WindowManager::display);
			
			XMapWindow(WindowManager::display,data->handle);
			XIfEvent(WindowManager::display, &event, &WindowManager::waitfor_mapnotify, (char*)data->handle);
            
            XMoveWindow(WindowManager::display, data->handle, rect.X, rect.Y);
	
            if(autoplaced) {
                XFlush(WindowManager::display);
                
                std::this_thread::sleep_for(std::chrono::milliseconds(10)); //wait for a short time to ensure window frame is ready.
                
                auto borders = WindowManager::GetX4Prop<Geometry::Margins>(WindowManager::XA_NET_FRAME_EXTENTS, data->handle, {0,0,0,0});
                std::swap(borders.Top, borders.Right);
                rect.Move( (monitor.GetUsable()-(rect.GetSize()+borders.Total())).Center() );
            }
            
            XMoveWindow(WindowManager::display, data->handle, rect.X, rect.Y);
            
			XFlush(WindowManager::display);			
		}
		else {
			data->move=true;
			data->moveto=rect.TopLeft();
		}
		
		data->ismapped=visible;
		
		Layer::Resize(rect.GetSize());

		createglcontext();
	}
	
	Window::Window(const Gorgon::Window::FullscreenTag &, const WindowManager::Monitor &mon, const std::string &name, const std::string &title) : data(new internal::windowdata) {
		
#ifndef NDEBUG
		ASSERT(WindowManager::display, "Window manager system is not initialized.");
#endif
		
		windows.Add(this);
		
		//using defaults
		int screen = DefaultScreen(WindowManager::display);
		int depth  = DefaultDepth(WindowManager::display,screen);
		
		//adjust atrributes
		XSetWindowAttributes attributes;
		
		attributes.event_mask = 
					StructureNotifyMask |    //move resize
					KeyPressMask |           //keyboard
					KeyReleaseMask |
					ButtonPressMask |        //mouse
					ButtonReleaseMask|    
					FocusChangeMask|         //activate/deactivate
					SubstructureRedirectMask //??
		;
		
		bool autoplaced=false;
		
		auto rootwin=XRootWindow(WindowManager::display,screen);
		
		data->handle = XCreateWindow(WindowManager::display, 
			rootwin,
			mon.GetLocation().X,mon.GetLocation().Y,mon.GetSize().Width,mon.GetSize().Height,
			0, depth, InputOutput, 
			WindowManager::visual, CWEventMask, &attributes
		);
		
		XClassHint *classhint=XAllocClassHint();
		classhint->res_name=(char*)malloc(name.length()+1);
		strcpy(classhint->res_name, name.c_str());
		classhint->res_class=(char*)malloc(name.length()+1);
		strcpy(classhint->res_class, name.c_str());
		XSetClassHint(WindowManager::display, data->handle, classhint);
		XFree(classhint);
	
		XStoreName(WindowManager::display, data->handle, (char*)title.c_str());

		XSizeHints *sizehints=XAllocSizeHints();
		sizehints->min_width=mon.GetSize().Width;
		sizehints->max_width=mon.GetSize().Width;
		sizehints->min_height=mon.GetSize().Height;
		sizehints->max_height=mon.GetSize().Height;
		sizehints->x=mon.GetLocation().X;
		sizehints->y=mon.GetLocation().Y;
		sizehints->flags=PMinSize | PMaxSize | PPosition ;
        
		XSetWMNormalHints(WindowManager::display, data->handle, sizehints);		
		XFree(sizehints);
		
		XSetWMProtocols(WindowManager::display, data->handle, &WindowManager::WM_DELETE_WINDOW, 1);
		
        XEvent event;
        
        XFlush(WindowManager::display);
        
        Atom flist[] = {WindowManager::XA_NET_WM_STATE_FULLSCREEN, 0};
        
        XChangeProperty(WindowManager::display, data->handle, WindowManager::XA_NET_WM_STATE, WindowManager::XA_ATOM, 32, PropModeReplace, (Byte*)flist, 1);
        
        XMapWindow(WindowManager::display,data->handle);
        XIfEvent(WindowManager::display, &event, &WindowManager::waitfor_mapnotify, (char*)data->handle);
        
        XMoveWindow(WindowManager::display, data->handle, mon.GetLocation().X, mon.GetLocation().Y);
        
        XFlush(WindowManager::display);
		
		data->ismapped=true;
		
		Layer::Resize(mon.GetSize());

		createglcontext();
	}
	
	Window::~Window() {
		delete data;
		windows.Remove(this);
	}
	
	void Window::Show() {
		Layer::Show();
		
		XEvent event;

		XMapWindow(WindowManager::display, data->handle);
		XIfEvent(WindowManager::display, &event, WindowManager::waitfor_mapnotify, (char*)data->handle);
		XRaiseWindow(WindowManager::display, data->handle);
		if(data->move) {
			XMoveWindow(WindowManager::display, data->handle, data->moveto.X, data->moveto.Y);
			data->move=false;
		}		
		XFlush(WindowManager::display);
		data->ismapped=true;
	}
	
	void Window::Hide() {
		Layer::Hide();
		
		XUnmapWindow(WindowManager::display, data->handle);
		data->ismapped=false;
	}
		
	void Window::HidePointer() {
		if(data->pointerdisplayed) {
			data->pointerdisplayed=false;
			XDefineCursor(WindowManager::display, data->handle, WindowManager::blank_cursor);
			XFlush(WindowManager::display);
		}
	}
		
	void Window::ShowPointer() {
		if(!data->pointerdisplayed) {
			data->pointerdisplayed=true;
			XDefineCursor(WindowManager::display, data->handle, 0);
			XFlush(WindowManager::display);
		}
	}
	
	void Window::Move(const Geometry::Point &location) {
		Layer::Move(location);
		
		if(data->ismapped) {
			XMoveWindow(WindowManager::display, data->handle, location.X, location.Y);
			XFlush(WindowManager::display);
		}
		else {
			data->move=true;
			data->moveto=location;
		}
	}
	
	void Window::Resize(const Geometry::Size &size) {
		Layer::Resize(size);

		XSizeHints *sizehints=XAllocSizeHints();
		sizehints->min_width=size.Width;
		sizehints->max_width=size.Width;
		sizehints->min_height=size.Height;
		sizehints->max_height=size.Height;
		sizehints->flags=PMinSize | PMaxSize;
		XSetWMNormalHints(WindowManager::display, data->handle, sizehints);
		XFree(sizehints);

		XResizeWindow(WindowManager::display, data->handle, size.Width, size.Height);
		XFlush(WindowManager::display);
	}

    Input::Mouse::Button buttonfromx11(unsigned btn) {
        using Input::Mouse::Button;
        switch(btn) {
        case 1:
            return Button::Left;
        case 2:
            return Button::Middle;
        case 3:
            return Button::Right;
        case 8:
            return Button::X1;
        case 9:
            return Button::X2;
        default:
            return Button::None;
        }
    }
	
	void Window::processmessages() {
		XEvent event;
        
        mouselocation = WindowManager::GetMousePosition(data);
        if(cursorover)
            mouse_location();

		while(XEventsQueued(WindowManager::display, QueuedAfterReading)) {
			XNextEvent(WindowManager::display, &event);
			unsigned key;
			switch(event.type) {
				case ClientMessage: {
					if (event.xclient.message_type == WindowManager::XA_PROTOCOLS
							&& event.xclient.format == 32
							&& event.xclient.data.l[0] == (long)WindowManager::WM_DELETE_WINDOW)
					{
						bool allow;
						allow=true;
						ClosingEvent(allow);
						
						if(allow) {
							Close();
							break;
						}
					}
				} // Client Message
				break;
                
                case EnterNotify:
                    cursorover = true;
                    break;
                    
                case LeaveNotify:
                    cursorover = false;
                    break;
					
				case FocusIn:
					ActivatedEvent();
					break;
					
				case FocusOut:
					//ReleaseAll();
					DeactivatedEvent();
					break;
					
				case KeyPress: {
					key=XLookupKeysym(&event.xkey,0);
					
					//modifiers
					switch(key) {
						case XK_Shift_L:
						case XK_Shift_R:
							Input::Keyboard::CurrentModifier.Add(Input::Keyboard::Modifier::Shift);
							break;
							
						case XK_Control_L:
						case XK_Control_R:
							Input::Keyboard::CurrentModifier.Add(Input::Keyboard::Modifier::Ctrl);
							break;
							
						case XK_Alt_L:
							Input::Keyboard::CurrentModifier.Add(Input::Keyboard::Modifier::Alt);
							break;
							
						case XK_Alt_R:
							Input::Keyboard::CurrentModifier.Add(Input::Keyboard::Modifier::Alt);
							break;
							
						case XK_Super_L:
						case XK_Super_R:
							Input::Keyboard::CurrentModifier.Add(Input::Keyboard::Modifier::Meta);
							break;
					}
					auto token=KeyEvent(key, true);
					if(token != KeyEvent.EmptyToken) {
						data->handlers[key]=token;
						
						break;
					}
					
					Byte buffer[2];
					
					int nchars = XLookupString(
						&(event.xkey),
						(char*)&buffer, 2, 
						(KeySym*)&key, nullptr 
					);
					
					if(nchars==1) {
						if((buffer[0]>=0x20 && buffer[0]!=0x7f) || buffer[0] == '\t' || buffer[0] == '\r') {
							CharacterEvent(buffer[0]);
						}
					}
				} //Keypress
				break;
					
					
				case KeyRelease: {
					key=XLookupKeysym(&event.xkey,0);
						
					if(XEventsQueued(WindowManager::display, QueuedAfterReading)) {
						XEvent nextevent;
						XPeekEvent(WindowManager::display, &nextevent);
					
						if(nextevent.type == KeyPress && nextevent.xkey.time == event.xkey.time && 
							nextevent.xkey.keycode == event.xkey.keycode
						) {
							
							if(data->handlers.count(key)>0 && data->handlers[key]!=KeyEvent.EmptyToken) {
								KeyEvent.FireFor(data->handlers[key], key, true);
							}
							else {
								Byte buffer[2];
						
								int nchars = XLookupString(
									&(event.xkey),
									(char*)&buffer, 2, 
									(KeySym*)&key, nullptr 
								);
								
								if(nchars==1) {
									if((buffer[0]>=0x20 && buffer[0]<0x7f) || buffer[0] == '\t') {
										CharacterEvent(buffer[0]);
									}
								}
							}
					
							XNextEvent(WindowManager::display, &nextevent);
							break;
						}
					}
					
					//modifiers
					switch(key) {
						case XK_Shift_L:
						case XK_Shift_R:
							Input::Keyboard::CurrentModifier.Remove(Input::Keyboard::Modifier::Shift);
							break;
							
						case XK_Control_L:
						case XK_Control_R:
							Input::Keyboard::CurrentModifier.Remove(Input::Keyboard::Modifier::Ctrl);
							break;
							
						case XK_Alt_L:
							Input::Keyboard::CurrentModifier.Remove(Input::Keyboard::Modifier::Alt);
							break;
							
						case XK_Alt_R:
							Input::Keyboard::CurrentModifier.Remove(Input::Keyboard::Modifier::Alt);
							break;
							
						case XK_Super_L:
						case XK_Super_R:
							Input::Keyboard::CurrentModifier.Remove(Input::Keyboard::Modifier::Meta);
							break;
					}
					
					
					if(data->handlers.count(key)>0 && data->handlers[key]!=KeyEvent.EmptyToken) {
						KeyEvent.FireFor(data->handlers[key], key, 0.f);
						data->handlers[key]=KeyEvent.EmptyToken;
					}
					else {
						KeyEvent(key, 0.f);
					}
					
				} //Keyrelease
				break;
					
                case ButtonPress:
                    if(event.xbutton.button==4) {
                        mouse_event(Input::Mouse::EventType::Scroll_Vert, {event.xbutton.x, event.xbutton.y}, buttonfromx11(event.xbutton.button), 1);
                    }
                    else if(event.xbutton.button==5) {
                        mouse_event(Input::Mouse::EventType::Scroll_Vert, {event.xbutton.x, event.xbutton.y}, buttonfromx11(event.xbutton.button), -1);
                    }
                    else {
                        mouse_down({event.xbutton.x, event.xbutton.y}, buttonfromx11(event.xbutton.button));
                    }
                    break;
                case ButtonRelease:
                    if(event.xbutton.button!=4 && event.xbutton.button!=5) {
                        mouse_up({event.xbutton.x, event.xbutton.y}, buttonfromx11(event.xbutton.button));
                    }
                    break;
			}
		}
	}
	
	void Window::Close() {
		DestroyedEvent();
	}
	
	void Window::createglcontext() {
		static int attributeListDbl[] = {
			GLX_RGBA,
			GLX_DOUBLEBUFFER,
			GLX_RED_SIZE,   1,
			GLX_GREEN_SIZE, 1,
			GLX_BLUE_SIZE,  1,
			internal::None
		};
		
		XVisualInfo *vi = glXChooseVisual(WindowManager::display, DefaultScreen(WindowManager::display), attributeListDbl);
		
		GLXContext prev=0;
		for(auto &w : windows) {
			if(w.data->context!=0) {
				prev=w.data->context;
			}
		}
		
		data->context = glXCreateContext(WindowManager::display, vi, prev, GL_TRUE);		
		WindowManager::internal::switchcontext(*data);

		if(data->context==0) {
			OS::DisplayMessage("OpenGL context creation failed");
			exit(1);
		}

		GL::SetupContext(bounds.GetSize());

		Graphics::Initialize();

		// test code
		glXSwapBuffers(WindowManager::display, data->handle);		
		XFlush(WindowManager::display);
	}
}
