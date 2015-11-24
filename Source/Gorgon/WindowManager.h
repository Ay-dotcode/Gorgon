/// @file WindowManager.h contains window manager dependent functionality.

#pragma once

#include "Geometry/Point.h"
#include "Geometry/Rectangle.h"
#include "Containers/Collection.h"
#include "Utils/Assert.h"
#include "Event.h"

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
		public:
			Icon();
			
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
		public:
			/// Returns the size of this monitor in pixels.
			Geometry::Size  GetSize() const {
				return size;
			}
			
			/// Returns the location of this monitor relative to the other monitors in pixels. 
			/// This function is expected to return (0, 0) if there are no other monitors in the system.
			Geometry::Point GetLocation() const {
				return location;
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
			
			Geometry::Size  size = {0,0};
			Geometry::Point location = {0,0};
			bool            isprimary = false;
			
			std::string name;
			
			static Containers::Collection<Monitor> monitors;
			static Monitor *primary;
		};
		
		/// Initializes window manager system
		void Initialize();

		/// Returns an identifier for the current context
		intptr_t CurrentContext();

		/// Returns the usable rectangle of the screen. Usable rectangle excludes the regions
		/// that are occupied by taskbar or any other panel that is set not to be covered
		/// by windows. If a window is created in these regions, behavior is window
		/// manager defined. It may move the window out of these regions, or leave it under
		/// or over the panel.
		Geometry::Rectangle GetUsableScreenRegion(int monitor=0);

		/// Returns the rectangle of the given screen. There might be an offset on multi-monitor
		/// systems.
		Geometry::Rectangle GetScreenRegion(int monitor=0);

		/// Returns the clipboard text. If there is no data or its incompatible with text, empty
		/// string is returned. May require an existing window.
		std::string GetClipboardText();
		
		/// Returns the monitors connected to this computer
		std::vector<Monitor> GetMonitors();

		/// Sets the clipboard text to given string. May require an existing window.
		void SetClipboardText(const std::string &text);
	}
}