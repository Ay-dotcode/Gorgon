/// @file OS.h contains operating system functionality. This
///       file does not refer to any operating system headers       

#pragma once


namespace Gorgon {
	/// This namespace contains operating system related functionality.
	/// All functions here behaves same way in all supported operating
	/// systems.
	namespace OS {
		/// Initializes operating system module.
		void Initialize();

		std::string GetUsername();

		void OpenTerminal(const std::string &Title="", int maxlines=1024);


	}
}


namespace gge { namespace os {
	////Initializes OS subsystem by setting up events
	////This function shows a OS message box to display errors, for other messages
	/// its better to use in-game dialogs
	void DisplayMessage(const char *Title, const char *Text);
	////Quits the application while returning the given code, if no errors exists
	/// return value should be 0, other return codes should be arranged by application
	void Quit(int ret);
	void Cleanup();
	////Sleep function, should be in ms
	void Sleep(int);
	////Hides the pointer displayed by OS
	void HidePointer();
	////Shows the pointer displayed by OS
	void ShowPointer();
	
	class Date {
	public:
		int 
			Year,
			Month,//-> january: 1
			Day,
			Hour,
			Minute,
			Second,
			Millisecond;
			
		enum DayOfWeek {
			Sunday=0,
			Monday,
			Tuesday,
			Wednesday,
			Thursday,
			Friday,
			Saturday
		} Weekday; //-> sunday: 0
	};
	
	Date CurrentDate();

	IconHandle IconFromImage(graphics::ImageData &image);

	unsigned int GetTime();

	std::string GetClipboardText();
	void SetClipboardText(const std::string &text);

	void RunInNewThread(int(threadfncall *fn)(void *), void *data);

	void OpenTerminal(std::string Title="", int maxlines=1024);

	namespace system {
		////This function processes OS messages
		void ProcessMessage();
		extern CursorHandle defaultcursor;
		extern bool pointerdisplayed;
		class mutex_data;
		static int threadfncall fnrunner(void *f) {
			auto &fn=(*(std::function<int()>*)f);
			int ret=fn();
			delete &fn;
			return ret;
		}
	}

	static void RunInNewThread(std::function<int()> fn) {
		auto *f=new std::function<int()>;
		*f=fn;
		RunInNewThread(system::fnrunner, (void*)f);
	}
	
	
	void RunInParallel(std::function<void(int)> fn, int threads);
	

	class Mutex {
	public:
		Mutex();
		~Mutex();
		void Lock();
		void Unlock();

	protected:
		system::mutex_data *data;
	};

	namespace user {
		std::string GetUsername();
		std::string GetDocumentsPath();
	}

	std::string GetAppDataPath();

	namespace filesystem {



		//Slow!
	}

	namespace input {
		////Gets the position of the cursor
		utils::Point GetMousePosition(os::WindowHandle Window);
		inline bool IsPointerDisplayed() { return system::pointerdisplayed; }
	}

	namespace window {
		extern utils::EventChain<> Activated;
		extern utils::EventChain<> Deactivated;
		extern utils::EventChain<> Destroyed;
		extern utils::EventChain<utils::Empty, bool&> Closing;

		//Monitor parameter is ignored for now
		utils::Rectangle UsableScreenMetrics(int Monitor=0);

		////This function creates a window
		///@Name		: Identifier name for the window, should abide by variable naming rules
		///@Title		: Title of the window
		///@Icon		: The icon to be used, depends on OS
		///@Instance	: The handle for the application instance
		///@Left		: Distance of the window from the left of the screen
		///@Top			: Distance of the window from the top of the screen
		///@Width		: The width of the window
		///@Height		: The height of the window
		///@FullScreen	: Whether to create a full screen window
		WindowHandle CreateWindow(std::string Name, std::string Title, os::IconHandle Icon, int Left, int Top, int Width, int Height, int BitDepth, bool Show, bool &FullScreen);

		void MoveWindow(WindowHandle,utils::Point);
		void ResizeWindow(WindowHandle,utils::Size);

		void Hide(WindowHandle);
		void Show(WindowHandle);
	}

} }