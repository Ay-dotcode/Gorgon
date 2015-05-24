#pragma once

#pragma warning(push)
#pragma warning(disable:4297)

#include <typeinfo>

#include "../String.h"
#include "../OS.h"
#include "../Console.h"
#include "Compiler.h"

namespace Gorgon {
	namespace Utils {
		
		/// Only works when TEST is defined
		class AssertationException : public std::runtime_error {
		public:
			//using std::runtime_error::runtime_error;

			AssertationException(const std::string &what) : std::runtime_error(what) { }
		};

		/// @cond INTERNAL
		class CrashHandler {
			template<class T_>
			typename std::enable_if<String::CanBeStringified<T_>::Value, std::string>::type
			fromhelper(const T_ &t) {
				return String::From(t);
			}

			template<class T_>
			typename std::enable_if<!String::CanBeStringified<T_>::Value, std::string>::type
			fromhelper(const T_ &t) {
				return "??";
			}
		public:
			static struct alltag {} AllTag;
			static struct dumponlytag {} DumpOnlyTag;
			
			CrashHandler(const std::string &expression, const std::string &message, int skip=1, 
						 int depth=4, bool se=false, bool so=false) :
				original(expression), message(message), skip(skip), depth(depth),
				show_expanded(se), show_original(so)
			{
			}
			CrashHandler(alltag, const std::string &expression, const std::string &message, int skip=1, 
						 int depth=4, bool se=true, bool so=true) :
				original(expression), message(message), skip(skip), depth(depth),
				show_expanded(se), show_original(so)
			{
			}
			CrashHandler(dumponlytag, const std::string &expression, const std::string &message, int skip=1, 
						 int depth=4, bool se=false, bool so=false) :
				original(expression), message(message), skip(skip), depth(depth),
				show_expanded(se), show_original(so), dumponly(true)
			{
			}
			
			
			~CrashHandler() {
				Console::SetColor(Console::Red);
				Console::SetBold();
				
				std::cout<<"Assertation failed: ";
				Console::SetColor(Console::Default);
				std::cout<<message<<std::endl;
				Console::Reset();
				
				if(show_original) {
					std::cout<<"  "<<original;
					Console::SetColor(Console::Red);
					std::cout<<" evaluates to false"<<std::endl;
					Console::Reset();
				}
				
				if(show_expanded) {
					Console::SetColor(Console::Green);
					std::cout<<"  "<<expanded;
					Console::SetColor(Console::Red);
					std::cout<<"  ==>  FALSE"<<std::endl;
					Console::Reset();
				}

				Backtrace();
				
				Console::Reset();
				if(!dumponly) {
#ifdef TEST
					throw AssertationException(message);
#else
					exit(1);
#endif
				}
			}
	
	//!Expansion is disabled
#define gorgon_makeoperator(op) \
	template<class T_> \
	CrashHandler &operator op(const T_ &arg) { \
		return *this; \
	}
//		expanded += " " #op " " + fromhelper<T_>(arg); \
			
			// traps right hand side
			template<class T_>
			CrashHandler &operator <(const T_ &arg) {
// 				if(expanded=="") {
// 					expanded = fromhelper<T_>(arg);
// 				}
// 				else {
// 					expanded+=" < " + fromhelper<T_>(arg);
// 				}
				
				return *this;
			}
			
			void Backtrace();
			
			gorgon_makeoperator(==)
			gorgon_makeoperator(!=)
			gorgon_makeoperator(>)
			gorgon_makeoperator(>=)
			gorgon_makeoperator(<=)
			gorgon_makeoperator(||)
			gorgon_makeoperator(&&)
			
#undef gorgon_makeoperator
			
		protected:
			std::string original;
			std::string expanded;
			std::string message;
			
			int skip;
			int depth;
			
			bool show_original;
			bool show_expanded;
			
			bool dumponly=false;
		};
		/// @endcond
		
#ifdef NDEBUG
	#define ASSERT(...)
	#define ASSERT_ALL(...)
	#define ASSERT_DUMP(...)
#else
		/** 
		 * Replaces regular assert to allow messages and backtrace. Has four additional parameters:
		 * skip, which controls how many entries should be skipped from the start in backtrace and
		 * depth to control how many entries should be shown in backtrace. Third parameter is to show
		 * expanded expression. Fourth decides if the original expression should be displayed
		 * skip defaults to 1, depth defaults to 4, expanded defaults to false, original defaults to false
		 */
		#define ASSERT(expression, message, ...) do { if(!bool(expression)) { \
			(Gorgon::Utils::CrashHandler(#expression, message, ##__VA_ARGS__)); } } while(0)
		
		/** 
		 * Replaces regular assert to allow messages and backtrace. Has four additional parameters:
		 * skip, which controls how many entries should be skipped from the start in backtrace and
		 * depth to control how many entries should be shown in backtrace. Third parameter is to show
		 * expanded expression. Fourth decides if the original expression should be displayed
		 * skip defaults to 1, depth defaults to 4, expanded defaults to true, original defaults to true
		 */
		#define ASSERT_ALL(expression, message, ...) do { if(!bool(expression)) { \
			(Gorgon::Utils::CrashHandler(Gorgon::Utils::CrashHandler::AllTag, #expression, message, ##__VA_ARGS__)); } } while(0)

		/** 
		 * Replaces regular assert to allow messages and backtrace.  This variant does not crash or exit the 
		 * program. It simply dumps the message and stacktrace if assertation fails. Has four additional parameters:
		 * skip, which controls how many entries should be skipped from the start in backtrace and
		 * depth to control how many entries should be shown in backtrace. Third parameter is to show
		 * expanded expression. Fourth decides if the original expression should be displayed
		 * skip defaults to 1, depth defaults to 4, expanded defaults to false, original defaults to false
		 */
		#define ASSERT_DUMP(expression, message, ...) do { if(!bool(expression)) { \
			(Gorgon::Utils::CrashHandler(Gorgon::Utils::CrashHandler::DumpOnlyTag,#expression, message, ##__VA_ARGS__)); } } while(0)
			
#	ifdef _MSC_VER
		__declspec(noreturn) inline void NotImplemented(const std::string &what="This feature") { ASSERT(false, what+" is not implemented.", 0, 8); }
		
		__declspec(noreturn) inline void ASSERT_FALSE(const std::string &message, int skip=1, int depth=4) {
			ASSERT(false, message, skip, depth); 		
		}
#	else
		void NotImplemented (const std::string &what="This feature") __attribute__((noreturn));
		void ASSERT_FALSE(const std::string &message, int skip=1, int depth=4) __attribute__((noreturn));
		
		inline void NotImplemented (const std::string &what) { 
			ASSERT(false, what+" is not implemented.", 0, 8); 
			throw 0;
		}
		inline void ASSERT_FALSE(const std::string &message, int skip, int depth) { 
			ASSERT(false, message, skip, depth);
			throw 0;
		}
#	endif
#endif
	}
}
#pragma warning(pop)
