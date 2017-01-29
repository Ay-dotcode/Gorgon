/// @file Window.h contains Window class. The functions of the Window class is defined in respective window manager
/// source code file (e.g WindowManager/X11.cpp)

#pragma once

#include "Geometry/Point.h"
#include "ConsumableEvent.h"
#include "Geometry/Rectangle.h"
#include "Containers/Collection.h"
#include "WindowManager.h"
#include "Event.h"
#include "Layer.h"
#include "Input.h"
#include "Input/Keyboard.h"
#include "Graphics/Pointer.h"

namespace Gorgon {
	/// @cond INTERNAL
	namespace internal {
		struct windowdata;
	}
	/// @endcond
	
	namespace Graphics {
        class Layer;
    }
        

	/// This class represents a window. 
	/// @nosubgrouping
	class Window : public Layer {
		friend internal::windowdata *WindowManager::internal::getdata(const Window&);
		friend struct internal::windowdata;
        friend class Layer;
	public:
		/// Fullscreen tag
		static const struct FullscreenTag {
			
		}Fullscreen;

		/// Creates a new window
		/// @param  rect the position and the **interior** size of the window unless
		///         use outer metrics is set to true
		/// @param  name of the window
		/// @param  visible after creation, window will be visible or invisible depending
		///         on this value. 
		Window(Geometry::Rectangle rect, const std::string &name, bool allowresize=false, bool visible=true) :
			Window(WindowManager::Monitor::Primary(), rect, name, name, allowresize, visible) {}

		/// Creates a new window
		/// @param  rect the position and the **interior** size of the window unless
		///         use outer metrics is set to true
		/// @param  name of the window
		/// @param  visible after creation, window will be visible or invisible depending
		///         on this value. 
		Window(Geometry::Rectangle rect, const char *name, bool allowresize=false, bool visible=true) :
			Window(WindowManager::Monitor::Primary(), rect, name, name, allowresize, visible) {}

		/// Creates a new window at the center of the screen
		/// @param  size of the window
		/// @param  name of the window
		/// @param  title of the window
		/// @param  visible after creation, window will be visible or invisible depending
		///         on this value. 
		Window(const Geometry::Size &size, const std::string &name, const std::string &title, bool allowresize=false, bool visible=true) :
			Window(WindowManager::Monitor::Primary(), {automaticplacement, size}, name, title, allowresize, visible) { }

		/// Creates a new window at the center of the screen
		/// @param  size of the window
		/// @param  name of the window
		/// @param  title of the window
		/// @param  visible after creation, window will be visible or invisible depending
		///         on this value. 
		Window(const Geometry::Size &size, const char *name, const char *title, bool allowresize=false, bool visible=true) :
			Window(WindowManager::Monitor::Primary(), {automaticplacement, size}, name, title, allowresize, visible) { }

		/// Creates a new window at the center of the screen
		/// @param  size of the window
		/// @param  name of the window
		/// @param  visible after creation, window will be visible or invisible depending
		///         on this value. 
		Window(const Geometry::Size &size, const std::string &name, bool allowresize=false, bool visible=true) :
			Window(WindowManager::Monitor::Primary(), {automaticplacement, size}, name, name, allowresize, visible) { }

		/// Creates a new window at the center of the screen
		/// @param  size of the window
		/// @param  name of the window
		/// @param  visible after creation, window will be visible or invisible depending
		///         on this value. 
		Window(const Geometry::Size &size, const char *name, bool allowresize=false, bool visible=true) :
			Window(WindowManager::Monitor::Primary(), {automaticplacement, size}, name, name, allowresize, visible) { }

		/// Creates a new window at the center of the screen
		/// @param  monitor that the window will be centered on
		/// @param  size of the window
		/// @param  name of the window
		/// @param  visible after creation, window will be visible or invisible depending
		///         on this value. 
		Window(const WindowManager::Monitor &monitor, const Geometry::Size &size, const std::string &name, bool allowresize=false, bool visible=true) :
			Window(monitor, {automaticplacement, size}, name, name, allowresize, visible) { }

		/// Creates a new window at the center of the screen
		/// @param  monitor that the window will be centered on
		/// @param  size of the window
		/// @param  name of the window
		/// @param  visible after creation, window will be visible or invisible depending
		///         on this value. 
		Window(const WindowManager::Monitor &monitor, const Geometry::Size &size, const char *name, bool allowresize=false, bool visible=true) :
			Window(monitor, {automaticplacement, size}, name, name, allowresize, visible) { }

		/// Creates a fullscreen window. Fullscreen windows do not have chrome and covers
		/// entire screen, including any panels it contains.
		Window(const FullscreenTag &, const WindowManager::Monitor &monitor, const std::string &name, const std::string &title="");

		/// Creates a fullscreen window. Fullscreen windows do not have chrome and covers
		/// entire screen, including any panels it contains.
		Window(const FullscreenTag &tag, const std::string &name, const std::string &title="") :
            Window(tag, WindowManager::Monitor::Primary(), name, title) { }

		/// Destroys this window
		~Window();

		/// This method is automatically called by the system.Unless its necessary, do not use it.
		/// 
		/// @warning Notice that it is a
		/// window manager defined behavior to handle tasks in Window classes differently
		/// or for all of them at the same time. Therefore, OS::processmessage
		/// should be used. 
		void processmessages();

		/// Activates the GL context of the window. This function is used internally and should
		/// not be called unless necessary.
		void activatecontext();

		/// Moves the window to the given position
		virtual void Move(const Geometry::Point &position) override;

		/// Resizes the window to the given size. The given size is considered as the
		/// interior region of the window. The restrictions for the smallest
		/// sized window might change depending on the window manager or theme.
		/// Largest window size can be obtained using UsableScreenRegion however,
		/// this size does not exclude window chrome. This function resizes all window
		/// sized layers.
		virtual void Resize(const Geometry::Size &size) override;

		/// Returns the exterior bounding box of the window. May throw or return invalid
		/// values if the window is not visible
		Geometry::Bounds GetExteriorBounds() const;

		/// Returns the current position of the window. This value is measured from the
		/// (0,0) of the virtual screen to the top-left of the window chrome.
		Geometry::Point GetPosition() const {
			return GetExteriorBounds().TopLeft();
		}

		/// Minimizes the window. Use Restore function to return it to its previous
		/// state.
		void Minimize();

		/// Returns if the window is minimized
		bool IsMinimized() const;

		/// Maximizes the window to cover the usable area of the screen. Use Restore function
		/// to return the window to its previous size and position. Maximize might take time.
		void Maximize();

		/// Returns if the window is maximized
		bool IsMaximized() const;

		/// Restores a minimized or maximized window
		void Restore();

		/// Returns the monitor that the window is currently on. May throw if the window is
		/// not visible, will throw if the window is not on any currently known monitors. 
		/// If this case occurs consider refreshing monitor list.
		const WindowManager::Monitor &GetMonitor() const;

		/// Displays this window, may generate Activated event
		virtual void Show() override;

		/// Hides this window, may generate Deactivated event
		virtual void Hide() override;
		
		/// Returns whether this layer is effectively visible
		virtual bool IsVisible() const override { return isvisible; }

		/// Focuses this window
		void Focus();

		/// Returns if this window has the focus
		bool IsFocused() const;

		/// Closes the window. After this function, any use of this object might fail.
		void Close();

		/// Changes the title of the window to the specified string
		void SetTitle(const std::string &title);

		/// Returns the current title of the window.
		std::string GetTitle() const;

		/// Returns the name of the window that is set at creation time
		std::string GetName() const {
			return name;
		}

		/// Whether the window is currently closed and cannot be acted on.
		bool IsClosed() const;

		/// Displays the pointer. If local pointers are activated, this function
        /// will show local pointer, otherwise it will show window manager pointer
		void ShowPointer();

		/// Hides the pointer. If local pointers are activated, this function
        /// will hide local pointer, otherwise it will hide window manager pointer.
        /// In both cases, after calling to this function, there will be no visible
        /// pointer on the screen.
		void HidePointer();
        
        /// Centers the window to the default monitor
        void Center() {
            Center(WindowManager::Monitor::Primary());
        }
        
        /// Centers the window to the given monitor
        void Center(const WindowManager::Monitor &monitor) {
            Move((monitor.GetUsable()-GetExteriorBounds().GetSize()).Center());
        }

        /// Changes the icon of the window.
		void SetIcon(const WindowManager::Icon &icon);

		/// Renders the contents of the window
		virtual void Render() override;
        
        /// Returns the mouse location on the window. If the cursor is outside the window and 
        /// its position cannot be determined (-1, -1) is returned. This function can return
        /// coordinates outside the bounds of the window.
        Geometry::Point GetMouseLocation() const {
            return mouselocation;
        }
        
        /// Returns currently pressed buttons.
        Input::Mouse::Button PressedButtons() const {
            return pressed;
        }
        
        /// Query whether the left mouse button is pressed
        bool IsLeftButtonPressed() const {
            return pressed&&Input::Mouse::Button::Left;
        }
        
        /// Query whether the right mouse button is pressed
        bool IsRightButtonPressed() const {
            return pressed&&Input::Mouse::Button::Right;
        }
        
        /// Query whether the middle mouse button is pressed
        bool IsMiddleButtonPressed() const {
            return pressed&&Input::Mouse::Button::Middle;
        }
        
        /// Query whether the X1 mouse button is pressed
        bool IsX1ButtonPressed() const {
            return pressed&&Input::Mouse::Button::X1;
        }
        
        /// Query whether the X2 mouse button is pressed
        bool IsX2ButtonPressed() const {
            return pressed&&Input::Mouse::Button::X2;
        }

		/// Allows window to be resized by the user
		void AllowResize();

		/// Prevents window to be resized by the user
		void PreventResize();
        
        /// Removes the operating system pointer and starts using Locally defined pointers.
        /// If there are no pointers added to the Pointers object, this function will throw.
        /// After switching, ShowPointer and HidePointer will show and hide local pointers
        /// not OS pointers. Calling this function will show the local pointer.
        void SwitchToLocalPointers();
        
        /// Stops showing local pointers and makes window manager pointer visible.
        void SwitchToWMPointers();

		/// Returns the minimum size required to fit any window inside.
		static Geometry::Size GetMinimumRequiredSize();
        
        Graphics::PointerStack Pointers; 

		/// @name Events 
		/// @{
		
		/// Called when this window is focused
		Event<Window> FocusedEvent{*this};

		/// Called when this window is deactivated
		Event<Window> LostFocusEvent{*this};

		/// Called when this window is destroyed. Note that at this point, any reference
		/// to this object may fail.
		Event<Window> DestroyedEvent{*this};

		/// Called when user tries to close the window. Will not be called if window is closed
		/// using Destructor or Close function
		/// ### Parameters: ###
		/// **allow** `(bool &)`: setting this value to false will stop window being closed.
		Event<Window, bool&> ClosingEvent{*this};

		/// Called after the window is moved, either by the user or programmatically
		Event<Window> MovedEvent{*this};

		/// Called after the window is resized, either by the user or programmatically
		Event<Window> ResizedEvent{*this};

		/// Called after the window is minimized, either by the user or programmatically
		Event<Window> MinimizedEvent{*this};

		/// Called after the window is restored from minimized state, either by the user or programmatically
		Event<Window> RestoredEvent{*this};
		
		/// Called when a key is pressed or released. This key could be a keyboard key
		/// or any other controller key including mouse. In case of mouse, this method
		/// will be triggered if mouse call is not handled by layers. 
		///
		/// If the input device is keyboard, key down event (amount=1) is repeated if the 
		/// first event is handled. The repeats will only be sent to the handler of the event.
		/// Additionally, Key up event (amount=0) is sent only to handler, if key down is handled
		/// otherwise it will be sent to all listeners.
		///
		/// ### Parameters: ###
		/// **Key** `(Input::Key)`: The key that is pressed or released
		/// **Amount** `(float)`: Depending on the device this can be a boolean 0 and 1, number
		///     of successive key strokes or amount of pressure (0 to 1) in analog controllers
		ConsumableEvent<Window, Input::Key, bool> KeyEvent{*this};

		/// Called when a character is received. This event is raised for regular characters
		/// that can be printed. If a key is handled in keypress event, this event will not be
		/// fired. This event will be called multiple times when the key repeats. 
		ConsumableEvent<Window, Input::Keyboard::Char> CharacterEvent{*this};
		///@}

		/// List of currently created windows
		static const Containers::Collection<Window> &Windows;

		static int ClickThreshold;

	protected:
		Window(const WindowManager::Monitor &monitor, Geometry::Rectangle rect, const std::string &name, const std::string &title, bool allowresize, bool visible);

		/// A window cannot be placed in another layer. This function always fails.
		virtual void located(Layer *) override { Utils::ASSERT_FALSE("A window cannot be placed in another layer"); }
		
		virtual void added(Layer &layer) override;
		
		void mouse_down(Geometry::Point location, Input::Mouse::Button button);
		void mouse_up(Geometry::Point location, Input::Mouse::Button button);
		void mouse_event(Input::Mouse::EventType event, Geometry::Point location, Input::Mouse::Button button, float amount);
        void mouse_location();
        
        void deleting(Layer *layer) { if(layer==down) down = nullptr; }

	private:
		void createglcontext();

		std::string name;
		
		internal::windowdata *data;

		static Containers::Collection<Window> windows;

		static const Geometry::Point automaticplacement;
        
        Input::Mouse::Button pressed = Input::Mouse::Button::None;
        
        MouseHandler down;
        MouseHandler over;
        
        bool allowresize = false;
        
        bool cursorover = false;

		Geometry::Point mousedownlocation = {-1, -1};
        Geometry::Point mouselocation = {-1, -1};
        
        Graphics::Layer *pointerlayer;
        bool iswmpointer = true;
        bool switchbacktolocalptr = false;

		Geometry::Size glsize;
	};
}
