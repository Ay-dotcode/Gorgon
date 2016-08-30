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

namespace Gorgon {
	/// @cond INTERNAL
	namespace internal {
		struct windowdata;
	}
	/// @endcond

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
		/// @param  title of the window
		/// @param  visible after creation, window will be visible or invisible depending
		///         on this value. 
		Window(Geometry::Rectangle rect, const std::string &name, const std::string &title, bool visible=true);
		
		/// Creates a new window
		/// @param  rect the position and the **interior** size of the window unless
		///         use outer metrics is set to true
		/// @param  name of the window
		/// @param  visible after creation, window will be visible or invisible depending
		///         on this value. 
		Window(Geometry::Rectangle rect, const std::string &name, bool visible=true) :
			Window(rect, name, name, visible) { }

		/// Creates a new window at the center of the screen
		/// @param  size of the window
		/// @param  name of the window
		/// @param  title of the window
		/// @param  visible after creation, window will be visible or invisible depending
		///         on this value. 
		Window(const Geometry::Size &size, const std::string &name, const std::string &title, bool visible=true) :
			Window({automaticplacement, size}, name, title, visible) { }

		/// Creates a new window at the center of the screen
		/// @param  size of the window
		/// @param  name of the window
		/// @param  visible after creation, window will be visible or invisible depending
		///         on this value. 
		Window(const Geometry::Size &size, const std::string &name, bool visible=true) :
			Window({automaticplacement, size}, name, name, visible) { }

		/// Creates a fullscreen window. Fullscreen windows do not have chrome and covers
		/// entire screen, including any panels it contains.
		Window(const FullscreenTag &, const std::string &name, const std::string &title="");

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

		//+GetChrome

		/// Displays this window, may generate Activated event
		virtual void Show() override;

		/// Hides this window, may generate Deactivated event
		virtual void Hide() override;
		
		/// Returns whether this layer is effectively visible
		virtual bool IsVisible() const override { return isvisible; }

		/// Closes the window. After this function, any use of this object might fail.
		void Close();

		/// Shows the pointer displayed by window manager
		void ShowPointer();

		/// Hides the pointer displayed by window manager
		void HidePointer();

		/// Renders the contents of the window
		virtual void Render() override;

		/// @name Events 
		/// @{
		
		/// Called when this window is activated
		Event<Window> ActivatedEvent{*this};

		/// Called when this window is deactivated
		Event<Window> DeactivatedEvent{*this};

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

		/// Called after the window is restored, either by the user or programmatically
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
		
	protected:
		/// A window cannot be placed in another layer. This function always fails.
		virtual void located(Layer *) override { Utils::ASSERT_FALSE("A window cannot be placed in another layer"); }


		/// Propagates a mouse event. Some events will be called directly.
		virtual MouseHandler propagate_mouseevent(Input::Mouse::EventType event, Geometry::Point location, Input::Mouse::Button button, int amount) override;
        
        void deleting(Layer *layer) { if(layer==down) down = nullptr; }

	private:
		void createglcontext();
		
		internal::windowdata *data;

		static Containers::Collection<Window> windows;

		static const Geometry::Point automaticplacement;
        
        MouseHandler down = nullptr;
	};
}
