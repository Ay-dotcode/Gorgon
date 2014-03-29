#include "Geometry/Point.h"

/// @file contains window manager dependent functionality.

namespace Gorgon {
	/// This namespace contains window manager specific functionality. 
	/// This including creating windows, obtaining input, creating
	/// graphics context.
	namespace WindowManager {
		/// Initializes window manager system
		void Initialize();

		Geometry::Rectangle UsableScreenMetrics(int Monitor=0);

		/// This class represents a window. 
		class Window {
		public:
			/// Creates a new window
			/// @param  rect the position and **interior** size of the window
			Window(const Geometry::Rectangle &rect, bool visible=true);

			/// Creates a fullscreen window
			Window();

			/// This method is automatically called by the system.
			/// Unless its necessary, do not use it.
			void processmessages();

			void Move(Geometry::Point);
			void Resize(Geometry::Size);

			void Show();
			void Hide();

			utils::EventChain<Window> Activated;
			utils::EventChain<Window> Deactivated;
			utils::EventChain<Window> Destroyed;
			utils::EventChain<Window, bool&> Closing;
		};
	}
}