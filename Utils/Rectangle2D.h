//DESCRIPTION
//	This file contains the class Rectangle2D which defines a rectangular
//	region using top, left corner and width and height information.
//	Missing some operators.

//REQUIRES:
//	GGE/Utils/Point2D, GGE/Utils/Size2D

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
#include <locale>


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
		{
			if(Width<0) {
				Left=Left+Width;
				Width=-Width;
			}
			if(Height<0) {
				Top=Top+Height;
				Height=-Height;
			}
		}

		basic_Rectangle2D(const basic_Point2D<T_> &TopLeft, const basic_Size2D<T_> &HeightWidth) : 
		Left(TopLeft.x), Top(TopLeft.y), Width(HeightWidth.Width), Height(HeightWidth.Height)
		{
			if(Width<0) {
				Left=Left+Width;
				Width=-Width;
			}
			if(Height<0) {
				Top=Top+Height;
				Height=-Height;
			}
		}

		basic_Rectangle2D(T_ Left, T_ Top, const basic_Size2D<T_> &HeightWidth) : 
		Left(Left), Top(Top), Width(HeightWidth.Width), Height(HeightWidth.Height)
		{
			if(Width<0) {
				Left=Left+Width;
				Width=-Width;
			}
			if(Height<0) {
				Top=Top+Height;
				Height=-Height;
			}
		}
			
		basic_Rectangle2D(const basic_Point2D<T_> &TopLeft, int Width, int Height) : 
			Left(TopLeft.x), Top(TopLeft.y), Width(Width), Height(Height)
		{
			if(Width<0) {
				Left=Left+Width;
				Width=-Width;
			}
			if(Height<0) {
				Top=Top+Height;
				Height=-Height;
			}
		}
		
		basic_Rectangle2D(const basic_Point2D<T_> &TopLeft, const basic_Point2D<T_> &BottomRight) :
			Left(TopLeft.x), Top(TopLeft.y),
			Width(BottomRight.x-TopLeft.x),	Height(BottomRight.y-TopLeft.y)
		{
			if(Width<0) {
				Left=Left+Width;
				Width=-Width;
			}
			if(Height<0) {
				Top=Top+Height;
				Height=-Height;
			}
		}

		template <class U>
		basic_Rectangle2D(const basic_Rectangle2D<U> &rect) : 
			Left(T_(rect.Left)), Top(T_(rect.Top)), Width(T_(rect.Width)), Height(T_(rect.Height))
		{ }

		basic_Rectangle2D(const basic_Bounds2D<T_> &bounds);

		operator basic_Bounds2D<T_>() const;

		basic_Rectangle2D& operator =(const basic_Bounds2D<T_> &bounds);

		operator std::string() const {
			stringstream ss;
			ss<<"< "<<Left<<", "<<Top<<", "<<Width<<"x"<<Height<<" >";
			return ss.str();
		}

		//Without any unnecessary markers
		std::string CompactString() const {
			stringstream ss;
			ss<<Left<<","<<Top<<","<<Width<<"x"<<Height;
			return ss.str();
		}

		////Calculates and returns the width of the region
		T_ Right() const { return Width +Left; }
		////Calculates and returns the height of the region
		T_ Bottom() const { return Height+Top;  }

		void SetRight(T_ right) { 
			if(right>Left) 
				Width=right-Left; 
			else {
				Width=Left-right;
				Left=right;
			}
		}
		void SetBottom(T_ bottom) {
			if(bottom>Top) 
				Height=bottom-Top; 
			else {
				Height=Top-bottom;
				Top=bottom;
			}
		}

		void SetSize(const basic_Size2D<T_> &s) { 
			Width=s.Width;
			Height=s.Height;
		}
		basic_Rectangle2D Enlarge(const basic_Size2D<T_> &s) const { 
			return basic_Rectangle2D(Left, Right, Width+s.Width, Height+s.Height);
		}
		basic_Rectangle2D Shrink(const basic_Size2D<T_> &s) const { 
			return basic_Rectangle2D(Left, Right, Width-s.Width, Height-s.Height);
		}
		basic_Rectangle2D Enlarge(T_ s) const { 
			return basic_Rectangle2D(Left, Right, Width+s, Height+s);
		}
		basic_Rectangle2D Shrink(T_ s) const { 
			return basic_Rectangle2D(Left, Right, Width-s, Height-s);
		}
		basic_Rectangle2D Resize(const basic_Size2D<T_> &s) const { 
			return basic_Rectangle2D(Left, Right, s.Width, s.Height);
		}

		//using top-left to scale
		basic_Rectangle2D Scale(FloatingPoint scale) const {
			return basic_Rectangle2D(Left, Top, Width*scale, Height*scale);
		}

		//using top-left to scale
		basic_Rectangle2D Scale(const Size2D &scale) const {
			return basic_Rectangle2D(Left, Top, Width*scale.Width, Height*scale.Height);
		}

		//using top-left to scale
		basic_Rectangle2D Scale(FloatingPoint x, FloatingPoint y) const {
			return basic_Rectangle2D(Left, Top, Width*x, Height*y);
		}

		//using pivot to scale
		basic_Rectangle2D Scale(FloatingPoint scale, const Point2D &pivot) const {
			return basic_Rectangle2D(
				(Left-pivot.x)*scale+pivot.x, 
				(Left-pivot.y)*scale+pivot.y, 
				Width *scale, 
				Height*scale
			);
		}

		//using pivot to scale
		basic_Rectangle2D Scale(const Size2D &scale, const Point2D &pivot) const {
			return basic_Rectangle2D(
				(Left-pivot.x)*scale.Width+pivot.x, 
				(Left-pivot.y)*scale.Height+pivot.y, 
				Width *x, 
				Height*y
			);
		}

		//using pivot to scale
		basic_Rectangle2D Scale(FloatingPoint x, FloatingPoint y, const Point2D &pivot) const {
			return basic_Rectangle2D(
				(Left-pivot.x)*x+pivot.x, 
				(Left-pivot.y)*y+pivot.y, 
				Width *x, 
				Height*y
			);
		}

		basic_Rectangle2D Translate(const basic_Point2D<T_> &amount) const {
			return basic_Rectangle2D(Left+amount.x, Top+amount.y, Width, Height);
		}

		basic_Point2D<T_> Center() const {
			return basic_Point2D<T_>(Left+Width/2,Top+Height/2);
		}

		basic_Point2D<T_> TopLeft() const {
			return basic_Point2D<T_>(Left,Top);
		}

		basic_Point2D<T_> BottomRight() const {
			return basic_Point2D<T_>(Left+Width,Top+Height);
		}

		basic_Size2D<T_> GetSize() const {
			return basic_Size2D<T_>(Width,Height);
		}

		bool operator ==(const basic_Rectangle2D &r) const {
			return Left==r.Left && Top==r.Top && Width==r.Width && Height==r.Height;
		}

		bool operator !=(const basic_Rectangle2D &r) const {
			return Left!=r.Left || Top!=r.Top || Width!=r.Width || Height!=r.Height;
		}

		basic_Rectangle2D operator +(const basic_Point2D<T_> &amount) const {
			return Translate(amount);
		}

		basic_Rectangle2D operator +(const basic_Size2D<T_> &amount) const {
			return basic_Rectangle2D(Left, Top, Width+amount.Width, Height+amount.Height);
		}

		basic_Rectangle2D operator -(const basic_Point2D<T_> &amount) const {
			return Translate(-amount);
		}

		basic_Rectangle2D operator -(const basic_Size2D<T_> &amount) const {
			return basic_Rectangle2D(Left, Top, Width-amount.Width, Height-amount.Height);
		}

		basic_Rectangle2D &operator +=(const basic_Point2D<T_> &amount) {
			Left+=amount.x;
			Top +=amount.y;

			return *this;
		}

		basic_Rectangle2D &operator +=(const basic_Size2D<T_> &amount) {
			Width +=amount.Width;
			Height+=amount.Height;

			return *this;
		}

		basic_Rectangle2D &operator -=(const basic_Point2D<T_> &amount) {
			Left-=amount.x;
			Top -=amount.y;

			return *this;
		}

		basic_Rectangle2D &operator -=(const basic_Size2D<T_> &amount) {
			Width -=amount.Width;
			Height-=amount.Height;

			return *this;
		}

		static basic_Rectangle2D Parse(std::string s) {
			static char tospace[] = ",<>x";

			std::string::size_type pos;
			while( (pos=s.find_first_of(tospace)) != std::string::npos ) {
				s[pos]=' ';
			}

			basic_Rectangle2D ret;

			istringstream is(s);
			is>>ret.Left;
			is>>ret.Top;
			is>>ret.Width;
			is>>ret.Height;

			if(is.fail()) {
				throw std::runtime_error("Input string is not properly formatted");
			}

			return ret;
		}

		bool isInside(const basic_Point2D<T_> &p) const {
			return p.x>Left && p.y>Top && p.x<Right() && p.y<Bottom();
		}

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

#ifdef BOUNDS2D_EXISTS
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
