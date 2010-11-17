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
	template <class T_>
	class basic_Margins2D {
	public:
		T_ Left,Top , Right,Bottom;
		
		basic_Margins2D() {}
		basic_Margins2D(T_ All) : Left(All), Right(All), Top(All), Bottom(All) { }
		basic_Margins2D(T_ Horizontal, T_ Vertical) : Left(Horizontal), Right(Horizontal), Top(Vertical), Bottom(Vertical) { }
		basic_Margins2D(T_ Left, T_ Top, T_ Right, T_ Bottom) : 
			Left(Left), Right(Right), Top(Top), Bottom(Bottom) { }


		operator std::string() {
			std::ostringstream str;
			str<<"("<<Left<<", "<<Top<<", "<<Right<<", "<<Bottom<<")";

			return str.str();
		}

		////Calculates and returns the total margins in X axis
		T_ TotalX() const  { return Right+Left; }
		////Calculates and returns the total margins in Y axis
		T_ TotalY() const { return Bottom+Top;  }

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
		basic_Margins2D operator +(T_ margin) {
			return basic_Margins2D(Left+margin, Top+margin, Right+margin, Bottom+margin);
		}
		basic_Margins2D &operator +=(T_ margin) {
			Left+=margin;
			Top+=margin;
			Right+=margin;
			Bottom+=margin;

			return *this;
		}

		basic_Margins2D AddToTop(T_ val,basic_Margins2D margin=basic_Margins2D(0)) {
			return basic_Margins2D(Left, Top+margin.TotalY()+val, Right, Bottom);
		}

		basic_Margins2D AddToLeft(T_ val,basic_Margins2D margin=basic_Margins2D(0)) {
			return basic_Margins2D(Left+margin.TotalX()+val, Top, Right, Bottom);
		}

		basic_Margins2D AddToBottom(T_ val,basic_Margins2D margin=basic_Margins2D(0)) {
			return basic_Margins2D(Left, Top, Right, Bottom+margin.TotalY()+val);
		}

		basic_Margins2D AddToRight(T_ val,basic_Margins2D margin=basic_Margins2D(0)) {
			return basic_Margins2D(Left, Top, Right+margin.TotalX()+val, Bottom);
		}
	};

#ifdef BOUNDS2D_EXISTS
	template <typename T_, typename R_>
	basic_Margins2D<T_> operator -(basic_Bounds2D<T_> b1, basic_Bounds2D<R_> b2) {
		return basic_Margins2D<T_>(
			b2.Left-b1.Left, b2.Top-b1.Top, b1.Right-b2.Right, b1.Bottom-b2.Bottom
		);
	}
#endif

	////Allows streaming of margins. It converts point to string,
	/// every row is printed on a line enclosed in braces.
	template <class T_>
	std::ostream &operator << (std::ostream &out, basic_Margins2D<T_> &bounds) {
		out<<"("<<bounds.Left<<", "<<bounds.Right<<", "<<bounds.Top<<", "<<bounds.Bottom<<")";

		return out;
	}


	////Adds the textual form of the point to another string.
	template <class T_>
	std::string &operator + (std::string &out, basic_Margins2D<T_> &margins) {
		return string+(string)margins;
	}


#ifdef GRAPH_USEDOUBLE
	typedef basic_Margins2D<double> Margins2D;
#else
	typedef basic_Margins2D<float> Margins2D;
#endif

	typedef basic_Margins2D<int> Margins;
}