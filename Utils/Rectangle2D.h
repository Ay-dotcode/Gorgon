//DESCRIPTION
//	This file contains the class Rectangle2D which defines a rectangular
//	region using top, left corner and width and height information.
//	Missing some operators.

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

#include <iostream>
#include <ostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <limits>

#ifdef GGE_GRAPHICS
	#include "../Graphics/Graphics.h"
#else
	#include "BasicGraphics.h"
#endif

#include "Point2D.h"
#include "Size2D.h"


#ifdef GGE_XMLSERVICES
#include "../External/XmlParser/xmlParser.h"
#endif


#define RECTANGLE2D_EXISTS


namespace gge { namespace utils {
#ifndef BOUNDS2D_EXISTS
	template <class T_> class basic_Bounds2D;
#endif

	template <class T_>
	class basic_Rectangle2D {
	public:
		T_ Left,Top , Width,Height;
		
		basic_Rectangle2D() {}
		basic_Rectangle2D(T_ Left, T_ Top, T_ Width, T_ Height) : 
			Left(Left), Top(Top), Width(Width),Height(Height)
		{ }
			
		basic_Rectangle2D(const basic_Point2D<T_> &TopLeft, const basic_Size2D<T_> &HeightWidth) : 
			Left(TopLeft.x), Top(TopLeft.y), Width(HeightWidth.Width), Height(HeightWidth.Height)
		{ }
			
		basic_Rectangle2D(const basic_Point2D<T_> &TopLeft, int Width, int Height) : 
			Left(TopLeft.x), Top(TopLeft.y), Width(Width), Height(Height)
		{ }
		
		basic_Rectangle2D(const basic_Point2D<T_> &TopLeft, const basic_Point2D<T_> &BottomRight) :
			Left(TopLeft.x), Top(TopLeft.y),
			Width(BottomRight.x-TopLeft.x),	Height(BottomRight.y-TopLeft.y)
		{ }

		template <class U>
		basic_Rectangle2D(const basic_Rectangle2D<U> &rect) : 
			Left(T_(rect.Left)), Top(T_(rect.Top)), Width(T_(rect.Width)), Height(T_(rect.Height))
		{ }

		basic_Rectangle2D(const basic_Bounds2D<T_> &bounds);

		operator basic_Bounds2D<T_>() const;

		basic_Rectangle2D& operator =(const basic_Bounds2D<T_> &bounds);

		////Calculates and returns the width of the region
		T_ Right() const { return Width +Left; }
		////Calculates and returns the height of the region
		T_ Bottom() const { return Height+Top;  }

		void SetRight(T_ right) { Width=right-Left; }
		void SetBottom(T_ bottom) { Height=bottom-Top; }

		//scale, translate, rotate?, +, +=, -, -=, &&, ||
	};

	////Allows streaming of Rectangle
	template <class T_>
	std::ostream &operator << (std::ostream &out, const basic_Rectangle2D<T_> &Rectangle) {
		out<<"< "<<Rectangle.Left<<", "<<Rectangle.Top<<", "<<Rectangle.Width<<"x"<<Rectangle.Height<<" >";

		return out;
	}


	////Adds the textual form of the point to another string.
	template <class T_>
	std::string &operator + (const std::string &out, const basic_Rectangle2D<T_> &Rectangle) {
		return string+(string)Rectangle;
	}


	typedef basic_Rectangle2D<FloatingPoint> Rectangle2D;

	typedef basic_Rectangle2D<int> Rectangle;
} }
