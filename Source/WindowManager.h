/// @file WindowManager.h contains window manager dependent functionality.

#pragma once

#include "Geometry/Point.h"
#include "Geometry/Rectangle.h"

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

		/// Sets the clipboard text to given string. May require an existing window.
		void SetClipboardText(const std::string &text);
	}
}