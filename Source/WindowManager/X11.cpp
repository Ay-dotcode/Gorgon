#include "../WindowManager.h"
#include "../Window.h"
#include "../Time.h"


#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <string.h>


namespace Gorgon { 

	namespace internal {
	
		struct windowdata {
			::Window handle = 0;
			bool pointerdisplayed=true;
		};
		
	}

	namespace WindowManager {
		
		/// @cond INTERNAL
		/// X11 display information
		Display *display = nullptr;
		
		/// Depends on monitor, might be moved
		Visual  *visual  = nullptr;

		/// Blank cursor to remove WindowManager cursor
		Cursor blank_cursor;
		
		/// Copied text
		std::string copiedtext;
		
		///@{ X11 atoms for various data identifiers
		Atom XA_CLIPBOARD;
		Atom XA_TIMESTAMP;
		Atom XA_TARGETS;
		Atom XA_PROTOCOLS;
		Atom WM_DELETE_WINDOW;
		Atom XA_STRING;
		///@}
			
		///@{ waits for specific events
		static int waitfor_mapnotify(Display *d, XEvent *e, char *arg) {
			return (e->type == MapNotify) && (e->xmap.window == (::Window)arg);
		}
		
		static int waitfor_selectionnotify(Display *d, XEvent *e, char *arg) {
			return (e->type == SelectionNotify);
		}
		namespace internal {
			Gorgon::internal::windowdata *getdata(const Window &w) {
				return w.data;
			}
		}
		///@}

		/// @endcond
		
		Geometry::Rectangle GetUsableScreenRegion(int monitor) {
			Geometry::Rectangle rect;
			rect.X=0;
			rect.Y=0;
			
			Screen  *screen  = XScreenOfDisplay(display, monitor);
			rect.Width =XWidthOfScreen(screen);
			rect.Height=XHeightOfScreen(screen);
			
			return rect;
		}
		
		Geometry::Rectangle GetScreenRegion(int monitor) {
			Geometry::Rectangle rect;
			rect.X=0;
			rect.Y=0;
			
			Screen  *screen  = XScreenOfDisplay(display, monitor);
			rect.Width =XWidthOfScreen(screen);
			rect.Height=XHeightOfScreen(screen);
			
			return rect;
		}
		
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
			WM_DELETE_WINDOW = XInternAtom(display, "WM_DELETE_WINDOW", 0);
			

			char data[1]={0};
			XColor dummy;
			Pixmap blank = XCreateBitmapFromData (display, DefaultRootWindow(display), data, 1, 1);
			blank_cursor = XCreatePixmapCursor(display, blank, blank, &dummy, &dummy, 0, 0);
			XFreePixmap (display, blank);
		}

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
			return;
#else
				throw std::runtime_error("Cannot copy without a window, if necessary create a hidden window");
#endif
			}
			
			XEvent event;
			
			XConvertSelection (display, XA_CLIPBOARD, XA_STRING, None, windowhandle, CurrentTime);
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
	}


	
	Window::Window(Geometry::Rectangle rect, const std::string &name, const std::string &title, bool visible, bool useoutermetrics) : 
	data(new internal::windowdata) {
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
		
		if(rect.TopLeft()==automaticplacement) {
			rect.Move( (WindowManager::GetUsableScreenRegion()-rect.GetSize()).Center() );
		}
		
		data->handle = XCreateWindow(WindowManager::display, 
			XRootWindow(WindowManager::display,screen),
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
		sizehints->flags=PMinSize | PMaxSize;
		XSetWMNormalHints(WindowManager::display, data->handle, sizehints);		
		XFree(sizehints);
		
		XSetWMProtocols(WindowManager::display, data->handle, &WindowManager::WM_DELETE_WINDOW, 1);
		XFlush(WindowManager::display);
		
		XEvent event;
		
		if(visible) {
			XMapWindow(WindowManager::display,data->handle);
			XIfEvent(WindowManager::display, &event, &WindowManager::waitfor_mapnotify, (char*)data->handle);
		
			XMoveWindow(WindowManager::display, data->handle, rect.X, rect.Y);
			XFlush(WindowManager::display);
		}
	}
	
	Window::Window(const FullscreenTag &, const std::string &name, const std::string &title) : data(new internal::windowdata) {
		
	}
	
	Window::~Window() {
		delete data;
		windows.Remove(this);
	}
	
	void Window::Hide() {
		XUnmapWindow(WindowManager::display, data->handle);
	}
	
	void Window::Show() {
		XEvent event;

		XMapWindow(WindowManager::display, data->handle);
		XFlush(WindowManager::display);
		XIfEvent(WindowManager::display, &event, WindowManager::waitfor_mapnotify, (char*)data->handle);
		XRaiseWindow(WindowManager::display, data->handle);
		XFlush(WindowManager::display);
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
		XMoveWindow(WindowManager::display, data->handle, location.X, location.Y);
		XFlush(WindowManager::display);
	}
	
	void Window::Resize(const Geometry::Size &size) {
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
	
	void Window::processmessages() {
	}
	
}
