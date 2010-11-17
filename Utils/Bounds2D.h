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
	template <class _T> class basic_Rectangle2D;
#endif

	template <class _T>
	class basic_Bounds2D {
	public:
		_T Left,Top , Right,Bottom;
		
		basic_Bounds2D() {}
		basic_Bounds2D(_T Left, _T Top, _T Right, _T Bottom) {
			this->Left=Left;
			this->Right=Right;
			this->Top=Top;
			this->Bottom=Bottom;
		}
		basic_Bounds2D(basic_Point2D<_T> TopLeft, basic_Point2D<_T> BottomRight) {
			this->Left=TopLeft.x;
			this->Top=TopLeft.y;
			this->Right=BottomRight.x;
			this->Bottom=BottomRight.y;
		}
		basic_Bounds2D(basic_Point2D<_T> TopLeft, basic_Size2D<_T> HeightWidth) {
			this->Left=TopLeft.x;
			this->Top=TopLeft.y;
			this->Right=TopLeft.x+HeightWidth.x;
			this->Bottom=TopLeft.y+HeightWidth.y;
		}
		basic_Bounds2D(basic_Point2D<_T> TopLeft, int Width, int Height) {
			this->Left=TopLeft.x;
			this->Top=TopLeft.y;
			this->Right=TopLeft.x+Width;
			this->Bottom=TopLeft.y+Height;
		}
		basic_Bounds2D(const basic_Rectangle2D<_T> &bounds);

		operator basic_Rectangle2D<_T>();

		operator std::string() {
			std::ostringstream str;
			str<<"{"<<Left<<","<<Top<<" - "<<Right<<","<<Bottom<<"}";

			return str.str();
		}

		basic_Bounds2D& operator =(const basic_Rectangle2D<_T> &bounds);

		////Calculates and returns the width of the region
		_T Width() const  { return Right -Left; }
		////Calculates and returns the height of the region
		_T Height() const { return Bottom-Top;  }

		//scale, translate, rotate?, +, +=, -, -=, &&, ||
	};

	////Allows streaming of point. It converts point to string,
	/// every row is printed on a line enclosed in braces.
	template <class _T>
	std::ostream &operator << (std::ostream &out, basic_Bounds2D<_T> &bounds) {
		out<<"< "<<bounds.Left<<"-"<<bounds.Right<<" , "<<bounds.Top<<"-"<<bounds.Bottom<<" >";

		return out;
	}


	////Adds the textual form of the point to another string.
	template <class _T>
	std::string &operator + (std::string &out, basic_Bounds2D<_T> &bounds) {
		return string+(string)bounds;
	}


#ifdef GRAPH_USEDOUBLE
	typedef basic_Bounds2D<double> Bounds2D;
#else
	typedef basic_Bounds2D<float> Bounds2D;
#endif

	typedef basic_Bounds2D<int> Bounds;
}