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


#ifdef GGE_XMLSERVICES
#include "../External/XmlParser/xmlParser.h"
#endif


#define RECTANGLE2D_EXISTS


namespace gge {
#ifndef BOUNDS2D_EXISTS
	template <class _T> class basic_Bounds2D;
#endif

	template <class _T>
	class basic_Rectangle2D {
	public:
		_T Left,Top , Width,Height;
		
		basic_Rectangle2D() {}
		basic_Rectangle2D(_T Left, _T Top, _T Width, _T Height) : 
			Left(Left), Top(Top), Width(Width),Height(Height)
		{ }
			
		basic_Rectangle2D(basic_Point2D<_T> TopLeft, basic_Size2D<_T> HeightWidth) : 
			Left(TopLeft.x), Top(TopLeft.y), Width(HeightWidth.Width), Height(HeightWidth.Height)
		{ }
			
		basic_Rectangle2D(basic_Point2D<_T> TopLeft, int Width, int Height) : 
			Left(TopLeft.x), Top(TopLeft.y), Width(Width), Height(Height)
		{ }
		
		basic_Rectangle2D(basic_Point2D<_T> TopLeft, basic_Point2D<_T> BottomRight) :
			Left(TopLeft.x), Top(TopLeft.y),
			Width(BottomRight.x-TopLeft.x),	Height(BottomRight.y-TopLeft.y)
		{ }

		template <class U>
		basic_Rectangle2D(const basic_Rectangle2D<U> &rect) : 
			Left(rect.Left), Top(rect.Top), Width(rect.Width), Height(rect.Height)
		{ }

		basic_Rectangle2D(const basic_Bounds2D<_T> &bounds);

		operator basic_Bounds2D<_T>();

		basic_Rectangle2D& operator =(const basic_Bounds2D<_T> &bounds);

		////Calculates and returns the width of the region
		_T Right() const { return Width +Left; }
		////Calculates and returns the height of the region
		_T Bottom() const { return Height+Top;  }

		//scale, translate, rotate?, +, +=, -, -=, &&, ||
	};

	////Allows streaming of point. It converts point to string,
	/// every row is printed on a line enclosed in braces.
	template <class _T>
	std::ostream &operator << (std::ostream &out, basic_Rectangle2D<_T> &Rectangle) {
		out<<"< "<<Rectangle.Left<<"-"<<Rectangle.Width<<" , "<<Rectangle.Top<<"-"<<Rectangle.Height<<" >";

		return out;
	}


	////Adds the textual form of the point to another string.
	template <class _T>
	std::string &operator + (std::string &out, basic_Rectangle2D<_T> &Rectangle) {
		return string+(string)Rectangle;
	}


#ifdef GRAPH_USEDOUBLE
	typedef basic_Rectangle2D<double> Rectangle2D;
#else
	typedef basic_Rectangle2D<float> Rectangle2D;
#endif

	typedef basic_Rectangle2D<int> Rectangle;
}