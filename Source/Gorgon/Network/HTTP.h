#pragma once

#include <string>
#include <fstream>
#include <memory>
#include <mutex>
#include <thread>

#include "../Event.h"
#include "../Main.h"

//!Add info about Encoding::URI

namespace Gorgon { namespace Network {
	namespace HTTP {
		/// Initializes HTTP networking system. It is called automatically
		void Initialize();

		///
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
		

		/// This class supports non-blocking HTTP operations. 
		class Nonblocking {
		public:

			Nonblocking();
			~Nonblocking() { 
				BeforeFrameEvent.Unregister(token);
			}

			Event<Nonblocking, std::string&> TextTransferCompletedEvent;
			
			//Given vector is temporary, it will be destroyed after used
			//you may swap its data if you need it
			Event<Nonblocking, std::vector<Byte>&> DataTransferCompletedEvent;
			
			Event<Nonblocking> FileTransferCompletedEvent;
			
			Event<Nonblocking, Error> TransferErrorEvent;

			void GetText(const std::string &URL);
			void GetData(const std::string &URL);
			void GetData(const std::string &URL, std::vector<Byte> &vec);
			void GetFile(const std::string &URL, const std::string &filename);
			void GetStream(const std::string &URL, std::ostream &stream);


			bool IsRunning() const { return isrunning; }

		protected:
			void deletevec(std::vector<Byte> *vec) {
				if(currentoperation!=OwnedData) {
					delete vec;
				}
			}
			
			void operation();

			void stream(const std::string &URL, std::ostream &stream);
			void getdata(const std::string &URL, std::vector<Byte> &vec);
			
			void* curl;
			std::string tempstr;
			std::unique_ptr<std::vector<Byte>, std::function<void(std::vector<Byte>*)>> tempvec={nullptr, std::bind(&Nonblocking::deletevec, this, std::placeholders::_1)};
			std::ofstream tempfile;
			Error err;
			Event<>::Token token;
			
			std::mutex mtx;
			std::thread runner;
			
			bool isrunning = false;
			
			

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

