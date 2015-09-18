#pragma once
#include <string>
#include "../Utils/EventChain.h"
#include <fstream>
#include "../Engine/GGEMain.h"

namespace gge { namespace network {
	namespace http {
		void Initialize();

		std::string BlockingGetText(const std::string &URL);

		class Error : public std::exception {
		public:

			enum Code {
				NoError					=  0,
				BadURL					=  3,
				HostResolutionFailed	=  6,
				ConnectionFailed		=  7,
				AccessDenied			=  9,
				OutOfMemory				= 27,
				LoginError				= 67,
				PageNotFound			=404,
				Unknown					= -1,
			};


			Error(const std::string &message="", Code error=NoError) : message(message), error(error) 
			{ }

			virtual const char* what() const throw() {
				return message.c_str();
			}
			
			virtual ~Error() throw() {}

			std::string message;
			Code   error;
		};
		
		int threadfncall nonblockingop(void *);

		class Nonblocking {
			friend int threadfncall nonblockingop(void *);
		public:

			Nonblocking();
			~Nonblocking() { 
				gge::Main.BeforeRenderEvent.Unregister(token); 
			}

			utils::EventChain<Nonblocking, std::string&> TextTransferComplete;
			//Given vector is temporary, it will be destroyed after used
			//you may swap its data if you need it
			utils::EventChain<Nonblocking, std::vector<Byte>&> DataTransferComplete;
			utils::EventChain<Nonblocking> FileTransferComplete;
			utils::EventChain<Nonblocking, Error> TransferError;

			void GetText(const std::string &URL);
			void GetData(const std::string &URL);
			void GetData(const std::string &URL, std::vector<Byte> &vec);
			void GetFile(const std::string &URL, const std::string &filename);
			void GetStream(const std::string &URL, std::ostream &stream);


			bool IsRunning() const { return isrunning; }

		protected:
			void* curl;
			bool isrunning;
			std::string tempstr;
			std::vector<Byte> *tempvec;
			std::ofstream tempfile;
			Error err;
			utils::EventChain<GGEMain>::Token token;

			void stream(const std::string &URL, std::ostream &stream);
			void getdata(const std::string &URL, std::vector<Byte> &vec);

			enum {
				None,
				Text,
				Data,
				OwnedData,
				File,
				Stream
			} currentoperation;
		};
	}

}}

