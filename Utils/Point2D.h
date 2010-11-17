#pragma once

#include <iostream>
#include <ostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <limits>
#include <cmath>

#ifdef GGE_GRAPHICS
	#include "../../Graphics/Graphics.h"
#else
	#include "BasicGraphics.h"
	#define Point2D_DecimalPoints	2
#endif


#ifdef GGE_XMLSERVICES
#include "../External/XmlParser/xmlParser.h"
#endif


#define POINT2D_EXISTS

#ifdef VECTOR_EXISTS
#error "Include Point2D before including Vector header to obtain extra functionality"
#endif



namespace gge {

	template <class _T>
	class basic_Point2D {
	public:
		_T x,y;

		basic_Point2D() {}
		basic_Point2D(_T X, _T Y) : x(X), y(Y) {  }
		template <class _U>
		basic_Point2D(const basic_Point2D<_U> &point) : x(point.x), y(point.y) { }

		template <class _U>
		basic_Point2D& operator =(const basic_Point2D<_U> &point) { x=point.x; y=point.y; return *this; }

		graphtype Distance(basic_Point2D &point) {
			return std::sqrt( (graphtype)(x-point.x)*(x-point.x) + (y-point.y)*(y-point.y) );
		}

		graphtype Distance() {
			return std::sqrt( (graphtype)(x*x) + (y*y) );
		}

		basic_Point2D operator - (basic_Point2D &point) {
			return basic_Point2D(x-point.x, y-point.y);
		}

		basic_Point2D operator - (_T value) {
			return basic_Point2D(x-value, y-value);
		}

		basic_Point2D operator + (basic_Point2D &point) {
			return basic_Point2D(x+point.x, y+point.y);
		}

		basic_Point2D operator + (_T value) {
			return basic_Point2D(x+value, y+value);
		}

		template <class _U>
		basic_Point2D operator * (_U value) {
			return basic_Point2D(x*value, y*value);
		}

		graphtype operator *(basic_Point2D<_T> &value) {
			return x*value.x+y*value.y;
		}

		template <class _U>
		basic_Point2D operator / (_U value) {
			return basic_Point2D(x/value, y/value);
		}

		basic_Point2D &operator -= (basic_Point2D &point) {
			x-=point.x;
			y-=point.y;
			return *this;
		}

		basic_Point2D &operator -= (_T value) {
			x-=value;
			y-=value;
			return *this;
		}

		basic_Point2D &operator += (basic_Point2D &point) {
			x+=point.x;
			y+=point.y;
			return *this;
		}

		basic_Point2D &operator += (_T value) {
			x+=value;
			y+=value;
			return *this;
		}

		template <class _U>
		basic_Point2D &operator *= (_U value) {
			x*=value;
			y*=value;
			return *this;
		}

		template <class _U>
		basic_Point2D &operator /= (_U value) {
			x/=value;
			y/=value;
			return *this;
		}

		graphtype Angle(basic_Point2D &point) {
			graphtype slope=Slope(point);
			if(x<point.x)
				return std::atan(slope)+PI;
			else if(y<point.y)
				return std::atan(slope)+PI*2;
			else
				return std::atan(slope);
		}

		graphtype Angle() {
			graphtype slope=Slope();
			if(x<0)
				return std::atan(slope)+PI;
			else if(y<0)
				return std::atan(slope)+PI*2;
			else
				return std::atan(slope);
		}

		graphtype Slope(basic_Point2D &point) {
			return (graphtype)(y-point.y)/(x-point.x);
		}

		graphtype Slope() {
			return (graphtype)y/x;
		}

		bool Compare(basic_Point2D &point) {
			_T v=std::numeric_limits<_T>::epsilon();
			if(v<1)
				v*=5;

			//for double and float operations
			_T dist1=x-point.x;
			_T dist2=y-point.y;
			return  dist1<=v && dist1>=-v && dist2<=v && dist2>=-v;
		}

		bool operator == (basic_Point2D &point) {
			return Compare(point);
		}

		bool operator !=(basic_Point2D &point) {
			return !Compare(point);
		}

		basic_Point2D &operator = (basic_Point2D &point) {
			x=point.x;
			y=point.y;

			return *this;
		}

#ifdef GRAPH_XMLSERVICES
		//only supported for scalar values
		std::string toXMLAttributes() {
			std::ostringstream str;
			
			str.precision(Point2D_DecimalPoints);
			str<<"x=\""<<x<<"\" y=\""<<y<<"\"";

			return str.str();
		}

		//only supported for scalar values
		std::string toXMLTag() {
			std::ostringstream str;
			
			str.precision(Point2D_DecimalPoints);
			str<<"<point x=\""<<x<<"\" y=\""<<y<<"\" />";

			return str.str();
		}

		//only supported for scalar values
		void fromXML(XMLNode node) {
			if(node.getAttribute("x", 0))
				x=atof(node.getAttribute("x", 0));
			if(node.getAttribute("y", 0))
				y=atof(node.getAttribute("y", 0));
		}
#endif

		void Translate(_T x, _T y) {
			this->x+=x;
			this->y+=y;
		}
		template <class _U>
		void Scale(_U size) {
			x *= size;
			y *= size;
		}
		template <class _U>
		void Scale(_U sizex, _U sizey) {
			x *= sizex;
			y *= sizey;
		}
		template <class _U>
		void Scale(_U size, basic_Point2D origin) {
			x = (x-origin.x)*size+origin.x;
			y = (y-origin.y)*size+origin.y;
		}
		template <class _U>
		void Scale(_U sizex, _U sizey, basic_Point2D origin) {
			x = (x-origin.x)*sizex+origin.x;
			y = (y-origin.y)*sizey+origin.y;
		}
		////no template on this since cos and cosf cannot be used as such
		void Rotate(graphtype angle) {
			_T new_x;
			graphtype cosa=cosfn(angle), sina=sinfn(angle);
			new_x = x*cosa - y*sina;
			y     = x*sina + y*cosa;

			x     = new_x;
		}
		void Rotate(graphtype angle, basic_Point2D origin) {
			graphtype cosa=cosfn(angle), sina=sinfn(angle);

			basic_Point2D temp=*this-origin;

			x	= temp.x*cosa - temp.y*sina;
			y   = temp.x*sina + temp.y*cosa;

			*this += origin;
		}
		template <class _U>
		void SkewX(_U rate) {
			x += y*rate;
		}
		template <class _U>
		void SkewY(_U rate) {
			y += x*rate;
		}
		template <class _U>
		void SkewX(_U rate, basic_Point2D origin) {
			x += (y-origin.y)*rate;
		}
		template <class _U>
		void SkewY(_U rate, basic_Point2D origin) {
			y += (x-origin.x)*rate;
		}
		void ReflectX() {
			y = -y;
		}
		void ReflectY() {
			x = -x;
		}
		void HorizontalMirror() {
			ReflectY();
		}
		void VerticleMirror() {
			ReflectX();
		}
		void ReflectX(basic_Point2D origin) {
			y = -y+origin.y*2;
		}
		void ReflectY(basic_Point2D origin) {
			x = -x+origin.x*2;
		}
		void HorizontalMirror(basic_Point2D origin) {
			ReflectY(origin);
		}
		void VerticleMirror(basic_Point2D origin) {
			ReflectX(origin);
		}
		/*operator _T() {
			return Distance();
		}*/

		template <class U_>
		static basic_Point2D CreateFrom(basic_Point2D point, U_ magnitute, graphtype angle) {
			return point+basic_Point2D(magnitute*std::cos(angle), magnitute*std::sin(angle));
		}

		operator std::string() {
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
	template <class _T>
	std::ostream &operator << (std::ostream &out, basic_Point2D<_T> &point) {
		out<<"("<<point.x<<", "<<point.y<<")";

		return out;
	}


	////Adds the textual form of the point to another string.
	template <class _T>
	std::string &operator + (std::string &out, basic_Point2D<_T> &point) {
		return string+(string)point;
	}


	typedef basic_Point2D<graphtype> Point2D;

	typedef basic_Point2D<int> Point;

}