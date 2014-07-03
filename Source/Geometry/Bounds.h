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

	/// This class represents boundaries of 2D objects. A bounds object contains the starting boundary
	/// but not the ending boundary. Therefore, a bounds that has a Width() of 100 and Left of 0, has Right value
	/// as 100, not 99. 
	/// 
	/// A Bounds object that has Left<Right and Top<Bottom is called normalized. Bounds should be normalized
	/// for its methods to work properly. Constructors normalizes input values, most methods leave bounds in 
	/// normal state.
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
		basic_Bounds(const basic_Point<T_> &topleft, const basic_Size<T_> &size) : Left(topleft.X), Top(topleft.Y), 
		Right(topleft.X+size.Width), Bottom(topleft.Y+size.Height) {
			Normalize();
		}

		/// Constructs bounds from the given coordinates and size
		basic_Bounds(const T_ &left, const T_ &top, const basic_Size<T_> &size) : Left(left), Top(top),
		Right(TopLeft.X+size.Width), Bottom(TopLeft.Y+size.Height) {
			Normalize();
		}

		/// Constructs bounds from the given coordinates and size
		basic_Bounds(const basic_Point<T_> &topleft, const T_ &width, const T_ &height) :
		Left(topleft.X), Top(topleft.Y), Right(topleft.X+width), Bottom(topleft.Y+height) {
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
			return{Left, Top};
		}

		/// Returns top right corner
		basic_Point<T_> TopRight() const {
			return{Right, Top};
		}

		/// Returns center of bounds
		basic_Point<T_> Center() const {
			return{Left+Width()/2, Top+Height()/2};
		}

		/// Returns bottom left corner
		basic_Point<T_> BottomLeft() const {
			return{Left, Bottom};
		}

		/// Returns bottom right corner
		basic_Point<T_> BottomRight() const {
			return{Right, Bottom};
		}

		/// Calculates and returns the width of the bounds
		T_ Width() const { 
			return Right-Left;
		}

		/// Calculates and returns the height of the bounds
		T_ Height() const { 
			return Bottom-Top;
		}


		/// Returns the size of the bounds object
		basic_Size<T_> GetSize() const {
			return{Width(), Height()};
		}

		/// Performs union operation. Returns a bounds that contains this bounds object
		/// as well as the given bounds
		basic_Bounds operator |(const basic_Bounds &r) const {
			return{
				Left  < r.Left  ? Left  : r.Left,
				Top   < r.Top   ? Top   : r.Top,
				Right > r.Right ? Right : r.Right,
				Bottom> r.Bottom? Bottom: r.Bottom
			};
		}

		/// Performs intersect operation. Returns a bounds that contains the region that
		/// this bounds and the given bounds covers. If they do not interact, an empty bounds
		/// <0-0, 0-0> is returned.
		basic_Bounds operator &(const basic_Bounds &r) const {
			basic_Bounds<T_> b;
			Left  = Left  > Left  ? Left  : r.Left;
			Top   = Top   > Top   ? Top   : r.Top;
			Right = Right < Right ? Right : r.Right;
			Bottom= Bottom< Bottom? Bottom: r.Bottom;

			if(Left>Right || Top>Bottom) {
				return{0, 0, 0, 0};
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

		/// Changes the size of the bounds
		void Resize(const basic_Size<T_> &s) {
			Right = Left + s.Width;
			Bottom= Top  + s.Height;
		}

		/// Changes the size of the bounds
		void Resize(const T_ &width, const T_ &height) {
			Right = Left + width;
			Bottom= Top  + height;
		}

		/// Changes the width of the bounds
		void SetWidth(const T_ &width) {
			Right = Left + width;
		}

		/// Changes the height of the bounds
		void SetHeight(const T_ &height) {
			Bottom= Top  + height;
		}

		/// Changes the position of the bounds
		void Move(const basic_Point<T_> &p) {
			Right = (Right-Left)+p.X	;
			Bottom= (Bottom-Top)+p.Y	;
			Left  = p.X	;
			Top   = p.Y ;
		}
		
		/// Changes the position of the bounds
		void Move(const T_ &x, const T_ &y) {
			Right = (Right-Left)+x	;
			Bottom= (Bottom-Top)+y	;
			Left  = x;
			Top   = y;
		}

		/// Creates a new bounds object that is the offset of this bounds by 
		/// the given point
		basic_Bounds operator +(const basic_Point<T_> &p) const {
			return{
				Left   + p.X,
				Top    + p.Y,
				Right  + p.X,
				Bottom + p.Y
			};
		}

		/// Creates a new bounds object that is the offset of this bounds by 
		/// the given point
		basic_Bounds operator -(const basic_Point<T_> &p) const {
			return{
				Left   - p.X,
				Top    - p.Y,
				Right  - p.X,
				Bottom - p.Y
			};
		}

		/// Creates a new bounds object that is the scale version of this bounds
		/// by the given size
		template<class O_>
		basic_Bounds operator *(const basic_Size<O_> &s) {
			return{
				T_(Left   * s.Width),
				T_(Top    * s.Height),
				T_(Right  * s.Width),
				T_(Bottom * s.Height)
			};
		}

		/// Creates a new bounds object that is the scale version of this bounds
		/// by the given size
		template<class O_>
		basic_Bounds operator /(const basic_Size<O_> &s) {
			return{
				T_(Left   / s.Width),
				T_(Top    / s.Height),
				T_(Right  / s.Width),
				T_(Bottom / s.Height)
			};
		}

		/// Creates a new bounds object that is the scale version of this bounds
		/// by the given size
		template<class O_>
		basic_Bounds operator *(const O_ &s) {
			return{
				T_(Left   * s),
				T_(Top    * s),
				T_(Right  * s),
				T_(Bottom * s)
			};
		}

		/// Creates a new bounds object that is the scale version of this bounds
		/// by the given size
		template<class O_>
		basic_Bounds operator /(const O_ &s) {
			return{
				T_(Left   / s),
				T_(Top    / s),
				T_(Right  / s),
				T_(Bottom / s)
			};
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


	/// Allows streaming of bounds. in string representation, bounds is shown as
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
		return{
			l.Left  < r.Left  ? l.Left  : r.Left  , 
			l.Top   < r.Top   ? l.Top   : r.Top   , 
			l.Right > r.Right ? l.Right : r.Right , 
			l.Bottom> r.Bottom? l.Bottom: r.Bottom
		};
	}

	/// Checks whether two bounds are colliding. It is possible to use this function
	/// without referring to Geometry namespace.
	template <class T_>
	bool IsColliding(const basic_Bounds<T_> &l, const basic_Bounds<T_> &r) {
		// check disjunction x-coordinates
		if(l.Left  > r.Right) return false;
		if(l.Right < r.Left) return false;

		// check disjunction on y-coordinates
		if(l.Top    > r.Bottom) return false;
		if(l.Bottom < r.Top) return false;

		return true;
	}

	/// Checks whether the given point is inside this bounds.
	template<class T_>
	bool IsInside(const basic_Bounds<T_> &b, const basic_Point<T_> &p) {
		return p.X>=b.Left && p.Y>=b.Top && p.X<=b.Right && p.Y<=b.Bottom;
	}

	/// Translation moves the given bounds *by* the given amount
	template<class T_, class O_>
	void Translate(basic_Bounds<T_> &bounds, O_ x, O_ y) {
		bounds.Left  += x;
		bounds.Right += x;
		bounds.Top   += y;
		bounds.Bottom+= y;
	}

	/// Translation moves the given bounds *by* the given amount
	template<class T_>
	void Translate(basic_Bounds<T_> &bounds, const basic_Point<T_> &other) {
		bounds.Left  += other.X;
		bounds.Right += other.X;
		bounds.Top   += other.Y;
		bounds.Bottom+= other.Y;
	}

	/// Scales the given bounds by the given factor. Center of the bounds is used as origin
	template <class T_, class O_>
	void Scale(basic_Bounds<T_> &bounds, const O_ &size) {
		Float xc = ( bounds.Left + bounds.Right ) / Float(2);
		Float yc = ( bounds.Top  + bounds.Bottom) / Float(2);

		bounds.Left  = T_( (bounds.Left  - xc)*size + xc );
		bounds.Right = T_( (bounds.Right - xc)*size + xc );
		bounds.Top   = T_( (bounds.Top   - yc)*size + yc );
		bounds.Left  = T_( (bounds.Left  - yc)*size + yc );
	}

	/// Scales the given bounds by the given factors for x and y coordinates. Center of the bounds is used as origin
	template <class T_, class O_>
	void Scale(basic_Bounds<T_> &bounds, const O_ &sizex, const O_ &sizey) {
		Float xc = ( bounds.Left + bounds.Right ) / Float(2);
		Float yc = ( bounds.Top  + bounds.Bottom) / Float(2);

		bounds.Left  = T_( (bounds.Left  - xc)*sizex + xc );
		bounds.Right = T_( (bounds.Right - xc)*sizex + xc );
		bounds.Top   = T_( (bounds.Top   - yc)*sizey + yc );
		bounds.Left  = T_( (bounds.Left  - yc)*sizey + yc );
	}

	/// Scales the given bounds by the given factors for x and y coordinates. Center of the bounds is used as origin
	template <class T_, class O_>
	void Scale(basic_Bounds<T_> &bounds, const basic_Size<O_> &size) {
		Float xc = ( bounds.Left + bounds.Right ) / Float(2);
		Float yc = ( bounds.Top  + bounds.Bottom) / Float(2);

		bounds.Left  = T_( (bounds.Left  - xc)*size.Width  + xc );
		bounds.Right = T_( (bounds.Right - xc)*size.Width  + xc );
		bounds.Top   = T_( (bounds.Top   - yc)*size.Height + yc );
		bounds.Left  = T_( (bounds.Left  - yc)*size.Height + yc );
	}

	/// Scales the given bounds by the given factor, considering specified point
	/// as origin
	template <class T_, class O_>
	void Scale(basic_Bounds<T_> &bounds, const O_ &size, const basic_Point<T_> &origin) {
		bounds.Left  = T_( (bounds.Left  - origin.X)*size + origin.X );
		bounds.Right = T_( (bounds.Right - origin.X)*size + origin.X );
		bounds.Top   = T_( (bounds.Top   - origin.Y)*size + origin.Y );
		bounds.Left  = T_( (bounds.Left  - origin.Y)*size + origin.Y );
	}

	/// Scales the given bounds by the given factor, considering specified point
	/// as origin. This method variant is mostly there to allow scaling by Size.
	template <class T_, class O_>
	void Scale(basic_Bounds<T_> &bounds, const O_ &sizex, const O_ &sizey, const basic_Point<T_> &origin) {
		bounds.Left  = T_( (bounds.Left  - origin.X)*sizex + origin.X );
		bounds.Right = T_( (bounds.Right - origin.X)*sizex + origin.X );
		bounds.Top   = T_( (bounds.Top   - origin.Y)*sizey + origin.Y );
		bounds.Left  = T_( (bounds.Left  - origin.Y)*sizey + origin.Y );
	}

	/// Scales the given bounds by the given factor, considering specified point
	/// as origin.
	template <class T_, class O_>
	void Scale(basic_Bounds<T_> &bounds, const basic_Size<O_> &size, const basic_Point<T_> &origin) {
		bounds.Left  = T_( (bounds.Left  - origin.X)*size.Width  + origin.X );
		bounds.Right = T_( (bounds.Right - origin.X)*size.Width  + origin.X );
		bounds.Top   = T_( (bounds.Top   - origin.Y)*size.Height + origin.Y );
		bounds.Left  = T_( (bounds.Left  - origin.Y)*size.Height + origin.Y );
	}


	/// Rotates the given bounds by the given angle. Rotation is performed as if given bounds is a rectangle and
	/// the result is the bounds of this rotated rectangle. Center of the bounds is used as origin. 
	/// @param  bounds object to be rotated
	/// @param  angle is the Euler rotation angle in radians
	template<class T_>
	void Rotate(basic_Bounds<T_> &bounds, Float angle) {
		basic_Point<T_> cn = bounds.Center();
		basic_Point<T_> tl = bounds.TopLeft();
		basic_Point<T_> tr = bounds.TopRight();
		basic_Point<T_> bl = bounds.BottomLeft();
		basic_Point<T_> br = bounds.BottomRight();

		Rotate(tl, angle, cn);
		Rotate(tr, angle, cn);
		Rotate(bl, angle, cn);
		Rotate(br, angle, cn);

		bounds.Left  = std::min( std::min(tl.X, tr.X) , std::min(bl.X, br.X) );
		bounds.Right = std::max( std::max(tl.X, tr.X) , std::max(bl.X, br.X) );
		bounds.Top   = std::min( std::min(tl.Y, tr.Y) , std::min(bl.Y, br.Y) );
		bounds.Bottom= std::max( std::max(tl.Y, tr.Y) , std::max(bl.Y, br.Y) );
	}

	/// Rotates the given bounds by the given angle around the given origin.
	/// @param  bounds object to be rotated
	/// @param  angle is the Euler rotation angle in radians
	/// @param  origin of the rotation
	template<class T_>
	void Rotate(basic_Bounds<T_> &bounds, Float angle, const basic_Point<T_> &origin) {
		basic_Point<T_> tl = bounds.TopLeft();
		basic_Point<T_> tr = bounds.TopRight();
		basic_Point<T_> bl = bounds.BottomLeft();
		basic_Point<T_> br = bounds.BottomRight();

		Rotate(tl, angle, origin);
		Rotate(tr, angle, origin);
		Rotate(bl, angle, origin);
		Rotate(br, angle, origin);

		bounds.Left  = std::min( std::min(tl.X, tr.X) , std::min(bl.X, br.X) );
		bounds.Right = std::max( std::max(tl.X, tr.X) , std::max(bl.X, br.X) );
		bounds.Top   = std::min( std::min(tl.Y, tr.Y) , std::min(bl.Y, br.Y) );
		bounds.Bottom= std::max( std::max(tl.Y, tr.Y) , std::max(bl.Y, br.Y) );
	}

	/// Skews the given bounds with the given rate along X axis. Skew
	/// operation transforms objects in a way that it converts
	/// a rectangle to a parallelogram. Require normalized bounds.
	template <class T_, class O_>
	void SkewX(basic_Bounds<T_> &bounds, const O_ &rate) {
		Float yc = ( bounds.Top  + bounds.Bottom) / Float(2);

		if(rate>0) {
			bounds.Left  = T_( bounds.Left  + (bounds.Top   -yc)*rate );
			bounds.Right = T_( bounds.Right + (bounds.Bottom-yc)*rate );
		}
		else {
			bounds.Left  = T_( bounds.Left  + (bounds.Bottom-yc)*rate );
			bounds.Right = T_( bounds.Right + (bounds.Top   -yc)*rate );
		}
	}

	/// Skews the given bounds with the given rate along Y axis. Skew
	/// operation transforms objects in a way that it converts
	/// a rectangle to a parallelogram.
	template <class T_, class O_>
	void SkewY(basic_Bounds<T_> &bounds, const O_ &rate) {
		Float xc = ( bounds.Left + bounds.Right ) / Float(2);

		if(rate>0) {
			bounds.Top   = T_( bounds.Top   + (bounds.Left  -xc)*rate );
			bounds.Bottom= T_( bounds.Bottom+ (bounds.Right -xc)*rate );
		}													 
		else {												 
			bounds.Top   = T_( bounds.Top   + (bounds.Right -xc)*rate );
			bounds.Bottom= T_( bounds.Bottom+ (bounds.Left  -xc)*rate );
		}
	}

	/// Skews the given bounds with the given rate along X axis considering
	/// given bounds as the origin. Skew operation transforms objects in 
	/// a way that it converts a rectangle to a parallelogram.
	template <class T_, class O_>
	void SkewX(basic_Bounds<T_> &bounds, const O_ &rate, const basic_Point<T_> &origin) {
		if(rate>0) {
			bounds.Left  = T_( bounds.Left  + (bounds.Top   -origin.Y)*rate );
			bounds.Right = T_( bounds.Right + (bounds.Bottom-origin.Y)*rate );
		}
		else {
			bounds.Left  = T_( bounds.Left  + (bounds.Bottom-origin.Y)*rate );
			bounds.Right = T_( bounds.Right + (bounds.Top   -origin.Y)*rate );
		}
	}

	/// Skews the given bounds with the given rate along Y axis considering
	/// given bounds as the origin. Skew operation transforms objects in 
	/// a way that it converts a rectangle to a parallelogram.
	template <class T_, class O_>
	void SkewY(basic_Bounds<T_> &bounds, const O_ &rate, const basic_Point<T_> &origin) {
		if(rate>0) {
			bounds.Top   = T_( bounds.Top   + (bounds.Left  -origin.X)*rate );
			bounds.Bottom= T_( bounds.Bottom+ (bounds.Right -origin.X)*rate );
		}													 
		else {												 
			bounds.Top   = T_( bounds.Top   + (bounds.Right -origin.X)*rate );
			bounds.Bottom= T_( bounds.Bottom+ (bounds.Left  -origin.X)*rate );
		}
	}
	
	/// @see basic_Bounds
	typedef basic_Bounds<Float> Boundsf;

	/// @see basic_Bounds
	typedef basic_Bounds<int>   Bounds;

} }
