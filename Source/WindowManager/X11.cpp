#include "../WindowManager.h"
#include "../Window.h"
#include "../Time.h"
#include "../OS.h"


#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <string.h>
#include <unistd.h>

#include <GL/glx.h>

namespace Gorgon { 

	namespace internal {
	
		struct windowdata {
			::Window handle = 0;
			bool pointerdisplayed=true;
			bool move=false;
			Geometry::Point moveto;
			bool ismapped=false;
			GLXContext context=0;
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
		Atom XA_NET_FRAME_EXTENTS;
		Atom XA_NET_WORKAREA;
		Atom XA_NET_REQUEST_FRAME_EXTENTS;
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
		}
		///@}

		/// @endcond
		
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
		
		Geometry::Rectangle GetUsableScreenRegion(int monitor) {
			Screen  *screen_data  = XScreenOfDisplay(display, monitor);
			int screen = DefaultScreen(WindowManager::display);
			
			Geometry::Rectangle rect{0, 0, XWidthOfScreen(screen_data), XHeightOfScreen(screen_data)};
			
			return GetX4Prop(XA_NET_WORKAREA, XRootWindow(display, screen), rect);
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
			XA_NET_FRAME_EXTENTS = XInternAtom(display, "_NET_FRAME_EXTENTS", 0);
			XA_NET_WORKAREA = XInternAtom(display, "_NET_WORKAREA", 0);
			XA_NET_REQUEST_FRAME_EXTENTS = XInternAtom(display, "_NET_REQUEST_FRAME_EXTENTS", 0);
			

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
	
	Window::Window(Geometry::Rectangle rect, const std::string &name, const std::string &title, bool visible) : 
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
		
		bool autoplaced=false;
		if(rect.TopLeft()==automaticplacement) {
			rect.Move( (WindowManager::GetUsableScreenRegion()-rect.GetSize()).Center() );
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
		sizehints->flags=PMinSize | PMaxSize;
		XSetWMNormalHints(WindowManager::display, data->handle, sizehints);		
		XFree(sizehints);
		
		XSetWMProtocols(WindowManager::display, data->handle, &WindowManager::WM_DELETE_WINDOW, 1);

		//!Replace with margins
		//auto extents=WindowManager::GetX4Prop<Geometry::Bounds>(WindowManager::XA_NET_FRAME_EXTENTS, data->handle, {0,0,0,0});
		
		
		if(visible) {
			XEvent event;
			
			XFlush(WindowManager::display);
			
			XMapWindow(WindowManager::display,data->handle);
			XIfEvent(WindowManager::display, &event, &WindowManager::waitfor_mapnotify, (char*)data->handle);
	
			XMoveWindow(WindowManager::display, data->handle, rect.X, rect.Y);
			XFlush(WindowManager::display);			
		}
		else {
			data->move=true;
			data->moveto=rect.TopLeft();
		}
		
		data->ismapped=visible;
		
		createglcontext();
	}
	
	Window::Window(const FullscreenTag &, const std::string &name, const std::string &title) : data(new internal::windowdata) {
		
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

	void Window::processmessages() {
		XEvent event;

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
							Keyboard::CurrentModifier.Add(Keyboard::Modifier::Shift);
							break;
							
						case XK_Control_L:
						case XK_Control_R:
							Keyboard::CurrentModifier.Add(Keyboard::Modifier::Ctrl);
							break;
							
						case XK_Alt_L:
							Keyboard::CurrentModifier.Add(Keyboard::Modifier::Alt);
							break;
							
						case XK_Alt_R:
							Keyboard::CurrentModifier.Add(Keyboard::Modifier::AltGr);
							break;
							
						case XK_Super_L:
						case XK_Super_R:
							Keyboard::CurrentModifier.Add(Keyboard::Modifier::Meta);
							break;
					}
					
					if(KeyEvent(key, +1.f)) break;
					
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
				} //Keypress
				break;
					
					
				case KeyRelease: {
					if(XEventsQueued(WindowManager::display, QueuedAfterReading)) {
						XEvent nextevent;
						XPeekEvent(WindowManager::display, &nextevent);
						if(nextevent.type == KeyPress && nextevent.xkey.time == event.xkey.time && 
							nextevent.xkey.keycode == event.xkey.keycode) {
							
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
					
							XNextEvent(WindowManager::display, &nextevent);
							break;
						}
					}
					
					key=XLookupKeysym(&event.xkey,0);
					
					//modifiers
					switch(key) {
						case XK_Shift_L:
						case XK_Shift_R:
							Keyboard::CurrentModifier.Remove(Keyboard::Modifier::Shift);
							break;
							
						case XK_Control_L:
						case XK_Control_R:
							Keyboard::CurrentModifier.Remove(Keyboard::Modifier::Ctrl);
							break;
							
						case XK_Alt_L:
							Keyboard::CurrentModifier.Remove(Keyboard::Modifier::Alt);
							break;
							
						case XK_Alt_R:
							Keyboard::CurrentModifier.Remove(Keyboard::Modifier::AltGr);
							break;
							
						case XK_Super_L:
						case XK_Super_R:
							Keyboard::CurrentModifier.Remove(Keyboard::Modifier::Meta);
							break;
					}
					
					
					if(KeyEvent(key, 0.f)) break;
					
				} //Keypress
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
			None
		};
		
		XVisualInfo *vi = glXChooseVisual(WindowManager::display, DefaultScreen(WindowManager::display), attributeListDbl);
		
		GLXContext prev=0;
		for(auto &w : windows) {
			if(w.data->context!=0) {
				prev=w.data->context;
			}
		}
		
		data->context = glXCreateContext(WindowManager::display, vi, prev, GL_TRUE);
		
		glXMakeCurrent(WindowManager::display, data->handle, data->context);

		if(data->context==0) {
			OS::DisplayMessage("OpenGL context creation failed");
			exit(1);
		}
		
		std::string gl_version(reinterpret_cast<const char*>(glGetString(GL_VERSION)));
		if(String::To<float>(gl_version)<3.0) {
			OS::DisplayMessage("OpenGL version 3.0 and above is required. Your OpenGL version is "+gl_version);
			exit(2);
		}
		
		glShadeModel(GL_SMOOTH);
		glClearColor(0.4f, 0.2f, 0.0f, 1.0f);
		glColor4f(1.0f,1.0f,1.0f,1.0f);
		
		glClearDepth(1.0f);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		
		glEnable(GL_TEXTURE_2D);
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		
		glViewport(0, 0, bounds.Width(), bounds.Height());
		
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glFinish();
		glXSwapBuffers(WindowManager::display, data->handle);
		
		XFlush(WindowManager::display);
	}
}
