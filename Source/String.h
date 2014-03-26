/// @file String.h This file contains string helper functions.
/// They work with std::string

#pragma once

#include <string>
#include <sstream>

namespace Gorgon { 
	
	/// Contains string related functions and classes.
	namespace String {

		/// Converts a string to another type. Works for integral types and
		/// Gorgon classes including Point, Size, etc... There is no error
		/// handling.
		template <class T_>
		T_ To(const std::string &value) {
			char *n;
			return (T_)std::strtol(value.c_str(), &n, 10);
		}

		template <>
		inline float To<float>(const std::string &value) {
			return (float)std::atof(value.c_str());
		}

		template <>
		inline double To<double>(const std::string &value) {
			return std::atof(value.c_str());
		}

		/// String replace that does not use regex. Works faster than regex variant.
		/// @param  str is the string to process
		/// @param  find is the substrings to be replaced
		/// @param  replace is the string to place instead of find. Can be empty string.
		inline std::string Replace(std::string str, const std::string &find, const std::string &replace) {
			std::string::size_type l;

			while( (l=str.find(find)) != str.npos ) {
				str.erase(l, find.length());
				str.insert(l, replace);
			}

			return str;
		}

		/// @todo ...
		inline std::string Trim(std::string str, const std::string chars=" \t\n\r") {
			while(str.length() && chars.find_first_of(str[0])!=chars.npos) {
				str=str.substr(1);
			}
			while(str.length() && chars.find_first_of(str[str.length()-1])!=chars.npos) {
				str.resize(str.length()-1);
			}

			return str;
		}

		/// @todo ...
		inline std::string TrimLeft(std::string str, const std::string chars=" \t\n\r") {
			while(str.length() && chars.find_first_of(str[0])!=chars.npos) {
				str=str.substr(1);
			}

			return str;
		}

		/// @todo ...
		inline std::string TrimRight(std::string str, const std::string chars=" \t\n\r") {
			while(str.length() && chars.find_first_of(str[str.length()-1])!=chars.npos) {
				str.resize(str.length()-1);
			}

			return str;
		}

		/// @todo ...
		inline std::string ToLower(std::string str) {
			for(auto it=str.begin();it!=str.end();++it) {
				*it=tolower(*it);
			}

			return str;
		}

		/// @todo ...
		inline std::string ToUpper(std::string str) {
			for(auto it=str.begin();it!=str.end();++it) {
				*it=toupper(*it);
			}

			return str;
		}

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
		
		inline std::string ToString(int value) {
			return std::to_string(value);
		}
		
		inline std::string ToString(long value) {
			return std::to_string(value);
		}
		
		inline std::string ToString(long long value) {
			return std::to_string(value);
		}
		
		inline std::string ToString(unsigned value) {
			return std::to_string(value);
		}
		
		inline std::string ToString(unsigned long value) {
			return std::to_string(value);
		}
		
		inline std::string ToString(unsigned long long value) {
			return std::to_string(value);
		}
		
		inline std::string ToString(float value) {
			return std::to_string(value);
		}
		
		inline std::string ToString(double value) {
			return std::to_string(value);
		}
		
		inline std::string ToString(long double value) {
			return std::to_string(value);
		}
		
		inline std::string ToString(std::string value) {
			return value;
		}

		template<class T_> 
		typename std::enable_if<has_stringoperator<T_>::value, std::string>::type ToString(const T_ &item) {
			return (std::string)item;
		}

		template<class T_> 
		typename std::enable_if<!has_stringoperator<T_>::value, std::string>::type ToString(const T_ &item) {
			std::stringstream ss;
			ss<<item;

			return ss.str();
		}
		
		//Modifies the original string, removes the marker too, if the marker not found
		//it extracts the entire string
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
		
		//extracts string but doesnot split inside quotes, supports both single and double 
		//quotes. Marker could be a quote
		inline std::string ExtractOutsideQuotes(std::string &original, char marker) {
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
				else if(c=='\'') {
					inquotes=1;
				}
				else if(c=='"') {
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


