//DESCRIPTION
//	This file contains functions for random number generation. Random seed
//	is automatically initialized using time.

//REQUIRES:
//	BasicMath
// 
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
#include "BasicMath.h"
#include <cstdlib>
#include "Point2D.h"
#include "Bounds2D.h"
#include "OrderedCollection.h"

namespace gge { namespace utils {

	static const char* randomstrings[] = {
		"lorem", "ipsum", "sit", "amet", "consectetur", "adipisicing", "elit", "sed", "do", "eiusmod", 
		"tempor", "incididunt", "ut", "labore", "et", "dolore", "magna", "aliqua", "ut", 
		"enim", "adum", "minim", "veniam", "quis", "nostrud", "exercitation", "ullamco", "laboris",
		"nisi", "aliquip", "ex", "ea", "commodo", "consequat", "duis", "aute", "irure",
		"in", "reprehenderit", "voluptate", "velit", "esse", "cillum", "dolore", "eustate", "fugiat", 
		"nulla", "pariatur", "excepteur", "sint"
	};

	template <class T_>
	T_ Random()                            { return ((T_)std::rand())/RAND_MAX; }
	template <>
	inline double Random<double>()         { return (double)Random<float>()*Random<float>(); }
	template <>
	inline int Random<int>()               { return std::rand()+(rand()<<16); }
	template <>
	inline Byte Random<Byte>()             { return std::rand()%256; }
	template <>
	inline char Random<char>()             { return std::rand()%256-128; }


	template<class T_>
	T_ Random(T_ min, T_ max)              { return Random<T_>()*(max-min) + min; }
	template<>
	inline Byte Random(Byte min, Byte max) { return (Random<Byte>()%(max-min)) + min; }
	template<>
	inline char Random(char min, char max) { return PositiveMod((int)Random<char>(),(max-min)) + min; }
	template<>
	inline int Random(int min, int max)    { return PositiveMod(Random<int>(),(max-min)) + min; }

	template<class T_>
	T_ Random(Range<T_> range)              { return Random<T_>()*(range.Difference()) + range.start; }
	template<>
	inline Byte Random(Range<Byte> range) { return (Random<Byte>()%(range.Difference())) + range.start; }
	template<>
	inline char Random(Range<char> range) { return (Random<char>()%(range.Difference())) + range.start; }
	template<>
	inline int Random(Range<int> range)    { return (PositiveMod(Random<int>(),range.Difference())) + range.start; }

	template<class T_>
	inline T_ Random(std::vector<T_> &vec) { return vec[Random(0,(int)vec.size())]; }

	template<class T_>
	inline T_ &Random(gge::utils::OrderedCollection<T_> &vec) { return vec[Random(0,(int)vec.GetCount())]; }

	inline gge::utils::Point Random(const gge::utils::Bounds &bounds) {
		return gge::utils::Point(Random(bounds.Left,bounds.Right), Random(bounds.Top,bounds.Bottom));
	}

	template<class I_>
	inline void Shuffle(const I_ &beg, const I_ &end) {
		auto diff=end-beg;
		for(int i=0;i<diff;i++) {
			std::swap(*(beg+Random(0, diff)), *(beg+Random(0,diff)));
		}
	}

	template <>
	inline std::string Random<std::string>()             { return randomstrings[Random(0,50)]; }

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
