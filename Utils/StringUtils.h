//DESCRIPTION
//	This file contains definitions for basic graphics systems.

//REQUIRES:
//	---

//LICENSE
//	This program is free software: you can redistribute it and/or modify
//	it under the terms of the Lesser GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//	Lesser GNU General Public License for more details.
//
//	You should have received a copy of the Lesser GNU General Public License
//	along with this program. If not, see < http://www.gnu.org/licenses/ >.

//COPYRIGHT
//	Cem Kalyoncu, DarkGaze.Org (cemkalyoncu[at]gmail[dot]com)

#pragma once

#include <string>
#include <sstream>

namespace gge { namespace utils {

	inline std::string Replace(const std::string str, const std::string find, const std::string replace) {
		std::string::size_type l;
		std::string ret=str;
		while( (l=ret.find(find)) != ret.npos ) {
			ret.erase(l, find.length());
			ret.insert(l, replace);
		}

		return ret;
	}

	inline std::string Trim(std::string str, const std::string chars=" \t\n\r") {
		while(str.length() && chars.find_first_of(str[0])!=chars.npos) {
			str=str.substr(1);
		}
		while(str.length() && chars.find_first_of(str[str.length()-1])!=chars.npos) {
			str.resize(str.length()-1);
		}

		return str;
	}

	inline std::string TrimLeft(std::string str, const std::string chars=" \t\n\r") {
		while(str.length() && chars.find_first_of(str[0])!=chars.npos) {
			str=str.substr(1);
		}

		return str;
	}

	inline std::string TrimRight(std::string str, const std::string chars=" \t\n\r") {
		while(str.length() && chars.find_first_of(str[str.length()-1])!=chars.npos) {
			str.resize(str.length()-1);
		}

		return str;
	}

	inline std::string ToLower(std::string str) {
		for(auto it=str.begin();it!=str.end();++it) {
			*it=tolower(*it);
		}

		return str;
	}

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

} }


