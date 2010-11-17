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


#define MARGINS2D_EXISTS


namespace gge {
	template <class _T>
	class basic_Margins2D {
	public:
		_T Left,Top , Right,Bottom;
		
		basic_Margins2D() {}
		basic_Margins2D(_T All) : Left(All), Right(All), Top(All), Bottom(All) { }
		basic_Margins2D(_T Horizontal, _T Verticle) : Left(Horizontal), Right(Horizontal), Top(Verticle), Bottom(Verticle) { }
		basic_Margins2D(_T Left, _T Top, _T Right, _T Bottom) : 
			Left(Left), Right(Right), Top(Top), Bottom(Bottom) { }


		operator std::string() {
			std::ostringstream str;
			str<<"("<<Left<<", "<<Top<<", "<<Right<<", "<<Bottom<<")";

			return str.str();
		}

		////Calculates and returns the total margins in X axis
		_T TotalX() const  { return Right+Left; }
		////Calculates and returns the total margins in Y axis
		_T TotalY() const { return Bottom+Top;  }

		//scale, translate, rotate?, +, +=, -, -=, &&, ||
		basic_Margins2D operator +(basic_Margins2D margin) {
			return basic_Margins2D(Left+margin.Left, Top+margin.Top, Right+margin.Right, Bottom+margin.Bottom);
		}
		basic_Margins2D &operator +=(basic_Margins2D margin) {
			Left+=margin.Left;
			Top+=margin.Top;
			Right+=margin.Right;
			Bottom+=margin.Bottom;

			return *this;
		}
		basic_Margins2D operator +(_T margin) {
			return basic_Margins2D(Left+margin, Top+margin, Right+margin, Bottom+margin);
		}
		basic_Margins2D &operator +=(_T margin) {
			Left+=margin;
			Top+=margin;
			Right+=margin;
			Bottom+=margin;

			return *this;
		}
	};

#ifdef BOUNDS2D_EXISTS
	template <typename _T, typename _R>
	basic_Margins2D<_T> operator -(basic_Bounds2D<_T> b1, basic_Bounds2D<_R> b2) {
		return basic_Margins2D<_T>(
			b2.Left-b1.Left, b2.Top-b1.Top, b1.Right-b2.Right, b1.Bottom-b2.Bottom
		);
	}
#endif

	////Allows streaming of margins. It converts point to string,
	/// every row is printed on a line enclosed in braces.
	template <class _T>
	std::ostream &operator << (std::ostream &out, basic_Margins2D<_T> &bounds) {
		out<<"("<<bounds.Left<<", "<<bounds.Right<<", "<<bounds.Top<<", "<<bounds.Bottom<<")";

		return out;
	}


	////Adds the textual form of the point to another string.
	template <class _T>
	std::string &operator + (std::string &out, basic_Margins2D<_T> &margins) {
		return string+(string)margins;
	}


#ifdef GRAPH_USEDOUBLE
	typedef basic_Margins2D<double> Margins2D;
#else
	typedef basic_Margins2D<float> Margins2D;
#endif

	typedef basic_Margins2D<int> Margins;
}