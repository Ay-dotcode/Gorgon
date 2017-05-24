#include "../WindowManager.h"
#include <thread>
#include <Gorgon/Geometry/Margin.h>
#include <limits.h>
#include "../Window.h"
#include "../Time.h"
#include "../OS.h"
#include "../Graphics.h"
#include "../Utils/Assert.h"

#include "../Graphics/Layer.h"

#include "../Encoding/URI.h"
#include "../Input/DnD.h"

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
            bool min = false;
			bool pointerdisplayed=true;
			bool move=false;
			Geometry::Point moveto;
			bool ismapped=false;
			GLXContext context=0;
            bool focused = false;
            Geometry::Point ppoint = {INT_MIN, INT_MIN};
			
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
        Atom XA_UTF8_STRING;
		Atom XA_ATOM;
		Atom XA_CARDINAL;
		Atom XA_NET_FRAME_EXTENTS;
		Atom XA_NET_WORKAREA;
		Atom XA_NET_REQUEST_FRAME_EXTENTS;
        Atom XA_WM_NAME;
        Atom XA_NET_WM_NAME;
        Atom XA_NET_WM_STATE;
        Atom XA_NET_WM_STATE_ADD = 1;
        Atom XA_NET_WM_STATE_FULLSCREEN;
        Atom XA_NET_WM_STATE_MAXIMIZED_HORZ;
        Atom XA_NET_WM_STATE_MAXIMIZED_VERT;
        Atom XA_NET_WM_STATE_HIDDEN;
        Atom XA_NET_ACTIVE_WINDOW;
        Atom XA_WM_CHANGE_STATE;
        Atom XA_STRUT;
        Atom XA_NET_WM_ICON;
        Atom XA_PRIMARY;
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
            
            struct icondata {
                int w = 0, h = 0;
                Byte *data = nullptr;
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
			
			void XdndInit(::Gorgon::internal::windowdata *w) {
				int version=5;
				
				XChangeProperty(display, w->handle, XdndAware, XA_ATOM, 32, PropModeReplace, (unsigned char *)&version, 1);
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
		
		void init() {
			//get display
			display = XOpenDisplay(NULL);
			visual = XDefaultVisualOfScreen(DefaultScreenOfDisplay(display));
			
			//query atoms
            XA_PRIMARY  =XInternAtom(display, "PRIMARY", False);
			XA_CLIPBOARD=XInternAtom(display, "CLIPBOARD", 1);
			XA_TIMESTAMP=XInternAtom(display, "TIMESTAMP", 1);
			XA_TARGETS  =XInternAtom (display, "TARGETS", 0);
			XA_PROTOCOLS=XInternAtom(display, "WM_PROTOCOLS", 0);
			XA_STRING   =XInternAtom(display, "STRING", 0);
			XA_UTF8_STRING   =XInternAtom(display, "UTF8_STRING", 0);
			XA_CARDINAL =XInternAtom(display, "CARDINAL", 0);
			XA_ATOM     =XInternAtom(display, "ATOM", 0);
			WM_DELETE_WINDOW = XInternAtom(display, "WM_DELETE_WINDOW", 0);
			XA_NET_FRAME_EXTENTS = XInternAtom(display, "_NET_FRAME_EXTENTS", 0);
			XA_NET_WORKAREA = XInternAtom(display, "_NET_WORKAREA", 0);
			XA_NET_REQUEST_FRAME_EXTENTS = XInternAtom(display, "_NET_REQUEST_FRAME_EXTENTS", 0);
            XA_NET_WM_STATE_MAXIMIZED_HORZ  =  XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_HORZ", False);
            XA_NET_WM_STATE_MAXIMIZED_VERT  =  XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_VERT", False);
            XA_NET_WM_STATE_HIDDEN  = XInternAtom(display, "_NET_WM_STATE_HIDDEN", False);
            XA_NET_ACTIVE_WINDOW = XInternAtom(display, "_NET_ACTIVE_WINDOW", False);
            
            XA_WM_NAME  = XInternAtom(display, "WM_NAME", False);
            XA_NET_WM_NAME  = XInternAtom(display, "_NET_WM_NAME", False);
            XA_NET_WM_STATE = XInternAtom(display, "_NET_WM_STATE", False);
            XA_NET_WM_STATE_FULLSCREEN = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", False);
            XA_STRUT = XInternAtom(display, "_NET_WM_STRUT_PARTIAL", False);
            XA_WM_CHANGE_STATE = XInternAtom(display, "WM_CHANGE_STATE", False);
            XA_NET_WM_ICON = XInternAtom(display, "_NET_WM_ICON", False);
            
            XdndAware         = XInternAtom(display, "XdndAware",         False);
            XdndSelection     = XInternAtom(display, "XdndSelection",     False);
            XdndStatus        = XInternAtom(display, "XdndStatus",     	  False);
            XdndTypeList      = XInternAtom(display, "XdndTypeList",      False);
            XdndEnter         = XInternAtom(display, "XdndEnter",         False);
            XdndFinished      = XInternAtom(display, "XdndFinished",      False);
            XdndPosition      = XInternAtom(display, "XdndPosition",      False);
            XdndLeave         = XInternAtom(display, "XdndLeave",         False);
            XdndDrop          = XInternAtom(display, "XdndDrop",          False);
            XdndActionCopy    = XInternAtom(display, "XdndActionCopy",    False);
            XdndActionMove    = XInternAtom(display, "XdndActionMove",    False);
            XA_Filelist       = XInternAtom(display, "text/uri-list",	  False);

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

        std::string GetAtomName(Atom atom) {
            if(!atom)
                return "[None]";
            else 
                return XGetAtomName(WindowManager::display, atom);
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

			XSetSelectionOwner (display, XA_CLIPBOARD, windowhandle, CurrentTime);
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
	
        Icon::Icon() {
            data = new internal::icondata;
        }
        
        Icon::Icon(const Containers::Image &image) {
            data = new internal::icondata;
            FromImage(image);
        }
        
        Icon::Icon(Icon &&icon) {
            data = new internal::icondata;
            std::swap(data, icon.data);
        }
        
        Icon &Icon::operator =(Icon &&icon) {
            Destroy();
            
            std::swap(data, icon.data);
            return *this;
        }
        
        void Icon::FromImage(const Containers::Image &image) {
			unsigned *img=new unsigned[2+image.GetWidth()*image.GetHeight()];
			
			img[0]=image.GetWidth();
			img[1]=image.GetHeight();
			
			image.CopyToBGRABuffer((Byte*)(img+2));
			
			data->w = image.GetWidth();
            data->h = image.GetHeight();
            
            data->data = (Byte*)img;
        }
        
        void Icon::Destroy() {
            if(data->data) {
                delete data->data;
                data->w = 0;
                data->h = 0;
            }
        }
        
        Icon::~Icon() {
            Destroy();
            
            delete data;
        }
	
		Event<> Monitor::ChangedEvent;
		Containers::Collection<Monitor> Monitor::monitors;
		Monitor *Monitor::primary=nullptr;
	}
	
	Window::Window(const WindowManager::Monitor &monitor, Geometry::Rectangle rect, const std::string &name, const std::string &title, bool allowresize, bool visible) : 
	data(new internal::windowdata) {
		
		this->name = name;
        this->allowresize = allowresize;
        pointerlayer = new Graphics::Layer;
        Add(pointerlayer);

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
					PropertyChangeMask |
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
        
        if(!allowresize) {
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
        }
		
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
                
                auto borders = WindowManager::GetX4Prop<Geometry::Margin>(WindowManager::XA_NET_FRAME_EXTENTS, data->handle, {0,0,0,0});
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
        data->ppoint=rect.TopLeft();

		createglcontext();
		glsize = rect.GetSize();
        
        WindowManager::internal::XdndInit(data);
	}
	
	Window::Window(const Gorgon::Window::FullscreenTag &, const WindowManager::Monitor &mon, const std::string &name, const std::string &title) : data(new internal::windowdata) {
		
		this->name = name;
        pointerlayer = new Graphics::Layer;
        Add(pointerlayer);

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
					PropertyChangeMask |
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
        data->ppoint=mon.GetLocation();
		
		Layer::Resize(mon.GetSize());

		createglcontext();
		glsize = mon.GetSize();
        
        WindowManager::internal::XdndInit(data);
	}
	
	void Window::Destroy() {
        if(data) {
            Close();
        }
        
		windows.Remove(this);

        delete pointerlayer;
        pointerlayer = nullptr;

        delete data;
        data = nullptr;
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
        if(iswmpointer) {
            if(data->pointerdisplayed) {
                data->pointerdisplayed=false;
                XDefineCursor(WindowManager::display, data->handle, WindowManager::blank_cursor);
                XFlush(WindowManager::display);
            }
        }
        else {
            pointerlayer->Clear();
            pointerlayer->Hide();
        }
		showptr = false;
	}
		
	void Window::ShowPointer() {
        if(iswmpointer) {
            if(!data->pointerdisplayed) {
                data->pointerdisplayed=true;
                XDefineCursor(WindowManager::display, data->handle, 0);
                XFlush(WindowManager::display);
            }
        }
        else {
            pointerlayer->Show();
        }
		showptr = true;
	}
	
	void Window::Move(const Geometry::Point &location) {
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
        if(!allowresize) {
            XSizeHints *sizehints=XAllocSizeHints();
            sizehints->min_width=size.Width;
            sizehints->max_width=size.Width;
            sizehints->min_height=size.Height;
            sizehints->max_height=size.Height;
            sizehints->flags=PMinSize | PMaxSize;
            XSetWMNormalHints(WindowManager::display, data->handle, sizehints);
            XFree(sizehints);
        }

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
                    else if(event.xclient.message_type==WindowManager::XdndEnter) {
                        unsigned long len, bytes, dummy;
                        unsigned char *data=NULL;
                        Atom type;
                        int format;
                        
                        cursorover = true;
                        
                        std::vector<Atom> atoms;
                        
                        for(int i=2; i<=4; i++)
                            if(event.xclient.data.l[i] != internal::None)
                                atoms.push_back(event.xclient.data.l[i]);
                        
                        if(event.xclient.data.l[1] & 1) {
                            //get the length
                            XGetWindowProperty(WindowManager::display, event.xclient.data.l[0], 
                                               WindowManager::XdndTypeList, 0, 0, 0, AnyPropertyType, 
                                               &type, &format, &len, &bytes, &data);
                            
                            //read the data
                            XGetWindowProperty(WindowManager::display, event.xclient.data.l[0], 
                                            WindowManager::XdndTypeList, 0,bytes,0,
                                            AnyPropertyType, &type, &format,
                                            &len, &dummy, &data);
                       
                            Atom *atomlist=(Atom*)data;
                            for(int i=0;i<(int)bytes/4;i++) {
                                if(atomlist[i] != internal::None)
                                    atoms.push_back(atomlist[i]);
                            }
                        }
                       
                        for(auto atom : atoms) {
                            std::cout<<WindowManager::GetAtomName(atom)<<std::endl;
                            
                            if(atom==WindowManager::XA_Filelist) {
                                /*gge::Pointers.Hide();*/
                                
                                XEvent ev;
                                auto dragdata=new FileData();

                                XConvertSelection(WindowManager::display, WindowManager::XdndSelection, WindowManager::XA_Filelist, WindowManager::XA_PRIMARY, this->data->handle, event.xclient.data.l[0]);
                                XFlush(WindowManager::display);
                                
                                XIfEvent(WindowManager::display, &ev, WindowManager::waitfor_selectionnotify, (char*)this->data->handle);
                                
                                /*Atom type;
                                unsigned long len, bytes = 0, dummy;
                                unsigned char *data;
                                int format;*/
                                
                                bytes = 0;

                                if(ev.xselection.property != internal::None)
                                    XGetWindowProperty(WindowManager::display, this->data->handle, ev.xselection.property, 0, 0, 0, AnyPropertyType, &type, &format, &len, &bytes, &data);

                                if(bytes) {
                                    XGetWindowProperty (WindowManager::display, this->data->handle, 
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
                                                
                                                dragdata->AddFile(Encoding::URIDecode(s));
                                                p=i+1;
                                            }
                                        }
                                    }
                                    
                                    XFree(data);

                                    auto &drag = Input::PrepareDrag();
                                    drag.AssumeData(*dragdata);
                                    Input::StartDrag();
                                }
                                else {
                                }
                            }
                            else if(atom==WindowManager::XA_STRING) {
                                /*gge::Pointers.Hide();*/
                                XEvent ev;
                                
                                XConvertSelection(WindowManager::display, WindowManager::XdndSelection, WindowManager::XA_STRING, WindowManager::XA_PRIMARY, this->data->handle, event.xclient.data.l[0]);
                                XFlush(WindowManager::display);
                                
                                
                                XIfEvent(WindowManager::display, &ev, WindowManager::waitfor_selectionnotify, (char*)this->data->handle);
                                
                                if (ev.xselection.property != (unsigned)internal::None) {
                                    Atom type;
                                    unsigned long len, bytes, dummy;
                                    unsigned char *data;
                                    int format;
                                
                                    XGetWindowProperty(WindowManager::display, this->data->handle, ev.xselection.property, 0, 0, 0, AnyPropertyType, &type, &format, &len, &bytes, &data);

                                    if(bytes) {
                                        XGetWindowProperty (WindowManager::display, this->data->handle, 
                                                ev.xselection.property, 0,bytes,0,
                                                AnyPropertyType, &type, &format,
                                                &len, &dummy, &data);
                                        
                                        std::string tmp((char*)data, bytes);
                                        XFree(data);
                                        
                                        Input::BeginDrag(tmp);
                                    }
                                }
                            }
                        }
                    }
                    else if(event.xclient.message_type==WindowManager::XdndPosition) {
                        XClientMessageEvent m;
                        memset(&m, sizeof(m), 0);
                        m.type = ClientMessage;
                        m.display = event.xclient.display;
                        m.window = event.xclient.data.l[0];
                        m.message_type = WindowManager::XdndStatus;
                        m.format=32;
                        m.data.l[0] = data->handle;
                        m.data.l[1] = 1;//gge::input::mouse::HasDragTarget() && gge::input::mouse::IsDragging();
                        m.data.l[2] = 0;
                        m.data.l[3] = 0;
                        
                        if(!Input::IsDragging()) { 
                            m.data.l[4] = internal::None;
                        }
                        else {
                            auto &drag = Input::GetDragOperation();
                            
                            if(drag.HasData(Resource::GID::File)) {
                                auto &data=dynamic_cast<FileData&>(drag.GetData(Resource::GID::File));

                                if(data.Action==data.Move)
                                    m.data.l[4] = WindowManager::XdndActionMove;
                                else
                                    m.data.l[4] = WindowManager::XdndActionCopy;
                            }
                            else if(drag.HasData(Resource::GID::Text)) {
                                    m.data.l[4] = WindowManager::XdndActionCopy;
                            }
                            else {
                                m.data.l[4] = internal::None;
                            }
                        }                        

                        XSendEvent(WindowManager::display, event.xclient.data.l[0], False, NoEventMask, (XEvent*)&m);
                        XFlush(WindowManager::display);
                        //gge::Pointers.Hide();
                    }
                    else if(event.xclient.message_type == WindowManager::XdndLeave) {
                        cursorover = true;

                        Input::CancelDrag();
                        //gge::Pointers.Show();
                    }
                    else if(event.xclient.message_type == WindowManager::XdndDrop) {                        
                        if(!Input::IsDragging())
                            return;
                        
                        auto &drag = Input::GetDragOperation();

                        
                        if(drag.HasData(Resource::GID::File)) {
                            XClientMessageEvent m;
                            memset(&m, sizeof(m), 0);
                            m.type = ClientMessage;
                            m.display = WindowManager::display;
                            m.window = event.xclient.data.l[0];
                            m.message_type = WindowManager::XdndFinished;
                            m.format=32;
                            m.data.l[0] = this->data->handle;
                            m.data.l[1] = 1;
                            m.data.l[2] = WindowManager::XdndActionCopy; //We only ever copy.

                            //Reply that all is well.
                            XSendEvent(WindowManager::display, event.xclient.data.l[0], False, NoEventMask, (XEvent*)&m);
                        }
                        else if(drag.HasData(Resource::GID::Text)) {                               
                            XClientMessageEvent m;
                            memset(&m, sizeof(m), 0);
                            m.type = ClientMessage;
                            m.display = WindowManager::display;
                            m.window = event.xclient.data.l[0];
                            m.message_type = WindowManager::XdndFinished;
                            m.format=32;
                            m.data.l[0] = this->data->handle;
                            m.data.l[1] = 1;
                            m.data.l[2] = WindowManager::XdndActionCopy; //We only ever copy.

                            //Reply that all is well.
                            XSendEvent(WindowManager::display, event.xclient.data.l[0], False, NoEventMask, (XEvent*)&m);
                            
                            Input::Drop(mouselocation);
                        }
                        //gge::Pointers.Show();
                    }
				} // Client Message
				break;
                
                case EnterNotify:
                    cursorover = true;
                    break;
                    
                case ConfigureNotify: {
                    auto xce = event.xconfigure;
                    
                    if(GetSize().Width != xce.width || GetSize().Height != xce.height) {
                        Layer::Resize({(int)xce.width, (int)xce.height});
                        
						activatecontext();
                        GL::Resize({(int)xce.width, (int)xce.height});
                        
                        ResizedEvent();
                    }
                    else {
                        int x, y;
                        ::Window r;
                        XTranslateCoordinates(WindowManager::display, data->handle, RootWindow(WindowManager::display, XDefaultScreen(WindowManager::display)), 
                            xce.x, xce.y, &x, &y, &r
                        );
                        
                        if(data->ppoint.X != x || data->ppoint.Y != y ) {
                            data->ppoint = {x, y};

                            MovedEvent();
                        }
                    }
                }
                break;
                    
                case LeaveNotify:
                    cursorover = false;
                    break;
					
				case FocusIn: {
                    XEvent nextevent;
                    nextevent.type = 0;
                    if(XEventsQueued(WindowManager::display, QueuedAfterReading))
                        XPeekEvent(WindowManager::display, &nextevent);
                
                    if(nextevent.type == FocusOut) {
                        XNextEvent(WindowManager::display, &nextevent);
                    }
                    
                    if(!data->focused) {
                        FocusedEvent();
                        data->focused = true;
                    }
                }
				break;
					
				case FocusOut: {
                    XEvent nextevent;
                    nextevent.type = 0;
                    if(XEventsQueued(WindowManager::display, QueuedAfterReading))
                        XPeekEvent(WindowManager::display, &nextevent);
                
                    if(nextevent.type == FocusIn) {
                        XNextEvent(WindowManager::display, &nextevent);
                    }
                    
                    if(data->focused) {
                        LostFocusEvent();
                        data->focused = false;
                    }
                }
				break;
                
                case SelectionRequest:
                    XEvent respond;
                    
                    if(event.xselectionrequest.target==WindowManager::XA_STRING && event.xselectionrequest.selection==WindowManager::XA_CLIPBOARD) {
                    
                        XChangeProperty (WindowManager::display,
                            event.xselectionrequest.requestor,
                            event.xselectionrequest.property,
                            WindowManager::XA_STRING,
                            8,
                            PropModeReplace,
                            (unsigned char*) &WindowManager::copiedtext[0],
                            (int)WindowManager::copiedtext.length());
                        respond.xselection.property=event.xselectionrequest.property;
                    }
                    else if(event.xselectionrequest.target==WindowManager::XA_TARGETS && event.xselectionrequest.selection==WindowManager::XA_CLIPBOARD) {
                        Atom supported[]={WindowManager::XA_STRING};
                        XChangeProperty (WindowManager::display,
                            event.xselectionrequest.requestor,
                            event.xselectionrequest.property,
                            WindowManager::XA_TARGETS,
                            8,
                            PropModeReplace,
                            (unsigned char *)(&supported),
                            sizeof(supported)
                        );
                    }
                    else {
                        respond.xselection.property= 0;
                    }
                    
                    respond.xselection.type= SelectionNotify;
                    respond.xselection.display= event.xselectionrequest.display;
                    respond.xselection.requestor= event.xselectionrequest.requestor;
                    respond.xselection.selection=event.xselectionrequest.selection;
                    respond.xselection.target= event.xselectionrequest.target;
                    respond.xselection.time = event.xselectionrequest.time;
                    XSendEvent (WindowManager::display, event.xselectionrequest.requestor,0,0,&respond);
                    XFlush (WindowManager::display);
                    break;
                    
                    
                case PropertyNotify:
                    if(event.xproperty.atom == WindowManager::XA_NET_WM_STATE) {
                        bool minstate = IsMinimized();
                        
                        if(minstate && !data->min) {
                            MinimizedEvent();
                            data->min = true;
                        }
                        else if(!minstate && data->min) {
                            RestoredEvent();
                            data->min = false;
                        }
                    }
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
        XDestroyWindow(WindowManager::display, data->handle);
        data->handle = 0;
        
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

		Graphics::Initialize();

		GL::SetupContext(bounds.GetSize());

		// test code
		glXSwapBuffers(WindowManager::display, data->handle);		
		XFlush(WindowManager::display);
	}

	void Window::SetTitle(const std::string &title) {	
		XStoreName(WindowManager::display, data->handle, title.c_str());
        XFlush(WindowManager::display);
	}

	std::string Window::GetTitle() const {
        Atom type;
        int format;
        unsigned long len, remainder;
        
        Byte *prop;
        
        XGetWindowProperty(WindowManager::display, data->handle, WindowManager::XA_NET_WM_NAME, 0, 1024,
                           False, WindowManager::XA_UTF8_STRING, &type, &format, &len, &remainder, &prop);
        
        if(!prop) {
            XGetWindowProperty(WindowManager::display, data->handle, WindowManager::XA_WM_NAME, 0, 1024,
                            False, AnyPropertyType, &type, &format, &len, &remainder, &prop);
        }
        
        std::string ret((char*)prop, len);
        XFree(prop);
        
		return ret;
	}

	bool Window::IsClosed() const {
		return data->handle == 0;
	}

	Geometry::Bounds Window::GetExteriorBounds() const {
        auto borders = WindowManager::GetX4Prop<Geometry::Margin>(WindowManager::XA_NET_FRAME_EXTENTS, data->handle, {0,0,0,0});
        std::swap(borders.Top, borders.Right);
        
        ::Window r;
        int x, y;
        unsigned w, h, bw, d;
        
        XGetGeometry(WindowManager::display, data->handle, &r, &x, &y, &w, &h, &bw, &d);
        
        return Geometry::Bounds(data->ppoint.X, data->ppoint.Y, data->ppoint.X+w, data->ppoint.Y+h) + borders;
    }

	void Window::Focus() {
        XClientMessageEvent ev;
        std::memset (&ev, 0, sizeof ev);
        ev.type = ClientMessage;
        ev.window = data->handle;
        ev.message_type = WindowManager::XA_NET_ACTIVE_WINDOW;
        ev.format = 32;
        ev.data.l[0] = 1;
        ev.data.l[1] = CurrentTime;
        XSendEvent (WindowManager::display, RootWindow(WindowManager::display, XDefaultScreen(WindowManager::display)), False, SubstructureRedirectMask | SubstructureNotifyMask, (XEvent*)&ev);
        XFlush (WindowManager::display);  
    }

	bool Window::IsFocused() const {
        ::Window focused;
        int r;

        XGetInputFocus(WindowManager::display, &focused, &r);
        
		return focused == data->handle;
	}

	void Window::Minimize() {
        XIconifyWindow(WindowManager::display, data->handle, 0);
        XFlush(WindowManager::display);
	}

	void Window::Maximize() {
        if(!allowresize) {
            XSizeHints *sizehints=XAllocSizeHints();
            sizehints->max_width=INT_MAX;
            sizehints->max_height=INT_MAX;
            sizehints->flags=PMaxSize;
            XSetWMNormalHints(WindowManager::display, data->handle, sizehints);	
            XFree(sizehints);
        
            XFlush(WindowManager::display);
            
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        XEvent xev;

        memset(&xev, 0, sizeof(xev));
        xev.type = ClientMessage;
        xev.xclient.window = data->handle;
        xev.xclient.message_type = WindowManager::XA_NET_WM_STATE;
        xev.xclient.format = 32;
        xev.xclient.data.l[0] = WindowManager::XA_NET_WM_STATE_ADD;
        xev.xclient.data.l[1] = WindowManager::XA_NET_WM_STATE_MAXIMIZED_HORZ;
        xev.xclient.data.l[2] = WindowManager::XA_NET_WM_STATE_MAXIMIZED_VERT;

        XSendEvent(WindowManager::display, DefaultRootWindow(WindowManager::display), False, SubstructureNotifyMask, &xev);
        XFlush(WindowManager::display);
        
        ::Window r;
        int x, y;
        unsigned w, h, bw, d;
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10)); //wait for a short time to ensure window frame is ready.
       
        XGetGeometry(WindowManager::display, data->handle, &r, &x, &y, &w, &h, &bw, &d);
        
        if(!allowresize) {
            XSizeHints *sizehints=XAllocSizeHints();
            sizehints->min_width=w;
            sizehints->max_width=w;
            sizehints->min_height=h;
            sizehints->max_height=h;
            sizehints->flags=PMinSize | PMaxSize;
            XSetWMNormalHints(WindowManager::display, data->handle, sizehints);	
            XFlush(WindowManager::display);
            XFree(sizehints);
        }
	}

	void Window::Restore() {
        Atom type;
        int format;
        unsigned long count, b;
        unsigned char *properties = NULL;
        
        bool min = false, max = false;
    
        ::XGetWindowProperty(WindowManager::display, data->handle, WindowManager::XA_NET_WM_STATE, 0, LONG_MAX, False, AnyPropertyType, &type, &format, &count, &b, &properties);
        for(int i=0; i<count; i++) {
            auto prop = reinterpret_cast<unsigned long *>(properties)[i];
            if(prop == WindowManager::XA_NET_WM_STATE_HIDDEN)
                min = true;
            else if(prop == WindowManager::XA_NET_WM_STATE_MAXIMIZED_HORZ || prop == WindowManager::XA_NET_WM_STATE_MAXIMIZED_VERT)
                max = true;
        }
            

		if(min) {
            Focus();
		}
		else if(max) {
            if(!allowresize) {
                XSizeHints *sizehints=XAllocSizeHints();
                sizehints->min_width=0;
                sizehints->min_height=0;
                sizehints->flags=PMinSize;
                XSetWMNormalHints(WindowManager::display, data->handle, sizehints);	
                XFree(sizehints);
                
                XFlush(WindowManager::display);
                
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            
            XEvent xev;

            memset(&xev, 0, sizeof(xev));
            xev.type = ClientMessage;
            xev.xclient.window = data->handle;
            xev.xclient.message_type = WindowManager::XA_NET_WM_STATE;
            xev.xclient.format = 32;
            xev.xclient.data.l[0] = 0;      
            xev.xclient.data.l[1] = WindowManager::XA_NET_WM_STATE_MAXIMIZED_HORZ;
            xev.xclient.data.l[2] = WindowManager::XA_NET_WM_STATE_MAXIMIZED_VERT;

            XSendEvent(WindowManager::display, DefaultRootWindow(WindowManager::display), False, SubstructureNotifyMask, &xev);
            XFlush(WindowManager::display);
            
            ::Window r;
            int x, y;
            unsigned w, h, bw, d;
            
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); //wait for a short time to ensure window frame is ready.
        
            XGetGeometry(WindowManager::display, data->handle, &r, &x, &y, &w, &h, &bw, &d);
            
            if(!allowresize) {
                XSizeHints *sizehints=XAllocSizeHints();
                sizehints->min_width=w;
                sizehints->max_width=w;
                sizehints->min_height=h;
                sizehints->max_height=h;
                sizehints->flags=PMinSize | PMaxSize;
                XSetWMNormalHints(WindowManager::display, data->handle, sizehints);	
                XFlush(WindowManager::display);
                XFree(sizehints);            
            }
        }
        XFree(properties);
	}

	bool Window::IsMinimized() const {
        Atom type;
        int format;
        unsigned long count, b;
        unsigned char *properties = NULL;
        
        bool min = false;
    
        ::XGetWindowProperty(WindowManager::display, data->handle, WindowManager::XA_NET_WM_STATE, 0, LONG_MAX, False, AnyPropertyType, &type, &format, &count, &b, &properties);
        for(int i=0; i<count; i++) {
            auto prop = reinterpret_cast<unsigned long *>(properties)[i];
            if(prop == WindowManager::XA_NET_WM_STATE_HIDDEN)
                min = true;
        }
        XFree(properties);
        
        return min;
	}

	bool Window::IsMaximized() const {
        Atom type;
        int format;
        unsigned long count, b;
        unsigned char *properties = NULL;
        
        bool min = false, max = false;
    
        ::XGetWindowProperty(WindowManager::display, data->handle, WindowManager::XA_NET_WM_STATE, 0, LONG_MAX, False, AnyPropertyType, &type, &format, &count, &b, &properties);
        for(int i=0; i<count; i++) {
            auto prop = reinterpret_cast<unsigned long *>(properties)[i];
            
            if(prop == WindowManager::XA_NET_WM_STATE_MAXIMIZED_HORZ || prop == WindowManager::XA_NET_WM_STATE_MAXIMIZED_VERT)
                max = true;
        }
        XFree(properties);
        
        return max;
	}
	
	void Window::AllowResize() {
        XSizeHints *sizehints=XAllocSizeHints();
        sizehints->min_width=0;
        sizehints->max_width=INT_MAX;
        sizehints->min_height=0;
        sizehints->max_height=INT_MAX;
        sizehints->flags=PMinSize | PMaxSize;
        XSetWMNormalHints(WindowManager::display, data->handle, sizehints);	
        XFlush(WindowManager::display);
        XFree(sizehints);
    }
	
	void Window::PreventResize() {
        auto sz = GetSize();
        XSizeHints *sizehints=XAllocSizeHints();
        sizehints->min_width=sz.Width;
        sizehints->max_width=sz.Width;
        sizehints->min_height=sz.Height;
        sizehints->max_height=sz.Height;
        sizehints->flags=PMinSize | PMaxSize;
        XSetWMNormalHints(WindowManager::display, data->handle, sizehints);	
        XFlush(WindowManager::display);
        XFree(sizehints);
    }

    void Window::SetIcon(const WindowManager::Icon& icon) {
        XChangeProperty(WindowManager::display, data->handle, WindowManager::XA_NET_WM_ICON, WindowManager::XA_CARDINAL , 32, PropModeReplace, icon.data->data, icon.data->w*icon.data->h+2);
        XSync(WindowManager::display, 1);
    }

	void Window::updatedataowner() {
	}
    
}
