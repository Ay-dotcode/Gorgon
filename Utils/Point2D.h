//DESCRIPTION
//	This file contains class Point which is compatible with 2 variable
//	point structures but offers extra functionality

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
#include <cmath>

#include "BasicGraphics.h"
#define Point2D_DecimalPoints	2


#ifdef GGE_XMLSERVICES
#include "../External/XML/tinyxml.h"
#endif
#include "BasicMath.h"


#define POINT2D_EXISTS

#ifdef VECTOR_EXISTS
#	error "Include Point2D before including Vector header to obtain extra functionality"
#endif



namespace gge { namespace utils {

	template <class T_>
	class basic_Point2D {
	public:
		T_ x,y;

		basic_Point2D() {}
		basic_Point2D(T_ X, T_ Y) : x(X), y(Y) {  }
		template <class U_>
		basic_Point2D(const basic_Point2D<U_> &point) : x((T_)point.x), y((T_)point.y) { }

		template <class U_>
		basic_Point2D& operator =(const basic_Point2D<U_> &point) { x=T_(point.x); y=T_(point.y); return *this; }

		FloatingPoint Distance(const basic_Point2D &point) {
			return std::sqrt( (FloatingPoint)(x-point.x)*(x-point.x) + (y-point.y)*(y-point.y) );
		}

		FloatingPoint Distance() const {
			return std::sqrt( (FloatingPoint)(x*x) + (y*y) );
		}

		basic_Point2D operator - (const basic_Point2D &point) const {
			return basic_Point2D(x-point.x, y-point.y);
		}

		basic_Point2D operator - (T_ value) const {
			return basic_Point2D(x-value, y-value);
		}

		basic_Point2D operator -() const {
			return basic_Point2D(-x, -y);
		}

		basic_Point2D operator + (const basic_Point2D &point) const {
			return basic_Point2D(x+point.x, y+point.y);
		}

		basic_Point2D operator + (T_ value) const {
			return basic_Point2D(x+value, y+value);
		}

		template <class U_>
		basic_Point2D operator * (U_ value) const {
			return basic_Point2D(x*value, y*value);
		}

		FloatingPoint operator *(const basic_Point2D<T_> &value) const {
			return x*value.x+y*value.y;
		}

		template <class U_>
		basic_Point2D operator / (U_ value) const {
			return basic_Point2D(x/value, y/value);
		}

		basic_Point2D &operator -= (const basic_Point2D &point) {
			x-=point.x;
			y-=point.y;
			return *this;
		}

		basic_Point2D &operator -= (T_ value) {
			x-=value;
			y-=value;
			return *this;
		}

		basic_Point2D &operator += (const basic_Point2D &point) {
			x+=point.x;
			y+=point.y;
			return *this;
		}

		basic_Point2D &operator += (T_ value) {
			x+=value;
			y+=value;
			return *this;
		}

		template <class U_>
		basic_Point2D &operator *= (U_ value) {
			x*=value;
			y*=value;
			return *this;
		}

		template <class U_>
		basic_Point2D &operator /= (U_ value) {
			x/=value;
			y/=value;
			return *this;
		}

		FloatingPoint Angle(const basic_Point2D &origin) const {
			return atan2(y-origin.y, x-origin.x);
		}

		FloatingPoint Angle() const {
			return atan2(y, x);
		}

		FloatingPoint Slope(const basic_Point2D &point) const {
			return (FloatingPoint)(y-point.y)/(x-point.x);
		}

		FloatingPoint Slope() const {
			return (FloatingPoint)y/x;
		}

		///!!!Do i have to change this??
		bool Compare(const basic_Point2D &point) const {
			T_ v=std::numeric_limits<T_>::epsilon();
			if(v<1)
				v*=5;

			//for double and float operations
			T_ dist1=x-point.x;
			T_ dist2=y-point.y;
			return  dist1<=v && dist1>=-v && dist2<=v && dist2>=-v;
		}

		//or at least this one
		bool operator == (const basic_Point2D &point) const {
			return Compare(point);
		}

		bool operator !=(const basic_Point2D &point) const {
			return !Compare(point);
		}

		basic_Point2D &operator = (const basic_Point2D &point) {
			x=point.x;
			y=point.y;

			return *this;
		}

#ifdef GRAPH_XMLSERVICES
		//only supported for scalar values
		std::string toXMLAttributes() const {
			std::ostringstream str;
			
			str.precision(Point2D_DecimalPoints);
			str<<"x=\""<<x<<"\" y=\""<<y<<"\"";

			return str.str();
		}

		//only supported for scalar values
		std::string toXMLTag() const {
			std::ostringstream str;
			
			str.precision(Point2D_DecimalPoints);
			str<<"<point x=\""<<x<<"\" y=\""<<y<<"\" />";

			return str.str();
		}

		//only supported for scalar values
		void fromXML(const xml::Element &node) {
			if(node.Attribute("x"))
				x=atof(*node.Attribute("x"));
			if(node.Attribute("y"))
				y=atof(*node.Attribute("y"));
		}
#endif

		void Translate(T_ x, T_ y) {
			this->x+=x;
			this->y+=y;
		}
		template <class U_>
		void Scale(U_ size) {
			x *= size;
			y *= size;
		}
		template <class U_>
		void Scale(U_ sizex, U_ sizey) {
			x *= sizex;
			y *= sizey;
		}
		template <class U_>
		void Scale(U_ size, const basic_Point2D &origin) {
			x = (x-origin.x)*size+origin.x;
			y = (y-origin.y)*size+origin.y;
		}
		template <class U_>
		void Scale(U_ sizex, U_ sizey, const basic_Point2D &origin) {
			x = (x-origin.x)*sizex+origin.x;
			y = (y-origin.y)*sizey+origin.y;
		}
		////no template on this since cos and cosf cannot be used as such
		void Rotate(FloatingPoint angle) {
			T_ new_x;
			FloatingPoint cosa=std::cos(angle), sina=std::sin(angle);
			new_x = (T_)(x*cosa - y*sina);
			y     = (T_)(x*sina + y*cosa);

			x     = (T_)(new_x);
		}
		void Rotate(FloatingPoint angle, const basic_Point2D &origin) {
			FloatingPoint cosa=std::cos(angle), sina=std::sin(angle);

			basic_Point2D temp=*this-origin;

			x	= (T_)(temp.x*cosa - temp.y*sina);
			y   = (T_)(temp.x*sina + temp.y*cosa);

			*this += origin;
		}
		template <class U_>
		void SkewX(U_ rate) {
			x += y*rate;
		}
		template <class U_>
		void SkewY(U_ rate) {
			y += x*rate;
		}
		template <class U_>
		void SkewX(U_ rate, const basic_Point2D &origin) {
			x += (y-origin.y)*rate;
		}
		template <class U_>
		void SkewY(U_ rate, const basic_Point2D &origin) {
			y += (x-origin.x)*rate;
		}
		void ReflectX() {
			x = -x;
		}
		void ReflectY() {
			y = -y;
		}
		void HorizontalMirror() {
			ReflectY();
		}
		void VerticleMirror() {
			ReflectX();
		}
		void ReflectX(const basic_Point2D &origin) {
			x = -x+origin.x*2;
		}
		void ReflectY(const basic_Point2D &origin) {
			y = -y+origin.y*2;
		}
		void HorizontalMirror(const basic_Point2D &origin) {
			ReflectY(origin);
		}
		void VerticleMirror(const basic_Point2D &origin) {
			ReflectX(origin);
		}

		template <class U_>
		static basic_Point2D CreateFrom(const basic_Point2D &point, U_ magnitute, FloatingPoint angle) {
			return point+basic_Point2D(magnitute*std::cos(angle), magnitute*std::sin(angle));
		}

		operator std::string() const {
			std::ostringstream str;

			str.precision(Point2D_DecimalPoints);
			str<<"("<<x<<", "<<y<<")";
			return str.str();
		}

		/*vector<_T, 2>
		vector<_T, 3>*/
	};

	////Allows streaming of point. It converts point to string,
	/// every row is printed on a line enclosed in braces.
	template <class T_>
	std::ostream &operator << (std::ostream &out, const basic_Point2D<T_> &point) {
		out<<"("<<point.x<<", "<<point.y<<")";

		return out;
	}
	template <class T_>
	std::istream &operator >> (std::istream &in, basic_Point2D<T_> &point) {
		while(in.peek()==' ' || in.peek()=='(')
			in.ignore(1);

		std::string s;
		std::stringstream ss;

		while(in.peek()!=',' && !in.eof())
			s.append(1, (char)in.get());

		in.ignore(1);

		ss.str(s);
		ss>>point.x;
			
		s="";

		while(in.peek()==' ' || in.peek()=='\t')
			in.ignore(1);

		while(in.peek()!=')' && in.peek()!=' ' && in.peek()!='\t' && in.peek()!='\n' && in.peek()!='\r' && !in.eof())
			s.append(1, in.get());


		ss.str(s);
		ss.clear();
		ss>>point.y;
		
		if(in.peek()==')')
			in.ignore(1);

		return in;
	}


	typedef basic_Point2D<FloatingPoint> Point2D;

	typedef basic_Point2D<int> Point;

} }
