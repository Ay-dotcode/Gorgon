#pragma once

#include <typeinfo>

#include "../String.h"
#include "../OS.h"
#include "../Console.h"

namespace Gorgon {
	namespace Utils {
		
		/// Only works when TEST is defined
		class AssertationException : public std::runtime_error {
		public:
			using std::runtime_error::runtime_error;
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
			CrashHandler(const std::string &expression, const std::string &message, int skip=1, 
						 int depth=4, bool se=false, bool so=false) :
				original(expression), message(message), skip(skip), depth(depth),
				show_expanded(se), show_original(so)
			{
			}
			CrashHandler(int, const std::string &expression, const std::string &message, int skip=1, 
						 int depth=4, bool se=true, bool so=true) :
				original(expression), message(message), skip(skip), depth(depth),
				show_expanded(se), show_original(so)
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
				
#ifdef TEST
				throw AssertationException(message);
#else
				exit(1);
#endif
			}
			
#define gorgon_makeoperator(op) \
	template<class T_> \
	CrashHandler &operator op(const T_ &arg) { \
		expanded += " " #op " " + fromhelper<T_>(arg); \
		return *this; \
	}
			
			// traps right hand side
			template<class T_>
			CrashHandler &operator <(const T_ &arg) {
				if(expanded=="") {
					expanded = fromhelper<T_>(arg);
				}
				else {
					expanded+=" < " + fromhelper<T_>(arg);
				}
				
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
		};
		std::string demangle(const std::string &);
		/// @endcond
		
		/// Returns the human readable form of the typename. By the standard typeid::name is 
		/// not required to be the same as declared type. This function uses compiler facilities
		/// to obtain readable name.
		template<class T_>
		std::string GetTypeName() {
			return demangle(typeid(T_).name());
		}
		
#ifdef NDEBUG
	#define ASSERT(...)
	#define ASSERT_ALL(...)
#else
		/** 
		 * Replaces regular assert to allow messages and backtrace. Has four additional parameters:
		 * skip, which controls how many entries should be skipped from the start in backtrace and
		 * depth to control how many entries should be shown in backtrace. Third parameter is to show
		 * expanded expression. Fourth decides if the original expression should be displayed
		 * skip defaults to 1, depth defaults to 4, expanded defaults to false, original defaults to false
		 */
		#define ASSERT(expression, message, ...) do { if(!bool(expression)) { \
			(Gorgon::Utils::CrashHandler(#expression, message, ##__VA_ARGS__) < expression); } } while(0)
		
		/** 
		 * Replaces regular assert to allow messages and backtrace. Has four additional parameters:
		 * skip, which controls how many entries should be skipped from the start in backtrace and
		 * depth to control how many entries should be shown in backtrace. Third parameter is to show
		 * expanded expression. Fourth decides if the original expression should be displayed
		 * skip defaults to 1, depth defaults to 4, expanded defaults to true, original defaults to true
		 */
		#define ASSERT_ALL(expression, message, ...) do { if(!bool(expression)) { \
			(Gorgon::Utils::CrashHandler(1, #expression, message, ##__VA_ARGS__) < expression); } } while(0)
#endif
	}
}
