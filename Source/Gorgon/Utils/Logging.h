#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include "UtilsBase.h"

namespace gge { namespace utils {
	class Logger {
	public:
		Logger() : stream(nullptr), owner(false) { }

		~Logger() {
			CleanUp();
		}

		void InitializeConsole() {
			CleanUp();
			stream=&std::cout;
			owner=false;
		}

		//ownership is not transferred
		void InitializeFile(std::ofstream &file) {
			CleanUp();
			stream=&file;
			owner=false;
		}

		void InitializeFile(const std::string& filename) {
			CleanUp();
			stream=new std::ofstream(filename);
			owner=true;
		}

		void CleanUp() {
			if(owner) {
				gge::utils::CheckAndDelete(stream);
			}
			else {
				stream=nullptr;
			}
		}

		template<class T_>
		Logger &operator<<(const T_ &v) {
			if(stream) {
				(*stream)<<v;
#ifdef _DEBUG
				if(!(*stream)) {
					throw std::runtime_error("Logging error!");
				}
#endif
			}

			return *this;
		}

	protected:
		std::ostream *stream;
		bool owner;
	};

} }
