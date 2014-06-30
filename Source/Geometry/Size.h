/// @file Size.h contains the Size class

#pragma once

#include <iostream>
#include <ostream>
#include <string>
#include <iomanip>
#include <limits>
#include <sstream>

#include "../Types.h"
#include "Point.h"
#include "../String.h"

namespace Gorgon { namespace Geometry {

	/// This class represents a 2D geometric size. Although negative size is meaningless,
	/// this class allows all operations over negative sizes.
	template<class T_>
	class basic_Size {
	public:
		/// Default constructor. This constructor does **not** zero initialize 
		/// the object.
		basic_Size() { }

		/// Filling constructor. This variant assigns the given value to both dimensions,
		/// effectively creating a square.
		explicit basic_Size(const T_ &s) : Width(s), Height(s) { }

		/// Filling constructor
		basic_Size(const T_ &w, const T_ &h) : Width(w), Height(h) { }

		/// Converting constructor. Converts a different typed size object to this
		/// type.
		template <class O_>
		basic_Size(const basic_Size<O_> &size) : Width((T_)size.Width), Height((T_)size.Height) { }

		/// Converts a point to size object. The size a point represents is the size of the 
		/// rectangle that starts from origin to the given point.
		template <class O_>
		explicit basic_Size(const basic_Point<O_> &point) : Width((T_)point.X), Height((T_)point.Y) { }
		
		/// Conversion from string
		explicit basic_Size(const std::string &str) {
			auto s=str.begin();
			
			while(*s==' ' || *s=='\t') s++;
			
			if(*s=='(') s++;
			
			Width=String::To<T_>(&str[s-str.begin()]);
			
			while(*s!='x' && s!=str.end()) s++;
			
			if(*s=='x') s++;
			
			Height=String::To<T_>(&str[s-str.begin()]);
		}
			
		/// Converts this object to string.
		operator std::string() const {
			std::string ret;
			ret += String::From(Width);
			ret.push_back('x');
			ret += String::From(Height);
			
			return ret;
		}
		
			
		/// Properly parses given string into a size. Throws errors if the
		/// size is not well formed. Works only on types that can be parsed using
		/// strtod. Following error codes are reported by this parse function:
		///
		/// * *IllegalTokenError* **121001**: Illegal token while reading Width
		/// * *IllegalTokenError* **121002**: Illegal token while looking for width/height separator
		/// * *IllegalTokenError* **121003**: Illegal token while reading Height
		/// * *IllegalTokenError* **121004**: Illegal token(s) at the end
		static basic_Size Parse(const std::string &str) {
			basic_Size sz;
			
			auto s=str.begin();
			
			while(*s==' ' || *s=='\t') s++;
						
			char *endptr;
			sz.Width = T_(strtod(&*s, &endptr));
			if(endptr==&*s) {
				throw String::IllegalTokenError(0, 120001);
			}
			s+=endptr-&*s;
			
			while(*s==' ' || *s=='\t') s++;				
			if(*s!='x') {
				throw String::IllegalTokenError(s-str.begin(), 121002, std::string("Illegal token: ")+*s);
			}
			s++;
			
			sz.Height=T_(strtod(&*s, &endptr));			
			if(endptr==&*s) {
				throw String::IllegalTokenError(s-str.begin(), 121003);
			}
			s+=endptr-&*s;
			
			//eat white space + a single )
			while(*s==' ' || *s=='\t') s++;
			
			if(*s!=0) {
				throw String::IllegalTokenError(s-str.begin(), 121004, std::string("Illegal token: ")+*s);
			}
			
			return sz;
		}

		/// Converting assignment operator.
		template <class O_>
		basic_Size &operator =(const basic_Size<O_> &size) { 
			Width=size.Width; 
			Height=size.Height; 

			return *this; 
		}

		/// Converting assignment operator. The size a point represents is the size of the 
		/// rectangle that starts from origin to the given point.
		template <class O_>
		basic_Size &operator =(const basic_Point<O_> &point) { 
			Width=point.X;
			Height=point.Y; 

			return *this; 
		}

		/// Compares two size objects
		bool operator ==(const basic_Size  &size) const { 
			return Width==size.Width && Height==size.Height;
		}

		/// Compares two size objects
		bool operator !=(const basic_Size  &size) const { 
			return Width!=size.Width || Height!=size.Height;
		}

		/// Adds two size objects
		template<class O_>
		basic_Size operator +(const basic_Size<O_>  &size) const { 
			return{size.Width+Width, size.Height+Height};
		}

		/// Subtracts two size objects
		template<class O_>
		basic_Size operator -(const basic_Size<O_>  &size) const { 
			return{Width-size.Width, Height-size.Height};
		}

		/// Negation operator
		basic_Size operator -() const {
			return{-Width, -Height};
		}

		/// Adds the given size object to this size
		template<class O_>
		basic_Size &operator +=(const basic_Size<O_>  &size) { 
			Width=size.Width  +Width;
			Height=size.Height+Height;

			return *this;
		}

		/// Subtracts the given size object from this size
		template<class O_>
		basic_Size &operator -=(const basic_Size<O_>  &size) { 
			Width=Width-size.Width;
			Height=Height-size.Height;

			return *this;
		}

		/// Multiplies this size object with the given factor
		template<class _S>
		basic_Size operator *=(_S size) { 
			Width=size.Width  *Width;
			Height=size.Height*Height;

			return *this;
		}

		/// Divides this size object to the given factor
		template<class _S>
		basic_Size operator /=(_S size) { 
			Width/=size.Width;
			Height/=size.Height;

			return *this;
		}

		/// Converts this size object to a point. Conversion is performed
		/// in a manner that the resultant point is the far corner of a
		/// rectangle that is placed at origin and the size of this object.
		operator basic_Point<T_>() const {
			return{Width, Height};
		}

		/// Returns the number of fully encompassed cells. For instance,
		/// a 3.2x2.2 size object has 6 cells.
		T_ Cells() const { 
			return std::floor(Width)*std::floor(Height); 
		}

		/// Returns the exact area of the rectangle has the size of this object
		T_ Area() const { 
			return Width*Height; 
		}
		
		/// Returns the maximum representable size. This function requires T_ to be
		/// standard arithmetic type
		static basic_Size Max() {
			static_assert(std::numeric_limits<T_>::is_specialized, "Max function can only be used with "
				"arithmetic types that have numeric_limits specialized");
			return {std::numeric_limits<T_>::max(), std::numeric_limits<T_>::max()};
		}

		/// Width of this size object
		T_ Width;

		/// Height of this size object
		T_ Height;
	};

	/// Multiplies a size with a scalar, effectively resizing it.
	template<class T_, class O_>
	basic_Size<T_> operator *(const basic_Size<T_> &size, const O_ &factor) {
		return{size.Width*factor, size.Height*factor};
	}

	/// Multiplies a size with a scalar, effectively resizing it.
	template<class T_, class O_>
	basic_Size<T_> operator *(const O_ &factor, const basic_Size<T_> &size) {
		return operator*(size, factor);
	}

	/// Divides a size with a scalar, effectively resizing it.
	template<class T_, class O_>
	basic_Size<T_> operator /(const basic_Size<T_> &size, const O_ &factor) {
		return{size.Width/factor, size.Height/factor};
	}

	/// Divides a size with a scalar, effectively resizing it.
	template<class T_, class O_>
	basic_Size<T_> operator /(const O_ &factor, const basic_Size<T_> &size) {
		return operator/(size, factor);
	}

	/// Writes the given size object to the stream. Width and Height components of
	/// size objects are separated by an x
	template<class T_>
	static std::ostream &operator <<(std::ostream &out, const basic_Size<T_> &size) {
		out<<size.Width<<"x"<<size.Height;

		return out;
	}

	/// Reads a size object from a stream. Width and Height components should be
	/// separated by an x. 
	template <class T_>
	std::istream &operator >> (std::istream &in, basic_Size<T_> &size) {
		while(in.peek()==' ' || in.peek()=='\t')
			in.ignore(1);

		std::string s;

		while(in.peek()!='x' && !in.eof())
			s.append(1, (char)in.get());

		if(in.eof()) {
			in.setstate(in.failbit);
			return in;
		}
		in.ignore(1);

		size.Width=String::To<T_>(s);

		s="";

		while(in.peek()==' ' || in.peek()=='\t')
			in.ignore(1);

		while(in.peek()!=' ' && in.peek()!='\t' && in.peek()!='\n' && in.peek()!='\r' && !in.eof())
			s.append(1, in.get());

		size.Height=String::To<T_>(s);

		return in;
	}

	/// Allows multiplication of point with a size object
	template<class T_, class O_>
	auto operator *(const basic_Point<T_> &l, const basic_Size<O_> &r) -> basic_Point<decltype(l.X*r.Width)> {
		return{l.X*r.Width, l.Y*r.Height};
	}

	/// Allows division of point with a size object
	template<class T_, class O_>
	auto operator /(const basic_Point<T_> &l, const basic_Size<O_> &r) -> basic_Point<decltype(l.X*r.Width)> {
		return{l.X/r.Width, l.Y/r.Height};
	}

	/// Scales the given point by the given factor
	template <class T_, class O_>
	void Scale(basic_Point<T_> &point, const basic_Size<O_> &size) {
		point.X = T_(point.X*size.Width);
		point.Y = T_(point.Y*size.Height);
	}

	/// Scales the given size by the given factor
	template <class T_, class O_>
	void Scale(basic_Size<T_> &l, const O_ &size) {
		l.X = T_(l.X*size);
		l.Y = T_(l.X*size);
	}

	/// Scales the given size by the given factors for x and y coordinates.
	template <class T_, class O_>
	void Scale(basic_Size<T_> &l, const O_ &sizex, const O_ &sizey) {
		l.X = T_(l.X*sizex);
		l.Y = T_(l.Y*sizey);
	}

	/// Scales the given l by the given factor
	template <class T_, class O_>
	void Scale(basic_Size<T_> &l, const basic_Size<O_> &size) {
		l.X = T_(l.X*size.Width);
		l.Y = T_(l.Y*size.Height);
	}


	/// @see basic_Size
	typedef basic_Size<int>   Size;

	/// @see basic_Size
	typedef basic_Size<Float> Sizef;

} }
