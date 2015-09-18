/// @file OS.h contains operating system functionality. This
///       file does not include any operating system headers       

#pragma once

#include <string>
#include <vector>

namespace Gorgon {
	/// This namespace contains operating system related functionality.
	/// All functions here behaves same way in all supported operating
	/// systems.
	namespace OS {

		/// Initializes operating system module.
		void Initialize();

		namespace User {
			/// Returns the current username
			std::string GetUsername();

			/// Returns the name of the current user
			std::string GetName();

			/// Returns the path where documents of the user should be saved
			std::string GetDocumentsPath();

			/// Returns the home directory of the user
			std::string GetHomePath();

			/// Returns the path where applications can save data related to this user. This path
			/// could simply be user's home directory. Best practice would be creating a directory
			/// that starts with a "." such as .gorgon
			std::string GetDataPath();
		}

		/// Opens a terminal window to display output from the stdout. This is not required
		/// for most operating systems and will not perform anything unless its necessary.
		void OpenTerminal();

		/// This function shows a OS message box to display errors, for other messages
		/// its better to use in-game dialogs
		void DisplayMessage(const std::string &message);

		/// Returns the directory where the system wide application data is stored. Most probably
		/// it will be read only
		std::string GetAppDataPath();

		/// Returns the directory where the system wide application settings is stored. Most probably
		/// it will be read only
		std::string GetAppSettingPath();
		
		/// Returns the value of an environment variable.
		std::string GetEnvVar(const std::string &var);
		
		/// Starts the given application. This application is searched from the installed applications
		/// unless it includes a path. You may use `./appname` to start the appname from the current
		/// directory. The application is started in a separate process and the current process does not
		/// stop to wait its execution.
		/// @warning Depending on the operating system, executables in the current directory might take
		///          precedence even if no path is given.
		/// @return  true if the program is found and started. Does not check if the program continues
		///          working properly
		bool Start(const std::string &name, const std::vector<std::string> &args=std::vector<std::string>());
		
		/// Opens the given file with the related application. This can also be a URI.
		/// @return true if the given file is somehow opened. This includes open with dialog if it can
		///         be displayed
		bool Open(const std::string &file);

		/// This method will notify the system should process any messages that coming from the operating
		/// system. Internally used. Should only be used when necessary.
		void processmessages();
	}
}
