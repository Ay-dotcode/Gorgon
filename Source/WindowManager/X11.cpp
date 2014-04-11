#include "../WindowManager.h"


#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <string.h>


namespace Gorgon { namespace WindowManager {
	
	namespace internal {
	
		struct windowdata {
			::Window handle = 0;
		};
		
	}
	
	/// @cond INTERNAL
	/// X11 display information
	Display *display = nullptr;
	
	/// Depends on monitor, might be moved
	Visual  *visual  = nullptr;

	/// Blank cursor to remove WindowManager cursor
	Cursor blank_cursor;
	
	///@{ X11 atoms for various data identifiers
	Atom XA_CLIPBOARD;
	Atom XA_TIMESTAMP;
	Atom XA_TARGETS;
	Atom XA_PROTOCOLS;
	Atom WM_DELETE_WINDOW;
	///@}
		
	///@{ waits for specific events
	static int waitfor_mapnotify(Display *d, XEvent *e, char *arg) {
		return (e->type == MapNotify) && (e->xmap.window == (::Window)arg);
	}
	
	static int waitfor_selectionnotify(Display *d, XEvent *e, char *arg) {
		return (e->type == SelectionNotify);
	}
	///@}

	/// @endcond
	
	Geometry::Rectangle UsableScreenRegion(int monitor) {
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
		WM_DELETE_WINDOW = XInternAtom(display, "WM_DELETE_WINDOW", 0);
		

		char data[1]={0};
		XColor dummy;
		Pixmap blank = XCreateBitmapFromData (display, DefaultRootWindow(display), data, 1, 1);
		blank_cursor = XCreatePixmapCursor(display, blank, blank, &dummy, &dummy, 0, 0);
		XFreePixmap (display, blank);
	}
	
	
	
	Window::Window(const Geometry::Rectangle &rect, const std::string &title, bool visible, bool useoutermetrics) : 
	data(new internal::windowdata) {
		//using defaults
		int screen = DefaultScreen(display);
		int depth  = DefaultDepth(display,screen); 
		
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
		
// 		Colormap cmap = XCreateColormap(display, RootWindow(display, screen), visual, AllocNone);
// 		attributes.colormap = cmap;
		
		data->handle = XCreateWindow(display, XRootWindow(display,screen),
                               rect.X,rect.Y, rect.Width,rect.Height,
							   0, depth, InputOutput, 
							   visual, CWEventMask, &attributes);
		
		XClassHint *classhint=XAllocClassHint();
		classhint->res_name=(char*)malloc(title.length()+1);
		strcpy(classhint->res_name, title.c_str());
		classhint->res_class=(char*)malloc(title.length()+1);
		strcpy(classhint->res_class, title.c_str());
		XSetClassHint(display, data->handle, classhint);
		XFree(classhint);
		
		XStoreName(display, data->handle, (char*)title.c_str());
		
		XEvent event;
		
		if(visible) {
			XMapWindow(display,data->handle);
			XIfEvent(display, &event, &waitfor_mapnotify, (char*)data->handle);
		}
	}
	
	Window::Window() : data(new internal::windowdata) {
		
	}
	
	Window::~Window() {
		delete data;
	}
	
} }
