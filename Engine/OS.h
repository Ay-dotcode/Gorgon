#pragma once

#include "../Utils/Point2D.h"
#include "../Utils/Any.h"
#include "../Utils/EventChain.h"

#include "OS.Win32.h"
#include "OS.Linux.h"
#include "../Utils/Rectangle2D.h"
#include "../Utils/Size2D.h"


namespace gge {
	namespace graphics {
		class ImageData;
	}
}



namespace gge { namespace os {
	////Initializes OS subsystem by setting up events
	void Initialize();
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

	int CurrentHour();
	int CurrentMinute();
	int CurrentSecond();
	int CurrentMillisecond();
	int CurrentWeekday(); //monday->0
	int CurrentDay();
	int CurrentMonth();
	
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

	namespace system {
		////This function processes OS messages
		void ProcessMessage();
		extern CursorHandle defaultcursor;
		extern bool pointerdisplayed;
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

	namespace user {
		std::string GetUsername();
		std::string GetDocumentsPath();
	}

	std::string GetAppDataPath();

	namespace filesystem {
		bool CreateDirectory(const std::string &name);

		bool IsDirectoryExists(const std::string &Path);

		bool IsFileExists(const std::string &Filename);

		void DeleteFile(const std::string &Filename);

		class DirectoryIterator {
		public:
			typedef std::forward_iterator_tag iterator_category;
			typedef std::string value_type;
			typedef std::string *pointer_type;
			typedef std::string &reference_type;
			typedef std::string *pointer;
			typedef std::string &reference;
			typedef int difference_type;
			typedef int distance_type;

			typedef std::string Type;

			DirectoryIterator(const std::string &dir, const std::string &pattern="*");
			DirectoryIterator(const DirectoryIterator &dir);
			DirectoryIterator();

			std::string Get() const {
				return current;
			}

			operator std::string() const {
				return Get();
			}

			std::string operator *() const {
				return Get();
			}
			
			std::string *operator ->() {
				return &current;
			}

			DirectoryIterator &operator ++() {
				Next();

				return *this;
			}

			void Next();

			DirectoryIterator &operator +=(int i) {
				for(int j=0;j<i;j++)
					++(*this);
				
				return *this;
			}

			bool IsValid() const;

			std::string Current() {
				return Get();
			}

			bool operator ==(const DirectoryIterator &it) const {
				return it.current==current;
			}

			bool operator !=(const DirectoryIterator &it) const {
				return it.current!=current;
			}

		protected:
			std::string current;
			osdirenum dirinfo;
		};

		extern DirectoryIterator EndOfDirectory;
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
		WindowHandle CreateWindow(std::string Name, std::string Title, os::IconHandle Icon, int Left, int Top, int Width, int Height, int BitDepth, bool &FullScreen);

		void MoveWindow(WindowHandle,utils::Point);
		void ResizeWindow(WindowHandle,utils::Size);
	}

} }