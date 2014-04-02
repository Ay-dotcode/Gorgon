/// @file Point.h contains point class.

#pragma once

#include <iostream>
#include <string>
#include <iomanip>
#include <limits>
#include <cmath>
#include <sstream>

#include "../Types.h"
#include "../String.h"

namespace Gorgon {
	/// This namespace contains geometric element classes
	namespace Geometry {

		/// This class represents a 2D point.
		template <class T_>
		class basic_Point {
		public:
			/// Default constructor, does not zero initialize point.
			basic_Point() { }

			/// Filling constructor
			basic_Point(T_ X, T_ Y) : X(X), Y(Y) { }

			/// Conversion from a different point type
			template <class O_>
			basic_Point(const basic_Point<O_> &point) : X((T_)point.X), Y((T_)point.Y) { }
			
			/// Conversion from string
			explicit basic_Point(const std::string &str) {
				auto s=str.begin();
				
				while(*s==' ' || *s=='\t') s++;
				
				if(*s=='(') s++;
				
				X=String::To<T_>(&str[s-str.begin()]);
				
				while(*s!=',' && s!=str.end()) s++;
				
				if(*s==',') s++;
				
				Y=String::To<T_>(&str[s-str.begin()]);
			}

			/// Assignment from a different point type
			template <class O_>
			basic_Point& operator =(const basic_Point<O_> &point) { 
				X=T_(point.X); 
				Y=T_(point.Y); 
				return *this; 
			}
			
			/// Converts this object to string.
			operator std::string() const {
				std::string ret;
				ret.push_back('(');
				ret += String::From(X);
				ret.push_back(',');
				ret.push_back(' ');
				ret += String::From(Y);
				ret.push_back(')');
				
				return ret;
			}
			
			/// Properly parses given string into a point. Throws errors if the
			/// point is not well formed. Works only on types that can be parsed using
			/// strtod. Following error codes are reported by this parse function:
			///
			/// * *IllegalTokenError* **111001**: Illegal token while reading X coordinate
			/// * *IllegalTokenError* **111002**: Illegal token while looking for coordinate separator
			/// * *IllegalTokenError* **111003**: Illegal token while reading Y coordinate
			/// * *IllegalTokenError* **111004**: Illegal token(s) at the end
			/// * *IllegalTokenError* **111005**: Unmatched parenthesis
			/// * *ParseError* **110006**: Missing parenthesis, only if require_parenthesis
			///   is set to true
			static basic_Point Parse(const std::string &str, bool require_parenthesis=false) {
				basic_Point p;
				
				auto s=str.begin();
				
				int par=0;
				
				while(*s==' ' || *s=='\t') s++;
				
				if(*s=='(') {
					par=s-str.begin();
					s++;
				}
				
				if(!par && require_parenthesis) {
					throw String::ParseError(110006, "Missing parenthesis");
				}
				
				char *endptr;
				p.X = T_(strtod(&*s, &endptr));
				if(endptr==&*s) {
					throw String::IllegalTokenError(0, 110001);
				}
				s+=endptr-&*s;
				
				while(*s==' ' || *s=='\t') s++;				
				if(*s!=',') {
					throw String::IllegalTokenError(s-str.begin(), 111002, std::string("Illegal token: ")+*s);
				}
				s++;
				
				p.Y=T_(strtod(&*s, &endptr));			
				if(endptr==&*s) {
					throw String::IllegalTokenError(s-str.begin(), 111003);
				}
				s+=endptr-&*s;
				
				//eat white space + a single )
				while(*s==' ' || *s=='\t') s++;
				if(*s==')') {
					if(par) {
						s++;
						par=false;
					}
					else {
						throw String::IllegalTokenError(s-str.begin(), 111005, "Unmatched parenthesis");
					}
				}
				while(*s==' ' || *s=='\t') s++;
				
				if(*s!=0) {
					throw String::IllegalTokenError(s-str.begin(), 111004, std::string("Illegal token: ")+*s);
				}
				
				if(par) {
					throw String::IllegalTokenError(par, 111005, "Unmatched parenthesis");
				}
				
				return p;
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
			template <class O_>
			basic_Point operator * (O_ value) const {
				return basic_Point(T_(X*value), T_(Y*value));
			}

			/// Multiplies two points. This is essentially a dot product
			Float operator *(const basic_Point<T_> &value) const {
				return X*value.X + Y*value.Y;
			}

			/// Divides this point to a scalar value. This is effectively
			/// scales the point
			template <class O_>
			basic_Point operator / (O_ value) const {
				return basic_Point(T_(X/value), T_(Y/value));
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
			template <class O_>
			basic_Point &operator *= (O_ value) {
				X=T_(X*value);
				Y=T_(Y*value);

				return *this;
			}

			/// Scales this point
			template <class O_>
			basic_Point &operator /= (O_ value) {
				X=T_(X/value);
				Y=T_(Y/value);

				return *this;
			}

			/// Calculates Euclidean distance from this point to the given target
			Float Distance(const basic_Point &target) const {
				return std::sqrt( (Float)(X-target.X)*(X-target.X) + (Y-target.Y)*(Y-target.Y) );
			}

			/// Calculates Euclidean distance from this point to origin
			Float Distance() const {
				return std::sqrt( (Float)(X*X) + (Y*Y) );
			}

			/// Calculates the angle of the line formed from the given point
			/// to this point.
			/// @param  origin is the starting point of the line
			/// @return angle in radians
			Float Angle(const basic_Point &origin) const {
				return (Float)atan2(Y-origin.Y, X-origin.X);
			}

			/// Calculates the angle of the line formed from the origin
			/// to this point.
			/// @return angle in radians
			Float Angle() const {
				return (Float)atan2(Y, X);
			}

			/// Calculates the slope of the line formed from the given point
			/// to this point.
			/// @param  origin is the starting point of the line
			Float Slope(const basic_Point &point) const {
				return (Float)(Y-point.Y)/(X-point.X);
			}

			/// Calculates the slope of the line formed from the origin
			/// to this point.
			Float Slope() const {
				return (Float)Y/X;
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

			template <class O1_>
			static basic_Point CreateFrom(const basic_Point<O1_> &point, Float magnitute, Float angle) {
				return basic_Point(T_(point.X+magnitute*std::cos(angle)), T_(point.Y+magnitute*std::sin(angle)));
			}

			T_ X;
			T_ Y;
		};

		/// Allows streaming of point. A point will be printed inside parenthesis with
		/// a comma separating X and Y values.
		template <class T_>
		std::ostream &operator << (std::ostream &out, const basic_Point<T_> &point) {
			out<<"("<<point.X<<", "<<point.Y<<")";

			return out;
		}


		/// Reads a point from a stream. Requires comma in between x and y. parentheses 
		/// are optional. They wont even be matched to each other. A point entry should
		/// not contain space before closing parenthesis otherwise, parenthesis will not
		/// be extracted.
		template <class T_>
		std::istream &operator >> (std::istream &in, basic_Point<T_> &point) {
			while(in.peek()==' ' || in.peek()=='\t' || in.peek()=='(')
				in.ignore(1);

			std::string s;
			std::stringstream ss;

			while(in.peek()!=',' && !in.eof())
				s.push_back((char)in.get());

			if(in.eof()) {
				in.setstate(in.failbit);
				return in;
			}
			in.ignore(1);

			point.X = String::To<T_>(s);

			s="";

			while(in.peek()==' ' || in.peek()=='\t')
				in.ignore(1);

			while(in.peek()!=')' && in.peek()!=' ' && in.peek()!='\t' && in.peek()!='\n' && in.peek()!='\r' && !in.eof())
				s.push_back(in.get());

			point.Y = String::To<T_>(s);

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
		template <class T_, class O_>
		void Scale(basic_Point<T_> &point, const O_ &size) {
			point.X = T_(point.X*size);
			point.Y = T_(point.X*size);
		}

		/// Scales the given point by the given factors for x and y coordinates.
		template <class T_, class O_>
		void Scale(basic_Point<T_> &point, const O_ &sizex, const O_ &sizey) {
			point.X = T_(point.X*sizex);
			point.Y = T_(point.Y*sizey);
		}

		/// Scales the given point by the given factor, considering given point
		/// as origin
		template <class T_, class O_>
		void Scale(basic_Point<T_> &point, const O_ &size, const basic_Point<T_> &origin) {
			point.X = T_((point.X-origin.X)*size+origin.X);
			point.Y = T_((point.Y-origin.Y)*size+origin.Y);
		}

		/// Scales the given point by the given factor, considering given point
		/// as origin.
		template <class T_, class O_>
		void Scale(basic_Point<T_> &point, const O_ &sizex, const O_ &sizey, const basic_Point<T_> &origin) {
			point.X = T_((point.X-origin.X)*sizex+origin.X);
			point.Y = T_((point.Y-origin.Y)*sizey+origin.Y);
		}


		/// Rotates the given point by the given angle.
		/// @param  angle is the Euler rotation angle in radians
		template<class T_>
		void Rotate(basic_Point<T_> &point, Float angle) {
			T_ new_x;
			Float cosa=std::cos(angle), sina=std::sin(angle);
			new_x		= T_(point.X*cosa - point.Y*sina);
			point.Y     = T_(point.X*sina + point.Y*cosa);

			point.X     = new_x;
		}

		/// Rotates the given point by the given angle around the given origin.
		/// @param  angle is the Euler rotation angle in radians
		template<class T_>
		void Rotate(basic_Point<T_> &point, Float angle, const basic_Point<T_> &origin) {
			Float cosa=std::cos(angle), sina=std::sin(angle);

			basic_Point<T_> temp=point-origin;

			point.X	  = T_(temp.X*cosa - temp.Y*sina);
			point.Y   = T_(temp.X*sina + temp.Y*cosa);

			point += origin;
		}

		/// Skews the given point with the given rate along X axis. Skew
		/// operation transforms objects in a way that it converts
		/// a rectangle to a parallelogram.
		template <class T_, class O_>
		void SkewX(basic_Point<T_> &point, const O_ &rate) {
			point.X += T_(point.Y*rate);
		}

		/// Skews the given point with the given rate along Y axis. Skew
		/// operation transforms objects in a way that it converts
		/// a rectangle to a parallelogram.
		template <class T_, class O_>
		void SkewY(basic_Point<T_> &point, const O_ &rate) {
			point.Y += T_(point.X*rate);
		}

		/// Skews the given point with the given rate along X axis considering
		/// given point as the origin. Skew operation transforms objects in 
		/// a way that it converts a rectangle to a parallelogram.
		template <class T_, class O_>
		void SkewX(basic_Point<T_> &point, const O_ &rate, const basic_Point<T_> &origin) {
			point.X += T_((point.Y-origin.Y)*rate);
		}

		/// Skews the given point with the given rate along Y axis considering
		/// given point as the origin. Skew operation transforms objects in 
		/// a way that it converts a rectangle to a parallelogram.
		template <class T_, class O_>
		void SkewY(basic_Point<T_> &point, const O_ &rate, const basic_Point<T_> &origin) {
			point.Y += T_((point.X-origin.X)*rate);
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
		void ReflectX(basic_Point<T_> &point, const basic_Point<T_> &origin) {
			point.X = -point.X+origin.X*2;
		}

		/// Reflects the given point along the Y axis considering given origin
		template<class T_>
		void ReflectY(basic_Point<T_> &point, const basic_Point<T_> &origin) {
			point.Y = -point.Y+origin.Y*2;
		}

		/// Reflects the given point horizontally considering given origin
		template<class T_>
		void HorizontalMirror(basic_Point<T_> &point, const basic_Point<T_> &origin) {
			ReflectY(point, origin);
		}

		/// Reflects the given point vertically considering given origin
		template<class T_>
		void VerticalMirror(basic_Point<T_> &point, const basic_Point<T_> &origin) {
			ReflectX(point, origin);
		}

		/// @see basic_Point
		typedef basic_Point<int  > Point;

		/// @see basic_Point
		typedef basic_Point<Float> Pointf;

		inline Pointf Round(Pointf num) {
			return Pointf(std::floor(num.X+Float(0.5)),std::floor(num.Y+Float(0.5))); 
		}

	} 
}
