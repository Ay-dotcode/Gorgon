#pragma once

#define PROPERTY_DEFINED


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
		return (Object.*getter)(); 
	}

	Property &operator =(T_ value) { 
		(Object.*setter)(value);

		return *this;
	}

	template <class O_>
	Property &operator =(O_ value) { 
		(Object.*setter)(value);

		return *this;
	}

	template <class AC_>
	Property &operator =(const Property<T_, AC_> &prop) {
		(Object.*setter)(prop);

		return *this;
	}
};


//THIS PART IS INCOMPLETE, IF IT CAUSES
//PROBLEMS JUST COMMENT OUT

//Specializations to allow operators
//we should allow specializations for
//int, float, char, double uint, uchar
//string
template<class C_>
class Property<int, C_> {
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



#define	INIT_PROPERTY(name, classtype) name(this, &classtype::get##name, &classtype::set##name)