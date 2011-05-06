//TONS OF THINGS TO FIX
#pragma once


#include <stdexcept>
#include <xutility>



////Generic iterator interface. Derive from this class
/// supplying self, type, and distance type
template <class I_, class T_, class D_=int>
class IteratorBase {
	///Following functions should be implemented for Iterators.
	// They are publicly shaped by this class.
	// These functions are not virtually defined to allow inlining
	// and to reduce memory usage

	//protected:
	//	T_& current() const				...
	//	bool moveby(int amount)			...
	//	bool isvalid() const			...
	//	bool compare(const I_ &) const	...
	//	void set(const I_ &)			...
	//	int  distance(const I_ &) const ...
	//	bool isbefore(const I_ &) const ...

	//The rest is handled by this class
public:
	//std compatibility
	typedef std::random_access_iterator_tag iterator_category;
	typedef T_ value_type;
	typedef T_ *pointer;
	typedef T_& reference;
	typedef D_ difference_type;
	typedef D_ distance_type;	// retained


	typedef T_ Type;
	typedef D_ Units;

private:
	I_ &iterator;

protected:
	IteratorBase(I_ &iterator) : iterator(iterator)
	{ }

public:
	T_ &Current() const {
		return iterator.current();
	}
	
	bool MoveBy(int amount) {
		return iterator.moveby(amount);
	}

	bool Next() {
		return iterator.moveby(1);
	}

	bool Previous() {
		return iterator.moveby(-1);
	}

	bool isValid() const {
		return iterator.isvalid();
	}

	bool Compare(const I_ &iterator) const {
		return this->iterator.compare(iterator);
	}

	bool operator ==(const I_ &iterator) const {
		return this->iterator.compare(iterator);
	}

	bool operator >(const I_ &iterator) const {
		return !(this->iterator <= iterator);
	}

	bool operator <(const I_ &iterator) const {
		return this->iterator.isbefore(iterator);
	}

	bool operator >=(const I_ &iterator) const {
		return !(this->iterator < iterator);
	}

	bool operator <=(const I_ &iterator) const {
		return this->iterator.isbefore(iterator) || this->iterator.compare(iterator);
	}

	bool operator !=(const I_ &iterator) const {
		return !this->iterator.compare(iterator);
	}

	I_ &operator =(const I_ &iterator) {
		this->iterator.set(iterator);

		return this->iterator;
	}

	distance_type operator -(const I_ &iterator) const {
		return this->iterator.distance(iterator);
	}
	
	I_ operator +(distance_type offset) const {
		I_ i(iterator);
		i.MoveBy(offset);

		return i;
	}

	I_ operator -(distance_type offset) const {
		I_ i(iterator);
		i.MoveBy(-offset);

		return i;
	}

	I_ &operator +=(distance_type offset) {
		iterator.moveby(offset);

		return iterator;
	}

	I_ &operator -=(distance_type offset) {
		iterator.moveby(-offset);

		return iterator;
	}

	I_ &operator ++() {
		iterator.moveby(1);

		return iterator;
	}

	I_ &operator --() {
		iterator.moveby(-1);

		return iterator;
	}

	I_ operator ++(int) {
		I_ it=iterator;
		it.moveby(1);

		return it;
	}

	I_ operator --(int) {
		I_ it=iterator;
		it.moveby(-1);

		return it;
	}

	operator T_&() const {
		return iterator.current();
	}

	operator T_*() const {
		return &iterator.current();
	}

	T_ &operator *() const {
		return iterator.current();
	}

	T_ *operator ->() const {
		return &iterator.current();
	}


	virtual ~IteratorBase() {  }
};


////Generic iterator interface. Derive from this class
/// supplying self, type, distance type, and comparable iterator
/// search iterators are bi-directional iterators, that cannot
/// be assigned to and can only be compared with base iterators
/// they can be converted to base iterator
template <class I_, class T_, class C_, class D_=int>
class SearchIteratorBase {
	///Following functions should be implemented for Iterators.
	// They are publicly shaped by this class.
	// These functions are not virtually defined to allow inlining
	// and to reduce memory usage

	//protected:
	//	T_& current() const				...
	//	bool moveby(int amount)			... //only -1, 0, and 1
	//	bool isvalid() const			...
	//	bool compare(const C_ &) const	...
	//	bool isbefore(const C_ &) const ...
	//	C_ cast() const					...

	//The rest is handled by this class
public:
	//std compatibility
	typedef std::bidirectional_iterator_tag iterator_category;
	typedef T_ value_type;
	typedef T_ *pointer;
	typedef T_& reference;
	typedef D_ difference_type;
	typedef D_ distance_type;	// retained


	typedef T_ ValueType;
	typedef D_ Units;
	typedef C_ BaseIterator;

private:
	I_ &iterator;

protected:
	SearchIteratorBase(I_ &iterator) : iterator(iterator)
	{ }

public:
	T_ &Current() const {
		return iterator.current();
	}

	bool Next() {
		return iterator.moveby(1);
	}

	bool Previous() {
		return iterator.moveby(-1);
	}

	bool isValid() const {
		return iterator.isvalid();
	}

	bool Compare(const C_ &iterator) const {
		return this->iterator.compare(iterator);
	}

	bool operator ==(const I_ &iterator) const {
		return this->iterator.compare(iterator);
	}

	bool operator >(const I_ &iterator) const {
		return !(this->iterator <= iterator);
	}

	bool operator <(const I_ &iterator) const {
		return this->iterator.isbefore(iterator);
	}

	bool operator >=(const I_ &iterator) const {
		return !(this->iterator < iterator);
	}

	bool operator <=(const I_ &iterator) const {
		return this->iterator.isbefore(iterator) || this->iterator.compare(iterator);
	}

	bool operator !=(const I_ &iterator) const {
		return !this->iterator.compare(iterator);
	}

	operator C_() {
		return this->iterator.cast();
	}

	I_ &operator ++() {
		iterator.moveby(1);

		return iterator;
	}

	I_ &operator --() {
		iterator.moveby(-1);

		return iterator;
	}

	I_ operator ++(int) {
		I_ it=iterator;
		it.moveby(1);

		return it;
	}

	I_ operator --(int) {
		I_ it=iterator;
		it.moveby(-1);

		return it;
	}

	operator T_&() const {
		return iterator.current();
	}

	operator T_*() const {
		return &iterator.current();
	}

	T_ &operator *() const {
		return iterator.current();
	}

	T_ *operator ->() const {
		return &iterator.current();
	}


	virtual ~SearchIteratorBase() {  }
};

