#pragma once

#undef CreateWindow

#include "../Utils/Any.h"
#include "../Utils/EventChain.h"
#include "../Utils/Size2D.h"
#include "OS.h"


#include "Layer.h"


namespace gge {
	struct IntervalObject;

	////Function definition required to handle interval signal events
	typedef std::function<void(IntervalObject &)> IntervalSignalEvent;

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
		////Whether this object is enabled, when enabled
		/// this interval will continue from where it left
		/// most probably triggering a signal in the next
		/// signal check, this behavior can be prevented by
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
	class GGEMain : public LayerBase{
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

		void Setup(string SystemName, int Width=800, int Height=600, int BitDepth=32, bool FullScreen=false);

		////Current system time. This time does not change in a given game loop
		unsigned int CurrentTime;

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
		os::WindowHandle CreateWindow(string Title, os::IconHandle Icon, int X=0, int Y=0);

		////Initializes graphic subsystem. All graphic related functions will fail if
		/// called before this method. 
		os::DeviceHandle InitializeGraphics();
		////Initializes sound subsystem. All sound related functions will fail if
		/// called before this method.
		void		 InitializeSound();
		////Initializes input subsystem. All check on keyboard state before calling
		/// this method is error prone. This method should be called before any
		/// call to BeforeGameLoop function.
		void		 InitializeInput();
		////Initializes OS subsystem allowing it to setup events. Should be called before creating a window
		void		 InitializeOS();
		////Initializes all systems creating the main window
		void		 InitializeAll(string Title, os::IconHandle Icon, int X, int Y);
		////Initializes all systems creating the main window and centering
		void		 InitializeAll(string Title, os::IconHandle Icon=NULL);
		////Initializes Animation subsystem. Before calling this function, animations does not progress
		/// automatically.
		void		 InitializeAnimation();
		////Initializes Pointer subsystem. Before calling this function, pointers cannot be used
		void		 InitializePointer();
		////Initializes Pointer subsystem. Before calling this function, pointers cannot be used
		void		 InitializeResource();
		void		 InitializeMultimedia();

		void		 Run();

		////Registers a signal handler to be called in every given time. Exact time passed from the
		/// last signal can be checked using LastSignal variable of the IntervalObject
		IntervalObject *RegisterInterval(unsigned int Timeout, IntervalSignalEvent Signal);
		////Unregisters and destroys given interval. Given interval object is destroyed therefore,
		/// it cannot be used after this function is called
		void UnregisterInterval(IntervalObject *Interval);

		void Exit(int code);

		void Cleanup();


		void MoveWindow(int X, int Y) {
			MoveWindow(utils::Point(X, Y));
		}

		void MoveWindow(utils::Point p) {
			if(!FullScreen)
				os::window::MoveWindow(Window, p);
		}

		void CenterWindow();

		void ResizeWindow(int W, int H) {
			ResizeWindow(utils::Size(W,H));
		}

		void ResizeWindow(utils::Size size);

		////This event is triggered before rendering, after intervals
		utils::EventChain<GGEMain, utils::empty_event_params> BeforeRenderEvent;

		////This event is triggered before game loop, before intervals and after processing window messages
		utils::EventChain<GGEMain, utils::empty_event_params> BeforeGameLoopEvent;

		////This event is triggered after rendering before the next
		/// game loop
		utils::EventChain<GGEMain> AfterRenderEvent;

		utils::EventChain<GGEMain> BeforeTerminateEvent;

		utils::EventChain<GGEMain> ResizeEvent;

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

		bool isrunning;

		static void adjustlayers_recurse(LayerBase &layer, utils::Size from, utils::Size to);
		void adjustlayers(utils::Size size);
		
		void initializerest(string Title, os::IconHandle Icon, int X, int Y);

		utils::Collection<IntervalObject> IntervalObjects;
	};

	extern GGEMain Main;
}
