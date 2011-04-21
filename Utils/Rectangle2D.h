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
			
		basic_Rectangle2D(basic_Point2D<T_> TopLeft, basic_Size2D<T_> HeightWidth) : 
			Left(TopLeft.x), Top(TopLeft.y), Width(HeightWidth.Width), Height(HeightWidth.Height)
		{ }
			
		basic_Rectangle2D(basic_Point2D<T_> TopLeft, int Width, int Height) : 
			Left(TopLeft.x), Top(TopLeft.y), Width(Width), Height(Height)
		{ }
		
		basic_Rectangle2D(basic_Point2D<T_> TopLeft, basic_Point2D<T_> BottomRight) :
			Left(TopLeft.x), Top(TopLeft.y),
			Width(BottomRight.x-TopLeft.x),	Height(BottomRight.y-TopLeft.y)
		{ }

		template <class U>
		basic_Rectangle2D(const basic_Rectangle2D<U> &rect) : 
			Left(T_(rect.Left)), Top(T_(rect.Top)), Width(T_(rect.Width)), Height(T_(rect.Height))
		{ }

		basic_Rectangle2D(const basic_Bounds2D<T_> &bounds);

		operator basic_Bounds2D<T_>();

		basic_Rectangle2D& operator =(const basic_Bounds2D<T_> &bounds);

		////Calculates and returns the width of the region
		T_ Right() const { return Width +Left; }
		////Calculates and returns the height of the region
		T_ Bottom() const { return Height+Top;  }

		//scale, translate, rotate?, +, +=, -, -=, &&, ||
	};

	////Allows streaming of point. It converts point to string,
	/// every row is printed on a line enclosed in braces.
	template <class T_>
	std::ostream &operator << (std::ostream &out, basic_Rectangle2D<T_> &Rectangle) {
		out<<"< "<<Rectangle.Left<<"-"<<Rectangle.Width<<" , "<<Rectangle.Top<<"-"<<Rectangle.Height<<" >";

		return out;
	}


	////Adds the textual form of the point to another string.
	template <class T_>
	std::string &operator + (std::string &out, basic_Rectangle2D<T_> &Rectangle) {
		return string+(string)Rectangle;
	}


#ifdef GRAPH_USEDOUBLE
	typedef basic_Rectangle2D<double> Rectangle2D;
#else
	typedef basic_Rectangle2D<float> Rectangle2D;
#endif

	typedef basic_Rectangle2D<int> Rectangle;
}