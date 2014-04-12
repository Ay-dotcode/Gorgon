/// @file WindowManager.h contains window manager dependent functionality.

#pragma once

#include "Geometry/Point.h"
#include "Geometry/Rectangle.h"

namespace Gorgon {

	/// This namespace contains window manager specific functionality. 
	/// This including creating windows, obtaining input, creating
	/// graphics context.
	namespace WindowManager {
		/// Initializes window manager system
		void Initialize();

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
		/// string is returned
		std::string GetClipboardText();

		/// Sets the clipboard text to given string.
		void SetClipboardText(const std::string &text);

		////Hides the pointer displayed by window manager
		void HidePointer();

		////Shows the pointer displayed by window manager
		void ShowPointer();
	}
}