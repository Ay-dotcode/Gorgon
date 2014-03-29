/// @file contains point class.

#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <limits>
#include <cmath>

#include "../Types.h"

namespace Gorgon {
	/// This namespace contains geometric element classes
	namespace Geometry {

		template <class T_>
		class basic_Point {
		public:
			/// Default constructor, does not zero initialize point.
			basic_Point() { }

			/// Filling constructor
			basic_Point(T_ X, T_ Y) : X(X), Y(Y) { }

			/// Conversion from a different point type
			template <class U_>
			basic_Point(const basic_Point<U_> &point) : X((T_)point.X), Y((T_)point.Y) { }

			/// Assignment from a different point type
			template <class U_>
			basic_Point& operator =(const basic_Point<U_> &point) { 
				X=T_(point.X); 
				Y=T_(point.Y); 
				return *this; 
			}

			/// Calculates Euclidean distance from this point to the given target
			FloatingPoint Distance(const basic_Point &target) const {
				return std::sqrt( (FloatingPoint)(X-target.X)*(X-target.X) + (Y-target.Y)*(Y-target.Y) );
			}

			/// Calculates Euclidean distance from this point to origin
			FloatingPoint Distance() const {
				return std::sqrt( (FloatingPoint)(X*X) + (Y*Y) );
			}

			/// Subtracts another point from this one
			basic_Point operator - (const basic_Point &point) const {
				return basic_Point(X-point.X, Y-point.Y);
			}

			/// Negates this point
			basic_Point operator -() const {
				return basic_Point(-X, -Y);
			}

			/// Adds another point to this one and returns the result
			basic_Point operator + (const basic_Point &point) const {
				return basic_Point(X+point.X, Y+point.Y);
			}

			/// Multiply this point with a scalar value. This is effectively
			/// scales the point
			template <class U_>
			basic_Point operator * (U_ value) const {
				return basic_Point(X*value, Y*value);
			}

			/// Multiplies two points. This is essentially a dot product
			Float operator *(const basic_Point<T_> &value) const {
				return X*value.X + Y*value.Y;
			}

			/// Divides this point to a scalar value. This is effectively
			/// scales the point
			template <class U_>
			basic_Point operator / (U_ value) const {
				return basic_Point(X/value, Y/value);
			}

			/// Subtracts another point from this point. Result is assigned
			/// to this point
			basic_Point &operator -= (const basic_Point &point) {
				X-=point.X;
				Y-=point.Y;

				return *this;
			}

			/// Adds another point from this point. Result is assigned
			/// to this point
			basic_Point &operator += (const basic_Point &point) {
				X+=point.X;
				Y+=point.Y;

				return *this;
			}

			/// Scales this point
			template <class U_>
			basic_Point &operator *= (U_ value) {
				X*=value;
				Y*=value;

				return *this;
			}

			/// Scales this point
			template <class U_>
			basic_Point &operator /= (U_ value) {
				X/=value;
				Y/=value;

				return *this;
			}

			/// Calculates the angle of the line formed from the given point
			/// to this point.
			/// @param  origin is the starting point of the line
			/// @return angle in radians
			Float Angle(const basic_Point &origin) const {
				return atan2(Y-origin.Y, X-origin.X);
			}

			/// Calculates the angle of the line formed from the origin
			/// to this point.
			/// @return angle in radians
			Float Angle() const {
				return atan2(Y, X);
			}

			/// Calculates the slope of the line formed from the given point
			/// to this point.
			/// @param  origin is the starting point of the line
			Float Slope(const basic_Point &point) const {
				return (FloatingPoint)(Y-point.Y)/(X-point.X);
			}

			/// Calculates the slope of the line formed from the origin
			/// to this point.
			Float Slope() const {
				return (FloatingPoint)Y/X;
			}

			/// Compares two points
			bool Compare(const basic_Point &point) const {
				return X==point.X && Y==point.Y;
			}

			/// Compares two points
			bool operator == (const basic_Point &point) const {
				return Compare(point);
			}

			/// Compares two points
			bool operator !=(const basic_Point &point) const {
				return !Compare(point);
			}

			/// Moves this point to the given coordinates
			void Move(T_ x, T_ y) {
				X=x;
				Y=y;
			}

			template <class U_>
			static basic_Point CreateFrom(const basic_Point &point, const U_ &magnitute, Float angle) {
				return point+basic_Point(magnitute*std::cos(angle), magnitute*std::sin(angle));
			}

			operator std::string() const {
				std::ostringstream str;

#ifdef GORGON_TOSTRING_DECIMALPRECISION
				str.precision(GORGON_TOSTRING_DECIMALPRECISION);
#endif
				str<<"("<<X<<", "<<Y<<")";

				return str.str();
			}

			T_ X;
			T_ Y;
		};

		////Allows streaming of point. It converts point to string,
		/// every row is printed on a line enclosed in braces.
		template <class T_>
		std::ostream &operator << (std::ostream &out, const basic_Point<T_> &point) {
			out<<"("<<point.X<<", "<<point.Y<<")";

			return out;
		}
		template <class T_>
		std::istream &operator >> (std::istream &in, basic_Point<T_> &point) {
			while(in.peek()==' ' || in.peek()=='(')
				in.ignore(1);

			std::string s;
			std::stringstream ss;

			while(in.peek()!=',' && !in.eof())
				s.append(1, (char)in.get());

			in.ignore(1);

			ss.str(s);
			ss>>point.X;

			s="";

			while(in.peek()==' ' || in.peek()=='\t')
				in.ignore(1);

			while(in.peek()!=')' && in.peek()!=' ' && in.peek()!='\t' && in.peek()!='\n' && in.peek()!='\r' && !in.eof())
				s.append(1, in.get());


			ss.str(s);
			ss.clear();
			ss>>point.Y;

			if(in.peek()==')')
				in.ignore(1);

			return in;
		}


		/// Translation moves the given point *by* the given amount
		template<class T_>
		void Translate(basic_Point<T_> &point, T_ x, T_ y) {
			point.X+=x;
			point.Y+=y;
		}

		/// Translation moves the given point *by* the given amount
		template<class T_>
		void Translate(basic_Point<T_> &point, const basic_Point<T_> &other) {
			point.X+=other.X;
			point.Y+=other.Y;
		}

		/// Scales the given point by the given factor
		template <class T_, class U_>
		void Scale(basic_Point<T_> &point, const U_ &size) {
			point.X *= size;
			point.Y *= size;
		}

		/// Scales the given point by the given factors for x and y coordinates.
		template <class T_, class U_>
		void Scale(basic_Point<T_> &point, const U_ &sizex, const U_ &sizey) {
			point.X *= sizex;
			point.Y *= sizey;
		}

		/// Scales the given point by the given factor, considering given point
		/// as origin
		template <class T_, class U_>
		void Scale(basic_Point<T_> &point, const U_ &size, const basic_Point<T_> &origin) {
			point.X = (point.X-origin.X)*size+origin.X;
			point.Y = (point.Y-origin.Y)*size+origin.Y;
		}

		/// Scales the given point by the given factor, considering given point
		/// as origin. This method variant is mostly there to allow scaling by Size.
		template <class T_, class U_>
		void Scale(basic_Point<T_> &point, const U_ &sizex, const U_ &sizey, const basic_Point<T_> &origin) {
			point.X = (point.X-origin.X)*sizex+origin.X;
			point.Y = (point.Y-origin.Y)*sizey+origin.Y;
		}


		/// Rotates the given point by the given angle.
		/// @param  angle is the Euler rotation angle in radians
		template<class T_>
		void Rotate(basic_Point<T_> &point, Float angle) {
			T_ new_x;
			Float cosa=std::cos(angle), sina=std::sin(angle);
			new_x		= (T_)(point.X*cosa - point.Y*sina);
			point.Y     = (T_)(point.X*sina + point.Y*cosa);

			point.X     = (T_)(new_x);
		}

		/// Rotates the given point by the given angle around the given origin.
		/// @param  angle is the Euler rotation angle in radians
		template<class T_>
		void Rotate(basic_Point<T_> &point, Float angle, const basic_Point<T_> &origin) {
			Float cosa=std::cos(angle), sina=std::sin(angle);

			basic_Point<T_> temp=point-origin;

			point.X	  = (T_)(temp.X*cosa - temp.Y*sina);
			point.Y   = (T_)(temp.X*sina + temp.Y*cosa);

			point += origin;
		}

		/// Skews the given point with the given rate along X axis. Skew
		/// operation transforms objects in a way that it converts
		/// a rectangle to a parallelogram.
		template <class T_, class U_>
		void SkewX(basic_Point<T_> &point, const U_ &rate) {
			point.X += point.Y*rate;
		}

		/// Skews the given point with the given rate along Y axis. Skew
		/// operation transforms objects in a way that it converts
		/// a rectangle to a parallelogram.
		template <class T_, class U_>
		void SkewY(basic_Point<T_> &point, const U_ &rate) {
			point.Y += point.X*rate;
		}

		/// Skews the given point with the given rate along X axis considering
		/// given point as the origin. Skew operation transforms objects in 
		/// a way that it converts a rectangle to a parallelogram.
		template <class T_, class U_>
		void SkewX(basic_Point<T_> &point, const U_ &rate, const basic_Point &origin) {
			point.X += (point.Y-origin.Y)*rate;
		}

		/// Skews the given point with the given rate along Y axis considering
		/// given point as the origin. Skew operation transforms objects in 
		/// a way that it converts a rectangle to a parallelogram.
		template <class T_, class U_>
		void SkewY(basic_Point<T_> &point, const U_ &rate, const basic_Point &origin) {
			point.Y += (point.X-origin.X)*rate;
		}

		/// Reflects the given point along the X axis
		template<class T_>
		void ReflectX(basic_Point<T_> &point) {
			point.Y = -point.Y;
		}

		/// Reflects the given point along the Y axis
		template<class T_>
		void ReflectY(basic_Point<T_> &point) {
			point.X = -point.X;
		}

		/// Reflects the given point horizontally
		template<class T_>
		void HorizontalMirror(basic_Point<T_> &point) {
			ReflectY(point);
		}

		/// Reflects the given point vertically
		template<class T_>
		void VerticalMirror(basic_Point<T_> &point) {
			ReflectY(point);
		}

		/// Reflects the given point along the X axis considering given origin
		template<class T_>
		void ReflectX(basic_Point<T_> &point, const basic_Point &origin) {
			point.X = -point.X+origin.X*2;
		}

		/// Reflects the given point along the Y axis considering given origin
		template<class T_>
		void ReflectY(basic_Point<T_> &point, const basic_Point &origin) {
			point.Y = -point.Y+origin.Y*2;
		}

		/// Reflects the given point horizontally considering given origin
		template<class T_>
		void HorizontalMirror(basic_Point<T_> &point, const basic_Point &origin) {
			ReflectY(point, origin);
		}

		/// Reflects the given point vertically considering given origin
		template<class T_>
		void VerticalMirror(basic_Point<T_> &point, const basic_Point &origin) {
			ReflectX(point, origin);
		}

		typedef basic_Point<int  > Point;
		typedef basic_Point<Float> Pointf;

		inline Pointf Round(Pointf num) {
			return Pointf(std::floor(num.X+0.5f),std::floor(num.Y+0.5f)); 
		}

	} 
}
