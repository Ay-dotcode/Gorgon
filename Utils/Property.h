//DESCRIPTION
//	Property classes allows property access much like vb and c#.
//	There are different property types for different tasks.
//	You should use the property type that suits your needs.
//	If necessary you should derive your own property object.
//	But do not forget to add assignment operators, as they
//	are not inherited.

//REQUIRES:
//	std::string

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

#define PROPERTY_DEFINED

#include "UtilsBase.h"
#include <string>
#include <iostream>
#include <stdexcept>

namespace gge { namespace utils {

	//This is generic property that can be set and retrieved
	// good for enums mostly, its ok to use with POD structs
	// for direct assignment but better not to use it with 
	// complex data types.
	template<class C_, class T_>
	class Property {
	public:
		typedef T_(C_::*Getter)() const;
		typedef void (C_::*Setter)(const T_&);

	protected:
		C_		&Object;
		Getter	getter;
		Setter	setter;

	public:
		Property(C_ *Object, Getter getter, Setter setter) : Object(*Object), getter(getter), setter(setter) 
		{ }

		operator T_() { 
			return (Object.*getter)(); 
		}

		operator const T_() const { 
			return (Object.*getter)(); 
		}

		Property &operator =(const T_ &value) { 
			(Object.*setter)(value);

			return *this;
		}

		template <class O_>
		Property &operator =(const O_ &value) { 
			(Object.*setter)(value);

			return *this;
		}

		T_ operator *() const {
			return (Object.*getter)();
		}

		template <class AC_>
		Property &operator =(const Property<AC_, T_> &prop) {
			(Object.*setter)((T_)prop);

			return *this;
		}

		bool operator ==(const T_ &v) const {
			return (Object.*getter)()==v;
		}

		bool operator !=(const T_ &v) const {
			return (Object.*getter)()!=v;
		}
	};


	//THIS PART IS INCOMPLETE
	
	
	
	//Supports arithmetic operators
	// including +, * ..., +=, ... 
	// ==, <, >
	// but not &, &&
	// float, int, double, math/Complex
	template<class C_, class T_>
	class NumericProperty : public Property<C_, T_> {
	public:
		NumericProperty(C_ *Object, typename Property<C_,T_>::Getter getter, typename Property<C_,T_>::Setter setter) : Property<C_, T_>(Object, getter, setter) 
		{ }

		NumericProperty &operator =(const T_ &value) { 
			(this->Object.*this->setter)(value);

			return *this;
		}

		template <class AC_, class O_>
		NumericProperty &operator =(const Property<AC_, O_> &prop) {
			(this->Object.*this->setter)((T_)prop);

			return *this;
		}

		T_ operator ++(int) {
			T_ v=(this->Object.*this->getter)();
			T_ k=v;
			k++;
			(this->Object.*this->setter)(k);
			return v;
		}

		T_ operator --(int) {
			T_ v=(this->Object.*this->getter)();
			T_ k=v;
			k--;
			(this->Object.*this->setter)(k);
			return v;
		}

		T_ operator ++() {
			T_ v=(this->Object.*this->getter)();
			v++;
			(this->Object.*this->setter)(v);
			return v;
		}

		T_ operator --() {
			T_ v=(this->Object.*this->getter)();
			v--;
			(this->Object.*this->setter)(v);
			return v;
		}

		T_ operator +(const T_ &v) const {
			return (this->Object.*this->getter)() + v;
		}

		T_ operator -(const T_ &v) const {
			return (this->Object.*this->getter)() - v;
		}

		T_ operator *(const T_ &v) const {
			return (this->Object.*this->getter)() * v;
		}

		T_ operator /(const T_ &v) const {
			return (this->Object.*this->getter)() / v;
		}

		T_ operator +=(const T_ &v) {
			(this->Object.*this->setter)((this->Object.*this->getter)()+v);
			return (this->Object.*this->getter)();
		}

		T_ operator -=(const T_ &v) {
			(this->Object.*this->setter)((this->Object.*this->getter)()-v);
			return (this->Object.*this->getter)();
		}

		T_ operator *=(const T_ &v) {
			(this->Object.*this->setter)((this->Object.*this->getter)()*v);
			return (this->Object.*this->getter)();
		}

		T_ operator /=(const T_ &v) {
			(this->Object.*this->setter)((this->Object.*this->getter)()/v);
			return (this->Object.*this->getter)();
		}

		bool operator >(const T_ &v) const {
			return (this->Object.*this->getter)()>v;
		}

		bool operator <(const T_ &v) const {
			return (this->Object.*this->getter)()<v;
		}

		bool operator >=(const T_ &v) const {
			return (this->Object.*this->getter)()>=v;
		}

		bool operator <=(const T_ &v) const {
			return (this->Object.*this->getter)()<=v;
		}
	};

	//Supports logic operators
	// &&, ||, !, and equalities ==, !=
	// bool mostly
	template<class C_, class T_=bool>
	class BooleanProperty : public Property<C_, T_> {
	public:
		BooleanProperty(C_ *Object, typename Property<C_, T_>::Getter getter, typename Property<C_, T_>::Setter setter) : Property<C_, T_>(Object, getter, setter) 
		{ }

		template <class O_>
		BooleanProperty &operator =(const O_ &value) { 
			(this->Object.*this->setter)(value);

			return *this;
		}

		template <class AC_>
		BooleanProperty &operator =(const Property<AC_, T_> &prop) {
			(this->Object.*this->setter)((T_)prop);

			return *this;
		}

		bool operator &&(const T_ &v) const {
			return (this->Object.*this->getter)() && v;
		}

		bool operator ||(const T_ &v) const {
			return (this->Object.*this->getter)() || v;
		}

		bool operator !() const {
			return !(this->Object.*this->getter)();
		}
	};

	//should allow everything that numeric
	// supports and |, &, ~, ...
	// unsigned int, GGE/Byte
	template<class C_, class T_=Byte>
	class BinaryProperty : public NumericProperty<C_,T_> {
	public:
		BinaryProperty(C_ *Object, typename Property<C_, T_>::Getter getter, typename Property<C_, T_>::Setter setter) : NumericProperty<C_,T_>(Object, getter, setter) 
		{ }

		template <class O_>
		BinaryProperty &operator =(const O_ &value) { 
			(this->Object.*this->setter)(T_(value));

			return *this;
		}

		template <class AC_>
		BinaryProperty &operator =(const Property<AC_, T_> &prop) {
			(this->Object.*this->setter)((T_)prop);

			return *this;
		}

		//NOT REQUIRED
		//T_ operator | (const T_ &v) const {
		//	return (Object.*getter)() | v;
		//}

		//T_ operator & (const T_ &v) const {
		//	return (Object.*getter)() & v;
		//}

		T_ operator |= (const T_ &v) {
			(this->Object.*this->setter)((this->Object.*this->getter)() | v);
			return (this->Object.*this->getter)();
		}

		T_ operator &= (const T_ &v) {
			(this->Object.*this->setter)((this->Object.*this->getter)() & v);
			return (this->Object.*this->getter)();
		}

		T_ operator ^= (const T_ &v) {
			(this->Object.*this->setter)((this->Object.*this->getter)() ^ v);
			return (this->Object.*this->getter)();
		}
	};

	//Object property allows the consumers of the property
	//to be able to access objects member functions and 
	//variables in const manner
	// Should allow non-const versions, should have a version allows null
	template<class C_, class T_>
	class ObjectProperty : public Property<C_, T_> {
		ObjectProperty(C_ *Object, typename Property<C_, T_>::Getter getter, typename Property<C_, T_>::Setter setter) : Property<C_, T_>(Object, getter, setter) 
		{ }

		template <class O_>
		ObjectProperty &operator =(const O_ &value) { 
			(this->Object.*this->setter)(value);

			return *this;
		}

		template <class AC_>
		ObjectProperty &operator =(const Property<AC_, T_> &prop) {
			(this->Object.*this->setter)((T_)prop);

			return *this;
		}

		const T_ &operator *() const {
			return (this->Object.*this->getter)();
		}

		const T_ *operator ->() const {
			return &(this->Object.*this->getter)();
		}
	};

	//Object property allows the consumers of the property
	//to be able to access objects member functions and 
	//variables in
	// This version allows modification of it, however 
	// modifications are not propagated to the object itself
	template<class C_, class T_>
	class MutableObjectProperty : public Property<C_, T_> {
		MutableObjectProperty(C_ *Object, typename Property<C_, T_>::Getter getter, typename Property<C_, T_>::Setter setter) : Property<C_, T_>(Object, getter, setter) 
		{ }

		template <class O_>
		MutableObjectProperty &operator =(const O_ &value) { 
			(this->Object.*this->setter)(value);

			return *this;
		}

		template <class AC_>
		MutableObjectProperty &operator =(const Property<AC_, T_> &prop) {
			(this->Object.*this->setter)((T_)prop);

			return *this;
		}

		T_ &operator *() {
			return (this->Object.*this->getter)();
		}

		T_ *operator ->() {
			return &(this->Object.*this->getter)();
		}

		const T_ &operator *() const {
			return (this->Object.*this->getter)();
		}

		const T_ *operator ->() const {
			return &(this->Object.*this->getter)();
		}
	};

	//Reference object
	template<class C_, class T_>
	class ReferenceProperty {
		typedef T_*(C_::*Getter)() const;
		typedef void (C_::*Setter)(T_*);

	protected:
		C_		&Object;
		Getter	getter;
		Setter	setter;

	public:
		ReferenceProperty(C_ *Object, Getter getter, Setter setter) : Object(*Object), getter(getter), setter(setter) 
		{ }

		operator T_ *() {
			return (this->Object.*this->getter)();
		}

		operator T_ *() const {
			return (this->Object.*this->getter)();
		}

		ReferenceProperty &operator =(T_ *value) { 
			(this->Object.*this->setter)(value);

			return *this;
		}

		ReferenceProperty &operator =(T_ &value) { 
			(this->Object.*this->setter)(&value);

			return *this;
		}

		bool operator ==(const T_ &v) const {
			return (this->Object.*this->getter)()==v;
		}

		bool operator !=(const T_ &v) const {
			return (this->Object.*this->getter)()!=v;
		}

		T_ *operator ->() {
			return (this->Object.*this->getter)();
		}

		T_ *GetPtr() {
			return (this->Object.*this->getter)();
		}

		T_ &Get() {
			T_ *o=(this->Object.*this->getter)();
			if(o)
				return *o;
			else
				throw std::runtime_error("Property is empty");
		}
	};

	//should support everything that string class
	// supports +, +=, length()
	// string
	template<class C_, class T_=std::string>
	class TextualProperty : public Property<C_, T_> {
	public:
		TextualProperty(C_ *Object, typename Property<C_, T_>::Getter getter, typename Property<C_, T_>::Setter setter) : Property<C_,T_>(Object, getter, setter) 
		{ }

		template <class O_>
		TextualProperty &operator =(const O_ &value) { 
			(this->Object.*this->setter)(T_(value));

			return *this;
		}

		template <class AC_>
		TextualProperty &operator =(const Property<AC_, T_> &prop) {
			(this->Object.*this->setter)((T_)prop);

			return *this;
		}

		T_ operator +(const T_ &v) {
			return (this->Object.*this->getter)()+v;
		}

		void operator +=(const T_ &v) {
			return (this->Object.*this->setter)((this->Object.*this->getter)()+v);
		}

		int length() const {
			return (this->Object.*this->getter)().length();
		}

		const char *c_str() const {
			return (this->Object.*this->getter)().c_str();
		}

		T_ substr(typename T_::size_type off=0U, typename T_::size_type len=T_::npos) const {
			return (this->Object.*this->getter)().substr(off,len);
		}

		typename T_::size_type find ( const T_& str, typename T_::size_type pos = 0 ) const {
			return (this->Object.*this->getter)().find(str, pos);
		}
		typename T_::size_type find ( const char* s, typename T_::size_type pos, typename T_::size_type n ) const {
			return (this->Object.*this->getter)().find(s, pos, n);
		}
		typename T_::size_type find ( const char* s, typename T_::size_type pos = 0 ) const {
			return (this->Object.*this->getter)().find(s, pos);
		}
		typename T_::size_type find ( char c, typename T_::size_type pos = 0 ) const {
			return (this->Object.*this->getter)().find(c, pos);
		}

		typename T_::size_type rfind ( const T_& str, typename T_::size_type pos = T_::npos ) const {
			return (this->Object.*this->getter)().rfind(str, pos);
		}
		typename T_::size_type rfind ( const char* s, typename T_::size_type pos, typename T_::size_type n ) const {
			return (this->Object.*this->getter)().rfind(s, pos, n);
		}
		typename T_::size_type rfind ( const char* s, typename T_::size_type pos = T_::npos ) const {
			return (this->Object.*this->getter)().rfind(s, pos);
		}
		typename T_::size_type rfind ( char c, typename T_::size_type pos = T_::npos ) const {
			return (this->Object.*this->getter)().rfind(c, pos);
		}

		typename T_::size_type find_first_of ( const T_& str, typename T_::size_type pos = T_::npos ) const {
			return (this->Object.*this->getter)().find_first_of(str, pos);
		}
		typename T_::size_type find_first_of ( const char* s, typename T_::size_type pos, typename T_::size_type n ) const {
			return (this->Object.*this->getter)().find_first_of(s, pos, n);
		}
		typename T_::size_type find_first_of ( const char* s, typename T_::size_type pos = T_::npos ) const {
			return (this->Object.*this->getter)().find_first_of(s, pos);
		}
		typename T_::size_type find_first_of ( char c, typename T_::size_type pos = T_::npos ) const {
			return (this->Object.*this->getter)().find_first_of(c, pos);
		}

		typename T_::size_type find_last_of ( const T_& str, typename T_::size_type pos = T_::npos ) const {
			return (this->Object.*this->getter)().find_last_of(str, pos);
		}
		typename T_::size_type find_last_of ( const char* s, typename T_::size_type pos, typename T_::size_type n ) const {
			return (this->Object.*this->getter)().find_last_of(s, pos, n);
		}
		typename T_::size_type find_last_of ( const char* s, typename T_::size_type pos = T_::npos ) const {
			return (this->Object.*this->getter)().find_last_of(s, pos);
		}
		typename T_::size_type find_last_of ( char c, typename T_::size_type pos = T_::npos ) const {
			return (this->Object.*this->getter)().find_last_of(c, pos);
		}

		typename T_::size_type find_first_not_of ( const T_& str, typename T_::size_type pos = T_::npos ) const {
			return (this->Object.*this->getter)().find_first_not_of(str, pos);
		}
		typename T_::size_type find_first_not_of ( const char* s, typename T_::size_type pos, typename T_::size_type n ) const {
			return (this->Object.*this->getter)().find_first_not_of(s, pos, n);
		}
		typename T_::size_type find_first_not_of ( const char* s, typename T_::size_type pos = T_::npos ) const {
			return (this->Object.*this->getter)().find_first_not_of(s, pos);
		}
		typename T_::size_type find_first_not_of ( char c, typename T_::size_type pos = T_::npos ) const {
			return (this->Object.*this->getter)().find_first_not_of(c, pos);
		}

		typename T_::size_type find_last_not_of ( const T_& str, typename T_::size_type pos = T_::npos ) const {
			return (this->Object.*this->getter)().find_last_not_of(str, pos);
		}
		typename T_::size_type find_last_not_of ( const char* s, typename T_::size_type pos, typename T_::size_type n ) const {
			return (this->Object.*this->getter)().find_last_not_of(s, pos, n);
		}
		typename T_::size_type find_last_not_of ( const char* s, typename T_::size_type pos = T_::npos ) const {
			return (this->Object.*this->getter)().find_last_not_of(s, pos);
		}
		typename T_::size_type find_last_not_of ( char c, typename T_::size_type pos = T_::npos ) const {
			return (this->Object.*this->getter)().find_last_not_of(c, pos);
		}

		/*
		char &operator[] (typename T_::size_type pos) {
			return (Object.*getter)()[pos];
		}
		*/

		const char &operator[] (typename T_::size_type pos) const {
			return (this->Object.*this->getter)()[pos];
		}
		
		void clear() {
			T_ s=(this->Object.*this->getter)();
			s.clear();
			(this->Object.*this->setter)(s);
		}

		TextualProperty &append(const T_ &str) {
			T_ s=(this->Object.*this->getter)();
			s.append(str);
			(this->Object.*this->setter)(s);

			return *this;
		}

		TextualProperty & append(const T_ &str, typename T_::size_type pos, typename T_::size_type n) {
			T_ s=(this->Object.*this->getter)();
			s.append(str, pos, n);
			(this->Object.*this->setter)(s);

			return *this;
		}

		TextualProperty &append(const char *str, typename T_::size_type n) {
			T_ s=(this->Object.*this->getter)();
			s.append(str, n);
			(this->Object.*this->setter)(s);

			return *this;
		}

		TextualProperty &append(const char *str) {
			T_ s=(this->Object.*this->getter)();
			s.append(str);
			(this->Object.*this->setter)(s);

			return *this;
		}

		TextualProperty &append(typename T_::size_type n, char c) {
			T_ s=(this->Object.*this->getter)();
			s.append(n, c);
			(this->Object.*this->setter)(s);

			return *this;
		}

		template <class InputIterator>
		TextualProperty& append ( InputIterator first, InputIterator last ) {
			T_ s=(this->Object.*this->getter)();
			s.append<InputIterator>(first, last);
			(this->Object.*this->setter)(s);

			return *this;
		}

		TextualProperty &erase(typename T_::size_type pos = 0, typename T_::size_type n = T_::npos) {
			T_ s=(this->Object.*this->getter)();
			s.erase(pos, n);
			(this->Object.*this->setter)(s);

			return *this;
		}

		TextualProperty &insert(typename T_::size_type pos, const T_ str) {
			T_ s=(this->Object.*this->getter)();
			s.insert(pos, str);
			(this->Object.*this->setter)(s);

			return *this;
		}

		TextualProperty &insert(typename T_::size_type pos1, const T_ str, typename T_::size_type pos2, typename T_::size_type n = T_::npos) {
			T_ s=(this->Object.*this->getter)();
			s.erase(pos1, str, pos2, n);
			(this->Object.*this->setter)(s);

			return *this;
		}

		TextualProperty &insert(typename T_::size_type pos, const char *str, typename T_::size_type n) {
			T_ s=(this->Object.*this->getter)();
			s.insert(pos, str, n);
			(this->Object.*this->setter)(s);

			return *this;
		}

		TextualProperty &insert(typename T_::size_type pos, const char *str) {
			T_ s=(this->Object.*this->getter)();
			s.insert(pos, str);
			(this->Object.*this->setter)(s);

			return *this;
		}

		TextualProperty &insert(typename T_::size_type pos, typename T_::size_type n, char c) {
			T_ s=(this->Object.*this->getter)();
			s.insert(pos, n, c);
			(this->Object.*this->setter)(s);

			return *this;
		}

		


	};

	template <class C_,class T_>
	inline std::ostream &operator <<(std::ostream &out, const TextualProperty<C_,T_> &p) {
		out<<(T_)p;

		return out;
	}

	template <class C_,class T_>
	inline std::ostream &operator <<(std::ostream &out, const NumericProperty<C_,T_> &p) {
		out<<(T_)p;

		return out;
	}

	template <class C_,class T_>
	inline std::istream &operator >>(std::istream &in, TextualProperty<C_,T_> &p) {
		T_ t;
		in>>t;
		p=t;

		return in;
	}

	template <class C_,class T_>
	inline std::istream &operator >>(std::istream &in, NumericProperty<C_,T_> &p) {
		T_ t;
		in>>t;
		p=t;

		return in;
	}

#define	INIT_PROPERTY(classtype, name) name(this, &classtype::get##name, &classtype::set##name)

//Good for testing nothing else, you probably should use normal variables if you need something like this.
#define MAP_PROPERTY(type, name, variable) type get##name() const { return variable; } void set##name(const type &v) { variable=v; } type variable;

} }
