/// @file Bounds.h contains the Bounds class

#pragma once

#include <iostream>
#include <ostream>
#include <string>
#include <iomanip>
#include <limits>
#include <sstream>

#include "Point.h"
#include "Size.h"
#include "../String.h"


namespace Gorgon { namespace Geometry {

	/// This class represents boundaries of 2D objects. A bounds object contains both the starting boundary
	/// and ending boundary. Therefore, a bounds that has a Width() of 100 and Left of 0, has Right value
	/// as 99, not 100.
	template <class T_>
	class basic_Bounds {
	public:
		
		/// Default constructor, does **not** zero initialize object
		basic_Bounds() { }

		/// Constructor that allows coordinates to be specified individually. Performs normalization
		/// therefore, left could be larger than right or top could be larger than bottom.
		basic_Bounds(const T_ &left, const T_ &top, const T_ &right, const T_ &bottom) : Left(left), Top(top), 
		Right(right), Bottom(bottom) {
			Normalize();
		}

		/// Constructs minimum bounds that includes the given points
		basic_Bounds(const basic_Point<T_> &topleft, const basic_Point<T_> &bottomright) : Left(topleft.x), Top(topleft.y), 
		Right(bottomright.x), Bottom(bottomright.y) {
			Normalize();
		}

		/// Constructs bounds from the given coordinates and size
		basic_Bounds(const basic_Point<T_> &topleft, const basic_Size<T_> &size) : Left(topleft.x), Top(topleft.y), 
		Right(topleft.x+size.Width-1), Bottom(topleft.y+size.Height-1) {
			Normalize();
		}

		/// Constructs bounds from the given coordinates and size
		basic_Bounds(const T_ &left, const T_ &top, const basic_Size<T_> &size) : Left(left), Top(top),
		Right(TopLeft.x+size.Width-1), Bottom(TopLeft.y+size.Height-1) {
			Normalize();
		}

		/// Constructs bounds from the given coordinates and size
		basic_Bounds(const basic_Point<T_> &topleft, const T_ &width, const T_ &height) :
		Left(topleft.x), Top(topleft.y), Right(topleft.x+width-1), Bottom(topleft.y+height-1) {
			Normalize();
		}

		/// Conversion constructor that creates bounds from another type
		template<class O_>
		basic_Bounds(const basic_Bounds<O_> &r) : Left((T_)r.Left), Top((T_)r.Top), 
		Right((T_)r.Right), Bottom((T_)r.Bottom) {
		}

		/// Converting assignment from another type
		template<class O_>
		basic_Bounds operator =(const basic_Bounds<O_> &other) {
			Left   = other.Left;
			Top    = other.Top;
			Right  = other.Right;
			Bottom = other.Bottom;
		}

		/// Compares two bounds objects
		bool operator ==(const basic_Bounds &other) {
			return Left==other.Left && Right==other.Right && Top==other.Top && Bottom==other.Bottom;
		}

		/// Compares two bounds objects
		bool operator !=(const basic_Bounds &other) {
			return !this->operator==(other);
		}

		/// Normalizes bounds object so that Left and Right and Top and Bottom are ordered
		/// properly.
		void Normalize() {
			if(Left>Right) std::swap(Left,Right);
			if(Top>Bottom) std::swap(Top,Bottom);
		}

		/// Returns top left corner
		basic_Point<T_> TopLeft() const {
			return basic_Point<T_>(Left, Top);
		}

		/// Returns top right corner
		basic_Point<T_> TopRight() const {
			return basic_Point<T_>(Right, Top);
		}

		/// Returns center of bounds
		basic_Point<T_> Center() const {
			return basic_Point<T_>(Left+Width()/2, Top+Height()/2);
		}

		/// Returns bottom left corner
		basic_Point<T_> BottomLeft() const {
			return basic_Point<T_>(Left, Bottom);
		}

		/// Returns bottom right corner
		basic_Point<T_> BottomRight() const {
			return basic_Point<T_>(Right, Bottom);
		}

		/// Returns the size of the bounds object
		basic_Size<T_> GetSize() const {
			return basic_Size<T_>(Width(), Height());
		}

		/// Performs union operation. Returns a bounds that contains this bounds object
		/// as well as the given bounds
		basic_Bounds operator |(const basic_Bounds &b) const {
			return basic_Bounds(
				l.Left  < r.Left  ? l.Left  : r.Left  , 
				l.Top   < r.Top   ? l.Top   : r.Top   , 
				l.Right > r.Right ? l.Right : r.Right , 
				l.Bottom> r.Bottom? l.Bottom: r.Bottom
			);						
		}

		/// Performs intersect operation. Returns a bounds that contains the region that
		/// this bounds and the given bounds covers. If they do not interact, an empty bounds
		/// <0-0, 0-0> is returned.
		basic_Bounds operator &(const basic_Bounds &b) const {
			basic_Bounds<T_> b;
			b.Left  = l.Left  > l.Left  ? l.Left  : r.Left;
			b.Top   = l.Top   > l.Top   ? l.Top   : r.Top;
			b.Right = l.Right < l.Right ? l.Right : r.Right;
			b.Bottom= l.Bottom< l.Bottom? l.Bottom: r.Bottom;

			if(b.Left>b.Right || b.Top>b.Bottom) {
				return basic_Bounds<T_>(0,0,0,0);
			}

			return b;
		}

		/// Performs union operation. Returns a bounds that contains this bounds object
		/// as well as the given bounds
		basic_Bounds &operator |=(const basic_Bounds &b) const {
			Left  =Left  < b.Left  ? Left  : b.Left  ;
			Top   =Top   < b.Top   ? Top   : b.Top   ;
			Right =Right > b.Right ? Right : b.Right ;
			Bottom=Bottom> b.Bottom? Bottom: b.Bottom;

			return *this;
		}

		/// Performs intersect operation. Returns a bounds that contains the region that
		/// this bounds and the given bounds covers. If they do not interact, an empty bounds
		/// <0-0, 0-0> is returned.
		basic_Bounds &operator &=(const basic_Bounds &b) const {
			Left  =Left  > b.Left  ? Left  : b.Left  ;
			Top   =Top   > b.Top   ? Top   : b.Top   ;
			Right =Right < b.Right ? Right : b.Right ;
			Bottom=Bottom< b.Bottom? Bottom: b.Bottom;

			return *this;
		}

		/// Checks whether the given point is inside this bounds.
		bool isInside(const basic_Point<T_> &p) const {
			return p.X>=Left && p.Y>=Top && p.X<=Right && p.Y<=Bottom;
		}

		/// Changes the size of the bounds
		void SetSize(const basic_Size2D<T_> &s) {
			Right = Left + s.Width-1;
			Bottom= Top  + s.Height-1;
		}

		/// Changes the size of the bounds
		void SetSize(const T_ &width, const T_ &height) {
			Right = Left + width-1;
			Bottom= Top  + height-1;
		}

		/// Changes the width of the bounds
		void SetWidth(const T_ &width) {
			Right = Left + width-1;
		}

		/// Changes the height of the bounds
		void SetHeight(const T_ &height) {
			Bottom= Top  + height-1;
		}

		/// Changes the position of the bounds
		void Move(const basic_Point<T_> &p) {
			Right = (Right-Left)+p.X	;
			Bottom= (Bottom-Top)+p.Y	;
			Left  = p.X	;
			Top   = p.Y ;
		}
		
		/// Changes the position of the bounds
		void Move(T_ x, T_ y) {
			Right = (Right-Left)+x	;
			Bottom= (Bottom-Top)+y	;
			Left  = x;
			Top   = y;
		}

		/// Creates a new bounds object that is the offset of this bounds by 
		/// the given point
		basic_Bounds operator +(const basic_Point<T_> &p) const {
			return basic_Bounds(
				Left   + p.X,
				Top    + p.Y,
				Right  + p.X,
				Bottom + p.Y
			);
		}

		/// Creates a new bounds object that is the offset of this bounds by 
		/// the given point
		basic_Bounds operator -(const basic_Point<T_> &p) const {
			return basic_Bounds(
				Left   - p.X,
				Top    - p.Y,
				Right  - p.X,
				Bottom - p.Y
			);
		}

		/// Creates a new bounds object that is the scale version of this bounds
		/// by the given size
		template<class O_>
		basic_Bounds operator *(const basic_Size<O_> &s) {
			return basic_Bounds(
				T_(Left   * s.Width),
				T_(Top    * s.Height),
				T_(Right  * s.Width),
				T_(Bottom * s.Height)
			);
		}

		/// Creates a new bounds object that is the scale version of this bounds
		/// by the given size
		template<class O_>
		basic_Bounds operator /(const basic_Size<O_> &s) {
			return basic_Bounds(
				T_(Left   / s.Width),
				T_(Top    / s.Height),
				T_(Right  / s.Width),
				T_(Bottom / s.Height)
			);
		}

		/// Creates a new bounds object that is the scale version of this bounds
		/// by the given size
		template<class O_>
		basic_Bounds operator *(const O_ &s) {
			return basic_Bounds(
				T_(Left   * s),
				T_(Top    * s),
				T_(Right  * s),
				T_(Bottom * s)
			);
		}

		/// Creates a new bounds object that is the scale version of this bounds
		/// by the given size
		template<class O_>
		basic_Bounds operator /(const O_ &s) {
			return basic_Bounds(
				T_(Left   / s),
				T_(Top    / s),
				T_(Right  / s),
				T_(Bottom / s)
			);
		}

		/// Offsets this bounds objects by the given coordinates
		basic_Bounds &operator +=(const basic_Point<T_> &p) {
			Left   += p.X;
			Top    += p.Y;
			Right  += p.X;
			Bottom += p.Y;

			return *this;
		}

		/// Offsets this bounds objects by the given coordinates
		basic_Bounds &operator -=(const basic_Point<T_> &p) {
			Left   -= p.X;
			Top    -= p.Y;
			Right  -= p.X;
			Bottom -= p.Y;

			return *this;
		}

		/// Resizes this bounds objects by the given size
		template<class O_>
		basic_Bounds &operator *=(const basic_Size<O_> &s) {
			Left   = Left  *s.Width;
			Top    = Top   *s.Height;
			Right  = Right *s.Width;
			Bottom = Bottom*s.Height;

			return *this;
		}

		/// Resizes this bounds objects by the given size
		template<class O_>
		basic_Bounds &operator /=(const basic_Size<O_> &s) {
			Left   = Left  /s.Width;
			Top    = Top   /s.Height;
			Right  = Right /s.Width;
			Bottom = Bottom/s.Height;

			return *this;
		}

		/// Resizes this bounds objects by the given size
		template<class O_>
		basic_Bounds &operator *=(const O_ &s) {
			Left   = Left  *s;
			Top    = Top   *s;
			Right  = Right *s;
			Bottom = Bottom*s;

			return *this;
		}

		/// Resizes this bounds objects by the given size
		template<class O_>
		basic_Bounds &operator /=(const O_ &s) {
			Left   = Left  /s;
			Top    = Top   /s;
			Right  = Right /s;
			Bottom = Bottom/s;

			return *this;
		}

		/// Left-most boundary
		T_ Left;

		/// Top-most boundary
		T_ Top;

		/// Right-most boundary
		T_ Right;

		/// Bottom-most boundary
		T_ Bottom;
	};


	/// Allows streaming of bounds. in string representation, bounds is show as
	/// <x_s-x_e , y_s-y_e>
	template <class T_>
	std::ostream &operator << (std::ostream &out, const basic_Bounds<T_> &bounds) {
		out<<"<"<<bounds.Left<<"-"<<bounds.Right<<" , "<<bounds.Top<<"-"<<bounds.Bottom<<">";

		return out;
	}

	/// Stream extractor for bounds
	template <class T_>
	std::istream &operator >> (std::istream &in, basic_Bounds<T_> &bounds) {
		while(in.peek()==' ' || in.peek()=='<')
			in.ignore(1);

		std::string s;

		bool first=true;
		while(!in.eof() && (first||in.peek()!='-') ) {
			first=false;
			s.append(1, (char)in.get());
		}

		if(in.eof()) {
			in.setstate(in.failbit);
			return in;
		}
		in.ignore(1);

		T_ l=String::To<T_>(s);

		s="";

		while(in.peek()==' ' || in.peek()=='\t')
			in.ignore(1);

		while(in.peek()!=',' && !in.eof())
			s.append(1, (char)in.get());

		if(in.eof()) {
			in.setstate(in.failbit);
			return in;
		}
		in.ignore(1);

		T_ t=String::To<T_>(s);

		s="";

		while(in.peek()==' ' || in.peek()=='\t')
			in.ignore(1);

		first=true;
		while(!in.eof() && (first||in.peek()!='-') ) {
			first=false;
			s.append(1, (char)in.get());
		}

		if(in.eof()) {
			in.setstate(in.failbit);
			return in;
		}
		in.ignore(1);

		T_ r=String::To<T_>(s);

		s="";

		while(in.peek()==' ' || in.peek()=='\t')
			in.ignore(1);

		while(in.peek()!='>' && in.peek()!=' ' && in.peek()!='\t' && in.peek()!='\n' && in.peek()!='\r' && !in.eof())
			s.append(1, in.get());

		bounds.Left  = l;
		bounds.Top   = t;
		bounds.Right = r;
		bounds.Bottom= String::To<T_>(s);

		if(in.peek()=='>')
			in.ignore(1);

		return in;
	}

	/// Creates a new bounds that contains only the intersection of two bounds. If they do not
	/// intersect, a bounds of <0-0, 0-0> is returned. It is possible to use this function
	/// without referring to Geometry namespace.
	template <class T_>
	basic_Bounds<T_> Intersect(const basic_Bounds<T_> &l, const basic_Bounds<T_> &r) {
		basic_Bounds<T_> b;
		b.Left  = l.Left  > l.Left  ? l.Left  : r.Left;
		b.Top   = l.Top   > l.Top   ? l.Top   : r.Top;
		b.Right = l.Right < l.Right ? l.Right : r.Right;
		b.Bottom= l.Bottom< l.Bottom? l.Bottom: r.Bottom;

		if(b.Left>b.Right || b.Top>b.Bottom) {
			return basic_Bounds<T_>(0,0,0,0);
		}

		return b;
	}

	/// Returns the smallest bounds that contains given bounds. It is possible to use this function
	/// without referring to Geometry namespace.
	template <class T_>
	basic_Bounds<T_> Union(const basic_Bounds<T_> &l, const basic_Bounds<T_> &r) {
		return basic_Bounds(
			l.Left  < r.Left  ? l.Left  : r.Left  , 
			l.Top   < r.Top   ? l.Top   : r.Top   , 
			l.Right > r.Right ? l.Right : r.Right , 
			l.Bottom> r.Bottom? l.Bottom: r.Bottom
		);						
	}

	/// Checks whether two bounds are colliding. It is possible to use this function
	/// without referring to Geometry namespace.
	template <class T_>
	bool IsColliding(const basic_Bounds<T_> &l, const basic_Bounds<T_> &r) const {
		// check disjunction x-coordinates
		if(l.Left  > r.Right) return false;
		if(l.Right < r.Left) return false;

		// check disjunction on y-coordinates
		if(l.Top    > r.Bottom) return false;
		if(l.Bottom < r.Top) return false;

		return true;
	}

	
	/// @see basic_Bounds
	typedef basic_Bounds<Float> Boundsf;

	/// @see basic_Bounds
	typedef basic_Bounds<int>   Bounds;

} }
