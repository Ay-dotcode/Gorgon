/// @file WindowManager.h contains window manager dependent functionality.

#pragma once

#include "Geometry/Point.h"
#include "Geometry/Rectangle.h"
#include "Containers/Collection.h"
#include "Utils/Assert.h"
#include "Event.h"
#include "Containers/Image.h"
#include "Resource/GID.h"

namespace Gorgon {
	
	/// @cond INTERNAL
	namespace internal {
		struct windowdata;
	}
	/// @endcond
	
	class Window;

	/// This namespace contains window manager specific functionality. 
	/// This including creating windows, obtaining input, creating
	/// graphics context.
	namespace WindowManager {
		
		/// @cond INTERNAL
		namespace internal {
			Gorgon::internal::windowdata *getdata(const Window&);

			void switchcontext(Gorgon::internal::windowdata &data);
			void finalizerender(Gorgon::internal::windowdata &data);

			struct icondata;
			struct pointerdata;
			struct monitordata;
		}
		/// @endcond
		
		/// Represents an icon object that can be used as window icon.
		class Icon {
			friend class Gorgon::Window;
		public:
			explicit Icon(const Containers::Image &image);
            
            Icon(const Icon &) = delete;
            
            Icon(Icon &&icon);
            
            Icon &operator =(const Icon &) = delete;
            
            Icon &operator =(Icon &&icon);
            
            Icon();
            
            void Destroy();
            
            void FromImage(const Containers::Image &image);
			
			~Icon();
			
		private:
			internal::icondata *data;
		};
		
		/// Represents a hardware/OS pointer graphic. Hardware pointers do not have
		/// the power of Gorgon internal pointers, however, they are mostly delay free
		/// and works even when the system is busy.
		class Pointer {
		public:
			Pointer();
			
			~Pointer();
			
		private:
			internal::pointerdata *data;
		};
		
		/*
		 * Represents a monitor.
		 */
		class Monitor {
			friend struct internal::monitordata;
            friend void addpadding(const Monitor*, int, int, int, int);
		public:
			/// Returns the size of this monitor in pixels.
			Geometry::Size  GetSize() const {
				return area.GetSize();
			}
			
			/// Returns the location of this monitor relative to the other monitors in pixels. 
			/// This function is expected to return (0, 0) if there are no other monitors in the system.
			Geometry::Point GetLocation() const {
				return area.TopLeft();
			}
			
			/// Returns the area of the entire monitor including shift for multihead displays.
			Geometry::Rectangle GetArea() const {
				return area;
			}
			
			/// Returns the area usable rectangle of the monitor. This region excludes any panels on the screen.
			Geometry::Bounds GetUsable() const {
				return usable;
			}
			
			/// Whether this display is primary.
			bool IsPrimary() const {
				return isprimary;
			}
			
			/// Returns the name of the monitor
			std::string GetName() const {
				return name;
			}
			
			~Monitor();
			
			/// Returns the default monitor
			static Monitor &Primary() {
#ifndef NDEBUG
				ASSERT(primary, "WindowManager module is not initialized or there are no connected monitors.");
#endif
				
				return *primary;
			}
			
			/// Returns all monitors connected to this device
			static const Containers::Collection<Monitor> &Monitors() {
				return monitors;
			}
			
			
			/// Returns the monitor from the given location. If none found, will return nullptr.
			static const Monitor *FromLocation(Geometry::Point location) {
                for(auto &monitor : monitors) {
                    if(IsInside(monitor.GetArea(), location)) {
                        return &monitor;
                    }
                }
                
                return nullptr;
            }
			
			/// Asks WindowManager to refresh the list of monitors. This may deallocate monitors and cause problems.
			/// Calling this method raises DisplayChanged to mitigate this problem partially. This function will not
			/// re-create monitor list if window manager determines that there are no changes in the monitors. You may
			/// set force parameter to true to ensure monitors list is re-created.
			static void Refresh(bool force=false);
			
			/// In some cases, Changed event is not supported. This function might be used to query if it works
			/// or not. Even this event is not supported, RefreshMonitors function causes it to be raised after gathering
			/// information.
			static bool IsChangeEventSupported();
			
			/// Fires when window manager raises an event about a change in the monitor or screen layout. Additionally,
			/// this event will be fired when RefreshMonitors causes monitor list to be re-created. If monitor pointers
			/// are stored, this event should be listened as a call to RefreshMonitors may invalidate these pointers.
			static Event<> ChangedEvent;
			
		private:
			Monitor();
			internal::monitordata *data;
			
			Geometry::Rectangle area        = {0,0,0,0};
            Geometry::Bounds    usable      = {0,0,0,0};
			bool                isprimary   = false;
			
			std::string name;
			
			static Containers::Collection<Monitor> monitors;
			static Monitor *primary;
		};
		
		/// Initializes window manager system
		void Initialize();

		/// Returns an identifier for the current context
		intptr_t CurrentContext();

		/// Returns the list of formats that is in the clipboard supported by the Gorgon Library.
		/// Use GetAllClipboardFormats to get OS dependent clipboard format list. The following
		/// list is the currently supported formats:
		/// * Image_Data: This is a Graphics::Bitmap object, this can be constructed from PNG, JPG,
		///               BMP, OS Bitmap formats (BMP, DIB), [Gorgon]Bitmap format which is copied
		///               from Graphics::Bitmap. Use GetClipboardBitmap
		/// * Text: Text data. Use GetClipboardText
		/// * HTML: HTML data. Use GetClipboardText
		std::vector<Resource::GID::Type> GetClipboardFormats();

		/// Returns the clipboard text. If there is no data or its incompatible with text, empty
		/// string is returned. May require an existing window. This function will prioritize Unicode
		/// text if it exists. type refers to the clipboard type. Currently Text and HTML are supported.
		std::string GetClipboardText(Resource::GID::Type type = Resource::GID::Text);
		
		/// Sets the clipboard text to given string. May require an existing window. Currently Text and HTML 
		/// are supported. If append is set, instead of clearing the clipboard, it will add the given text to 
		/// the list of clipboard data. This function will copy the text, thus is not suitable if the data is 
		/// too large. If unicode is true, both unicode and regular text would be set to the given data.
		/// If you wish to advertise non-unicode text, you should do it after setting unicode text by setting
		/// unicode to false and append to true.
		void SetClipboardText(const std::string &text, Resource::GID::Type type = Resource::GID::Text, 
							  bool unicode = true, bool append = false);
	}
}
