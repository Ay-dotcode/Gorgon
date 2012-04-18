#pragma once
#include <string>
#include "../Utils/EventChain.h"

namespace gge { namespace network {
	namespace http {
		void Initialize();

		std::string BlockingGetText(const std::string &URL);

		class Error : public std::exception {
		public:

			enum Code {
				BadURL					=  3,
				HostResolutionFailed	=  6,
				ConnectionFailed		=  7,
				AccessDenied			=  9,
				OutOfMemory				= 27,
				LoginError				= 67,
				PageNotFound			=404,
				Unknown					= -1,
			};


			Error(const std::string &message, Code error) : message(message), error(error) 
			{ }

			virtual const char* what() const throw() {
				return message.c_str();
			}

			std::string message;
			Code   error;
		};

		class Nonblocking {
		public:

			Nonblocking() : TextTransferComplete("Text", this),
				DataTransferComplete("Data", this),
				FileTransferComplete("File", this)
			{ }

			utils::EventChain<Nonblocking, std::string&> TextTransferComplete;
			utils::EventChain<Nonblocking, std::vector<Byte>&> DataTransferComplete;
			utils::EventChain<Nonblocking, std::string> FileTransferComplete;
			utils::EventChain<Nonblocking, Error> TransferError;

			void GetText(const std::string &URL);
			void GetData(const std::string &URL);
			void GetFile(const std::string &URL, const std::string &filename);

		protected:
			void* curl;
			bool isrunning;
		};

		extern Nonblocking NonblockingTransfer;
	}

}}

