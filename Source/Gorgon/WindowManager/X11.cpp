#include "../WindowManager.h"
#include <thread>
#include "../Geometry/Margin.h"
#include <limits.h>
#include "../Window.h"
#include "../Time.h"
#include "../OS.h"
#include "../Graphics.h"
#include "../Utils/Assert.h"

#include "../Graphics/Layer.h"

#include "../Encoding/URI.h"
#include "../Input/DnD.h"
#include "../Containers/Vector.h"
#include "../Encoding/PNG.h"
#include "../Encoding/JPEG.h"
#include "../IO/MemoryStream.h"
#include "../Any.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xrandr.h>
#include <X11/XKBlib.h>
#include <X11/extensions/Xinerama.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <GL/glx.h>

#include "X11Keysym.h"


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
            
            struct xdnddata {
                bool filelist = false;
                bool utf8 = false;
                bool string = false;
                
                bool requested = false;
                int  drop = 0;
                
                bool localpointer = false;
            } xdnd;
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
		
		///@{ X11 atoms for various data identifiers
		Atom XA_CLIPBOARD;
		Atom XA_TIMESTAMP;
		Atom XA_TARGETS;
		Atom XA_PROTOCOLS;
		Atom WM_DELETE_WINDOW;
		Atom XA_TEXT;
		Atom XA_STRING;
        Atom XA_UTF8_STRING;
        Atom XA_TEXT_HTML;
        Atom XA_URL;
        Atom XA_PNG;
        Atom XA_JPG;
        Atom XA_BMP;
		Atom XA_ATOM;
		Atom XA_CARDINAL;
		Atom XA_INCR;
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
        Atom XA_CP_PROP;
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
		
		static int waitfor_cppropertynotify(Display *d, XEvent *e, char *arg) {
			return (e->type == PropertyNotify) && (e->xproperty.atom == XA_CP_PROP) && (e->xproperty.window == (::Window)arg);
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
                ~icondata() {
                    delete[] data;
                }
                
                long w = 0, h = 0;
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
				Atom version = 5;
				
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
			
			long *cardinals=reinterpret_cast<long*>(data);
			
			T_ ret={
				(int)cardinals[0],
				(int)cardinals[1],
				(int)cardinals[2],
				(int)cardinals[3]
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
			XA_TEXT   =XInternAtom(display, "TEXT", 0);
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
            
            XA_TEXT_HTML = XInternAtom(display, "text/html", False);
            XA_URL = XInternAtom(display, "text/x-moz-url", False);
            XA_PNG = XInternAtom(display, "image/png", False);
            XA_JPG = XInternAtom(display, "image/jpeg", False);
            XA_BMP = XInternAtom(display, "image/bmp", False);
            XA_CP_PROP = XInternAtom(display, "GORGON_CP_PROP", False);
            XA_INCR= XInternAtom(display, "INCR", False);
            
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
            
            setenv("__GL_YIELD", "USLEEP", 1);
		}

        std::string GetAtomName(Atom atom) {
            if(!atom)
                return "[None]";
            else 
                return XGetAtomName(WindowManager::display, atom);
        }
		//BEGIN Clipboard related
        
        ///@cond internal        
        template<class T_>
        std::shared_ptr<CopyFreeAny> make_clipboarddata(T_ data) {
            return std::shared_ptr<CopyFreeAny>{new CopyFreeAny_impl<T_>(std::move(data))};            
        }
       
        struct clipboardentry {
            Atom type;
            std::shared_ptr<CopyFreeAny> data;
            
            bool operator ==(const clipboardentry &other) const {
                return type == other.type;
            }
        };
        
        std::vector<clipboardentry> clipboard_entries;
        
        ::Window getanywindow() {
            ::Window windowhandle=0;
			for(auto &w : Window::Windows) {
				auto data=internal::getdata(w);
				if(data && data->handle) {
					windowhandle=data->handle;
					break;
				}
			}
			
			//if 0 try creating an unmapped window
			if(windowhandle==0) {
#ifdef NDEBUG
				return windowhandle;
#else
				throw std::runtime_error("Cannot copy without a window, if necessary create a hidden window");
#endif
			}
			
			return windowhandle;
        }
		
        std::vector<Atom> getclipboardformats() {
            std::vector<Atom> ret;
            
            
			::Window owner=XGetSelectionOwner(display, XA_CLIPBOARD);
			if(!owner)
				return ret;
			
			auto windowhandle = getanywindow();
            
			if(windowhandle==0) {
				return ret;
			}
			
			//check if we own the clipboard
			for(auto &w : Window::Windows) {
				auto data=internal::getdata(w);
				if(data && data->handle == owner) {
					//we are the owner!
                    
                    //get the list from our own buffer and be done with it
                    for(auto &d : clipboard_entries) {
                        ret.push_back(d.type);
                    }
                    
                    return ret;
				}
			}
			
			XEvent event;
            
            XConvertSelection (display, XA_CLIPBOARD, XA_TARGETS, XA_CP_PROP, windowhandle, CurrentTime);
            XFlush(display);
            
            XIfEvent(display, &event, waitfor_selectionnotify, (char*)windowhandle);
            
            if(event.xselection.property == XA_CP_PROP) {
                //process targets
				Atom type;
				unsigned long len, bytes, dummy;
				unsigned char *data;
				int format;

				XGetWindowProperty(display, windowhandle, XA_CP_PROP, 0, 0, 0, XA_ATOM, &type, &format, &len, &bytes, &data);
				
				if(bytes) {
					XGetWindowProperty (display, windowhandle, 
							XA_CP_PROP, 0,bytes,0,
							XA_ATOM, &type, &format,
							&len, &dummy, &data);
					
                    Atom *atoms = (Atom*)data;
                    
                    for(int i=0;i<bytes/4;i++) {
                        ret.push_back(atoms[i]);
                    }
                    
					XFree(data);
                    XDeleteProperty(display, windowhandle, XA_CP_PROP);
				}
            }
            
            return ret;
        }
        ///@endcond
		
		std::vector<Resource::GID::Type> GetClipboardFormats() {
            std::vector<Resource::GID::Type> ret;
            
			auto list = getclipboardformats();
                    
            for(auto atom : list) {
                if(atom == XA_TEXT || atom == XA_STRING || atom == XA_UTF8_STRING)
                    Containers::PushBackUnique(ret, Resource::GID::Text);
                else if(atom == XA_TEXT_HTML)
                    Containers::PushBackUnique(ret, Resource::GID::HTML);
                else if(atom == XA_URL)
                    Containers::PushBackUnique(ret, Resource::GID::URL);
                else if(atom == XA_PNG || atom == XA_JPG || atom == XA_BMP)
                    Containers::PushBackUnique(ret, Resource::GID::Image_Data);
                else if(atom == XA_Filelist) {
                    Containers::PushBackUnique(ret, Resource::GID::FileList);
                    Containers::PushBackUnique(ret, Resource::GID::URIList);
                }
                else
                    ;//std::cout<<GetAtomName(atom)<<std::endl;
            }
            //std::cout<<std::endl;
            
            return ret;
        }
		
		std::string GetClipboardText(Resource::GID::Type requesttype) {
			::Window owner=XGetSelectionOwner(display, XA_CLIPBOARD);
			if(!owner)
				return "";
			
			::Window windowhandle = getanywindow();
			if(windowhandle==0) {
				return "";
			}
			
			Atom request = 0;
            
            //fallback for text, rest is not that important, modern implementations support TARGETS
            if(requesttype == Resource::GID::Text)
                request = XA_TEXT;
			
			XEvent event;
            
            auto list = getclipboardformats();
            for(auto atom : list) {
                if(requesttype == Resource::GID::Text && atom == XA_UTF8_STRING)  {
                    request = XA_UTF8_STRING;
                    break; //perfect match no need to continue
                }
                else if(requesttype == Resource::GID::Text && atom == XA_STRING) {
                    request = XA_STRING;
                    //utf8 is better, search for it
                }
                else if(requesttype == Resource::GID::Text && atom == XA_TEXT && request != XA_STRING) {
                    request = XA_TEXT;
                    //utf8 is better, search for it
                }
                else if(requesttype == Resource::GID::HTML && atom == XA_TEXT_HTML) {
                    request = XA_TEXT_HTML;
                    break; //perfect match no need to continue
                }
                else if(requesttype == Resource::GID::URL && atom == XA_URL) {
                    request = XA_URL;
                    break; //perfect match no need to continue
                }
            }
            
            if(request == 0) return "";
            
            
			//check if we own the clipboard
			for(auto &w : Window::Windows) {
				auto data=internal::getdata(w);
				if(data && data->handle == owner) {
					//we are the owner!
                    
                    //get the data from our own buffer and be done with it
                    for(auto &d : clipboard_entries) {
                        if(d.type == request) {
                            if( d.type == WindowManager::XA_STRING || 
                                d.type == WindowManager::XA_TEXT || 
                                d.type == WindowManager::XA_UTF8_STRING || 
                                d.type == WindowManager::XA_TEXT_HTML || 
                                d.type == WindowManager::XA_URL)
                            {
                                return d.data->GetData<std::string>();
                            }
                            else {
                                return "";
                            }
                        }
                    }
                    
                    return "";
				}
			}
			
			XConvertSelection (display, XA_CLIPBOARD, request, XA_CP_PROP, windowhandle, CurrentTime);
			XFlush(display);
			
			XIfEvent(display, &event, waitfor_selectionnotify, (char*)windowhandle);
			if (event.xselection.property == XA_CP_PROP) {
				Atom type;
				unsigned long len, bytes, dummy;
				unsigned char *data;
				int format;

				XGetWindowProperty(display, windowhandle, XA_CP_PROP, 0, 0, 0, AnyPropertyType, &type, &format, &len, &bytes, &data);
				
				if(bytes) {
					XGetWindowProperty (display, windowhandle, 
							XA_CP_PROP, 0,bytes,0,
							AnyPropertyType, &type, &format,
							&len, &dummy, &data);
					
					std::string tmp((char*)data, bytes);
					XFree(data);
                    XDeleteProperty(display, windowhandle, XA_CP_PROP);

					return tmp;
				}
			}
			
			return "";
		}

		void SetClipboardText(const std::string &text, Resource::GID::Type type, bool unicode, bool append) {
			::Window windowhandle=getanywindow();
            
            if(!windowhandle) return;
			
			if(!append)
                clipboard_entries.clear();

			if(type == Resource::GID::Text) {
                auto d = make_clipboarddata(text);
                Containers::PushBackOrUpdate(clipboard_entries, clipboardentry{XA_TEXT, d});
                Containers::PushBackOrUpdate(clipboard_entries, clipboardentry{XA_STRING, d});
                
                if(unicode) {
                    Containers::PushBackOrUpdate(clipboard_entries, clipboardentry{XA_UTF8_STRING, d});
                }
            }
            else if(type == Resource::GID::HTML) {
                auto d = make_clipboarddata(text);
                Containers::PushBackOrUpdate(clipboard_entries, clipboardentry{XA_TEXT_HTML, d});
            }
            else if(type == Resource::GID::URL) {
                auto d = make_clipboarddata(text);
                Containers::PushBackOrUpdate(clipboard_entries, clipboardentry{XA_URL, d});
            }
            else {
                return;
            }
			
			XSetSelectionOwner(display, XA_CLIPBOARD, windowhandle, CurrentTime);
			XFlush(display);
		}
		
		std::vector<std::string> GetClipboardList(Resource::GID::Type requesttype) {
            std::vector<std::string> ret;
            
			::Window owner=XGetSelectionOwner(display, XA_CLIPBOARD);
			if(!owner)
				return ret;
			
			::Window windowhandle = getanywindow();
			if(windowhandle==0) {
				return ret;
			}
			
			Atom request = 0;
			
			XEvent event;
            
            auto list = getclipboardformats();
            for(auto atom : list) {
                if((requesttype == Resource::GID::FileList || requesttype == Resource::GID::URIList) && atom == XA_Filelist)  {
                    request = XA_Filelist;
                    break; //perfect match no need to continue
                }
            }
            
            if(request == 0) return ret;
            
            unsigned long len = 0, bytes = 0, dummy;
            unsigned char *data = nullptr;
            
			//check if we own the clipboard
			for(auto &w : Window::Windows) {
				auto wdata=internal::getdata(w);
				if(wdata && wdata->handle == owner) {
					//we are the owner!
                    
                    //get the data from our own buffer and be done with it
                    for(auto &d : clipboard_entries) {
                        if(d.type == request) {
                            if( d.type == WindowManager::XA_Filelist ) {
                                auto &str = d.data->GetData<std::string>();
                                data = (unsigned char*)&str[0];
                                len = str.length();
                            }
                        }
                    }
                    
                    if(!data) return ret;
				}
			}
			
			
			if(!data) {
                XConvertSelection (display, XA_CLIPBOARD, request, XA_CP_PROP, windowhandle, CurrentTime);
                XFlush(display);
                
                XIfEvent(display, &event, waitfor_selectionnotify, (char*)windowhandle);
            }
            
			if (data || event.xselection.property == XA_CP_PROP) {
				Atom type;
				int format;

                if(!data)
                    XGetWindowProperty(display, windowhandle, XA_CP_PROP, 0, 0, 0, AnyPropertyType, &type, &format, &len, &bytes, &data);
				
				if(bytes) {
					XGetWindowProperty (display, windowhandle, 
							XA_CP_PROP, 0,bytes,0,
							AnyPropertyType, &type, &format,
							&len, &dummy, &data);

                }
                if(data) {
                    int p=0;
                    for(int i=0;i<(int)len+1;i++) {
                        if(i==(int)len || (char)data[i]=='\n') {
                            if(i-p>1) {
                                std::string s((char*)(data+p), (int)(i-p));
                                if(requesttype == Resource::GID::FileList) {
                                    if(s.length()>6 && s.substr(0, 7)=="file://") 
                                        s=s.substr(7);
                                    else
                                        continue;
                                    
                                    if(s[s.length()-1]=='\r')
                                        s.resize(s.length()-1);
                                }
                                else if(requesttype == Resource::GID::URIList) {
                                    if(s[s.length()-1]=='\r')
                                        s.resize(s.length()-1);
                                }
                                
                                ret.push_back(Encoding::URIDecode(s));
                                p=i+1;
                            }
                        }
                    }
                    
                    if(bytes) {
                        XFree(data);
                        XDeleteProperty(display, windowhandle, XA_CP_PROP);
                    }

					return ret;
				}
			}
			
			return ret;
		}
	
        void SetClipboardList(std::vector<std::string> list, Resource::GID::Type type, bool append) {
			::Window windowhandle=getanywindow();
            
            if(!windowhandle) return;
			
			if(!append)
                clipboard_entries.clear();

            if(type == Resource::GID::FileList) {
                std::string txt;
                
                for(auto &e : list) {
                    if(!txt.empty())
                        txt += "\n";
                    
                    txt = txt + "file://" + e;
                }
                
                auto d = make_clipboarddata(std::move(txt));
                Containers::PushBackOrUpdate(clipboard_entries, clipboardentry{XA_Filelist, d});
                Containers::PushBackOrUpdate(clipboard_entries, clipboardentry{XA_UTF8_STRING, d});
                Containers::PushBackOrUpdate(clipboard_entries, clipboardentry{XA_STRING, d});
                Containers::PushBackOrUpdate(clipboard_entries, clipboardentry{XA_TEXT, d});
                
                XSetSelectionOwner(display, XA_CLIPBOARD, windowhandle, CurrentTime);
                XFlush(display);
            }
            else if(type == Resource::GID::URIList) {
                std::string txt;
                
                for(auto &e : list) {
                    if(!txt.empty())
                        txt += "\n";
                    
                    txt = txt + e;
                }
                
                auto d = make_clipboarddata(std::move(txt));
                Containers::PushBackOrUpdate(clipboard_entries, clipboardentry{XA_Filelist, d});
                Containers::PushBackOrUpdate(clipboard_entries, clipboardentry{XA_UTF8_STRING, d});
                Containers::PushBackOrUpdate(clipboard_entries, clipboardentry{XA_STRING, d});
                Containers::PushBackOrUpdate(clipboard_entries, clipboardentry{XA_TEXT, d});
                
                XSetSelectionOwner(display, XA_CLIPBOARD, windowhandle, CurrentTime);
                XFlush(display);
            }
        }
        
        Containers::Image GetClipboardBitmap() {
            Containers::Image ret;
            
			::Window owner=XGetSelectionOwner(display, XA_CLIPBOARD);
			if(!owner)
				return ret;
			
			::Window windowhandle = getanywindow();
			if(windowhandle==0) {
				return ret;
			}
            
			//check if we own the clipboard
			for(auto &w : Window::Windows) {
				auto wdata=internal::getdata(w);
				if(wdata && wdata->handle == owner) {
					//we are the owner!
                    
                    //get the data from our own buffer and be done with it
                    for(auto &d : clipboard_entries) {
                        if(d.type == XA_PNG || d.type == XA_BMP || d.type == XA_JPG) {
                            auto &data = d.data->GetData<Containers::Image>();
                            return data.Duplicate();
                        }
                    }
                    
                    return ret;
				}
			}
			
			Atom request = 0;
			
			XEvent event;
			
            auto list = getclipboardformats();
            for(auto atom : list) {
                if(atom == XA_PNG)  { //best tı go for png, as most systems do not add alpha on other types
                    request = XA_PNG;
                    break; //perfect match no need to continue
                }
                else if(atom == XA_BMP) {
                    request = XA_BMP;
                }
                else if(request == 0 && atom == XA_JPG) {
                    request = XA_JPG;
                    //this is the worst case
                }
            }
						
			XConvertSelection (display, XA_CLIPBOARD, request, XA_CP_PROP, windowhandle, CurrentTime);
			XFlush(display);
			
			XIfEvent(display, &event, waitfor_selectionnotify, (char*)windowhandle);
			if (event.xselection.property == XA_CP_PROP) {
				Atom type;
				unsigned long len, bytes, dummy;
				unsigned char *data;
				int format;

				XGetWindowProperty(display, windowhandle, XA_CP_PROP, 0, (unsigned long)-1, 1, AnyPropertyType, &type, &format, &bytes, &dummy, &data);
                bytes *= format/8;
                
                if(type == XA_INCR) {
                    std::vector<Byte> imgdata;
                    //std::cout<<"Starting INCR"<<std::endl;
                    unsigned long initsize = *(int32_t*)data;
                    
                    while(true) {                        
                        while(XCheckIfEvent(display, &event, waitfor_cppropertynotify, (char*)windowhandle) == False) {
                            XFlush(display);
                            std::this_thread::yield();
                        }
                        
                        XFlush(display);
                        XGetWindowProperty(display, windowhandle, XA_CP_PROP, 0, (unsigned long)-1, 1, AnyPropertyType, &type, &format, &bytes, &dummy, &data);
                        
                        if(!bytes) {
                            if(imgdata.size()<initsize)
                                continue;
                            else
                                break;
                        }
                        
                        if(type == 0)
                            continue;
                        
                        bytes *= format/8;
                        
                        auto cur = imgdata.size();
                        imgdata.resize(imgdata.size()+bytes);
                        
                        memcpy(&imgdata[cur], data, bytes);
                        XFree(data);
                    }
                    //std::cout<<"INCR done: "<<imgdata.size()<<std::endl;
                   
					if(request == XA_PNG) {
                        Encoding::Png.Decode(imgdata, ret);
                    }
                    else if(request == XA_BMP) {
                        IO::MemoryInputStream stream((char *)&imgdata[0], (char *)&imgdata[0]+imgdata.size());
                        ret.ImportBMP(stream);
                    }
                    else if(request == XA_JPG) {
                        Encoding::Jpg.Decode(imgdata, ret);
                    }
                    //std::cout<<"Decode done"<<std::endl;
                }
				else if(bytes) {					
					if(request == XA_PNG) {
                        Encoding::Png.Decode(data, bytes, ret);
                    }
                    else if(request == XA_BMP) {
                        IO::MemoryInputStream stream((char *)data, (char *)data+bytes);
                        ret.ImportBMP(stream);
                    }
                    else if(request == XA_JPG) {
                        Encoding::Jpg.Decode(data, bytes, ret);
                    }
                    
					XFree(data);
				}
			}
			
            return ret;
        }
        
        void SetClipboardBitmap(Containers::Image img, bool append) {
			::Window windowhandle=getanywindow();
            
            if(!windowhandle) return;
			
			if(!append)
                clipboard_entries.clear();

            Graphics::ColorMode mode = img.GetMode();
            
            auto d = make_clipboarddata(std::move(img));
            //believe or not BMPv5 is a better fit, allows alpha only images
            Containers::PushBackOrUpdate(clipboard_entries, clipboardentry{XA_BMP, d}); 
            Containers::PushBackOrUpdate(clipboard_entries, clipboardentry{XA_PNG, d});
            if(!Graphics::HasAlpha(mode))
                Containers::PushBackOrUpdate(clipboard_entries, clipboardentry{XA_JPG, d});
            
            XSetSelectionOwner(display, XA_CLIPBOARD, windowhandle, CurrentTime);
            XFlush(display);
        }
        //END

		std::string osgetkeyname(Input::Keyboard::Key key) {
            int keycount;
            
            KeySym *keys = XGetKeyboardMapping(display,
                key,
                1,
                &keycount
            );
            
            if(keycount < 1) return "";
            
            KeySym keysym;
            
            if(keycount > 1) keysym = keys[1]; //capital key
            else keysym = keys[0];
            
            
            XFree(keys);
            
            std::string ret;
            Char c = keysym2ucs(keysym);
            String::AppendUnicode(ret, c);
            
			return ret;
		}

		//BEGIN Monitor Related
		
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
                    long *cardinals=reinterpret_cast<long*>(data);
                    
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
		
		bool Monitor::IsChangeEventSupported() {
			return false;//xrandr;
		}
		
		//END
		
		    
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
			unsigned long*img=new unsigned long[2+image.GetWidth()*image.GetHeight()];
			
			img[0]=image.GetWidth();
			img[1]=image.GetHeight();
			
			image.CopyToBGRABufferLong(img+2);
			
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
		
		bool autoplaced = false;
		
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
            auto borders = WindowManager::GetX4Prop<Geometry::Margin>(WindowManager::XA_NET_FRAME_EXTENTS, data->handle, {0,0,0,0});
            std::swap(borders.Top, borders.Right);

 			XMoveWindow(WindowManager::display, data->handle, location.X+borders.Left, location.Y+borders.Top);
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
	
	Input::Keyboard::Key mapx11key(KeySym key, unsigned int keycode) {
        if(key == 'i') {
            int keycount;
            
            KeySym *keys = XGetKeyboardMapping(WindowManager::display,
                keycode,
                1,
                &keycount
            );
            
            if(keycount < 2) return 'I';
            
            KeySym keysym;
            
            keysym = keys[1]; //capital key
            
            if(keysym != 'I')
                return keycode + Input::Keyboard::Keycodes::OSTransport;
        }
        
        if(key >= 'a' && key <='z')
            return key + ('A' - 'a');
        
        if(key >= '0' && key <='9')
            return key;
        
        if(key >= 'A' && key <='Z')
            return key;
        
        switch(key) {
            case XK_Shift_L:
                return Input::Keyboard::Keycodes::Shift;
            case XK_Shift_R:
                return Input::Keyboard::Keycodes::RShift;
            case XK_Control_L:
                return Input::Keyboard::Keycodes::Control;
            case XK_Control_R:
                return Input::Keyboard::Keycodes::RControl;
            case XK_Alt_L:
                return Input::Keyboard::Keycodes::Alt;
            case XK_Alt_R:
                return Input::Keyboard::Keycodes::RAlt;
            case XK_Super_L:
                return Input::Keyboard::Keycodes::Meta;
            case XK_Super_R:
                return Input::Keyboard::Keycodes::RMeta;
                
            case XK_Home:
                return Input::Keyboard::Keycodes::Home;
            case XK_End:
                return Input::Keyboard::Keycodes::End;
            case XK_Insert:
                return Input::Keyboard::Keycodes::Insert;
            case XK_Delete:
                return Input::Keyboard::Keycodes::Delete;
            case XK_Prior:
                return Input::Keyboard::Keycodes::PageUp;
            case XK_Next:
                return Input::Keyboard::Keycodes::PageDown;

            case XK_Print:
                return Input::Keyboard::Keycodes::PrintScreen;
            case XK_Pause:
                return Input::Keyboard::Keycodes::Pause;

            case XK_Menu:
                return Input::Keyboard::Keycodes::Menu;

            case XK_Caps_Lock:
                return Input::Keyboard::Keycodes::CapsLock;
            case XK_Num_Lock:
                return Input::Keyboard::Keycodes::Numlock;
            case XK_Scroll_Lock:
                return Input::Keyboard::Keycodes::ScrollLock;

            case XK_Return:
                return Input::Keyboard::Keycodes::Enter;
            case XK_Tab:
                return Input::Keyboard::Keycodes::Tab;
            case XK_BackSpace:
                return Input::Keyboard::Keycodes::Backspace;
            case XK_space:
                return Input::Keyboard::Keycodes::Space;
            case XK_Escape:
                return Input::Keyboard::Keycodes::Escape;

            case XK_Left:
                return Input::Keyboard::Keycodes::Left;
            case XK_Up:
                return Input::Keyboard::Keycodes::Up;
            case XK_Right:
                return Input::Keyboard::Keycodes::Right;
            case XK_Down:
                return Input::Keyboard::Keycodes::Down;

            case XK_F1:
                return Input::Keyboard::Keycodes::F1;
            case XK_F2:
                return Input::Keyboard::Keycodes::F2;
            case XK_F3:
                return Input::Keyboard::Keycodes::F3;
            case XK_F4:
                return Input::Keyboard::Keycodes::F4;
            case XK_F5:
                return Input::Keyboard::Keycodes::F5;
            case XK_F6:
                return Input::Keyboard::Keycodes::F6;
            case XK_F7:
                return Input::Keyboard::Keycodes::F7;
            case XK_F8:
                return Input::Keyboard::Keycodes::F8;
            case XK_F9:
                return Input::Keyboard::Keycodes::F9;
            case XK_F10:
                return Input::Keyboard::Keycodes::F10;
            case XK_F11:
                return Input::Keyboard::Keycodes::F11;
            case XK_F12:
                return Input::Keyboard::Keycodes::F12;


            case XK_KP_0:
            case XK_KP_Insert:
                return Input::Keyboard::Keycodes::Numpad_0;
            case XK_KP_1:
            case XK_KP_End:
                return Input::Keyboard::Keycodes::Numpad_1;
            case XK_KP_2:
            case XK_KP_Down:
                return Input::Keyboard::Keycodes::Numpad_2;
            case XK_KP_3:
            case XK_KP_Next:
                return Input::Keyboard::Keycodes::Numpad_3;
            case XK_KP_4:
            case XK_KP_Left:
                return Input::Keyboard::Keycodes::Numpad_4;
            case XK_KP_5:
            case XK_KP_Begin:
                return Input::Keyboard::Keycodes::Numpad_5;
            case XK_KP_6:
            case XK_KP_Right:
                return Input::Keyboard::Keycodes::Numpad_6;
            case XK_KP_7:
            case XK_KP_Home:
                return Input::Keyboard::Keycodes::Numpad_7;
            case XK_KP_8:
            case XK_KP_Up:
                return Input::Keyboard::Keycodes::Numpad_8;
            case XK_KP_9:
            case XK_KP_Prior:
                return Input::Keyboard::Keycodes::Numpad_9;
            case XK_KP_Decimal:
                return Input::Keyboard::Keycodes::Numpad_Decimal;
            case XK_KP_Divide:
                return Input::Keyboard::Keycodes::Numpad_Div;
            case XK_KP_Multiply:
                return Input::Keyboard::Keycodes::Numpad_Mult;
            case XK_KP_Enter:
                return Input::Keyboard::Keycodes::Numpad_Enter;
            case XK_KP_Add:
                return Input::Keyboard::Keycodes::Numpad_Plus;
            case XK_KP_Subtract:
                return Input::Keyboard::Keycodes::Numpad_Minus;
        }
        return keycode + Input::Keyboard::Keycodes::OSTransport;
    }
	
	void Window::processmessages() {
		XEvent event;
        
        mouselocation = WindowManager::GetMousePosition(data);
        if(cursorover)
            mouse_location();

		while(XEventsQueued(WindowManager::display, QueuedAfterReading)) {
			XNextEvent(WindowManager::display, &event);
			KeySym key;
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
                        
                        this->data->xdnd.drop = false;
                        this->data->xdnd.requested = false;
                        
                        cursorover = true;
                        
                        std::vector<Atom> atoms;
                        
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
                        else {
                            for(int i=2; i<=4; i++)
                                if(event.xclient.data.l[i] != internal::None)
                                    atoms.push_back(event.xclient.data.l[i]);
                        }
                        
                        for(auto atom : atoms) {
                            //std::cout<<WindowManager::GetAtomName(atom)<<std::endl;
                            
                            if(atom == WindowManager::XA_Filelist)
                                this->data->xdnd.filelist = true;
                            
                            if(atom == WindowManager::XA_UTF8_STRING)
                                this->data->xdnd.utf8 = true;
                            else if(atom == WindowManager::XA_STRING)
                                this->data->xdnd.string = true;
                        }
                        
                        this->data->xdnd.localpointer = this->IsLocalPointer();
                        this->SwitchToWMPointers();
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
                        m.data.l[1] = this->data->xdnd.filelist || this->data->xdnd.utf8 || this->data->xdnd.string;
                        m.data.l[2] = 0;
                        m.data.l[3] = 0; 
                        
                        if(!Input::IsDragging() || !Input::GetDragOperation().HasTarget()) { 
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

                        if(!Input::IsDragging()) {
                            if(this->data->xdnd.filelist) {
                                if(this->data->xdnd.requested || OS::GetEnvVar("XDG_CURRENT_DESKTOP") == "KDE") {
                                    auto &drag = Input::PrepareDrag();
                                    drag.AssumeData(*new FileData);
                                    Input::StartDrag();
                                    Input::GetDragOperation().MarkAsOS();
                                }
                                else {
                                    XConvertSelection(WindowManager::display, WindowManager::XdndSelection, 
                                                    WindowManager::XA_Filelist, WindowManager::XA_PRIMARY, 
                                                    this->data->handle, event.xclient.data.l[2]);
                                }
                            }
                            
                            if(this->data->xdnd.utf8 || this->data->xdnd.string) {
                                if(this->data->xdnd.requested || OS::GetEnvVar("XDG_CURRENT_DESKTOP") == "KDE") {
                                    auto &drag = Input::PrepareDrag();
                                    drag.AddTextData("");
                                    Input::StartDrag();
                                    Input::GetDragOperation().MarkAsOS();
                                }
                                else {
                                    XConvertSelection(WindowManager::display, WindowManager::XdndSelection, 
                                                    WindowManager::XA_STRING, WindowManager::XA_PRIMARY, 
                                                    this->data->handle, event.xclient.data.l[2]);
                                }
                            }
                            
                            this->data->xdnd.requested = true;

                        }
                    }
                    else if(event.xclient.message_type == WindowManager::XdndLeave) {
                        cursorover = true;

                        Input::CancelDrag();
                        if(this->data->xdnd.localpointer)
                            SwitchToLocalPointers();
                    }
                    else if(event.xclient.message_type == WindowManager::XdndDrop) {
                        //std::cout<<"Drop"<<std::endl;

                        this->data->xdnd.drop = 0;
                        
                        if(this->data->xdnd.filelist) {
                            XConvertSelection(WindowManager::display, WindowManager::XdndSelection, 
                                              WindowManager::XA_Filelist, WindowManager::XA_PRIMARY, 
                                              this->data->handle, event.xclient.data.l[2]);
                            
                            this->data->xdnd.drop++;
                        }
                        
                        if(this->data->xdnd.utf8 || this->data->xdnd.string) {
                            XConvertSelection(WindowManager::display, WindowManager::XdndSelection, 
                                            WindowManager::XA_STRING, 
                                            WindowManager::XA_PRIMARY, this->data->handle, event.xclient.data.l[2]);
                            
                            this->data->xdnd.drop++;
                        }
                       
                        if(this->data->xdnd.localpointer)
                            SwitchToLocalPointers();
                    }
				} // Client Message
				break;
                
                case SelectionNotify:
                    if (event.xselection.property != (unsigned)internal::None) {
                        if(this->data->xdnd.filelist) {
                            unsigned long len, bytes, dummy;
                            unsigned char *data=NULL;
                            Atom type;
                            int format;
                            
                            FileData *dragdata = nullptr;
                            if(Input::IsDragging() && Input::GetDragOperation().HasData(Resource::GID::File)) {
                                dragdata = dynamic_cast<FileData*>(&Input::GetDragOperation().GetData(Resource::GID::File));
                            }
                            else {
                                dragdata = new FileData();
                            }
                            
                            bytes = 0;

                            if(event.xselection.property != internal::None)
                                XGetWindowProperty(WindowManager::display, this->data->handle, event.xselection.property, 0, 0, 0, AnyPropertyType, &type, &format, &len, &bytes, &data);

                            if(bytes) {
                                XGetWindowProperty(WindowManager::display, this->data->handle, 
                                        event.xselection.property, 0,bytes,0,
                                        AnyPropertyType, &type, &format,
                                        &len, &dummy, &data);
                                
                                
                                dragdata->Clear();
                                
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
                                XDeleteProperty(WindowManager::display, this->data->handle, event.xselection.property);

                                if(!Input::IsDragging()) {
                                    auto &drag = Input::PrepareDrag();
                                    drag.AssumeData(*dragdata);
                                    Input::StartDrag();
                                    Input::GetDragOperation().MarkAsOS();
                                    Input::GetDragOperation().DataReady();
                                }
                                else if(!Input::GetDragOperation().HasData(Resource::GID::File)) {
                                    auto &drag = Input::GetDragOperation();
                                    drag.AssumeData(*dragdata);
                                    Input::GetDragOperation().DataReady();
                                }
                                else if(!Input::IsDragging() || !Input::GetDragOperation().HasData(Resource::GID::File)) {
                                    delete dragdata;
                                }
                                else {
                                    Input::GetDragOperation().DataReady();
                                }
                            }
                        }
                        else if(this->data->xdnd.utf8 || this->data->xdnd.string) {
                            Atom type;
                            unsigned long len, bytes, dummy;
                            unsigned char *data;
                            int format;
                        
                            XGetWindowProperty(WindowManager::display, this->data->handle, event.xselection.property, 0, 0, 0, AnyPropertyType, &type, &format, &len, &bytes, &data);

                            if(bytes) {
                                XGetWindowProperty (WindowManager::display, this->data->handle, 
                                        event.xselection.property, 0,bytes,0,
                                        AnyPropertyType, &type, &format,
                                        &len, &dummy, &data);
                                
                                std::string tmp((char*)data, bytes);
                                
                                XFree(data);
                                XDeleteProperty(WindowManager::display, this->data->handle, event.xselection.property);
                                
                                if(Input::IsDragging() && Input::GetDragOperation().HasData(Resource::GID::Text)) {
                                    auto &dragdata = dynamic_cast<TextData&>(Input::GetDragOperation().GetData(Resource::GID::Text));
                                    dragdata.SetText(tmp);
                                    Input::GetDragOperation().DataReady();
                                }
                                else {
                                    if(Input::IsDragging()) {
                                        Input::GetDragOperation().AddTextData(tmp);
                                        Input::GetDragOperation().DataReady();
                                    }
                                    else {
                                        Input::BeginDrag(tmp);
                                        Input::GetDragOperation().MarkAsOS();
                                        Input::GetDragOperation().DataReady();
                                    }
                                }
                            }
                        }
                    }
                    if(this->data->xdnd.drop == 1) {
                        Input::Drop(mouselocation);
                        this->data->xdnd = decltype(this->data->xdnd)();
                    }
                    else if(this->data->xdnd.drop)
                        this->data->xdnd.drop--;
                    
                    break;
                    
                case SelectionClear:
                    WindowManager::clipboard_entries.clear();
                    break;
                
                case SelectionRequest: {
                    XEvent respond;
                    respond.xselection.property= 0;
                    
                    Atom proptoset;
                    proptoset = event.xselectionrequest.property==internal::None ? WindowManager::XA_PRIMARY : event.xselectionrequest.property;
                    
                    if(event.xselectionrequest.selection==WindowManager::XA_CLIPBOARD) {
                        if(event.xselectionrequest.target==WindowManager::XA_TARGETS) {
                            std::vector<Atom> supported = {WindowManager::XA_TARGETS};
                            
                            for(auto &d : WindowManager::clipboard_entries) {
                                supported.push_back(d.type);
                            }
                            
                            XChangeProperty (WindowManager::display,
                                event.xselectionrequest.requestor,
                                proptoset,
                                WindowManager::XA_ATOM,
                                32,
                                PropModeReplace,
                                (unsigned char *)(&supported[0]),
                                supported.size()
                            );
                            respond.xselection.property=proptoset;
                        }
                        else {
                            WindowManager::clipboardentry *entry = nullptr;
                            for(auto &d : WindowManager::clipboard_entries) {
                                if(d.type == event.xselectionrequest.target) {
                                    entry = &d;
                                }
                            }
                            
                            if(entry) {
                                if( entry->type == WindowManager::XA_STRING || 
                                    entry->type == WindowManager::XA_TEXT || 
                                    entry->type == WindowManager::XA_UTF8_STRING || 
                                    entry->type == WindowManager::XA_TEXT_HTML || 
                                    entry->type == WindowManager::XA_Filelist || 
                                    entry->type == WindowManager::XA_URL) 
                                {
                                    std::string &str = entry->data->GetData<std::string>();
                                    
                                    XChangeProperty (WindowManager::display,
                                        event.xselectionrequest.requestor,
                                        proptoset,
                                        entry->type,
                                        8,
                                        PropModeReplace,
                                        (unsigned char*) &str[0],
                                        (int)str.length()
                                    );
                                    
                                    respond.xselection.property=proptoset;
                                }
                                else if(entry->type == WindowManager::XA_PNG) {
                                    Containers::Image &img = entry->data->GetData<Containers::Image>();
                                    
                                    std::vector<Byte> data;
                                    Encoding::Png.Encode(img, data);
                                    
                                    XChangeProperty (WindowManager::display,
                                        event.xselectionrequest.requestor,
                                        proptoset,
                                        entry->type,
                                        8,
                                        PropModeReplace,
                                        &data[0],
                                        (int)data.size()
                                    );
                                    
                                    respond.xselection.property=proptoset;
                                }
                                else if(entry->type == WindowManager::XA_JPG) {
                                    Containers::Image &img = entry->data->GetData<Containers::Image>();
                                    
                                    std::vector<Byte> data;
                                    Encoding::Jpg.Encode(img, data);
                                    
                                    XChangeProperty (WindowManager::display,
                                        event.xselectionrequest.requestor,
                                        proptoset,
                                        entry->type,
                                        8,
                                        PropModeReplace,
                                        &data[0],
                                        (int)data.size()
                                    );
                                    
                                    respond.xselection.property=proptoset;
                                }
                                else if(entry->type == WindowManager::XA_BMP) {
                                    Containers::Image &img = entry->data->GetData<Containers::Image>();
                                    
                                    std::ostringstream data;
                                    img.ExportBMP(data);
                                    
                                    XChangeProperty (WindowManager::display,
                                        event.xselectionrequest.requestor,
                                        proptoset,
                                        entry->type,
                                        8,
                                        PropModeReplace,
                                        (Byte*)&data.str()[0],
                                        (int)data.str().size()
                                    );
                                    
                                    respond.xselection.property=proptoset;
                                }
                            }
                        }
                    }
                    
                    respond.xselection.type= SelectionNotify;
                    respond.xselection.display= event.xselectionrequest.display;
                    respond.xselection.requestor= event.xselectionrequest.requestor;
                    respond.xselection.selection=event.xselectionrequest.selection;
                    respond.xselection.target= event.xselectionrequest.target;
                    respond.xselection.time = event.xselectionrequest.time;
                    XSendEvent (WindowManager::display, event.xselectionrequest.requestor,0,0,&respond);
                    XFlush (WindowManager::display);
                }
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
					auto ggekey = mapx11key(key, event.xkey.keycode);
					auto token=KeyEvent(ggekey, true);
					if(token != KeyEvent.EmptyToken) {
						data->handlers[ggekey]=token;
						
						break;
					}
					
					if(!Input::Keyboard::CurrentModifier.IsModified()) {
                        XLookupString(&event.xkey, nullptr, 0, &key, nullptr); //append shift and other mods                    
                        Input::Keyboard::Char c = keysym2ucs(key);
                        
                        if(c != 0xfffd) {
                            if( (c>=0x20 || c == '\t' || c =='\n') && (c < 0x7f || c > 0x9f)) { //exclude c0 & c1 but keep enter and tab
                                CharacterEvent(c);
                            }
                        }
                    }
				} //Keypress
				break;
					
					
				case KeyRelease: {
					key=XLookupKeysym(&event.xkey,0);
                        
                    auto ggekey = mapx11key(key, event.xkey.keycode);
						
					if(XEventsQueued(WindowManager::display, QueuedAfterReading)) {
						XEvent nextevent;
						XPeekEvent(WindowManager::display, &nextevent);
                        
						if(nextevent.type == KeyPress && nextevent.xkey.time == event.xkey.time && 
							nextevent.xkey.keycode == event.xkey.keycode
						) {
							
							if(data->handlers.count(ggekey)>0 && data->handlers[ggekey]!=KeyEvent.EmptyToken) {
								//if keypress handled, key will not be repeated.
                                //what about backspace, arrow keys and delete?
							}
							else if(!Input::Keyboard::CurrentModifier.IsModified()) {
                                XLookupString(&event.xkey, nullptr, 0, &key, nullptr);
                                Input::Keyboard::Char c = keysym2ucs(key);
                                
                                if(c != 0xfffd) {
                                    if( (c>=0x20 || c == '\t' || c =='\n') && (c < 0x7f || c > 0x9f)) { //exclude c0 & c1 but keep enter and tab
                                        CharacterEvent(c);
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
					
					
					if(data->handlers.count(ggekey)>0 && data->handlers[ggekey]!=KeyEvent.EmptyToken) {
						KeyEvent.FireFor(data->handlers[ggekey], ggekey, 0.f);
						data->handlers[ggekey]=KeyEvent.EmptyToken;
					}
					else {
						KeyEvent(ggekey, 0.f);
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
        
        ::Window r, c;
        int x, y;
        unsigned w, h, bw, d;
        
        XGetGeometry(WindowManager::display, data->handle, &r, &x, &y, &w, &h, &bw, &d);
        
        XTranslateCoordinates( WindowManager::display, data->handle, r, 0, 0, &x, &y, &c );
        
        return Geometry::Bounds(x, y, x+w, y+h) + borders;
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
