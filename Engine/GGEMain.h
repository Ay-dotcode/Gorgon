#pragma once

#include "../Utils/Any.h"
#include "../Utils/EventChain.h"

#include "../Widgets/WidgetMain.h"

#include "OS.h"
#include "Sound.h"
#include "Pointer.h"
#include "Graphics.h"
#include "Input.h"
#include "Layer.h"
#include "Animator.h"

namespace gge {
	struct IntervalObject;

	////Function definition required to handle interval signal events
	typedef void (*IntervalSignalEvent)(IntervalObject *interval, void *data);

	////Function definition to handle notifications
	typedef void (*Notification)();

	////This structure holds information about intervals
	/// its variables can be modified, intervals are checked
	/// before "BeforeRender" event, so if this object is modified
	/// the modifications will not be effective before the next loop
	struct IntervalObject {
		////Timeout interval in msec
		unsigned int Timeout;
		////This variable is used internally to track when
		/// this interval is signaled
		unsigned int LastSignal;
		////The data to ba passed signal handler
		void *Data;
		////Wheter this object is enabled, when enabled
		/// this interval will continue from where it left
		/// most probably triggering a signal in the next
		/// signal check, this behaviour can be prevented by
		/// setting LastSignal to current time while enabling
		/// the interval object
		bool Enabled;
		////The signal handler
		IntervalSignalEvent Signal;

		void Reset();
	};

	////This is main class of Gorgon Game Engine. This class is responsible
	/// of organizing various resources. Although there is no mechanism to
	/// prevent it, multiple GGEMain classes are not possible currently.
	/// Before calling any initialization function apart from OS, a window should be created.
	/// GGEMain is also the topmost layer.
	class GGEMain : public LayerBase, public input::BasicPointerTarget {
	public:
		////Handle of the device context specifying drawable area of
		/// the created window. Can be changed but should be modified with care
		os::DeviceHandle Device;

		int FPS;

		////Width of the graphics area
		inline int getWidth() { return Width; }
		////Height of the graphics area
		inline int getHeight() { return Height; }
		////Bitdepth of screen if running full screen, this function does not
		/// return active bitdepth, only the specified value
		inline int getBitDepth() { return BitDepth; }
		////Whether running in windowed mode
		inline bool getFullScreen() { return FullScreen; }
		////The name of the system (code name of the game)
		inline string getSystemName() { return SystemName; }
		////Handle of the window
		inline os::WindowHandle getWindow() { return Window; }
		////Handle of the application instance
		inline os::InstanceHandle getInstance() { return Instance; }

		void Setup(string SystemName, os::InstanceHandle Instance, int Width=800, int Height=600, int BitDepth=32, bool FullScreen=false);

		////Current system time. This time does not change in a given game loop
		unsigned int CurrentTime;

		////This is the top-level mouse event propagator
		virtual bool PropagateMouseEvent(input::MouseEventType event, int x, int y, void *data) {
			return LayerBase::PropagateMouseEvent(event, x, y, data);
		}

		////This is the top-level mouse event propagator
		virtual bool PropagateMouseScrollEvent(int amount, input::MouseEventType event, int x, int y, void *data) {
			return LayerBase::PropagateMouseScrollEvent(amount, event, x, y, data);
		}

		////Constructor
		GGEMain();

		////This function should be called within the game loop before anything else
		void BeforeGameLoop();
		////This function must be called after every drawing function before
		/// calling Render function. The order shall not be changed but you may
		/// insert additional operations between this function and Render function.
		void BeforeRender();
		////Renders everything that is marked to be drawn. Must be called after
		/// "BeforeRender" function
		virtual void Render();
		////This function should be called after Render function. At this step, any time consuming
		/// operations that are not related with graphics are made.
		void AfterRender();

		////This function creates the game window. You may specify title, icon and position
		/// of the window. However, position is ignored if this is a full screen application.
		/// OS should be initialized before calling this function
		os::WindowHandle CreateWindow(string Title, os::IconHandle Icon, int X=0, int Y=0) {
			Window=os::window::CreateWindow(SystemName,Title,Icon,Instance,X,Y,Width,Height,BitDepth,FullScreen);
			return Window;
		}

		////Initializes graphic subsystem. All graphic related functions will fail if
		/// called before this method. 
		os::DeviceHandle InitializeGraphics() { Device = gge::graphics::Initialize(Window, BitDepth, Width, Height); return Device; }
		////Initializes sound subsystem. All sound related functions will fail if
		/// called before this method.
		void		 InitializeSound() { gge::sound::system::InitializeSound(Window); }
		////Initializes input subsystem. All check on keyboard state before calling
		/// this method is error prone. This method should be called before any
		/// call to BeforeGameLoop function.
		void		 InitializeInput() { 
			gge::input::Initialize(); 
			gge::input::AddPointerTarget(this,0); 
		}
		////Initializes OS subsystem allowing it to setup events. Should be called before creating a window
		void		 InitializeOS() { gge::os::Initialize(); }
		////Initializes all systems creating the main window
		void		 InitializeAll(string Title, os::IconHandle Icon, int X=0, int Y=0);
		////Initializes Animation subsystem. Before calling this function, animations does not progress
		/// automatically.
		void		 InitializeAnimation() { gge::InitializeAnimation(this); }
		////Initializes Pointer subsystem. Before calling this function, pointers cannot be used
		void		 InitializePointer() { Pointers.Initialize(*this); }
		void		 InitializeWidgets() { gge::widgets::InitializeWidgets(this); }

		////Registers a signal handler to be called in every given time. Exact time passed from the
		/// last signal can be checked using LastSignal variable of the IntervalObject
		IntervalObject *RegisterInterval(unsigned int Timeout, void* Data, IntervalSignalEvent Signal);
		////Unregisters and destroys given interval. Given interval object is destroyed therefore,
		/// it cannot be used after this function is called
		void UnregisterInterval(IntervalObject *Interval);


		////This event is triggered before rendering, after intervals
		utils::EventChain<GGEMain, utils::empty_event_params> BeforeRenderEvent;
		////This event is triggered after rendering before the next
		/// game loop
		utils::EventChain<GGEMain, utils::empty_event_params> AfterRenderEvent;

		////Destructor, cleans up resources
		~GGEMain();

	protected:
		////Width of the graphics area
		int Width;
		////Height of the graphics area
		int Height;
		////Bitdepth of screen if running fullscreen
		int BitDepth;
		////Whether running in fullscreen mode
		bool FullScreen;
		////The name of the system (code name of the game)
		/// should contain alpha numeric characters obeying
		/// variable naming rules
		string SystemName;
		////Handle of the window
		os::WindowHandle Window;
		////Handle of the application instance
		os::InstanceHandle Instance;

		utils::Collection<IntervalObject> IntervalObjects;
	};

	extern GGEMain Main;
}
