//DESCRIPTION
//	This file contains the class Margins2D which stores margins for a rectangular
//	object. Include Utils/Bounds2D before this file for additional functionality
//	allowing subtraction of two Bounds objects, adding or subtracting Margins
//	object to/from Bounds objects.

//REQUIRES:
//	GGE/Utils/Bounds2D (optional)

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


#define MARGINS2D_EXISTS


namespace gge { namespace utils {
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

		////Calculates and returns the total margins in X axis
		T_ Horizontal() const  { return Right+Left; }
		////Calculates and returns the total margins in Y axis
		T_ Vertical() const { return Bottom+Top;  }

		//scale, translate, rotate?, +, +=, -, -=, &&, ||
		basic_Margins2D operator +(basic_Margins2D margin) {
			return basic_Margins2D(Left+margin.Left, Top+margin.Top, Right+margin.Right, Bottom+margin.Bottom);
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

		basic_Point2D<T_> TopLeft() const {
			return basic_Point2D<T_>(Left, Top);
		}

		basic_Margins2D operator -(const basic_Margins2D &right) const {
			return basic_Margins2D(Left-right.Left, Top-right.Top, Right-right.Right, Bottom-right.Bottom);
		}

		basic_Margins2D operator +(const basic_Margins2D &right) const {
			return basic_Margins2D(Left+right.Left, Top+right.Top, Right+right.Right, Bottom+right.Bottom);
		}

		basic_Margins2D &operator -=(const basic_Margins2D &right) {
			Left-=right.Left;
			Top-=right.Top;
			Right-=right.Right;
			Bottom-=right.Bottom;

			return *this;
		}

		basic_Margins2D &operator +=(const basic_Margins2D &right) {
			Left+=right.Left;
			Top+=right.Top;
			Right+=right.Right;
			Bottom+=right.Bottom;

			return *this;
		}
	};

	template <typename T_, typename R_>
	basic_Size2D<T_> operator +(const basic_Size2D<T_> &s, const basic_Margins2D<R_> &m) {
		return basic_Size2D<T_>(s.Width+m.TotalX(), s.Height+m.TotalY());
	}

	template <typename T_, typename R_>
	basic_Size2D<T_> operator -(const basic_Size2D<T_> &s, const basic_Margins2D<R_> &m) {
		return basic_Size2D<T_>(s.Width-m.TotalX(), s.Height-m.TotalY());
	}


#ifdef BOUNDS2D_EXISTS
	template <typename T_, typename R_>
	basic_Margins2D<T_> operator -(const basic_Bounds2D<T_> &b1, const basic_Bounds2D<R_> &b2) {
		return basic_Margins2D<T_>(
			b2.Left-b1.Left, b2.Top-b1.Top, b1.Right-b2.Right, b1.Bottom-b2.Bottom
			);
	}

	template <typename T_, typename R_>
	basic_Bounds2D<T_> operator +(const basic_Bounds2D<T_> &b, const basic_Margins2D<R_> &m) {
		return basic_Bounds2D<T_>(
			b.Left-m.Left, b.Top-m.Top, b.Right+m.Right, b.Bottom+m.Bottom
			);
	}

	template <typename T_, typename R_>
	basic_Bounds2D<T_> operator -(const basic_Bounds2D<T_> &b, const basic_Margins2D<R_> &m) {
		return basic_Bounds2D<T_>(
			b.Left+m.Left, b.Top+m.Top, b.Right-m.Right, b.Bottom-m.Bottom
			);
	}
#endif

	////Allows streaming of margins.
	template <class T_>
	std::ostream &operator << (std::ostream &out, const basic_Margins2D<T_> &margins) {
		out<<"("<<margins.Left<<", "<<margins.Right<<", "<<margins.Top<<", "<<margins.Bottom<<")";

		return out;
	}
	template <class T_>
	std::istream &operator >> (std::istream &in, basic_Margins2D<T_> &margins) {
		while(in.peek()==' ' || in.peek()=='(')
			in.ignore(1);

		std::string s;
		std::stringstream ss;

		while(in.peek()!=',' && !in.eof())
			s.append(1, (char)in.get());

		in.ignore(1);

		ss.str(s);
		ss>>margins.Left;

		s="";

		while(in.peek()==' ' || in.peek()=='\t')
			in.ignore(1);

		while(in.peek()!=',' && !in.eof())
			s.append(1, (char)in.get());

		in.ignore(1);

		ss.str(s);
		ss>>margins.Top;

		s="";

		while(in.peek()==' ' || in.peek()=='\t')
			in.ignore(1);

		while(in.peek()!=',' && !in.eof())
			s.append(1, (char)in.get());

		in.ignore(1);

		ss.str(s);
		ss>>margins.Right;

		s="";

		while(in.peek()==' ' || in.peek()=='\t')
			in.ignore(1);

		while(in.peek()!='>' && in.peek()!=' ' && in.peek()!='\t' && in.peek()!='\n' && in.peek()!='\r' && !in.eof())
			s.append(1, in.get());


		ss.str(s);
		ss.clear();
		ss>>margins.Bottom;

		if(in.peek()==')')
			in.ignore(1);

		return in;
	}



	template<class T_>
	basic_Margins2D<T_> &operator +=(basic_Margins2D<T_> &l, const basic_Margins2D<T_> &r) {
		l.Left+=r.Left;
		l.Top+=r.Top;
		l.Right+=r.Right;
		l.Bottom+=r.Bottom;

		return l;
	}

#ifdef GRAPH_USEDOUBLE
	typedef basic_Margins2D<double> Margins2D;
#else
	typedef basic_Margins2D<float> Margins2D;
#endif

	typedef basic_Margins2D<int> Margins;
} }
