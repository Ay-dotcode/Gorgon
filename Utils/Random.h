//DESCRIPTION
//	This file contains functions for random number generation. Random seed
//	is automatically initialized using time.

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

#include "UtilsBase.h"
#include <cstdlib>

namespace gge { namespace utils {
	template <class T_>
	T_ Random()                            { return ((T_)std::rand())/RAND_MAX; }
	template <>
	inline double Random<double>()         { return (double)Random<float>()*Random<float>(); }
	template <>
	inline int Random<int>()               { return std::rand()+(rand()<<16); }
	template <>
	inline Byte Random<Byte>()             { return std::rand()%256; }


	template<class T_>
	T_ Random(T_ min, T_ max)              { return Random<T_>()*(max-min) + min; }
	template<>
	inline Byte Random(Byte min, Byte max) { return (Random<Byte>()%(max-min)) + min; }
	template<>
	inline int Random(int min, int max)    { return (Random<int>()%(max-min)) + min; }

	template<class T_, class I_>
	void RandomFill(I_ &begin, I_ &end)	{
		for(auto i=begin;i!=end;++i) {
			*i=Random<T_>();
		}
	}

	template<class T_, class O_>
	void RandomFill(O_ &obj)	{
		for(auto i=obj.begin();i!=obj.end();++i) {
			*i=Random<T_>();
		}
	}

	template<class T_, class O_>
	void RandomFill(O_ &obj, T_ min, T_ max)	{
		for(auto i=obj.begin();i!=obj.end();++i) {
			*i=Random<T_>(min,max);
		}
	}

	template<class T_>
	void RandomFill(T_ *begin, T_ *end) {
		for(auto i=begin;i!=end;++i) {
			*i=Random<T_>();
		}
	}

	template<class T_, class I_>
	void RandomFill(I_ &begin, I_ &end, T_ min, T_ max)	{
		for(auto i=begin;i!=end;++i) {
			*i=Random<T_>(min,max);
		}
	}

	template<class T_>
	void RandomFill(T_ *begin, T_ *end, T_ min, T_ max) {
		for(auto i=begin;i!=end;++i) {
			*i=Random<T_>(min,max);
		}
	}
} }
