//DESCRIPTION
//	This file contains IteratorBase which performs most common iterator
//	related functions for a random access iterator. It uses recurring
//	templates to reduce any overhead caused by virtual functions.

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


#include <stdexcept>
#include <xutility>
#include <vector>


namespace gge { namespace utils {

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
		//	D_  distance(const I_ &) const  ...
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
		I_ &iterator() { return *static_cast<I_*>(this); }
		const I_ &iterator() const { return *static_cast<const I_*>(this); }

	protected:
		IteratorBase() { }

	public:
		////Returns current item
		T_ &Current() {
			return iterator().current();
		}

		////Returns current item
		T_ &Current() const {
			return iterator().current();
		}
		////Returns current item
		T_ *CurrentPtr() {
			return &iterator().current();
		}

		////Returns current item
		T_ *CurrentPtr() const {
			return &iterator().current();
		}

		////Moves the iterator by the given amount
		bool MoveBy(int amount) {
			return iterator().moveby(amount);
		}

		////Advances the iterator to the next item
		bool Next() {
			return iterator().moveby(1);
		}

		////Moves to the previous item
		bool Previous() {
			return iterator().moveby(-1);
		}

		////Checks if the iterator is pointing to a valid item
		bool IsValid() const {
			return iterator().isvalid();
		}
		
		operator bool() const {
			return IsValid();
		}

		////Compares two iterators if they point to the same item
		bool Compare(const I_ &iterator) const {
			return this->iterator().compare(iterator);
		}

		////Returns the distance to the given iterator
		distance_type Distance(const I_ &iterator) const {
			return this->iterator().distance(iterator);
		}


		////Compares two iterators if they point to the same item
		bool operator ==(const I_ &iterator) const {
			return this->iterator().compare(iterator);
		}

		bool operator >(const I_ &iterator) const {
			return !(this->iterator() <= iterator);
		}

		bool operator <(const I_ &iterator) const {
			return this->iterator().isbefore(iterator);
		}

		bool operator >=(const I_ &iterator) const {
			return !(this->iterator() < iterator);
		}

		bool operator <=(const I_ &iterator) const {
			return this->iterator().isbefore(iterator) || this->iterator().compare(iterator);
		}

		bool operator !=(const I_ &it) const {
			return !iterator().compare(it);
		}

		////Moves this iterator to the item pointed by the given
		I_ &operator =(const I_ &iterator) {
			this->iterator().set(iterator);

			return this->iterator();
		}

		////Returns the distance to the given iterator
		distance_type operator -(const I_ &iterator) const {
			return iterator.distance(this->iterator());
		}

		////Creates a new iterator adding the given offset
		I_ operator +(distance_type offset) const {
			I_ i(iterator());
			i.MoveBy(offset);

			return i;
		}

		////Creates a new iterator subtracting the given offset
		I_ operator -(distance_type offset) const {
			I_ i(iterator());
			i.MoveBy(-offset);

			return i;
		}

		////Moves the iterator by the given offset to forwards
		I_ &operator +=(distance_type offset) {
			iterator().moveby(offset);

			return iterator();
		}

		////Moves the iterator by the given offset to backwards
		I_ &operator -=(distance_type offset) {
			iterator().moveby(-offset);

			return iterator();
		}

		////Moves the iterator to forwards
		I_ &operator ++() {
			iterator().moveby(1);

			return iterator();
		}

		////Moves the iterator to backwards
		I_ &operator --() {
			iterator().moveby(-1);

			return iterator();
		}

		////Moves the iterator to forwards
		I_ operator ++(int) {
			I_ it=iterator();
			it.moveby(1);

			return it;
		}

		////Moves the iterator to backwards
		I_ operator --(int) {
			I_ it=iterator();
			it.moveby(-1);

			return it;
		}

		operator T_&() const {
			return iterator().current();
		}

		T_ &operator *() const {
			return iterator().current();
		}

		T_ *operator ->() const {
			return &iterator().current();
		}


		virtual ~IteratorBase() {  }
	};



	////This function works with collection iterators
	template<class I_>
	void Remove(const I_ &first, const I_ &end) {
		for(I_ i=first;i!=end;++i) {
			i.Remove();
		}
	}

	////This function works with collection iterators
	template<class I_>
	void Delete(const I_ &first, const I_ &end) {
		for(I_ i=first;i!=end;++i) {
			i.Delete();
		}
	}

	template<class I_, class T_>
	I_ Find(const I_ &first, const I_ &end, const T_ &item) {
		for(I_ i=first;i!=end;++i) {
			if(*i==item)
				return i;
		}

		return I_();
	}

	template<class C_, class I_>
	void AddCopy(C_ &target, const I_ &it) {
		for(I_ i=it;i.IsValid();i.Next()) {
			target.Add(*i);
		}
	}

	template<class T_, class I_>
	void AddCopy(std::vector<T_> &target, const I_ &it) {
		for(I_ i=it;i.IsValid();i.Next()) {
			target.push_back(*i);
		}
	}

} }
