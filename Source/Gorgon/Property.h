#pragma once

/// Property classes allows property access much like vb and c#.
/// There are different property types for different tasks.
/// You should use the property type that suits your needs.
/// If necessary you should derive your own property object.
/// But do not forget to add assignment operators, as they
/// are not inherited.

#include "Types.h"
#include <string>
#include <iostream>
#include <stdexcept>

namespace Gorgon {

	/// This is generic property that can be set and retrieved
	/// good for enums mostly, its ok to use with POD structs
	/// for direct assignment but better not to use it with 
	/// complex data types.
	template<class C_, class T_, T_(C_::*Getter_)() const, void(C_::*Setter_)(T_)>
	class Property {
	public:

	protected:
		C_		&Object;

	public:
		Property(C_ *Object) : Object(*Object)
		{ }
		
		Property(C_ &Object) : Object(Object)
		{ }

		operator T_() { 
			return (Object.*Getter_)(); 
		}

		operator const T_() const { 
			return (Object.*Getter_)(); 
		}

		T_ Get() const {
			return (Object.*Getter_)();
		}

		void Set(const T_ &value) {
			(Object.*Setter_)(value);
		}

		Property &operator =(const T_ &value) { 
			(Object.*Setter_)(value);

			return *this;
		}

		template <class O_>
		Property &operator =(const O_ &value) { 
			(Object.*Setter_)(value);

			return *this;
		}

		T_ operator *() const {
			return (Object.*Getter_)();
		}

		template <class AC_, T_(AC_::*G_)() const, void(AC_::*S_)(T_)>
		Property &operator =(const Property<AC_, T_, G_, S_> &prop) {
			(Object.*Setter_)((T_)prop);

			return *this;
		}

		bool operator ==(const T_ &v) const {
			return (Object.*Getter_)()==v;
		}

		bool operator !=(const T_ &v) const {
			return (Object.*Getter_)()!=v;
		}
	};


	
	/// Supports arithmetic operators
	/// including +, * ..., +=, ... 
	/// ==, <, >
	/// but not &, &&
	/// float, int, double, math/Complex
	template<class C_, class T_, T_(C_::*Getter_)() const, void(C_::*Setter_)(T_)>
	class NumericProperty : public Property<C_, T_, Getter_, Setter_> {
	public:
		NumericProperty(C_ *Object) : Property<C_, T_, Getter_, Setter_>(Object) 
		{ }

		NumericProperty &operator =(const T_ &value) { 
			(this->Object.*this->Setter_)(value);

			return *this;
		}

        template<class AC_, class O_, O_(C_::*G_)() const, void(C_::*S_)(O_)>
		NumericProperty &operator =(const Property<AC_, O_, G_, S_> &prop) {
			(this->Object.*this->Setter_)((T_)(O_)prop);

			return *this;
		}

		T_ operator ++(int) {
			T_ v=(this->Object.*this->Getter_)();
			T_ k=v;
			k++;
			(this->Object.*this->Setter_)(k);
			return v;
		}

		T_ operator --(int) {
			T_ v=(this->Object.*this->Getter_)();
			T_ k=v;
			k--;
			(this->Object.*this->Setter_)(k);
			return v;
		}

		T_ operator ++() {
			T_ v=(this->Object.*this->Getter_)();
			v++;
			(this->Object.*this->Setter_)(v);
			return v;
		}

		T_ operator --() {
			T_ v=(this->Object.*this->Getter_)();
			v--;
			(this->Object.*this->Setter_)(v);
			return v;
		}

		T_ operator +(const T_ &v) const {
			return (this->Object.*this->Getter_)() + v;
		}

		T_ operator -(const T_ &v) const {
			return (this->Object.*this->Getter_)() - v;
		}

		T_ operator *(const T_ &v) const {
			return (this->Object.*this->Getter_)() * v;
		}

		T_ operator /(const T_ &v) const {
			return (this->Object.*this->Getter_)() / v;
		}

		T_ operator +=(const T_ &v) {
			(this->Object.*this->Setter_)((this->Object.*this->Getter_)()+v);
			return (this->Object.*this->Getter_)();
		}

		T_ operator -=(const T_ &v) {
			(this->Object.*this->Setter_)((this->Object.*this->Getter_)()-v);
			return (this->Object.*this->Getter_)();
		}

		T_ operator *=(const T_ &v) {
			(this->Object.*this->Setter_)((this->Object.*this->Getter_)()*v);
			return (this->Object.*this->Getter_)();
		}

		T_ operator /=(const T_ &v) {
			(this->Object.*this->Setter_)((this->Object.*this->Getter_)()/v);
			return (this->Object.*this->Getter_)();
		}

		bool operator >(const T_ &v) const {
			return (this->Object.*this->Getter_)()>v;
		}

		bool operator <(const T_ &v) const {
			return (this->Object.*this->Getter_)()<v;
		}

		bool operator >=(const T_ &v) const {
			return (this->Object.*this->Getter_)()>=v;
		}

		bool operator <=(const T_ &v) const {
			return (this->Object.*this->Getter_)()<=v;
		}
	};

	/// Supports logic operators. Mostly designed to be used with bool
	/// &&, ||, !, and equalities ==, !=
	template<class C_, class T_, T_(C_::*Getter_)() const, void(C_::*Setter_)(T_)>
	class BooleanProperty : public Property<C_, T_, Getter_, Setter_> {
	public:
		BooleanProperty(C_ *Object) : Property<C_, T_, Getter_, Setter_>(Object) 
		{ }

		template <class O_>
		BooleanProperty &operator =(const O_ &value) { 
			(this->Object.*this->Setter_)(value);

			return *this;
		}

        template <class AC_, class O_, O_(C_::*G_)() const, void(C_::*S_)(O_)>
		BooleanProperty &operator =(const Property<AC_, O_, G_, S_> &prop) {
			(this->Object.*this->Setter_)((T_)(O_)prop);

			return *this;
		}

		bool operator &&(const T_ &v) const {
			return (this->Object.*this->Getter_)() && v;
		}

		bool operator ||(const T_ &v) const {
			return (this->Object.*this->Getter_)() || v;
		}

		bool operator !() const {
			return !(this->Object.*this->Getter_)();
		}
	};

	/// Supports all operators that the numeric property supports.
	/// Additionally supports and |, &, ~, ...
	/// Use with unsigned int, Gorgon::Byte
	template<class C_, class T_=Byte>
	class BinaryProperty : public NumericProperty<C_,T_> {
	public:
		BinaryProperty(C_ *Object, typename Property<C_, T_>::Getter Getter_, typename Property<C_, T_>::Setter Setter_) : NumericProperty<C_,T_>(Object, Getter_, Setter_) 
		{ }

		template <class O_>
		BinaryProperty &operator =(const O_ &value) { 
			(this->Object.*this->Setter_)(T_(value));

			return *this;
		}

		template <class AC_>
		BinaryProperty &operator =(const Property<AC_, T_> &prop) {
			(this->Object.*this->Setter_)((T_)prop);

			return *this;
		}

		//NOT REQUIRED
		//T_ operator | (const T_ &v) const {
		//	return (Object.*Getter_)() | v;
		//}

		//T_ operator & (const T_ &v) const {
		//	return (Object.*Getter_)() & v;
		//}

		T_ operator |= (const T_ &v) {
			(this->Object.*this->Setter_)((this->Object.*this->Getter_)() | v);
			return (this->Object.*this->Getter_)();
		}

		T_ operator &= (const T_ &v) {
			(this->Object.*this->Setter_)((this->Object.*this->Getter_)() & v);
			return (this->Object.*this->Getter_)();
		}

		T_ operator ^= (const T_ &v) {
			(this->Object.*this->Setter_)((this->Object.*this->Getter_)() ^ v);
			return (this->Object.*this->Getter_)();
		}
	};

	//Object property allows the consumers of the property
	//to be able to access objects member functions and 
	//variables in const manner
	// Should allow non-const versions, should have a version allows null
	template<class C_, class T_, T_(C_::*Getter_)() const, void(C_::*Setter_)(T_)>
	class ObjectProperty : public Property<C_, T_, Getter_, Setter_> {
		ObjectProperty(C_ *Object, typename Property<C_, T_>::Getter Getter_, typename Property<C_, T_>::Setter Setter_) : Property<C_, T_>(Object, Getter_, Setter_) 
		{ }

		template <class O_>
		ObjectProperty &operator =(const O_ &value) { 
			(this->Object.*this->Setter_)(value);

			return *this;
		}

		template <class AC_>
		ObjectProperty &operator =(const Property<AC_, T_> &prop) {
			(this->Object.*this->Setter_)((T_)prop);

			return *this;
		}

		const T_ &operator *() const {
			return (this->Object.*this->Getter_)();
		}

		const T_ *operator ->() const {
			return &(this->Object.*this->Getter_)();
		}
	};

	//Object property allows the consumers of the property
	//to be able to access objects member functions and 
	//variables in
	// This version allows modification of it, however 
	// modifications are not propagated to the object itself
	template<class C_, class T_, T_(C_::*Getter_)() const, void(C_::*Setter_)(T_)>
	class MutableObjectProperty : public Property<C_, T_, Getter_, Setter_> {
		MutableObjectProperty(C_ *Object, typename Property<C_, T_>::Getter Getter_, typename Property<C_, T_>::Setter Setter_) : Property<C_, T_>(Object, Getter_, Setter_) 
		{ }

		template <class O_>
		MutableObjectProperty &operator =(const O_ &value) { 
			(this->Object.*this->Setter_)(value);

			return *this;
		}

		template <class AC_>
		MutableObjectProperty &operator =(const Property<AC_, T_> &prop) {
			(this->Object.*this->Setter_)((T_)prop);

			return *this;
		}

		T_ &operator *() {
			return (this->Object.*this->Getter_)();
		}

		T_ *operator ->() {
			return &(this->Object.*this->Getter_)();
		}

		const T_ &operator *() const {
			return (this->Object.*this->Getter_)();
		}

		const T_ *operator ->() const {
			return &(this->Object.*this->Getter_)();
		}
	};

	//Reference object
	template<class C_, class T_, T_(C_::*Getter_)() const, void(C_::*Setter_)(T_)>
	class ReferenceProperty {
		typedef T_*(C_::*Getter)() const;
		typedef void (C_::*Setter)(T_*);

	protected:
		C_		&Object;
		Getter	Getter_;
		Setter	Setter_;

	public:
		ReferenceProperty(C_ *Object, Getter Getter_, Setter Setter_) : Object(*Object), Getter_(Getter_), Setter_(Setter_) 
		{ }

		operator T_ *() {
			return (this->Object.*this->Getter_)();
		}

		operator T_ *() const {
			return (this->Object.*this->Getter_)();
		}

		ReferenceProperty &operator =(T_ *value) { 
			(this->Object.*this->Setter_)(value);

			return *this;
		}

		ReferenceProperty &operator =(T_ &value) { 
			(this->Object.*this->Setter_)(&value);

			return *this;
		}

		bool operator ==(const T_ &v) const {
			return (this->Object.*this->Getter_)()==v;
		}

		bool operator !=(const T_ &v) const {
			return (this->Object.*this->Getter_)()!=v;
		}

		T_ *operator ->() {
			return (this->Object.*this->Getter_)();
		}

		T_ *GetPtr() {
			return (this->Object.*this->Getter_)();
		}

		T_ &Get() {
			T_ *o=(this->Object.*this->Getter_)();
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
	class TextualProperty : public Property<C_, T_, Getter_, Setter_> {
	public:
		TextualProperty(C_ *Object, typename Property<C_, T_>::Getter Getter_, typename Property<C_, T_>::Setter Setter_) : Property<C_,T_>(Object, Getter_, Setter_) 
		{ }

		template <class O_>
		TextualProperty &operator =(const O_ &value) { 
			(this->Object.*this->Setter_)(T_(value));

			return *this;
		}

		template <class AC_>
		TextualProperty &operator =(const Property<AC_, T_> &prop) {
			(this->Object.*this->Setter_)((T_)prop);

			return *this;
		}

		void operator +=(const T_ &v) {
			return (this->Object.*this->Setter_)((this->Object.*this->Getter_)()+v);
		}

		int length() const {
			return (this->Object.*this->Getter_)().length();
		}

		const char *c_str() const {
			return (this->Object.*this->Getter_)().c_str();
		}

		T_ substr(typename T_::size_type off=0U, typename T_::size_type len=T_::npos) const {
			return (this->Object.*this->Getter_)().substr(off,len);
		}

		typename T_::size_type find ( const T_& str, typename T_::size_type pos = 0 ) const {
			return (this->Object.*this->Getter_)().find(str, pos);
		}
		typename T_::size_type find ( const char* s, typename T_::size_type pos, typename T_::size_type n ) const {
			return (this->Object.*this->Getter_)().find(s, pos, n);
		}
		typename T_::size_type find ( const char* s, typename T_::size_type pos = 0 ) const {
			return (this->Object.*this->Getter_)().find(s, pos);
		}
		typename T_::size_type find ( char c, typename T_::size_type pos = 0 ) const {
			return (this->Object.*this->Getter_)().find(c, pos);
		}

		typename T_::size_type rfind ( const T_& str, typename T_::size_type pos = T_::npos ) const {
			return (this->Object.*this->Getter_)().rfind(str, pos);
		}
		typename T_::size_type rfind ( const char* s, typename T_::size_type pos, typename T_::size_type n ) const {
			return (this->Object.*this->Getter_)().rfind(s, pos, n);
		}
		typename T_::size_type rfind ( const char* s, typename T_::size_type pos = T_::npos ) const {
			return (this->Object.*this->Getter_)().rfind(s, pos);
		}
		typename T_::size_type rfind ( char c, typename T_::size_type pos = T_::npos ) const {
			return (this->Object.*this->Getter_)().rfind(c, pos);
		}

		typename T_::size_type find_first_of ( const T_& str, typename T_::size_type pos = T_::npos ) const {
			return (this->Object.*this->Getter_)().find_first_of(str, pos);
		}
		typename T_::size_type find_first_of ( const char* s, typename T_::size_type pos, typename T_::size_type n ) const {
			return (this->Object.*this->Getter_)().find_first_of(s, pos, n);
		}
		typename T_::size_type find_first_of ( const char* s, typename T_::size_type pos = T_::npos ) const {
			return (this->Object.*this->Getter_)().find_first_of(s, pos);
		}
		typename T_::size_type find_first_of ( char c, typename T_::size_type pos = T_::npos ) const {
			return (this->Object.*this->Getter_)().find_first_of(c, pos);
		}

		typename T_::size_type find_last_of ( const T_& str, typename T_::size_type pos = T_::npos ) const {
			return (this->Object.*this->Getter_)().find_last_of(str, pos);
		}
		typename T_::size_type find_last_of ( const char* s, typename T_::size_type pos, typename T_::size_type n ) const {
			return (this->Object.*this->Getter_)().find_last_of(s, pos, n);
		}
		typename T_::size_type find_last_of ( const char* s, typename T_::size_type pos = T_::npos ) const {
			return (this->Object.*this->Getter_)().find_last_of(s, pos);
		}
		typename T_::size_type find_last_of ( char c, typename T_::size_type pos = T_::npos ) const {
			return (this->Object.*this->Getter_)().find_last_of(c, pos);
		}

		typename T_::size_type find_first_not_of ( const T_& str, typename T_::size_type pos = T_::npos ) const {
			return (this->Object.*this->Getter_)().find_first_not_of(str, pos);
		}
		typename T_::size_type find_first_not_of ( const char* s, typename T_::size_type pos, typename T_::size_type n ) const {
			return (this->Object.*this->Getter_)().find_first_not_of(s, pos, n);
		}
		typename T_::size_type find_first_not_of ( const char* s, typename T_::size_type pos = T_::npos ) const {
			return (this->Object.*this->Getter_)().find_first_not_of(s, pos);
		}
		typename T_::size_type find_first_not_of ( char c, typename T_::size_type pos = T_::npos ) const {
			return (this->Object.*this->Getter_)().find_first_not_of(c, pos);
		}

		typename T_::size_type find_last_not_of ( const T_& str, typename T_::size_type pos = T_::npos ) const {
			return (this->Object.*this->Getter_)().find_last_not_of(str, pos);
		}
		typename T_::size_type find_last_not_of ( const char* s, typename T_::size_type pos, typename T_::size_type n ) const {
			return (this->Object.*this->Getter_)().find_last_not_of(s, pos, n);
		}
		typename T_::size_type find_last_not_of ( const char* s, typename T_::size_type pos = T_::npos ) const {
			return (this->Object.*this->Getter_)().find_last_not_of(s, pos);
		}
		typename T_::size_type find_last_not_of ( char c, typename T_::size_type pos = T_::npos ) const {
			return (this->Object.*this->Getter_)().find_last_not_of(c, pos);
		}

		/*
		char &operator[] (typename T_::size_type pos) {
			return (Object.*Getter_)()[pos];
		}
		*/

		const char &operator[] (typename T_::size_type pos) const {
			return (this->Object.*this->Getter_)()[pos];
		}
		
		void clear() {
			T_ s=(this->Object.*this->Getter_)();
			s.clear();
			(this->Object.*this->Setter_)(s);
		}

		TextualProperty &append(const T_ &str) {
			T_ s=(this->Object.*this->Getter_)();
			s.append(str);
			(this->Object.*this->Setter_)(s);

			return *this;
		}

		TextualProperty & append(const T_ &str, typename T_::size_type pos, typename T_::size_type n) {
			T_ s=(this->Object.*this->Getter_)();
			s.append(str, pos, n);
			(this->Object.*this->Setter_)(s);

			return *this;
		}

		TextualProperty &append(const char *str, typename T_::size_type n) {
			T_ s=(this->Object.*this->Getter_)();
			s.append(str, n);
			(this->Object.*this->Setter_)(s);

			return *this;
		}

		TextualProperty &append(const char *str) {
			T_ s=(this->Object.*this->Getter_)();
			s.append(str);
			(this->Object.*this->Setter_)(s);

			return *this;
		}

		TextualProperty &append(typename T_::size_type n, char c) {
			T_ s=(this->Object.*this->Getter_)();
			s.append(n, c);
			(this->Object.*this->Setter_)(s);

			return *this;
		}

		template <class InputIterator>
		TextualProperty& append ( InputIterator first, InputIterator last ) {
			T_ s=(this->Object.*this->Getter_)();
			s.template append<InputIterator>(first, last);
			(this->Object.*this->Setter_)(s);

			return *this;
		}

		TextualProperty &erase(typename T_::size_type pos = 0, typename T_::size_type n = T_::npos) {
			T_ s=(this->Object.*this->Getter_)();
			s.erase(pos, n);
			(this->Object.*this->Setter_)(s);

			return *this;
		}

		TextualProperty &insert(typename T_::size_type pos, const T_ str) {
			T_ s=(this->Object.*this->Getter_)();
			s.insert(pos, str);
			(this->Object.*this->Setter_)(s);

			return *this;
		}

		TextualProperty &insert(typename T_::size_type pos1, const T_ str, typename T_::size_type pos2, typename T_::size_type n = T_::npos) {
			T_ s=(this->Object.*this->Getter_)();
			s.erase(pos1, str, pos2, n);
			(this->Object.*this->Setter_)(s);

			return *this;
		}

		TextualProperty &insert(typename T_::size_type pos, const char *str, typename T_::size_type n) {
			T_ s=(this->Object.*this->Getter_)();
			s.insert(pos, str, n);
			(this->Object.*this->Setter_)(s);

			return *this;
		}

		TextualProperty &insert(typename T_::size_type pos, const char *str) {
			T_ s=(this->Object.*this->Getter_)();
			s.insert(pos, str);
			(this->Object.*this->Setter_)(s);

			return *this;
		}

		TextualProperty &insert(typename T_::size_type pos, typename T_::size_type n, char c) {
			T_ s=(this->Object.*this->Getter_)();
			s.insert(pos, n, c);
			(this->Object.*this->Setter_)(s);

			return *this;
		}

		operator T_() {
			return (this->Object.*this->Getter_)();
		}

		operator const T_() const {
			return (this->Object.*this->Getter_)();
		}

	};


	template<class C_, class T_, T_(C_::*Getter_)() const, void(C_::*Setter_)(T_)>
	T_ operator +(TextualProperty<C_, T_> &t, const T_ &v) {
		return T_(t)+v;
	}

	template<class C_, class T_, T_(C_::*Getter_)() const, void(C_::*Setter_)(T_)>
	T_ operator +(const T_ &v, TextualProperty<C_, T_> &t) {
		return v+T_(t);
	}

	template<class C_, class T_, T_(C_::*Getter_)() const, void(C_::*Setter_)(T_)>
	T_ operator +(TextualProperty<C_, T_> &t, const char *v) {
		return T_(t)+v;
	}

	template<class C_, class T_, T_(C_::*Getter_)() const, void(C_::*Setter_)(T_)>
	T_ operator +(const char *v, TextualProperty<C_, T_> &t) {
		return v+T_(t);
	}



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

}
