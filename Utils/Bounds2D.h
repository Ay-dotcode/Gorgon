//DESCRIPTION
//	This file contains the class Bounds2D which defines bounds of an object
//	with left,top ; right,bottom coordinates. This class can be converted
//	to rectangle class (top,left ; width,height)
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

#ifdef MARGINS2D_EXISTS
#	error "Include bounds before margins for extra features"
#endif

#define BOUNDS2D_EXISTS


namespace gge { namespace utils {
#ifndef RECTANGLE2D_EXISTS
	template <class T_> class basic_Rectangle2D;
#endif

	template <class T_>
	class basic_Bounds2D {
	public:
		T_ Left,Top , Right,Bottom;
		
		basic_Bounds2D() {}
		explicit basic_Bounds2D(T_ value) : Left(value), Top(value), Right(value), Bottom(value)
		{ }
		basic_Bounds2D(T_ Left, T_ Top, T_ Right, T_ Bottom) : Left(Left), Top(Top), Right(Right), Bottom(Bottom)
		{
			if(this->Left>this->Right) std::swap(this->Left,this->Right);
			if(this->Top>this->Bottom) std::swap(this->Top,this->Bottom);
		}
		basic_Bounds2D(const basic_Point2D<T_> &TopLeft, const basic_Point2D<T_> &BottomRight) : 
		Left(TopLeft.x), Top(TopLeft.y), Right(BottomRight.x), Bottom(BottomRight.y)
		{
			if(Left>Right) std::swap(Left,Right);
			if(Top>Bottom) std::swap(Top,Bottom);
		}
		basic_Bounds2D(const basic_Point2D<T_> &TopLeft, const basic_Size2D<T_> &HeightWidth) :
		Left(TopLeft.x), Top(TopLeft.y), Right(TopLeft.x+HeightWidth.Width), Bottom(TopLeft.y+HeightWidth.Height)
		{
			if(Left>Right) std::swap(Left,Right);
			if(Top>Bottom) std::swap(Top,Bottom);
		}
		basic_Bounds2D(const basic_Point2D<T_> &TopLeft, int Width, int Height) :
		Left(TopLeft.x), Top(TopLeft.y), Right(TopLeft.x+Width), Bottom(TopLeft.y+Height)
		{
			if(Left>Right) std::swap(Left,Right);
			if(Top>Bottom) std::swap(Top,Bottom);
		}
		template<class O_>
		basic_Bounds2D(const basic_Bounds2D<O_> &r) : Left((T_)r.Left), Top((T_)r.Top), Right((T_)r.Right), Bottom((T_)r.Bottom) {}
		basic_Bounds2D(const basic_Rectangle2D<T_> &bounds);

		operator basic_Rectangle2D<T_>() const;

		operator std::string() const {
			std::ostringstream str;
			str<<"{"<<Left<<","<<Top<<" - "<<Right<<","<<Bottom<<"}";

			return str.str();
		}

		basic_Bounds2D& operator =(const basic_Rectangle2D<T_> &rect);

		////Calculates and returns the width of the region
		T_ Width() const  { return Right -Left; }
		////Calculates and returns the height of the region
		T_ Height() const { return Bottom-Top;  }

		basic_Bounds2D Intersect(const basic_Bounds2D &b) const {
			return basic_Bounds2D(
				Left  > b.Left  ? Left  : b.Left  , 
				Top   > b.Top   ? Top   : b.Top   , 
				Right < b.Right ? Right : b.Right , 
				Bottom< b.Bottom? Bottom: b.Bottom
			);
		}

		basic_Bounds2D Union(const basic_Bounds2D &b) const {
			return basic_Bounds2D(
				Left  < b.Left  ? Left  : b.Left  , 
				Top   < b.Top   ? Top   : b.Top   , 
				Right > b.Right ? Right : b.Right , 
				Bottom> b.Bottom? Bottom: b.Bottom
			);
		}

		basic_Point2D<T_> TopLeft() const {
			return basic_Point2D<T_>(Left, Top);
		}

		basic_Point2D<T_> TopRight() const {
			return basic_Point2D<T_>(Right, Top);
		}

		basic_Point2D<T_> Center() const {
			return basic_Point2D<T_>(Left+Width()/2, Top+Height()/2);
		}

		basic_Point2D<T_> BottomRight() const {
			return basic_Point2D<T_>(Right, Bottom);
		}

		basic_Point2D<T_> BottomLeft() const {
			return basic_Point2D<T_>(Left, Bottom);
		}

		basic_Size2D<T_> GetSize() const {
			return basic_Size2D<T_>(Width(), Height());
		}

		basic_Bounds2D Scale(FloatingPoint s) const {
			return Scale(s, Center());
		}

		basic_Bounds2D Scale(const Size2D &s) const {
			return Scale(s.Width, s.Height, Center());
		}

		basic_Bounds2D Scale(FloatingPoint x, FloatingPoint y) const {
			return Scale(x,y, Center());
		}

		basic_Bounds2D Scale(FloatingPoint s, const basic_Point2D<T_> &pivot) const {
			return basic_Bounds2D(
				(Left  - pivot.x)*s + pivot.x,
				(Top   - pivot.y)*s + pivot.y,
				(Right - pivot.x)*s + pivot.x,
				(Bottom- pivot.y)*s + pivot.y
			);
		}

		basic_Bounds2D Scale(const Size2D &s, const basic_Point2D<T_> &pivot) const {
			return Scale(s.Width,s.Height, pivot);
		}

		basic_Bounds2D Scale(FloatingPoint x, FloatingPoint y, const basic_Point2D<T_> &pivot) const {
			return basic_Bounds2D(
				(Left  - pivot.x)*x + pivot.x,
				(Top   - pivot.y)*x + pivot.y,
				(Right - pivot.x)*y + pivot.x,
				(Bottom- pivot.y)*y + pivot.y
			);
		}

		basic_Bounds2D Translate(const basic_Point2D<T_> &amount) const {
			return basic_Bounds2D(
				Left+amount.x,
				Top+amount.y,
				Right+amount.x,
				Bottom+amount.y
			);
		}

		bool operator ==(const basic_Bounds2D &b) const {
			return Left==b.Left && Top==b.Top && Right==b.Right && Bottom==b.Bottom;
		}

		bool operator !=(const basic_Bounds2D &b) const {
			return Left!=b.Left || Top!=b.Top || Right!=b.Right || Bottom!=b.Bottom;
		}

		basic_Bounds2D operator +(const basic_Bounds2D &b) const {
			return Union(b);
		}

		basic_Bounds2D operator |(const basic_Bounds2D &b) const {
			return Union(b);
		}

		basic_Bounds2D operator &(const basic_Bounds2D &b) const {
			return Intersect(b);
		}

		basic_Bounds2D &operator +=(const basic_Bounds2D &b) const {
			Left  =Left  < b.Left  ? Left  : b.Left  ;
			Top   =Top   < b.Top   ? Top   : b.Top   ;
			Right =Right > b.Right ? Right : b.Right ;
			Bottom=Bottom> b.Bottom? Bottom: b.Bottom;

			return *this;
		}

		basic_Bounds2D &operator |=(const basic_Bounds2D &b) const {
			Left  =Left  < b.Left  ? Left  : b.Left  ;
			Top   =Top   < b.Top   ? Top   : b.Top   ;
			Right =Right > b.Right ? Right : b.Right ;
			Bottom=Bottom> b.Bottom? Bottom: b.Bottom;

			return *this;
		}

		basic_Bounds2D &operator &=(const basic_Bounds2D &b) const {
			Left  =Left  > b.Left  ? Left  : b.Left  ;
			Top   =Top   > b.Top   ? Top   : b.Top   ;
			Right =Right < b.Right ? Right : b.Right ;
			Bottom=Bottom< b.Bottom? Bottom: b.Bottom;

			return *this;
		}

		static basic_Bounds2D Parse(std::string s) {
			static char tospace[] = ",-<>";

			std::string::size_type pos;
			while( (pos=s.find_first_of(tospace)) != std::string::npos ) {
				s[pos]=' ';
			}

			basic_Bounds2D ret;

			std::istringstream is(s);
			is>>ret.Left;
			is>>ret.Right;
			is>>ret.Top;
			is>>ret.Bottom;

			if(is.fail()) {
				throw std::runtime_error("Input string is not properly formatted");
			}

			return ret;
		}

		bool isInside(const basic_Point2D<T_> &p) const {
			return p.x>=Left && p.y>=Top && p.x<Right && p.y<Bottom;
		}

		void SetSize(const basic_Size2D<T_> &s) {
			Right = Left + s.Width;
			Bottom= Top  + s.Height;
		}

		void SetSize(T_ Width, T_ Height) {
			Right = Left + Width;
			Bottom= Top  + Height;
		}

		void SetWidth(T_ Width) {
			Right = Left + Width;
		}

		void SetHeight(T_ Height) {
			Bottom= Top  + Height;
		}

		void MoveTo(const basic_Point2D<T_> &p) {
			Right = (Right-Left)+p.x	;
			Bottom= (Bottom-Top)+p.y	;
			Left  = p.x	;
			Top   = p.y ;
		}
		
		void MoveTo(T_ x, T_ y) {
			Right = (Right-Left)+x	;
			Bottom= (Bottom-Top)+y	;
			Left  = x;
			Top   = y;
		}

		
		bool isColliding(const basic_Bounds2D<T_> &p) const {
			// check collision on x-coordinate
			if(this->Left > p.Right) return false;
			if(this->Right < p.Left) return false;

			// check collision on y-coordinate
			if(this->Top > p.Bottom) return false;
			if(this->Bottom < p.Top) return false;

			// no collision
			return true;
		}

		//!More operators are needed
		basic_Bounds2D &operator +=(const basic_Point2D<T_> &p) {
			Left   += p.x;
			Top    += p.y;
			Right  += p.x;
			Bottom += p.y;

			return *this;
		}
	};

	////Allows streaming of bounds. in string representation, bounds is show as
	//// < x_s - x_e , y_s - y_e >
	template <class T_>
	std::ostream &operator << (std::ostream &out, const basic_Bounds2D<T_> &bounds) {
		out<<"<"<<bounds.Left<<"-"<<bounds.Right<<" , "<<bounds.Top<<"-"<<bounds.Bottom<<">";

		return out;
	}
	template <class T_>
	std::istream &operator >> (std::istream &in, basic_Bounds2D<T_> &rect) {
		while(in.peek()==' ' || in.peek()=='<')
			in.ignore(1);

		std::string s;
		std::stringstream ss;

		while(in.peek()!=',' && !in.eof())
			s.append(1, (char)in.get());

		in.ignore(1);

		ss.str(s);
		ss>>rect.Left;

		s="";

		while(in.peek()==' ' || in.peek()=='\t')
			in.ignore(1);

		while(in.peek()!=',' && in.peek()!='-' && !in.eof())
			s.append(1, (char)in.get());

		in.ignore(1);

		ss.str(s);
		ss.clear();
		ss>>rect.Top;

		s="";

		while(in.peek()==' ' || in.peek()=='\t')
			in.ignore(1);

		while(in.peek()!=',' && !in.eof())
			s.append(1, (char)in.get());

		in.ignore(1);

		ss.str(s);
		ss.clear();
		ss>>rect.Right;

		s="";

		while(in.peek()==' ' || in.peek()=='\t')
			in.ignore(1);

		while(in.peek()!='>' && in.peek()!=' ' && in.peek()!='\t' && in.peek()!='\n' && in.peek()!='\r' && !in.eof())
			s.append(1, in.get());


		ss.str(s);
		ss.clear();
		ss>>rect.Bottom;

		if(in.peek()=='>')
			in.ignore(1);

		return in;
	}



	typedef basic_Bounds2D<FloatingPoint> Bounds2D;

	typedef basic_Bounds2D<int> Bounds;


#ifdef RECTANGLE2D_EXISTS
# ifndef GGE_RECT_BOUNDS_SYNERGY
#   define GGE_RECT_BOUNDS_SYNERGY

	template <class T_>
	inline basic_Rectangle2D<T_>::basic_Rectangle2D(const basic_Bounds2D<T_> &bounds) :
	Left(bounds.Left), Top(bounds.Top), Width(bounds.Width()), Height(bounds.Height())
	{ }

	template <class T_>
	inline basic_Rectangle2D<T_>::operator basic_Bounds2D<T_>() const {
		return basic_Bounds2D<T_>(*this);
	}

	template <class T_>
	inline basic_Rectangle2D<T_>& basic_Rectangle2D<T_>::operator =(const basic_Bounds2D<T_> &bounds) {
		Left=bounds.Left;
		Top=bounds.Top;
		Width=bounds.Width();
		Height=bounds.Height();

		return *this;
	}

	template <class T_>
	inline basic_Bounds2D<T_>::basic_Bounds2D(const basic_Rectangle2D<T_> &rectangle) :
	Left(rectangle.Left), Top(rectangle.Top), Right(rectangle.Right()), Bottom(rectangle.Bottom())
	{ }

	template <class T_>
	inline basic_Bounds2D<T_>::operator basic_Rectangle2D<T_>() const {
		return basic_Rectangle2D<T_>(*this);
	}

	template <class T_>
	inline basic_Bounds2D<T_>& basic_Bounds2D<T_>::operator =(const basic_Rectangle2D<T_> &rect) {
		Left=rect.Left;
		Top=rect.Top;
		Right=rect.Right();
		Bottom=rect.Bottom();

		return *this;
	}

#  endif
#endif

} }
