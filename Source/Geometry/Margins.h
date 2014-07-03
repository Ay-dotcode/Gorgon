/// @file Margins.h contains Margins class

#pragma once

#include <iostream>
#include <ostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <limits>

#include "Point.h"
#include "Size.h"
#include "Bounds.h"

namespace Gorgon { namespace Geometry {
	
	/// This class defines margins of an object or an area. This class is designed to be used
	/// with Bounds object. Order of components are Left, Top, Right, Bottom. Negative margin
	/// values are allowed.
	template <class T_>
	class basic_Margins {
	public:
		
		/// Default constructor
		basic_Margins() {}
		
		/// Sets all margins to the given value. Intentionally left implicit as margins can
		/// be represented as a simple integer
		basic_Margins(T_ all) : Left(all), Right(all), Top(all), Bottom(all) { }
		
		/// Sets horizontal and vertical margins separately
		basic_Margins(T_ horizontal, T_ vertical) : Left(horizontal), Right(horizontal), 
		Top(vertical), Bottom(vertical) { }
		
		/// Sets all margins separately
		basic_Margins(T_ left, T_ top, T_ right, T_ bottom) : 
			Left(left), Right(right), Top(top), Bottom(bottom) { }

		/// Converts this object to a string.
		/// TODO
		operator std::string() const {
			std::ostringstream str;
			str<<"("<<Left<<", "<<Top<<", "<<Right<<", "<<Bottom<<")";

			return str.str();
		}

		/// Calculates and returns the total margins in X axis
		T_ TotalX() const  { return Right+Left; }
		
		/// Calculates and returns the total margins in Y axis
		T_ TotalY() const { return Bottom+Top;  }

		/// Calculates and returns the total margins in X axis
		T_ Horizontal() const  { return Right+Left; }
		
		/// Calculates and returns the total margins in Y axis
		T_ Vertical() const { return Bottom+Top;  }

		/// Adds two margins
		basic_Margins operator +(const basic_Margins &right) const {
			return basic_Margins(Left+right.Left, Top+right.Top, Right+right.Right, Bottom+right.Bottom);
		}

		/// Subtracts two margins
		basic_Margins operator -(const basic_Margins &right) const {
			return basic_Margins(Left-right.Left, Top-right.Top, Right-right.Right, Bottom-right.Bottom);
		}

		/// Adds a margins to this one
		basic_Margins &operator +=(const basic_Margins &right) {
			Left+=right.Left;
			Top+=right.Top;
			Right+=right.Right;
			Bottom+=right.Bottom;

			return *this;
		}

		/// Subtracts a margins from this one
		basic_Margins &operator -=(const basic_Margins &right) {
			Left-=right.Left;
			Top-=right.Top;
			Right-=right.Right;
			Bottom-=right.Bottom;

			return *this;
		}

		/// Compares two margins
		bool operator ==(const basic_Margins &margin) const {
			return Left==margin.Left && Right==margin.Right && Top==margin.Top && Bottom==margin.Bottom;
		}

		/// Compares two margins
		bool operator !=(const basic_Margins &margin) const {
			return Left!=margin.Left || Right!=margin.Right || Top!=margin.Top || Bottom!=margin.Bottom;
		}

		/// Adds an object to the left of this margins to create a new margins
		/// marking the used area. 
		/// @param  width of the object
		/// @param  margins to be applied to the object
		basic_Margins AddToLeft(T_ width,basic_Margins margins=0) {
			return basic_Margins(Left+margins.TotalX()+width, Top, Right, Bottom);
		}
		
		/// Adds an object to the top of this margins to create a new margins
		/// marking the used area. 
		/// @param  height of the object
		/// @param  margins to be applied to the object
		basic_Margins AddToTop(T_ height,const basic_Margins &margins=0) {
			return basic_Margins(Left, Top+margins.TotalY()+height, Right, Bottom);
		}

		/// Adds an object to the right of this margins to create a new margins
		/// marking the used area. 
		/// @param  width of the object
		/// @param  margins to be applied to the object
		basic_Margins AddToRight(T_ width,basic_Margins margins=0) {
			return basic_Margins(Left, Top, Right+margins.TotalX()+width, Bottom);
		}

		/// Adds an object to the bottom of this margins to create a new margins
		/// marking the used area. 
		/// @param  height of the object
		/// @param  margins to be applied to the object
		basic_Margins AddToBottom(T_ height,basic_Margins margins=0) {
			return basic_Margins(Left, Top, Right, Bottom+margins.TotalY()+height);
		}

		/// Top left coordinate of the object that will be placed within a <0:inf, 0:inf> bounds
		/// that has this margins.
		basic_Point<T_> TopLeft() const {
			return {Left, Top};
		}

		/// Left margin
		T_ Left;
		
		/// Top margin
		T_ Top;
		
		/// Right margin
		T_ Right;
		
		/// Bottom margin
		T_ Bottom;
	};

	/// Adds a margins object to a size structure, resultant size can contain previous size
	/// with the given margins
	template <typename T_, typename R_>
	basic_Size<T_> operator +(const basic_Size<T_> &s, const basic_Margins<R_> &m) {
		return basic_Size<T_>(s.Width+m.TotalX(), s.Height+m.TotalY());
	}

	/// Subtracts a margins from a size
	template <typename T_, typename R_>
	basic_Size<T_> operator -(const basic_Size<T_> &s, const basic_Margins<R_> &m) {
		return basic_Size<T_>(s.Width-m.TotalX(), s.Height-m.TotalY());
	}

	/// Subtracts two bounds to find marginal difference between them.
	template <typename T_, typename R_>
	basic_Margins<T_> operator -(const basic_Bounds<T_> &b1, const basic_Bounds<R_> &b2) {
		return basic_Margins<T_>(
			b2.Left-b1.Left, b2.Top-b1.Top, b1.Right-b2.Right, b1.Bottom-b2.Bottom
		);
	}

	/// Adds a margins object to a bounds
	template <typename T_, typename R_>
	basic_Bounds<T_> operator +(const basic_Bounds<T_> &b, const basic_Margins<R_> &m) {
		return basic_Bounds<T_>(
			b.Left-m.Left, b.Top-m.Top, b.Right+m.Right, b.Bottom+m.Bottom
			);
	}

	/// Removes a margins object from a bounds object, 
	template <typename T_, typename R_>
	basic_Bounds<T_> operator -(const basic_Bounds<T_> &b, const basic_Margins<R_> &m) {
		return basic_Bounds<T_>(
			b.Left+m.Left, b.Top+m.Top, b.Right-m.Right, b.Bottom-m.Bottom
		);
	}

	////Allows streaming of margins.
	template <class T_>
	std::ostream &operator << (std::ostream &out, const basic_Margins<T_> &margins) {
		out<<"("<<margins.Left<<", "<<margins.Right<<", "<<margins.Top<<", "<<margins.Bottom<<")";

		return out;
	}
	
	/// Allows margins to be read from a stream
	template <class T_>
	std::istream &operator >> (std::istream &in, basic_Margins<T_> &margins) {
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
		ss.clear();
		ss>>margins.Top;

		s="";

		while(in.peek()==' ' || in.peek()=='\t')
			in.ignore(1);

		while(in.peek()!=',' && !in.eof())
			s.append(1, (char)in.get());

		in.ignore(1);

		ss.str(s);
		ss.clear();
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


	typedef basic_Margins<int> Margins;
} }
