#pragma once

/**
 * @page property Properties
 * Property classes allows property access much like vb and c#.
 * 
 * Properties are compile-time constructs, maximum cost they will
 * bear is the call to setter or getter function.
 * 
 * There are different property types for different tasks. You
 * should use the property type that suits your needs. If necessary
 * you should derive your own property object. But do not forget to
 * add assignment operators, as they are not inherited. 
 *
 * There are more property types in Geometry namespace.
 */

#include "Types.h"
#include <string>
#include <iostream>
#include <stdexcept>

namespace Gorgon {

	/// This is generic property that can be set and retrieved
	/// good for enums mostly, its ok to use with POD structs
	/// for direct assignment but better not to use it with 
	/// complex data types.
	template<class C_, class T_, T_(C_::*Getter_)() const = &C_::get, void(C_::*Setter_)(const T_ &) = &C_::set>
	class Property {
	public:
		using Type = T_;

	protected:
		C_		&Object;

	public:
		Property(C_ *Object) : Object(*Object)
		{ }

		Property(C_ &Object) : Object(Object) {}

		Property(const Property &) = delete;

		Property &operator =(const Property &) = delete;

		Property(Property &&) = default;

		Property &operator =(Property &&) = default;

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

		template <class AC_, T_(AC_::*G_)() const, void(AC_::*S_)(const T_ &)>
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
	template<class C_, class T_, T_(C_::*Getter_)() const, void(C_::*Setter_)(const T_ &)>
	class NumericProperty : public Property<C_, T_, Getter_, Setter_> {
	public:
		using Type = T_;

		NumericProperty(C_ *Object) : Property<C_, T_, Getter_, Setter_>(Object) 
		{ }

		NumericProperty(C_ &Object) : Property<C_, T_, Getter_, Setter_>(Object) 
		{ }


		NumericProperty(NumericProperty &&) = default;

		NumericProperty &operator =(NumericProperty &&) = default;
        
		NumericProperty &operator =(const T_ &value) { 
			(this->Object.*Setter_)(value);

			return *this;
		}

		NumericProperty &operator =(const NumericProperty &other) { 
			(this->Object.*Setter_)(other.Get());

			return *this;
		}

        template<class AC_, class O_, O_(C_::*G_)() const, void(C_::*S_)(const O_ &)>
		NumericProperty &operator =(const Property<AC_, O_, G_, S_> &prop) {
			(this->Object.*Setter_)((T_)(O_)prop);

			return *this;
		}

		T_ operator ++(int) {
			T_ v=(this->Object.*Getter_)();
			T_ k=v;
			k++;
			(this->Object.*Setter_)(k);
			return v;
		}

		T_ operator --(int) {
			T_ v=(this->Object.*Getter_)();
			T_ k=v;
			k--;
			(this->Object.*Setter_)(k);
			return v;
		}

		T_ operator ++() {
			T_ v=(this->Object.*Getter_)();
			v++;
			(this->Object.*Setter_)(v);
			return v;
		}

		T_ operator --() {
			T_ v=(this->Object.*Getter_)();
			v--;
			(this->Object.*Setter_)(v);
			return v;
		}

		T_ operator +(const T_ &v) const {
			return (this->Object.*Getter_)() + v;
		}

		T_ operator -(const T_ &v) const {
			return (this->Object.*Getter_)() - v;
		}

		T_ operator *(const T_ &v) const {
			return (this->Object.*Getter_)() * v;
		}

		T_ operator /(const T_ &v) const {
			return (this->Object.*Getter_)() / v;
		}

		T_ operator +=(const T_ &v) {
			(this->Object.*Setter_)((this->Object.*Getter_)()+v);
			return (this->Object.*Getter_)();
		}

		T_ operator -=(const T_ &v) {
			(this->Object.*Setter_)((this->Object.*Getter_)()-v);
			return (this->Object.*Getter_)();
		}

		template <class O_>
		T_ operator *=(const O_ &v) {
			(this->Object.*Setter_)((this->Object.*Getter_)()*v);
			return (this->Object.*Getter_)();
		}

		template <class O_>
		T_ operator /=(const O_ &v) {
			(this->Object.*Setter_)((this->Object.*Getter_)()/v);
			return (this->Object.*Getter_)();
		}

		bool operator >(const T_ &v) const {
			return (this->Object.*Getter_)()>v;
		}

		bool operator <(const T_ &v) const {
			return (this->Object.*Getter_)()<v;
		}

		bool operator >=(const T_ &v) const {
			return (this->Object.*Getter_)()>=v;
		}

		bool operator <=(const T_ &v) const {
			return (this->Object.*Getter_)()<=v;
		}
	};

	/// Supports logic operators. Mostly designed to be used with bool
	/// &&, ||, !, and equalities ==, !=
	template<class C_, class T_, T_(C_::*Getter_)() const, void(C_::*Setter_)(const T_ &)>
	class BooleanProperty : public Property<C_, T_, Getter_, Setter_> {
	public:
		using Type = T_;

		BooleanProperty(C_ *Object) : Property<C_, T_, Getter_, Setter_>(Object) 
		{ }
		
		BooleanProperty(C_ &Object) : Property<C_, T_, Getter_, Setter_>(Object) 
		{ }

		BooleanProperty(BooleanProperty &&) = default;

		BooleanProperty &operator =(BooleanProperty &&) = default;

		template <class O_>
		BooleanProperty &operator =(const O_ &value) { 
			(this->Object.*Setter_)(value);

			return *this;
		}

        template <class AC_, class O_, O_(C_::*G_)() const, void(C_::*S_)(const O_ &)>
		BooleanProperty &operator =(const Property<AC_, O_, G_, S_> &prop) {
			(this->Object.*Setter_)((T_)(O_)prop);

			return *this;
		}

		bool operator &&(const T_ &v) const {
			return (this->Object.*Getter_)() && v;
		}

		bool operator ||(const T_ &v) const {
			return (this->Object.*Getter_)() || v;
		}

		bool operator !() const {
			return !(this->Object.*Getter_)();
		}
	};

	/// Supports all operators that the numeric property supports.
	/// Additionally supports and |, &, ~, ...
	/// Use with unsigned int, Gorgon::Byte
	template<class C_, class T_, T_(C_::*Getter_)() const, void(C_::*Setter_)(const T_ &)>
	class BinaryProperty : public NumericProperty<C_, T_, Getter_, Setter_> {
	public:
		using Type = T_;

		BinaryProperty(C_ *Object) : NumericProperty<C_,T_, Getter_, Setter_>(Object) 
		{ }
		
		BinaryProperty(C_ &Object) : NumericProperty<C_,T_, Getter_, Setter_>(Object) 
		{ }

		BinaryProperty(BinaryProperty &&) = default;

		BinaryProperty &operator =(BinaryProperty &&) = default;

		template <class O_>
		BinaryProperty &operator =(const O_ &value) { 
			(this->Object.*Setter_)(T_(value));

			return *this;
		}

		template <class AC_, T_(AC_::*G_)() const, void(AC_::*S_)(const T_ &)>
		BinaryProperty &operator =(const Property<AC_, T_, G_, S_> &prop) {
			(this->Object.*Setter_)((T_)prop);

			return *this;
		}

		T_ operator |= (const T_ &v) {
			(this->Object.*Setter_)((this->Object.*Getter_)() | v);
			return (this->Object.*Getter_)();
		}

		T_ operator &= (const T_ &v) {
			(this->Object.*Setter_)((this->Object.*Getter_)() & v);
			return (this->Object.*Getter_)();
		}

		T_ operator ^= (const T_ &v) {
			(this->Object.*Setter_)((this->Object.*Getter_)() ^ v);
			return (this->Object.*Getter_)();
		}
	};

	/// Object property allows the consumers of the property
	/// to be able to access object's member functions and 
	/// data members in a const manner
	template<class C_, class T_, T_&(C_::*Getter_)() const, void(C_::*Setter_)(T_ &)>
	class ObjectProperty : public Property<C_, T_&, Getter_, Setter_> {
	public:
		using Type = T_;

		ObjectProperty(C_ *Object) : Property<C_, T_&, Getter_, Setter_>(Object) 
		{ }
		
		ObjectProperty(C_ &Object) : Property<C_, T_&, Getter_, Setter_>(Object) 
		{ }

		ObjectProperty(ObjectProperty &&) = default;

		ObjectProperty &operator =(ObjectProperty &&) = default;

		template <class O_>
		ObjectProperty &operator =(const O_ &value) { 
			(this->Object.*Setter_)(value);

			return *this;
		}

		template <class AC_, T_&(C_::*G_)() const, void(C_::*S_)(const T_ &)>
		ObjectProperty &operator =(const Property<AC_, T_&, G_, S_> &prop) {
			(this->Object.*Setter_)(dynamic_cast<T_&>(prop));

			return *this;
		}

		const T_ &operator *() const {
			return (this->Object.*Getter_)();
		}

		const T_ *operator ->() const {
			return &(this->Object.*Getter_)();
		}
	};

	/// @cond internal
	namespace internal {
		template <class T_>
		class ReferencePropertyWatch;

		template <class T_>
		class ObjectPropertyWatch;
	}
	/// @endcond

	/// Object property allows the consumers of the property
	/// to be able to access objects member functions and 
	/// variables. This alternative calls Setter anytime a
	/// member of the object is accessed, even if it is not
	/// changed. However, usage of dereferencing operator
	/// cannot be tracked.
	template<class C_, class T_, T_(C_::*Getter_)() const, void(C_::*Setter_)(const T_ &)>
	class MutableObjectProperty : public Property<C_, T_, Getter_, Setter_> {
	public:
		using Type = T_;

		MutableObjectProperty(C_ *Object) : Property<C_, T_, Setter_, Getter_>(Object) 
		{ }

		MutableObjectProperty(C_ &Object) : Property<C_, T_, Setter_, Getter_>(Object) 
		{ }

		MutableObjectProperty(MutableObjectProperty &&) = default;

		MutableObjectProperty &operator =(MutableObjectProperty &&) = default;

		template <class O_>
		MutableObjectProperty &operator =(const O_ &value) { 
			(this->Object.*Setter_)(value);

			return *this;
		}

		template <class AC_, T_(C_::*G_)() const, void(C_::*S_)(const T_ &)>
		MutableObjectProperty &operator =(const Property<AC_, T_, G_, S_> &prop) {
			(this->Object.*Setter_)((T_)prop);

			return *this;
		}

		T_ &operator *() {
			return (this->Object.*Getter_)();
		}

		const T_ &operator *() const {
			return (this->Object.*Getter_)();
		}

		internal::ObjectPropertyWatch<MutableObjectProperty<C_, T_, Getter_, Setter_>> operator ->();

		const T_ *operator ->() const {
			return &(this->Object.*Getter_)();
		}
	};

	/// Reference property allows clients to access a reference object
	/// within the class. This property does not have a setter, but instead
	/// it uses update function that is called whenever reference object
	/// is accessed by using -> operator. The client can call update function
	/// manually.
	template<class C_, class T_, T_ *(C_::*Getter_)() const, void(C_::*Setter_)(T_ *), void(C_::*Update_)()>
	class ReferenceProperty {
	protected:
		C_		&Object;

	public:
		using Type = T_;

		ReferenceProperty(C_ *Object) : Object(*Object) { }

		ReferenceProperty(C_ &Object) : Object(Object) { }

		ReferenceProperty(const ReferenceProperty &) = delete;

		ReferenceProperty &operator =(const ReferenceProperty &) = delete;

		ReferenceProperty(ReferenceProperty &&) = default;

		ReferenceProperty &operator =(ReferenceProperty &&) = default;

		operator T_ &() {
			return (this->Object.*Getter_)();
		}

		operator T_ &() const {
			return (this->Object.*Getter_)();
		}

		ReferenceProperty &operator =(T_ *value) {
			(this->Object.*Setter_)(value);

			return *this;
		}

		ReferenceProperty &operator =(T_ &value) {
			(this->Object.*Setter_)(&value);

			return *this;
		}

		/// Compares two objects, this performs reference comparison, not lexical.
		bool operator ==(const T_ &v) const {
			return (this->Object.*Getter_)()==&v;
		}

		/// Compares two objects, this performs reference comparison, not lexical.
		bool operator !=(const T_ &v) const {
			return (this->Object.*Getter_)()!=&v;
		}

		/// Compares two objects, this performs reference comparison, not lexical.
		bool operator ==(const T_ *v) const {
			return (this->Object.*Getter_)()==v;
		}

		/// Compares two objects, this performs reference comparison, not lexical.
		bool operator !=(const T_ *v) const {
			return (this->Object.*Getter_)()!=v;
		}

		void Update() {
			(this->Object.*Update_)();
		}

		internal::ReferencePropertyWatch<ReferenceProperty<C_, T_, Getter_, Setter_, Update_>> operator ->();

		const T_ *operator ->() const {
			return (this->Object.*Getter_)();
		}

		T_ *GetPtr() const {
			return (this->Object.*Getter_)();
		}

		T_ &Get() const {
			T_ *o=(this->Object.*Getter_)();
			if(o)
				return *o;
			else
				throw std::runtime_error("Property is empty");
		}
	};

	/// @cond internal
	namespace internal {
		template <class T_>
		class ReferencePropertyWatch {
		public:
			ReferencePropertyWatch(T_ *obj) : obj(obj) {}

			~ReferencePropertyWatch() {
				obj->Update();
			}

			auto operator ->() const {
				return obj->GetPtr();
			}

		private:
			T_ *obj;
		};

		template <class T_>
		class ObjectPropertyWatch {
		public:
			ObjectPropertyWatch(T_ *obj) : obj(obj) {
				val = obj->Get();
			}

			~ObjectPropertyWatch() {
				obj->Set(val);
			}

			auto &operator ->() const {
				return val;
			}

		private:
			T_ *obj;
			typename T_::Type val;
		};
	}
	/// @endcond


	template<class C_, class T_, T_(C_::*Getter_)() const, void(C_::*Setter_)(const T_ &)>
	internal::ObjectPropertyWatch<MutableObjectProperty<C_, T_, Getter_, Setter_>> MutableObjectProperty<C_, T_, Getter_, Setter_>::operator ->() {
		return {this};
	}

	template<class C_, class T_, T_ *(C_::*Getter_)() const, void(C_::*Setter_)(T_ *), void(C_::*Update_)()>
	internal::ReferencePropertyWatch<ReferenceProperty<C_, T_, Getter_, Setter_, Update_>> ReferenceProperty<C_, T_, Getter_, Setter_, Update_>::operator ->() {
		return {this};
	}

	/// Supports everything that string class supports including +, +=, length()
	template<class C_, class T_, T_(C_::*Getter_)() const, void(C_::*Setter_)(const T_ &)>
	class TextualProperty : public Property<C_, T_, Getter_, Setter_> {
	public:
		TextualProperty(C_ *Object) : Property<C_,T_, Getter_, Setter_>(Object) 
		{ }

		TextualProperty(C_ &Object) : Property<C_,T_, Getter_, Setter_>(Object) 
		{ }

		TextualProperty(TextualProperty &&) = default;

		TextualProperty &operator =(TextualProperty &&) = default;

		template <class O_>
		TextualProperty &operator =(const O_ &value) { 
			(this->Object.*Setter_)(T_(value));

			return *this;
		}

		template <class AC_, T_(C_::*G_)() const, void(C_::*S_)(const T_ &)>
		TextualProperty &operator =(const Property<AC_, T_, G_, S_> &prop) {
			(this->Object.*Setter_)((T_)prop);

			return *this;
		}

		void operator +=(const T_ &v) {
			return (this->Object.*Setter_)((this->Object.*Getter_)()+v);
		}

		typename T_::size_type length() const {
			return (this->Object.*Getter_)().length();
		}

		const char *c_str() const {
			return (this->Object.*Getter_)().c_str();
		}

		T_ substr(typename T_::size_type off=0U, typename T_::size_type len=T_::npos) const {
			return (this->Object.*Getter_)().substr(off,len);
		}

		typename T_::size_type find ( const T_& str, typename T_::size_type pos = 0 ) const {
			return (this->Object.*Getter_)().find(str, pos);
		}
		typename T_::size_type find ( const char* s, typename T_::size_type pos, typename T_::size_type n ) const {
			return (this->Object.*Getter_)().find(s, pos, n);
		}
		typename T_::size_type find ( const char* s, typename T_::size_type pos = 0 ) const {
			return (this->Object.*Getter_)().find(s, pos);
		}
		typename T_::size_type find ( char c, typename T_::size_type pos = 0 ) const {
			return (this->Object.*Getter_)().find(c, pos);
		}

		typename T_::size_type rfind ( const T_& str, typename T_::size_type pos = T_::npos ) const {
			return (this->Object.*Getter_)().rfind(str, pos);
		}
		typename T_::size_type rfind ( const char* s, typename T_::size_type pos, typename T_::size_type n ) const {
			return (this->Object.*Getter_)().rfind(s, pos, n);
		}
		typename T_::size_type rfind ( const char* s, typename T_::size_type pos = T_::npos ) const {
			return (this->Object.*Getter_)().rfind(s, pos);
		}
		typename T_::size_type rfind ( char c, typename T_::size_type pos = T_::npos ) const {
			return (this->Object.*Getter_)().rfind(c, pos);
		}

		typename T_::size_type find_first_of ( const T_& str, typename T_::size_type pos = T_::npos ) const {
			return (this->Object.*Getter_)().find_first_of(str, pos);
		}
		typename T_::size_type find_first_of ( const char* s, typename T_::size_type pos, typename T_::size_type n ) const {
			return (this->Object.*Getter_)().find_first_of(s, pos, n);
		}
		typename T_::size_type find_first_of ( const char* s, typename T_::size_type pos = T_::npos ) const {
			return (this->Object.*Getter_)().find_first_of(s, pos);
		}
		typename T_::size_type find_first_of ( char c, typename T_::size_type pos = T_::npos ) const {
			return (this->Object.*Getter_)().find_first_of(c, pos);
		}

		typename T_::size_type find_last_of ( const T_& str, typename T_::size_type pos = T_::npos ) const {
			return (this->Object.*Getter_)().find_last_of(str, pos);
		}
		typename T_::size_type find_last_of ( const char* s, typename T_::size_type pos, typename T_::size_type n ) const {
			return (this->Object.*Getter_)().find_last_of(s, pos, n);
		}
		typename T_::size_type find_last_of ( const char* s, typename T_::size_type pos = T_::npos ) const {
			return (this->Object.*Getter_)().find_last_of(s, pos);
		}
		typename T_::size_type find_last_of ( char c, typename T_::size_type pos = T_::npos ) const {
			return (this->Object.*Getter_)().find_last_of(c, pos);
		}

		typename T_::size_type find_first_not_of ( const T_& str, typename T_::size_type pos = T_::npos ) const {
			return (this->Object.*Getter_)().find_first_not_of(str, pos);
		}
		typename T_::size_type find_first_not_of ( const char* s, typename T_::size_type pos, typename T_::size_type n ) const {
			return (this->Object.*Getter_)().find_first_not_of(s, pos, n);
		}
		typename T_::size_type find_first_not_of ( const char* s, typename T_::size_type pos = T_::npos ) const {
			return (this->Object.*Getter_)().find_first_not_of(s, pos);
		}
		typename T_::size_type find_first_not_of ( char c, typename T_::size_type pos = T_::npos ) const {
			return (this->Object.*Getter_)().find_first_not_of(c, pos);
		}

		typename T_::size_type find_last_not_of ( const T_& str, typename T_::size_type pos = T_::npos ) const {
			return (this->Object.*Getter_)().find_last_not_of(str, pos);
		}
		typename T_::size_type find_last_not_of ( const char* s, typename T_::size_type pos, typename T_::size_type n ) const {
			return (this->Object.*Getter_)().find_last_not_of(s, pos, n);
		}
		typename T_::size_type find_last_not_of ( const char* s, typename T_::size_type pos = T_::npos ) const {
			return (this->Object.*Getter_)().find_last_not_of(s, pos);
		}
		typename T_::size_type find_last_not_of ( char c, typename T_::size_type pos = T_::npos ) const {
			return (this->Object.*Getter_)().find_last_not_of(c, pos);
		}

		const char &operator[] (typename T_::size_type pos) const {
			return (this->Object.*Getter_)()[pos];
		}
		
		void clear() {
			T_ s=(this->Object.*Getter_)();
			s.clear();
			(this->Object.*Setter_)(s);
		}

		TextualProperty &append(const T_ &str) {
			T_ s=(this->Object.*Getter_)();
			s.append(str);
			(this->Object.*Setter_)(s);

			return *this;
		}

		TextualProperty &append(const T_ &str, typename T_::size_type pos, typename T_::size_type n) {
			T_ s=(this->Object.*Getter_)();
			s.append(str, pos, n);
			(this->Object.*Setter_)(s);

			return *this;
		}

		TextualProperty &append(const char *str, typename T_::size_type n) {
			T_ s=(this->Object.*Getter_)();
			s.append(str, n);
			(this->Object.*Setter_)(s);

			return *this;
		}

		TextualProperty &append(const char *str) {
			T_ s=(this->Object.*Getter_)();
			s.append(str);
			(this->Object.*Setter_)(s);

			return *this;
		}

		TextualProperty &append(typename T_::size_type n, char c) {
			T_ s=(this->Object.*Getter_)();
			s.append(n, c);
			(this->Object.*Setter_)(s);

			return *this;
		}

		template <class InputIterator>
		TextualProperty& append ( InputIterator first, InputIterator last ) {
			T_ s=(this->Object.*Getter_)();
			s.template append<InputIterator>(first, last);
			(this->Object.*Setter_)(s);

			return *this;
		}

		TextualProperty &erase(typename T_::size_type pos = 0, typename T_::size_type n = T_::npos) {
			T_ s=(this->Object.*Getter_)();
			s.erase(pos, n);
			(this->Object.*Setter_)(s);

			return *this;
		}

		TextualProperty &insert(typename T_::size_type pos, const T_ str) {
			T_ s=(this->Object.*Getter_)();
			s.insert(pos, str);
			(this->Object.*Setter_)(s);

			return *this;
		}

		TextualProperty &insert(typename T_::size_type pos1, const T_ str, typename T_::size_type pos2, typename T_::size_type n = T_::npos) {
			T_ s=(this->Object.*Getter_)();
			s.erase(pos1, str, pos2, n);
			(this->Object.*Setter_)(s);

			return *this;
		}

		TextualProperty &insert(typename T_::size_type pos, const char *str, typename T_::size_type n) {
			T_ s=(this->Object.*Getter_)();
			s.insert(pos, str, n);
			(this->Object.*Setter_)(s);

			return *this;
		}

		TextualProperty &insert(typename T_::size_type pos, const char *str) {
			T_ s=(this->Object.*Getter_)();
			s.insert(pos, str);
			(this->Object.*Setter_)(s);

			return *this;
		}

		TextualProperty &insert(typename T_::size_type pos, typename T_::size_type n, char c) {
			T_ s=(this->Object.*Getter_)();
			s.insert(pos, n, c);
			(this->Object.*Setter_)(s);

			return *this;
		}

		operator T_() {
			return (this->Object.*Getter_)();
		}

		operator const T_() const {
			return (this->Object.*Getter_)();
		}

	};


	template<class C_, class T_, T_(C_::*Getter_)() const, void(C_::*Setter_)(const T_ &)>
	T_ operator +(TextualProperty<C_, T_, Setter_, Getter_> &t, const T_ &v) {
		return T_(t)+v;
	}

	template<class C_, class T_, T_(C_::*Getter_)() const, void(C_::*Setter_)(const T_ &)>
	T_ operator +(const T_ &v, TextualProperty<C_, T_, Setter_, Getter_> &t) {
		return v+T_(t);
	}

	template<class C_, class T_, T_(C_::*Getter_)() const, void(C_::*Setter_)(const T_ &)>
	T_ operator +(TextualProperty<C_, T_, Setter_, Getter_> &t, const char *v) {
		return T_(t)+v;
	}

	template<class C_, class T_, T_(C_::*Getter_)() const, void(C_::*Setter_)(const T_ &)>
	T_ operator +(const char *v, TextualProperty<C_, T_, Setter_, Getter_> &t) {
		return v+T_(t);
	}



    template <class C_, class T_, T_(C_::*G_)() const, void(C_::*S_)(const T_ &)>
	inline std::ostream &operator <<(std::ostream &out, const TextualProperty<C_, T_, G_, S_> &p) {
		out<<(T_)p;

		return out;
	}

    template <class C_, class T_, T_(C_::*G_)() const, void(C_::*S_)(const T_ &)>
	inline std::ostream &operator <<(std::ostream &out, const NumericProperty<C_,T_, G_, S_> &p) {
		out<<(T_)p;

		return out;
	}

    template <class C_, class T_, T_(C_::*G_)() const, void(C_::*S_)(const T_ &)>
	inline std::istream &operator >>(std::istream &in, TextualProperty<C_,T_, G_, S_> &p) {
		T_ t;
		in>>t;
		p=t;

		return in;
	}

    template <class C_, class T_, T_(C_::*G_)() const, void(C_::*S_)(const T_ &)>
	inline std::istream &operator >>(std::istream &in, NumericProperty<C_,T_, G_, S_> &p) {
		T_ t;
		in>>t;
		p=t;

		return in;
	}


/// Good for testing nothing else, you probably should use normal variables if you need something like this.
#define MAP_PROPERTY(type, name, variable) \
    type get##name() const { return variable; } \
    void set##name(const type &v) { variable=v; } \
    type variable

/// If property is there to call Update every time the value is changed, this mapper can handle this situation.
/// Names the member variable as m_name. For proptype use the prefix part of the property name: Numeric for 
/// NumericProperty
#define PROPERTY_GETSET(cls, proptype, type, name) \
    Gorgon::proptype##Property<cls, type, &cls::Get##name, &cls::Set##name> name = Gorgon::proptype##Property<cls, type, &cls::Get##name, &cls::Set##name>{this}

/// If property is there to call Update every time the value is changed, this mapper can handle this situation.
/// Names the member variable as m_name. For proptype use the prefix part of the property name: Numeric for 
/// NumericProperty
#define PROPERTY_UPDATE(cls, proptype, type, name, def) \
private: \
    type m_##name = def; \
public: \
    type Get##name() const { return m_##name; } \
    void Set##name(const type &value) { m_##name=value; Update(); } \
    \
    Gorgon::proptype##Property<cls, type, &cls::Get##name, &cls::Set##name> name = Gorgon::proptype##Property<cls, type, &cls::Get##name, &cls::Set##name>{this}

/// If property is there to call Update every time the value is changed, this mapper can handle this situation.
/// Names the member variable as m_name. For proptype use the prefix part of the property name: Numeric for 
/// NumericProperty
#define PROPERTY_UPDATE_VN(cls, proptype, type, name, varname) \
public: \
    type Get##name() const { return varname; } \
    void Set##name(const type &value) { this->varname=value; Update(); } \
    \
    Gorgon::proptype##Property<cls, type, &cls::Get##name, &cls::Set##name> name = Gorgon::proptype##Property<cls, type, &cls::Get##name, &cls::Set##name>{this}
    
/// If property is there to call Refresh every time the value is changed, this mapper can handle this situation.
/// Names the member variable as m_name. For proptype use the prefix part of the property name: Numeric for 
/// NumericProperty
#define PROPERTY_REFRESH(cls, proptype, type, name, def) \
private: \
    type m_##name = def; \
public: \
    type Get##name() const { return m_##name; } \
    void Set##name(const type &value) { if(m_##name==value) return;  m_##name=value; Refresh(); } \
    \
    Gorgon::proptype##Property<cls, type, &cls::Get##name, &cls::Set##name> name = Gorgon::proptype##Property<cls, type, &cls::Get##name, &cls::Set##name>{this}

/// If property is there to call Refresh every time the value is changed, this mapper can handle this situation.
/// Names the member variable as m_name. For proptype use the prefix part of the property name: Numeric for 
/// NumericProperty
#define PROPERTY_REFRESH_VN(cls, proptype, type, name, varname) \
public: \
    type Get##name() const { return varname; } \
    void Set##name(const type &value) { if(this->varname==value) return; this->varname=value; Refresh(); } \
    \
    Gorgon::proptype##Property<cls, type, &cls::Get##name, &cls::Set##name> name = Gorgon::proptype##Property<cls, type, &cls::Get##name, &cls::Set##name>{this}
    
    
}
