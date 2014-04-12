/// @file OS.h contains operating system functionality. This
///       file does not include any operating system headers       

#pragma once

#include <string>

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

			/// Returns the path where applications can save data related to this user
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

		/// Internally used. Should only be used when necessary.
		void processmessages();
	}
}
