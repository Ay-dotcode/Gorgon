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

		/// @cond INTERNAL
		template <>
		inline float To<float>(const std::string &value) {
			return (float)std::atof(value.c_str());
		}

		template <>
		inline double To<double>(const std::string &value) {
			return std::atof(value.c_str());
		}
		/// @endcond

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

		/// Strips whitespace around the given string both from start and end.
		/// @param  str is the string to process
		/// @param  chars is the characters to be considered as whitespace
		inline std::string Trim(std::string str, const std::string &chars=" \t\n\r") {
			while(str.length() && chars.find_first_of(str[0])!=chars.npos) {
				str=str.substr(1);
			}
			while(str.length() && chars.find_first_of(str[str.length()-1])!=chars.npos) {
				str.resize(str.length()-1);
			}

			return str;
		}

		/// Strips the whitespace from the start of a string.
		/// @param  str is the string to process
		/// @param  chars is the characters to be considered as whitespace
		inline std::string TrimStart(std::string str, const std::string &chars=" \t\n\r") {
			while(str.length() && chars.find_first_of(str[0])!=chars.npos) {
				str=str.substr(1);
			}

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
		
		
		/// @cond
		inline std::string From(int value) {
			return std::to_string(value);
		}
		
		inline std::string From(long value) {
			return std::to_string(value);
		}
		
		inline std::string From(long long value) {
			return std::to_string(value);
		}
		
		inline std::string From(unsigned value) {
			return std::to_string(value);
		}
		
		inline std::string From(unsigned long value) {
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
		typename std::enable_if<internal::has_stringoperator<T_>::value, std::string>::type From(const T_ &item) {
			return (std::string)item;
		}

		template<class T_> 
		typename std::enable_if<!internal::has_stringoperator<T_>::value, std::string>::type From(const T_ &item) {
			std::stringstream ss;
			ss<<item;

			return ss.str();
		}
		/// @endcond
		
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
		
		/// Extracts the part of the string up to the given marker. This function will
		/// skipped quoted sections of the string. Both single and double quotes are
		/// considered, however, double quotes should match with double quotes and single
		/// quotes should match with single quotes. A different quote type inside quote
		/// region is ignored. Extracted string and the marker is removed from the original 
		/// string. If the given string does not contain marker outside the quotes, 
		/// entire string will be extracted. It is possible to tokenize
		/// the given string using repeated calls to this function.
		/// @param  original string that will be processed. This string will be modified
		///         by the program
		/// @param  marker string that will be searched. It is possible to specify quote as
		///         a marker.
		/// @return Extracted string. Does not contain the marker.
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


