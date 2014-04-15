/// @file WindowManager.h contains window manager dependent functionality.

#pragma once

#include "Geometry/Point.h"
#include "Geometry/Rectangle.h"
#include "Containers/Collection.h"
#include "WindowManager.h"
#include "Event.h"

namespace Gorgon {
	/// @cond INTERNAL
	namespace internal {
		struct windowdata;
	}
	/// @endcond

	/// This class represents a window. 
	/// @nosubgrouping
	class Window {
		friend internal::windowdata *WindowManager::internal::getdata(const Window&);
	public:
		static const 
		class FullscreenTag {
		} Fullscreen;
		
		/// Creates a new window
		/// @param  rect the position and the **interior** size of the window unless
		///         use outer metrics is set to true
		/// @param  name of the window
		/// @param  title of the window
		/// @param  visible after creation, window will be visible or invisible depending
		///         on this value. 
		/// @param  useoutermetrics by default, window is resized and positioned depending
		///         on its interior region. Setting this parameter to true will switch to
		///         include the window chrome
		Window(Geometry::Rectangle rect, const std::string &name, const std::string &title, bool visible=true, bool useoutermetrics=false);

		/// Creates a new window
		/// @param  rect the position and the **interior** size of the window unless
		///         use outer metrics is set to true
		/// @param  name of the window
		/// @param  title of the window
		/// @param  visible after creation, window will be visible or invisible depending
		///         on this value. 
		/// @param  useoutermetrics by default, window is resized and positioned depending
		///         on its interior region. Setting this parameter to true will switch to
		///         include the window chrome
		Window(Geometry::Rectangle rect, const std::string &name, const char *title, bool visible=true, bool useoutermetrics=false) :
			Window(rect, name, std::string(title), visible, useoutermetrics) { }

		/// Creates a new window
		/// @param  rect the position and the **interior** size of the window unless
		///         use outer metrics is set to true
		/// @param  name of the window
		/// @param  visible after creation, window will be visible or invisible depending
		///         on this value. 
		/// @param  useoutermetrics by default, window is resized and positioned depending
		///         on its interior region. Setting this parameter to true will switch to
		///         include the window chrome
		Window(Geometry::Rectangle rect, const std::string &name, bool visible=true, bool useoutermetrics=false) :
			Window(rect, name, "", visible, useoutermetrics) { }

		/// Creates a new window at the center of the screen
		/// @param  size of the window
		/// @param  name of the window
		/// @param  title of the window
		/// @param  visible after creation, window will be visible or invisible depending
		///         on this value. 
		/// @param  useoutermetrics by default, window is resized and positioned depending
		///         on its interior region. Setting this parameter to true will switch to
		///         include the window chrome
		Window(const Geometry::Size &size, const std::string &name, const std::string &title, bool visible=true, bool useoutermetrics=false) :
			Window({automaticplacement, size}, name, title, visible, useoutermetrics) { }

		/// Creates a new window at the center of the screen
		/// @param  size of the window
		/// @param  name of the window
		/// @param  title of the window
		/// @param  visible after creation, window will be visible or invisible depending
		///         on this value. 
		/// @param  useoutermetrics by default, window is resized and positioned depending
		///         on its interior region. Setting this parameter to true will switch to
		///         include the window chrome
		Window(const Geometry::Size &size, const std::string &name, const char *title, bool visible=true, bool useoutermetrics=false) :
			Window({automaticplacement, size}, name, std::string(title), visible, useoutermetrics) { }

		/// Creates a new window at the center of the screen
		/// @param  size of the window
		/// @param  name of the window
		/// @param  visible after creation, window will be visible or invisible depending
		///         on this value. 
		/// @param  useoutermetrics by default, window is resized and positioned depending
		///         on its interior region. Setting this parameter to true will switch to
		///         include the window chrome
		Window(const Geometry::Size &size, const std::string &name, bool visible=true, bool useoutermetrics=false) :
			Window({automaticplacement, size}, name, "", visible, useoutermetrics) { }

		/// Creates a fullscreen window. Fullscreen windows do not have chrome and covers
		/// entire screen, including any panels it contains.
		Window(const FullscreenTag &, const std::string &name, const std::string &title="");

		/// Destroys this window
		~Window();

		/// This method is automatically called by the system. Unless its necessary, 
		/// do not use it.
		void processmessages();

		/// Moves the window to the given position
		void Move(const Geometry::Point &position);

		/// Resizes the window to the given size. The given size is considered as the
		/// interior region of the window. The restrictions for the smallest
		/// sized window might change depending on the window manager or theme.
		/// Largest window size can be obtained using UsableScreenRegion however,
		/// this size does not exclude window chrome. This function resizes all window
		/// sized layers.
		void Resize(const Geometry::Size &size);

		/// Resizes the window to the given size. The given size is considered as the
		/// interior region of the window. The restrictions for the smallest
		/// sized window might change depending on the window manager or theme.
		/// Largest window size can be obtained using UsableScreenRegion however,
		/// this size does not exclude window chrome. This function resizes all window
		/// sized layers.
		void Resize(int width, int height) { Resize({width, height}); }

		/// Resizes the window to the given size. The given size is considered as the
		/// whole window including its chrome. The restrictions for the smallest
		/// sized window might change depending on the window manager or theme.
		/// Largest window size can be obtained using UsableScreenRegion however,
		/// this size does not exclude window chrome. This function resizes all window
		/// sized layers.
		void ResizeOuter(const Geometry::Size &size);

		/// Resizes the window to the given size. The given size is considered as the
		/// whole window including its chrome. The restrictions for the smallest
		/// sized window might change depending on the window manager or theme.
		/// Largest window size can be obtained using UsableScreenRegion however,
		/// this size does not exclude window chrome. This function resizes all window
		/// sized layers.
		void ResizeOuter(int width, int height) { ResizeOuter({width, height}); }

		/// Displays this window, may generate Activated event
		void Show();

		/// Hides this window, may generate Deactivated event
		void Hide();

		/// Closes the window. After this function, any use of this object might fail.
		void Close();

		////Hides the pointer displayed by window manager
		void HidePointer();

		////Shows the pointer displayed by window manager
		void ShowPointer();

		/// @name Events 
		/// @{

		/// Called when this window is activated
		Event<Window> ActivateEvent=Event<Window>(*this);

		/// Called when this window is deactivated
		Event<Window> DeactivateEvent=Event<Window>(*this);

		/// Called when this window is destroyed. Note that at this point, any reference
		/// to this object may fail.
		Event<Window> DestroyEvent=Event<Window>(*this);

		/// Called when user tries to close the window. Will not be called if window is closed
		/// using Destructor or Close function
		/// ### Parameters: ###
		/// **allow** `(bool &)`: setting this value to false will stop window being closed.
		Event<Window, bool&> ClosingEvent=Event<Window, bool&>(*this);

		/// Called after the window is moved, either by the user or programmatically
		Event<Window> MoveEvent=Event<Window>(*this);

		/// Called after the window is resized, either by the user or programmatically
		Event<Window> ResizeEvent=Event<Window>(*this);

		/// Called after the window is minimized, either by the user or programmatically
		Event<Window> MinimizedEvent=Event<Window>(*this);

		/// Called after the window is restored, either by the user or programmatically
		Event<Window> RestoredEvent=Event<Window>(*this);

		///@}

		/// List of currently created windows
		static const Containers::Collection<Window> &Windows;


	private:
		internal::windowdata *data;

		static Containers::Collection<Window> windows;

		static const Geometry::Point automaticplacement;
	};
}