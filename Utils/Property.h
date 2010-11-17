#pragma once

#define PROPERTY_DEFINED

//ENTIRELY UNTEST, EVEN FOR COMPILATION

template<class T_, class C_>
class Property {
public:
	typedef T_(C_::*Getter)() const;
	typedef void (C_::*Setter)(T_);

protected:
	C_		&Object;
	Getter	getter;
	Setter	setter;

public:
	Property(C_ &Object, Getter getter, Setter setter) : Object(Object), getter(getter), setter(setter) 
	{ }

	Property(C_ *Object, Getter getter, Setter setter) : Object(*Object), getter(getter), setter(setter) 
	{ }

	operator T_() { 
		return getter(); 
	}

	Property &operator =(T_ value) { 
		setter(value);

		return *this;
	}

	template <class AC_>
	Property &operator =(const Property<T_, AC_> &prop) {
		setter(prop);

		return *this;
	}
};

//Specializations to allow operators
//we should allow specializations for
//int, float, char, double uint, uchar
//string
template<class C_>
class Property {
public:
	typedef int(C_::*Getter)() const;
	typedef void (C_::*Setter)(int);

protected:
	C_		&Object;
	Getter	getter;
	Setter	setter;

public:
	Property(C_ &Object, Getter getter, Setter setter) : Object(Object), getter(getter), setter(setter) 
	{ }

	Property(C_ *Object, Getter getter, Setter setter) : Object(*Object), getter(getter), setter(setter) 
	{ }

	operator int() { 
		return getter(); 
	}

	Property &operator =(int value) { 
		setter(value);

		return *this;
	}

	template <class AC_>
	Property &operator =(const Property<int, AC_> &prop) {
		setter(prop);

		return *this;
	}

	int operator ++() {
		int o=getter();
		setter(o+1);

		return o;
	}

	int operator --() {
		int o=getter();
		setter(o-1);

		return o;
	}

	int operator ++(int) {
		setter(getter()+1);

		return getter();
	}

	int operator --(int) {
		setter(getter()-1);

		return getter();
	}

	template<class T_>
	int operator +(T_ v) {
		return getter()+v;
	}

	template<class T_>
	int operator -(T_ v) {
		return getter()-v;
	}

	template<class T_>
	int operator *(T_ v) {
		return getter()*v;
	}

	template<class T_>
	int operator /(T_ v) {
		return getter()/v;
	}

	template<class T_>
	int operator %(T_ v) {
		return getter()%v;
	}

	template<class T_>
	int operator &&(T_ v) {
		return getter()&&v;
	}

	template<class T_>
	int operator ||(T_ v) {
		return getter()||v;
	}

	template<class T_>
	int operator &(T_ v) {
		return getter()&v;
	}

	template<class T_>
	int operator |(T_ v) {
		return getter()|v;
	}

	template<class T_>
	int operator ^(T_ v) {
		return getter()^v;
	}

	template<class T_>
	int operator <<(T_ v) {
		return getter()<<v;
	}

	template<class T_>
	int operator >>(T_ v) {
		return getter()>>v;
	}

	template<class T_>
	int operator +=(T_ v) {
		setter(getter()+v);
		return getter();
	}

	template<class T_>
	int operator -=(T_ v) {
		setter(getter()-v);
		return getter();
	}

	template<class T_>
	int operator *=(T_ v) {
		setter(getter()*v);
		return getter();
	}

	template<class T_>
	int operator /=(T_ v) {
		setter(getter()-v);
		return getter();
	}

	template<class T_>
	int operator %=(T_ v) {
		setter(getter()%v);
		return getter();
	}

	template<class T_>
	int operator &&=(T_ v) {
		setter(getter()&&v);
		return getter();
	}

	template<class T_>
	int operator ||=(T_ v) {
		setter(getter()||v);
		return getter();
	}

	template<class T_>
	int operator &=(T_ v) {
		setter(getter()|v);
		return getter();
	}

	template<class T_>
	int operator |=(T_ v) {
		setter(getter()|v);
		return getter();
	}

	template<class T_>
	int operator ^=(T_ v) {
		setter(getter()^v);
		return getter();
	}

	template<class T_>
	int operator <<=(T_ v) {
		setter(getter()<<v);
		return getter();
	}

	template<class T_>
	int operator >>=(T_ v) {
		setter(getter()>>v);
		return getter();
	}
};


