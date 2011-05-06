#pragma once

#define PROPERTY_DEFINED

//This is generic property that can be set and retrieved
// good for enums mostly, its ok to use with POD  structs
// but you better not to use it with complex data types.
template<class C_, class T_>
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
	Property &operator =(const Property<AC_, T_> &prop) {
		(Object.*setter)((T_)prop);

		return *this;
	}
};


//THIS PART IS INCOMPLETE
//should support arithmetic operators
// including +, * ..., +=, ... 
// ==, <, >
// but not &, &&
// float, int, double, math/Complex
template<class C_, class T_>
class NumericProperty {

};

//should support logic operators
// &&, ||, !, and equalities ==, !=
// boolean mostly
template<class C_, class T_>
class BooleanProperty {

};

//should allow everything that numeric
// supports + |, &, ~, ...
// unsigned int, GGE/Byte
template<class C_, class T_>
class BinaryProperty {

};

//should allow reference, r-value and pointer
// assignment, -> and * operators
template<class C_, class T_>
class ReferenceProperty {

};

//similar to reference property but without
// a setter
template<class C_, class T_>
class FixedReferenceProperty {

};

//should support everything that string class
// supports +, +=, length()
template<class C_, class T_>
class TextualProperty {

};

#define	INIT_PROPERTY(classtype, name) name(this, &classtype::get##name, &classtype::set##name)
