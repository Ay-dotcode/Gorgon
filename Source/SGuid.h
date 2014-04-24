/// @file SGuid.h contains implementation of a 8-byte short GUID.

#pragma once

#include <iostream>
#include <time.h>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <stdlib.h>
#include <iomanip>

#include "Types.h"

namespace Gorgon { 

	class SGuid {
	public:

		union {
			Byte				Bytes[8];
			unsigned long long	Integer;
		};
		
		struct NewGuidType { };

		SGuid() : Integer(0) { }

		SGuid(const NewGuidType&) { 
			New();
		}

		SGuid(const Byte data[8]) {
			if(data==nullptr) {
				Integer=0;
			} 
			else {
#ifdef MSVC
				memcpy_s(Bytes, 8, data, 8);
#else
				std::memcpy(Bytes, data, 8);
#endif
				checknewserial();
			}
		}

		SGuid(unsigned long long data) {
			Integer=data;
			checknewserial();
		}

		SGuid(unsigned serial, unsigned random, unsigned time) {
			Set(serial, random, time);
		}

		SGuid(std::istream &in) {
			Load(in);
		}

		bool operator <(const SGuid &g) const {
			return Integer<g.Integer;
		}

		void Set(unsigned serial, unsigned random, unsigned time) {
			//time component - 2
			/*ints[0] = (time & 0xffff);

			//random component - 3
			*(int*)(bytes+2) = random & 0xffffff; //this part is 3 bytes, not 4

			//serial component - 3
			bytes[5] = Byte( serial      & 0xff);
			bytes[6] = Byte((serial>>8 ) & 0xff);
			bytes[7] = Byte((serial>>16) & 0xff);*/
			
			memcpy(Bytes+0, &time  , 2);
			memcpy(Bytes+2, &random, 3);
			memcpy(Bytes+5, &serial, 3);

			checknewserial();
		}

		void New() {
			time_t t=time(nullptr);
			int random=rand();
			serial++;

			Set(serial, random, (unsigned)t);
		}

		operator std::string() const {
			static char hexlist[]="0123456789abcdef";
			std::string str;
			
			str.resize(18);
			int j=0;
			for(int i=7;i>=0;i--) { 
				str[j++]=hexlist[Bytes[i]/16];
				str[j++]=hexlist[Bytes[i]%16];
				if(i==5 || i==2) {
					str[j++]='-';
				}
			}

			return str;
		}

		void Load(std::istream &Data) {
			Data.read((char*)Bytes, 8);

			checknewserial();
		}

		void LoadLong(std::istream &file) {
			file.read((char*)Bytes, 8);
			file.read((char*)Bytes, 8);
		}

		void Save(std::ostream &file) const {
			file.write((char*)Bytes, 8);
		}

		bool operator ==(const SGuid &right) const  {
			return Integer==right.Integer;
		}

		bool isEmpty() {
			return Integer==0;
		}

		operator bool() {
			return !isEmpty();
		}

		static const SGuid Empty;		
		static const NewGuidType CreateNew;

	protected:
		void checknewserial() const {
			unsigned s=((Integer>>40) & 0xffffff);
			if(s>serial)
				serial=s;
		}
		
		static unsigned serial;
	};

	inline std::ostream &operator <<(std::ostream &stream, const SGuid &guid) {
		stream<<(std::string)guid;

		return stream;
	}
	
	inline std::istream &operator>>(std::istream &in, SGuid &guid) {
		guid.Integer=0;
		for(int i=15; i>=0; i--) {	
			char c;
			c=in.get();
			c=tolower(c);
			
			if(c>='0' && c<='9') {
				guid.Bytes[i/2]+=(c-'0')<<(i%2 ? 4 : 0);
			}
			else if(c>='a' && c<='f') {
				guid.Bytes[i/2]+=(c-'a'+10)<<(i%2 ? 4 : 0);
			}
			else if(c=='-' || c==' ' || c=='\t') i++;
		
			if(!in.good()) break;
		}
		
		return in;
	}

} 
