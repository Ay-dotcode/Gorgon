/// @file String.h This file contains string helper functions.
/// They work with std::string

#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include "String/Exceptions.h"
#include "Enum.h"

namespace Gorgon { 
	
	/// Contains string related functions and classes.
	namespace String {
		
		/// @cond INTERNAL
		namespace internal {
			/// SFINAE trick to detect if class has string operator
			template <typename T>
			class has_stringoperator
			{
				typedef char one;
				struct two {
					char a[2];
				};

				template <typename C> static one test( decltype(((C*)(nullptr))->operator std::string()) aa ) {return one();}
				template <typename C> static two test(...){return two();}

			public:
				enum { value = sizeof(test<T>(""))==sizeof(char) };
			};
		}
		/// @endcond
		
		struct CaseInsensitiveLess {
			bool operator()(const std::string &left, const std::string &right) const {
				unsigned len=std::min(left.length(), right.length());
				
				auto l=left.begin();
				auto r=right.begin();
				for(unsigned i=0; i<len; i++) {
					auto lc=tolower(*l);
					auto rc=tolower(*r);
					if(lc<rc) {
						return true; 
					}
					else if(lc>rc) {
						return false;
					}
					++l;
					++r;
				}
				
				return left.length()<right.length();
			}
		};
		
		/// Compares two strings case insensitive. Works similar to strcmp
		inline int CaseInsensitiveCompare(const std::string &left, const std::string &right) {
			unsigned len=std::min(left.length(), right.length());
			
			auto l=left.begin();
			auto r=right.begin();
			for(unsigned i=0; i<len; i++) {
				auto lc=tolower(*l);
				auto rc=tolower(*r);
				if(lc<rc) {
					return -1;
				}
				else if(lc>rc) {
					return 1;
				}
				++l;
				++r;
			}
			
			return left.length()<right.length() ? -1 : (left.length()==right.length() ? 0 : 1);
		}
		

#ifdef DOXYGEN
		/// Converts a string to another type. Works for integral types and
		/// Gorgon classes including Point, Size, etc... There is no error
		/// handling. If conversion does not work, you may end up with
		/// uninitialized object. This system will be fixed at a later point.
		template <class T_>
		T_ To(const std::string &value) {
			return T_();
		}
#endif		
		
		/// @cond

#ifdef _MSC_VER
#	define decltype(...) std::identity<decltype(__VA_ARGS__)>::type
#	define decltypetype(...) typename decltype(__VA_ARGS__)
#	define ISENUMUPGRADED enumupgradedhelper<T_>::value
	template<class T_>
	struct enumupgradedhelper {
		enum {
			value = decltype(gorgon__enum_trait_locator(T_()))::isupgradedenum,
		};
	};
#else
#	define decltypetype(...) decltype(__VA_ARGS__)
#	define ISENUMUPGRADED	decltype(gorgon__enum_trait_locator(T_()))::isupgradedenum
#endif
		template <class T_>
		typename std::enable_if<std::is_constructible<T_, std::string>::value, T_>::type
		To(const std::string &value) {
			return T_(value);
		}
		
		template <class T_>
		typename std::enable_if<!std::is_constructible<T_, std::string>::value && !ISENUMUPGRADED, T_>::type
		To(const std::string &value) {
			std::stringstream ss(value);

			T_ ret;
			ss>>ret;

			return ret;
		}
		
		template <class T_>
		typename std::enable_if<std::is_constructible<T_, const char*>::value, T_>::type
		To(const char *value) {
			return T_(value);
		}
		
		template <class T_>
		typename std::enable_if<!std::is_constructible<T_, const char*>::value && std::is_constructible<T_, std::string>::value, T_>::type
		To(const char *value) {
			return T_(value);
		}
		
		template <class T_>
		typename std::enable_if<!std::is_constructible<T_, const char*>::value && 
			!std::is_constructible<T_, std::string>::value && 
			!ISENUMUPGRADED,
			T_>::type
		To(const char *value) {
			std::stringstream ss(value);

			T_ ret;
			ss>>ret;

			return ret;
		}

		template <>
		inline char To<char>(const std::string &value) {
			char *n;
			return (char)std::strtol(value.c_str(), &n, 10);
		}

		template <>
		inline unsigned char To<unsigned char>(const std::string &value) {
			char *n;
			return (unsigned char)std::strtol(value.c_str(), &n, 10);
		}

		template <>
		inline short To<short>(const std::string &value) {
			char *n;
			return (short)std::strtol(value.c_str(), &n, 10);
		}

		template <>
		inline unsigned short To<unsigned short>(const std::string &value) {
			char *n;
			return (unsigned short)std::strtol(value.c_str(), &n, 10);
		}

		template <>
		inline int To<int>(const std::string &value) {
			char *n;
			return (int)std::strtol(value.c_str(), &n, 10);
		}

		template <>
		inline unsigned To<unsigned>(const std::string &value) {
			char *n;
			return (unsigned)std::strtol(value.c_str(), &n, 10);
		}

		template <>
		inline long To<long>(const std::string &value) {
			char *n;
			return (long)std::strtol(value.c_str(), &n, 10);
		}

		template <>
		inline unsigned long To<unsigned long>(const std::string &value) {
			char *n;
			return (unsigned long)std::strtol(value.c_str(), &n, 10);
		}

		template <>
		inline long long To<long long>(const std::string &value) {
			char *n;
			return (long long)std::strtol(value.c_str(), &n, 10);
		}

		template <>
		inline unsigned long long To<unsigned long long>(const std::string &value) {
			char *n;
			return (unsigned long long)std::strtol(value.c_str(), &n, 10);
		}

		template <>
		inline float To<float>(const std::string &value) {
			return (float)std::atof(value.c_str());
		}

		template <>
		inline double To<double>(const std::string &value) {
			return std::atof(value.c_str());
		}

		template <>
		inline long double To<long double>(const std::string &value) {
			return std::atof(value.c_str());
		}
		
		template <>
		inline char To<char>(const char *value) {
			char *n;
			return (char)std::strtol(value, &n, 10);
		}

		template <>
		inline unsigned char To<unsigned char>(const char *value) {
			char *n;
			return (unsigned char)std::strtol(value, &n, 10);
		}

		template <>
		inline short To<short>(const char *value) {
			char *n;
			return (short)std::strtol(value, &n, 10);
		}

		template <>
		inline unsigned short To<unsigned short>(const char *value) {
			char *n;
			return (unsigned short)std::strtol(value, &n, 10);
		}

		template <>
		inline int To<int>(const char *value) {
			char *n;
			return (int)std::strtol(value, &n, 10);
		}

		template <>
		inline unsigned To<unsigned>(const char *value) {
			char *n;
			return (unsigned)std::strtol(value, &n, 10);
		}

		template <>
		inline long To<long>(const char *value) {
			char *n;
			return (long)std::strtol(value, &n, 10);
		}

		template <>
		inline unsigned long To<unsigned long>(const char *value) {
			char *n;
			return (unsigned long)std::strtol(value, &n, 10);
		}

		template <>
		inline long long To<long long>(const char *value) {
			char *n;
			return (long long)std::strtol(value, &n, 10);
		}

		template <>
		inline unsigned long long To<unsigned long long>(const char *value) {
			char *n;
			return (unsigned long long)std::strtol(value, &n, 10);
		}

		template <>
		inline float To<float>(const char *value) {
			return (float)std::atof(value);
		}

		template <>
		inline double To<double>(const char *value) {
			return std::atof(value);
		}

		template <>
		inline long double To<long double>(const char *value) {
			return std::atof(value);
		}
		/// @endcond

		/// String replace that does not use regex. Works faster than regex variant.
		/// @param  str is the string to process
		/// @param  find is the substrings to be replaced
		/// @param  replace is the string to place instead of find. Can be empty string.
		inline std::string Replace(std::string str, const std::string &find, const std::string &replace) {
			std::string::size_type l=0;
			
			auto flen=find.length();
			auto rlen=replace.length();
			
			if(!find.length()) return str;

			while( (l=str.find(find, l)) != str.npos ) {
				str.erase(l, flen);
				str.insert(l, replace);
				l+=rlen;
			}

			return str;
		}

		/// Strips whitespace around the given string both from start and end.
		/// @param  str is the string to process
		/// @param  chars is the characters to be considered as whitespace
		inline std::string Trim(std::string str, const std::string &chars=" \t\n\r") {
			if(!str.length()) return "";
			
			const char *ptr=str.c_str();
			while(*ptr && chars.find_first_of(*ptr)!=chars.npos) {
				ptr++;
			}
			str=str.substr(ptr-str.c_str());
			
			while(str.length() && chars.find_first_of(str[str.length()-1])!=chars.npos) {
				str.resize(str.length()-1);
			}

			return str;
		}

		/// Strips the whitespace from the start of a string.
		/// @param  str is the string to process
		/// @param  chars is the characters to be considered as whitespace
		inline std::string TrimStart(std::string str, const std::string &chars=" \t\n\r") {
			if(!str.length()) return "";
			
			const char *ptr=str.c_str();
			while(*ptr && chars.find_first_of(*ptr)!=chars.npos) {
				ptr++;
			}
			str=str.substr(ptr-str.c_str());

			return str;
		}

		/// Strips the whitespace at the end of a string.
		/// @param  str is the string to process
		/// @param  chars is the characters to be considered as whitespace
		inline std::string TrimEnd(std::string str, const std::string &chars=" \t\n\r") {
			while(str.length() && chars.find_first_of(str[str.length()-1])!=chars.npos) {
				str.resize(str.length()-1);
			}

			return str;
		}

		/// Converts the given string to lowercase.
		inline std::string ToLower(std::string str) {
			for(auto it=str.begin();it!=str.end();++it) {
				*it=tolower(*it);
			}

			return str;
		}

		/// Converts the given string to uppercase
		inline std::string ToUpper(std::string str) {
			for(auto it=str.begin();it!=str.end();++it) {
				*it=toupper(*it);
			}

			return str;
		}

		
		/// @cond
		inline std::string From(char value) {
			return std::to_string(value);
		}

		inline std::string From(unsigned char value) {
			return std::to_string(value);
		}

		inline std::string From(int value) {
			return std::to_string(value);
		}
		
		inline std::string From(unsigned value) {
			return std::to_string(value);
		}

		inline std::string From(long value) {
			return std::to_string(value);
		}
		
		inline std::string From(unsigned long value) {
			return std::to_string(value);
		}
		
		inline std::string From(long long value) {
			return std::to_string(value);
		}
		
		inline std::string From(unsigned long long value) {
			return std::to_string(value);
		}
		
		inline std::string From(float value) {
			return std::to_string(value);
		}
		
		inline std::string From(double value) {
			return std::to_string(value);
		}
		
		inline std::string From(long double value) {
			return std::to_string(value);
		}
		
		inline std::string From(std::string value) {
			return value;
		}

		template<class T_> 
		typename std::enable_if<internal::has_stringoperator<T_>::value, std::string>::type 
		From(const T_ &item) {
			return (std::string)item;
		}

		template<class T_> 
		typename std::enable_if<!internal::has_stringoperator<T_>::value && !decltype(gorgon__enum_trait_locator((*(T_*)nullptr)))::isupgradedenum, std::string>::type 
		From(const T_ &item) {
			std::stringstream ss;
			ss<<item;

			return ss.str();
		}
		
		template<typename T>
		class IsStreamable
		{
			using one = char;
			struct two {
				char dummy[2];
			};

			template<class TT>
			static one test(decltypetype(((std::ostream*)nullptr)->operator<<((TT*)nullptr)))  { return one(); }
			
			static two test(...)  { return two();  }
						
		public:
			static const bool Value = sizeof( test(*(std::ostream*)nullptr) )==1;
		};
		
		inline void streamthis(std::stringstream &stream) {			
		}
		
		template<class T_, class ...P_>
		void streamthis(std::stringstream &stream, const T_ &first, const P_&... rest) {
			stream<<first;
			
			streamthis(stream, rest...);
		}
		/// @endcond
		
		template<class T_>
		struct CanBeStringified {
			static const bool Value = 
				IsStreamable<T_>::Value || 
				internal::has_stringoperator<T_>::value || 
				decltype(gorgon__enum_trait_locator(*((typename std::decay<T_>::type*)nullptr)))::isupgradedenum;
		};
		
		/// Streams the given parameters into a stringstream and returns the result, effectively
		/// concatinating all parameters.
		template<class ...P_>
		std::string Concat(const P_&... rest) {
			std::stringstream ss;
			streamthis(ss, rest...);
			
			
			return ss.str();
		}
		
		//for pretty documentation
#ifdef DOXYGEN
		/// Creates a string from the given data. Similar to to_string but allows 
		/// conversion of a type if it can be casted or streamed to output. Also uses
		/// std::to_string where possible.
		template<class T_>
		std::string From(const T_ &item) { return ""; }
#endif
		
		/// Extracts the part of the string up to the given marker. Extracted string and
		/// the marker is removed from the original string. If the given string does not
		/// contain marker, entire string will be extracted. It is possible to tokenize
		/// the given string using repeated calls to this function. However, its more
		/// convenient to use Tokenizer.
		/// @param  original string that will be processed. This string will be modified
		///         by the program
		/// @param  marker string that will be searched.
		/// @return Extracted string. Does not contain the marker.
		inline std::string Extract(std::string &original, const std::string marker) {
			auto pos=original.find(marker);
			
			if(pos==original.npos) {
				std::string ret;
				std::swap(ret, original);
				
				return ret;
			}
			
			std::string ret=original.substr(0, pos);
			original=original.substr(pos+marker.length());
			
			return ret;
		}

		
		/// Extracts the part of the string up to the given marker. Extracted string and
		/// the marker is removed from the original string. If the given string does not
		/// contain marker, entire string will be extracted. It is possible to tokenize
		/// the given string using repeated calls to this function. However, its more
		/// convenient to use Tokenizer.
		/// @param  original string that will be processed. This string will be modified
		///         by the program
		/// @param  marker character that will be searched.
		/// @return Extracted string. Does not contain the marker.
		inline std::string Extract(std::string &original, char marker) {
			auto pos=original.find_first_of(marker);
			
			if(pos==original.npos) {
				std::string ret;
				std::swap(ret, original);
				
				return ret;
			}
			
			std::string ret=original.substr(0, pos);
			original=original.substr(pos+1);
			
			return ret;
		}
		
		enum class QuoteType {
			None,
			Single,
			Double,
			Both
		};
		
		/// Extracts the part of the string up to the given marker. This function will
		/// skipped quoted sections of the string. Both single and double quotes can be
		/// considered, however, double quotes should match with double quotes and single
		/// quotes should match with single quotes. A different quote type inside quote
		/// region is ignored. Extracted string and the marker is removed from the original 
		/// string. If the given string does not contain marker outside the quotes, 
		/// entire string will be extracted. It is possible to tokenize
		/// the given string using repeated calls to this function. Unbalanced quotes will
		/// be treated ending at the end of the string.
		/// @param  original string that will be processed. This string will be modified
		///         by the program
		/// @param  marker string that will be searched. It is possible to specify quote as
		///         a marker.
		/// @param  quotetype controls which type of quotes will be considered.
		/// @return Extracted string. Does not contain the marker. Quotes will not be removed
		inline std::string ExtractOutsideQuotes(std::string &original, char marker, QuoteType quotetype=QuoteType::Both) {
			int inquotes=0;
			int pos=0;
			
			for(auto &c : original) {
				if(inquotes==1) {
					if(c=='\'') {
						inquotes=0;
					}
				}
				else if(inquotes==2) {
					if(c=='"') {
						inquotes=0;
					}
				}
				else if(c==marker) {
					std::string temp=original.substr(0, pos);
					original=original.substr(pos+1);
					
					return temp;
				}
				else if(c=='\'' && (quotetype==QuoteType::Single || quotetype==QuoteType::Both)) {
					inquotes=1;
				}
				else if(c=='"' && (quotetype==QuoteType::Double || quotetype==QuoteType::Both)) {
					inquotes=2;
				}
				
				pos++;
			}
			
			std::string temp;
			std::swap(temp, original);
			
			return temp;
		}

	}
}

#ifdef _MSC_VER
#	undef decltype
#	undef decltypetype
#	undef ISENUMUPGRADED
#else
#	undef decltypetype
#	undef ISENUMUPGRADED
#endif
