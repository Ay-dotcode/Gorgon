//DESCRIPTION
//	This file contains SGuid, short GUID class which is an 8-byte
//	unique identifier, may not be unique between instances however,
//	while operating on same data SGUID advances its serial to the
//	highest known serial, in addition to that current time and a
//	random component guarantees uniqueness.

//REQUIRES:
//	GGE/Utils/Random

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

#include <iostream>
#include <time.h>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <iomanip>

#include "UtilsBase.h"
#include "Random.h"

namespace gge { namespace utils {

	class SGuid {
	public:

		union {
			Byte				bytes[8];
			unsigned int		ints [2];
			unsigned long long	int64;
		};

		SGuid() { }

		SGuid(const Byte data[8]) {
			if(data==nullptr) {
				int64=0;
			} 
			else {
#ifdef MSVC
				memcpy_s(bytes, 8, data, 8);
#else
				std::memcpy(bytes, data, 8);
#endif
				checknewserial();
			}
		}

		SGuid(unsigned long long data) {
			int64=data;
			checknewserial();
		}

		inline
		SGuid(unsigned first, unsigned second) {
			ints[0]=first;
			ints[1]=second;
			checknewserial();
		}

		SGuid(unsigned serial, unsigned random, unsigned time) {
			Set(serial, random, time);
		}

		SGuid(std::istream &in) {
			Load(in);
		}

		void Set(unsigned serial, unsigned random, unsigned time) {
			//time component - 2
			ints[0] = (time & 0xffff);

			//random component - 3
			*(int*)(bytes+2) = random & 0xffffff; //this part is 3 bytes, not 4

			//serial component - 3
			bytes[5] = Byte( serial      & 0xff);
			bytes[6] = Byte((serial>>8 ) & 0xff);
			bytes[7] = Byte((serial>>16) & 0xff);

			checknewserial();
		}

		inline
		void New() {
			time_t t=time(NULL);
			int random=rand();
			serial++;

			Set(serial, random, (unsigned)t);
		}

		//Use R-value semantics
		inline
		static SGuid CreateNew() {
			time_t t=time(NULL);
			int random=rand();
			serial++;

			return SGuid(serial, random, (unsigned)t);
		}

		operator std::string() const;

		std::string ToString() const {
			return (std::string)(*this);
		}

		inline
		void Load(FILE *file) {
			fread(bytes, 1, 8, file);

			checknewserial();
		}

		inline
		void Load(std::istream &Data) {
			Data.read((char*)bytes, 8);

			checknewserial();
		}

		inline
		void LoadLong(FILE *Data) {
			fread(bytes, 1, 8, Data);
			fread(bytes, 1, 8, Data);
		}

		inline
		void LoadLong(std::istream &file) {
			file.read((char*)bytes, 8);
			file.read((char*)bytes, 8);
		}

		inline
		void Save(FILE *file) const {
			fwrite(bytes, 1, 8, file);
		}

		inline
		void Save(std::ofstream &file) const {
			file.write((char*)bytes, 8);
		}

		inline
		bool operator ==(const SGuid &right) const  {
			return ints[0] == right.ints[0] && ints[1] == right.ints[1];
		}

		inline
		bool isEmpty() {
			return ints[0]==0 && ints[1]==0;
		}

		operator bool() {
			return !isEmpty();
		}

		static const SGuid Empty;

	protected:
		inline
		void checknewserial() const {
			unsigned s=((ints[1]>>8) & 0xffffff);
			if(s>serial)
				serial=s;
		}
		
		static unsigned serial;
	};

	inline
	std::ostream &operator <<(std::ostream &stream, SGuid &guid) {
		stream<<(std::string)guid;

		return stream;
	}

} }
