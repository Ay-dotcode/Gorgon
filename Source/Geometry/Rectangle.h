/// @file Rectangle.h contains the Rectangle class

#pragma once

#include <iostream>
#include <ostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <limits>

#include "../Types.h"
#include "Point.h"
#include "Size.h"
#include "Bounds.h"

namespace Gorgon { namespace Geometry {

	template <class T_>
	class basic_Rectangle {
	public:
		
		basic_Rectangle() {}

		basic_Rectangle(T_ left, T_ top, T_ width, T_ height) : 
			X(left), Y(top), Width(width), Height(height)
		{
			if(Width<0) {
				X=X+Width;
				Width=-Width;
			}
			if(Height<0) {
				Y=Y+Height;
				Height=-Height;
			}
		}

		basic_Rectangle(const basic_Point<T_> &position, const basic_Size<T_> &size) : 
			X(position.x), Y(position.y), Width(size.Width), Height(size.Height)
		{
			if(Width<0) {
				X=X+Width;
				Width=-Width;
			}
			if(Height<0) {
				Y=Y+Height;
				Height=-Height;
			}
		}

		basic_Rectangle(T_ left, T_ top, const basic_Size<T_> &size) : 
			X(left), Y(top), Width(size.Width), Height(size.Height)
		{
			if(Width<0) {
				X=X+Width;
				Width=-Width;
			}
			if(Height<0) {
				Y=Y+Height;
				Height=-Height;
			}
		}
			
		basic_Rectangle(const basic_Point<T_> &position, int width, int height) : 
			X(position.x), Y(position.y), Width(width), Height(height)
		{
			if(Width<0) {
				X=X+Width;
				Width=-Width;
			}
			if(Height<0) {
				Y=Y+Height;
				Height=-Height;
			}
		}
		
		basic_Rectangle(const basic_Point<T_> &topleft, const basic_Point<T_> &bottomright) :
			X(topleft.x), Y(topleft.y),
			Width(bottomright.x-topleft.x), Height(bottomright.y-topleft.y)
		{
			if(Width<0) {
				X=X+Width;
				Width=-Width;
			}
			if(Height<0) {
				Y=Y+Height;
				Height=-Height;
			}
		}

		template <class U>
		basic_Rectangle(const basic_Rectangle<U> &rect) : 
			X(T_(rect.Left)), Y(T_(rect.Top)), Width(T_(rect.Width)), Height(T_(rect.Height))
		{ }

		basic_Rectangle(const basic_Bounds<T_> &bounds) : X(bounds.Left), Y(bounds.Top), 
			Width(bounds.Right-bounds.Left), Height(bounds.Bottom-bounds.Top)
		{ }

		static basic_Rectangle Parse(std::string s) {
			static char tospace[] = ",<>x";

			std::string::size_type pos;
			while((pos=s.find_first_of(tospace)) != std::string::npos) {
				s[pos]=' ';
			}

			basic_Rectangle ret;

			std::istringstream is(s);
			is>>ret.Left;
			is>>ret.Top;
			is>>ret.Width;
			is>>ret.Height;

			if(is.fail()) {
				throw std::runtime_error("Input string is not properly formatted");
			}

			return ret;
		}

		operator basic_Bounds<T_>() const {
			return{TopLeft(), GetSize()};
		}
		
		operator std::string() const {
			std::stringstream ss;
			ss<<"< "<<Left<<", "<<Top<<", "<<Width<<"x"<<Height<<" >";
			return ss.str();
		}

		////Calculates and returns the width of the region
		T_ Right() const { return Width +Left; }

		////Calculates and returns the height of the region
		T_ Bottom() const { return Height+Top;  }

		void SetRight(T_ right) { 
			if(right>X) 
				Width=right-X; 
			else {
				Width=X-right;
				X=right;
			}
		}
		void SetBottom(T_ bottom) {
			if(bottom>Y) 
				Height=bottom-Y; 
			else {
				Height=Y-bottom;
				Y=bottom;
			}
		}

		void SetSize(const basic_Size<T_> &s) { 
			Width=s.Width;
			Height=s.Height;
		}
		basic_Rectangle Enlarge(const basic_Size<T_> &s) const { 
			return{X, Y, Width+s.Width, Height+s.Height};
		}
		basic_Rectangle Shrink(const basic_Size<T_> &s) const { 
			return{X, Y, Width-s.Width, Height-s.Height};
		}
		basic_Rectangle Enlarge(T_ s) const { 
			return{X, Y, Width+s, Height+s};
		}
		basic_Rectangle Shrink(T_ s) const { 
			return{X, Y, Width-s, Height-s};
		}

		basic_Rectangle Resize(const basic_Size<T_> &s) const { 
			return{X, Y, s.Width, s.Height};
		}

		basic_Point<T_> Center() const {
			return{X+Width/2, Y+Height/2};
		}

		basic_Point<T_> TopLeft() const {
			return{X, Y};
		}

		basic_Point<T_> TopRight() const {
			return{X+Width, Y};
		}

		basic_Point<T_> BottomLeft() const {
			return{X, Y+Height};
		}

		basic_Point<T_> BottomRight() const {
			return{X+Width, Y+Height};
		}

		basic_Size<T_> GetSize() const {
			return{Width, Height};
		}

		bool operator ==(const basic_Rectangle &r) const {
			return Left==r.Left && Top==r.Top && Width==r.Width && Height==r.Height;
		}

		bool operator !=(const basic_Rectangle &r) const {
			return Left!=r.Left || Top!=r.Top || Width!=r.Width || Height!=r.Height;
		}

		basic_Rectangle operator +(const basic_Point<T_> &amount) const {
			return{X+amount.X, Y+amount.Y, Width, Height};
		}

		basic_Rectangle operator +(const basic_Size<T_> &amount) const {
			return{X, Y, Width+amount.Width, Height+amount.Height};
		}

		basic_Rectangle operator -(const basic_Point<T_> &amount) const {
			return{X-amount.X, Y-amount.Y, Width, Height};
		}

		basic_Rectangle operator -(const basic_Size<T_> &amount) const {
			return{X, Y, Width-amount.Width, Height-amount.Height};
		}

		basic_Rectangle &operator +=(const basic_Point<T_> &amount) {
			X+=amount.X;
			Y+=amount.Y;

			return *this;
		}

		basic_Rectangle &operator +=(const basic_Size<T_> &amount) {
			Width +=amount.Width;
			Height+=amount.Height;

			return *this;
		}

		basic_Rectangle &operator -=(const basic_Point<T_> &amount) {
			X-=amount.X;
			Y -=amount.Y;

			return *this;
		}

		basic_Rectangle &operator -=(const basic_Size<T_> &amount) {
			Width -=amount.Width;
			Height-=amount.Height;

			return *this;
		}

		//operator * / (scalar & size)

		T_ X;
		T_ Y;
		T_ Width;
		T_ Height;
	};

	////Allows streaming of Rectangle
	template <class T_>
	std::ostream &operator << (std::ostream &out, const basic_Rectangle<T_> &Rectangle) {
		out<<"<"<<Rectangle.X<<", "<<Rectangle.Y<<", "<<Rectangle.Width<<"x"<<Rectangle.Height<<">";

		return out;
	}
	template <class T_>
	std::istream &operator >> (std::istream &in, basic_Rectangle<T_> &rect) {
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

		while(in.peek()!=',' && !in.eof())
			s.append(1, (char)in.get());

		in.ignore(1);

		ss.str(s);
		ss.clear();
		ss>>rect.Top;

		s="";

		while(in.peek()==' ' || in.peek()=='\t')
			in.ignore(1);

		while(in.peek()!=',' && in.peek()!='x' && !in.eof())
			s.append(1, (char)in.get());

		in.ignore(1);

		ss.str(s);
		ss.clear();
		ss>>rect.Width;

		s="";

		while(in.peek()==' ' || in.peek()=='\t')
			in.ignore(1);

		while(in.peek()!='>' && in.peek()!=' ' && in.peek()!='\t' && in.peek()!='\n' && in.peek()!='\r' && !in.eof())
			s.append(1, in.get());


		ss.str(s);
		ss.clear();
		ss>>rect.Height;

		if(in.peek()=='>')
			in.ignore(1);

		return in;
	}

	template<class T_>
	bool IsInside(const basic_Rectangle<T_> &r, const basic_Point<T_> &p) {
		return p.X>=r.X && p.Y>=r.Y && p.X<r.Right() && p.Y<r.Bottom();
	}

	typedef basic_Rectangle<Float> Rectanglef;

	typedef basic_Rectangle<int> Rectangle;

} }
