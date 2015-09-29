#pragma once

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <map>
#include <memory>

#include "../Utils/Assert.h"
#include "../Filesystem.h"
#include "../IO/Stream.h"

namespace Gorgon { namespace Resource {

	/// This error is fired from a write operations
	class WriteError : public std::runtime_error {
	public:
		
		/// Error types
		enum ErrorType {
			
			/// The cause of the error cannot be determined
			Unknown = 0,
			
			/// The given file cannot be opened, probably its path does not exists
			/// or the operation is denied.
			CannotOpenFile = 1,
			
			///There is no data to save
			NoData = 2,
		};
		
		
		/// Regular constructor, creates error text from error number
		WriteError(ErrorType number=Unknown) : runtime_error(ErrorStrings[number]), number(number) {
			
		}
		
		/// A constructor to allow custom text for the error
		LoadError(ErrorType number, const std::string &text) : runtime_error(text), number(number) {
			
		}
		
		/// The type of loading error occurred
		ErrorType number;
		
		/// Strings for error codes
		static const std::string ErrorStrings[3];
	};
	
	/**
	* This class allows resource objects to save their data to a stream. It provides functionality
	* to write data platform independently. This class also allows back and forth writing to easy
	* writing Gorgon resources.
	*/
	class Writer {
	public:
		
	protected:
		/// This function should close the stream. The pointer will be unset
		/// by Reader class
		virtual void close() = 0;

		/// This function should open the stream and set stream member. If thrw is set
		/// to true and stream cannot be opened, a WriteError should be thrown. Otherwise
		/// this function is not allowed to throw.
		virtual bool open(bool thrw) = 0;

		/// This is the stream that will be used to write data to. Underlying writers
		/// can have specialized copies of this member.
		std::ostream *stream = nullptr;

	};
} }
