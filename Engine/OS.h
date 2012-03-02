#pragma once

#include "../Utils/Point2D.h"
#include "../Utils/Any.h"
#include "../Utils/EventChain.h"

#include "OS.Win32.h"
#include "../Utils/Rectangle2D.h"

using std::string;

namespace gge { namespace os {
	////Initializes OS subsystem by setting up events
	void Initialize();
	////This function shows a OS message box to display errors, for other messages
	/// its better to use in-game dialogs
	void DisplayMessage(const char *Title, const char *Text);
	////Quits the application while returning the given code, if no errors exists
	/// return value should be 0, other return codes should be arranged by application
	void Quit(int ret);
	////Sleep function, should be in ms
	void Sleep(int);
	////Hides the pointer displayed by OS
	void HidePointer();
	////Shows the pointer displayed by OS
	void ShowPointer();

	unsigned int GetTime();

	std::string GetClipbardText();
	void SetClipboardText(const std::string &text);

	namespace system {
		////This function processes OS messages
		void ProcessMessage();
		extern CursorHandle defaultcursor;
		extern bool pointerdisplayed;
	}

	namespace user {
		std::string GetUsername();
		std::string GetDocumentsPath();
	}

	std::string GetAppDataPath();

	namespace filesystem {
		bool CreateDirectory(const std::string &name);

		bool IsDirectoryExists(const std::string Path);

		bool IsFileExists(const std::string Filename);

		class DirectoryIterator {
		public:
			typedef std::forward_iterator_tag iterator_category;
			typedef std::string value_type;
			typedef std::string *pointer_type;
			typedef std::string &reference_type;

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

			DirectoryIterator &operator ++() {
				Next();

				return *this;
			}

			void Next();

			DirectoryIterator &operator +=(int i) {
				for(int j=0;j<i;j++)
					++(*this);
			}

			bool IsValid() const;

			std::string Current() {
				return Get();
			}

			bool operator ==(const class EndOfDirectory &) const {
				return !IsValid();
			}

			bool operator ==(const DirectoryIterator &it) const {
				return it.current==current;
			}

		protected:
			std::string current;
			osdirenum dirinfo;
		};

		static DirectoryIterator EndOfDirectory;
	}

	namespace input {
		////Gets the position of the cursor
		utils::Point getMousePosition(os::WindowHandle Window);
		inline bool isPointerDisplayed() { return system::pointerdisplayed; }
	}

	namespace window {
		extern utils::EventChain<> Activated;
		extern utils::EventChain<> Deactivated;
		extern utils::EventChain<> Destroyed;

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
		WindowHandle CreateWindow(std::string Name, string Title, os::IconHandle Icon, int Left, int Top, int Width, int Height, int BitDepth, bool &FullScreen);

		void MoveWindow(WindowHandle,utils::Point);
		inline void MoveWindow(WindowHandle h,int X,int Y) { MoveWindow(h,utils::Point(X,Y)); }
	}

} }