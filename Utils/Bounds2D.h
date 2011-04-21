//DESCRIPTION
//	This file contains the class Bounds2D which defines bounds of an object
//	with left,top ; right,bottom coordinates. This class can be converted
//	to rectangle class (top,left ; width,height)

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


#define BOUNDS2D_EXISTS


namespace gge {
#ifndef RECTANGLE2D_EXISTS
	template <class T_> class basic_Rectangle2D;
#endif

	template <class T_>
	class basic_Bounds2D {
	public:
		T_ Left,Top , Right,Bottom;
		
		basic_Bounds2D() {}
		basic_Bounds2D(T_ Left, T_ Top, T_ Right, T_ Bottom) {
			this->Left=Left;
			this->Right=Right;
			this->Top=Top;
			this->Bottom=Bottom;
		}
		basic_Bounds2D(basic_Point2D<T_> TopLeft, basic_Point2D<T_> BottomRight) {
			this->Left=TopLeft.x;
			this->Top=TopLeft.y;
			this->Right=BottomRight.x;
			this->Bottom=BottomRight.y;
		}
		basic_Bounds2D(basic_Point2D<T_> TopLeft, basic_Size2D<T_> HeightWidth) {
			this->Left=TopLeft.x;
			this->Top=TopLeft.y;
			this->Right=TopLeft.x+HeightWidth.x;
			this->Bottom=TopLeft.y+HeightWidth.y;
		}
		basic_Bounds2D(basic_Point2D<T_> TopLeft, int Width, int Height) {
			this->Left=TopLeft.x;
			this->Top=TopLeft.y;
			this->Right=TopLeft.x+Width;
			this->Bottom=TopLeft.y+Height;
		}
		basic_Bounds2D(const basic_Rectangle2D<T_> &bounds);

		operator basic_Rectangle2D<T_>();

		operator std::string() {
			std::ostringstream str;
			str<<"{"<<Left<<","<<Top<<" - "<<Right<<","<<Bottom<<"}";

			return str.str();
		}

		basic_Bounds2D& operator =(const basic_Rectangle2D<T_> &rect);

		////Calculates and returns the width of the region
		T_ Width() const  { return Right -Left; }
		////Calculates and returns the height of the region
		T_ Height() const { return Bottom-Top;  }

		//scale, translate, rotate?, +, +=, -, -=, &&, ||
	};

	////Allows streaming of point. It converts point to string,
	/// every row is printed on a line enclosed in braces.
	template <class T_>
	std::ostream &operator << (std::ostream &out, basic_Bounds2D<T_> &bounds) {
		out<<"< "<<bounds.Left<<"-"<<bounds.Right<<" , "<<bounds.Top<<"-"<<bounds.Bottom<<" >";

		return out;
	}


	////Adds the textual form of the point to another string.
	template <class T_>
	std::string &operator + (std::string &out, basic_Bounds2D<T_> &bounds) {
		return string+(string)bounds;
	}


#ifdef GRAPH_USEDOUBLE
	typedef basic_Bounds2D<double> Bounds2D;
#else
	typedef basic_Bounds2D<float> Bounds2D;
#endif

	typedef basic_Bounds2D<int> Bounds;
}