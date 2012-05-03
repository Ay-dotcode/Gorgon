//DESCRIPTION
//	OrderedCollection class is an ordered add/remove list. Ordering is manual,
//	its not automatic sorting depending on either values or keys. 
//	Allows iteration using std methods. 
//	Also hosts search iteration. This Collection is preferable for small
//	number of objects. Unlike Standard Collection, insert and remove takes linear
//	time, while access is instant. Total size of the object
//	is 16 bytes. It can be passed by value, contents will not be duplicated
//	instead it will ref count the object list.
//	
//	IMPORTANT: Stored objects are not duplicated nor destroyed at the end
//	of the collection's life time. To destruct objects that are contained
//	within use .Destroy() method. This applies for removed objects, use
//	delete to destruct and remove them

//REQUIRES:
//	gge::utils::IteratorBase
//	gge::utils::ManagedBuffer

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

#include <cstdlib>
#include <cstring>
#include <stdexcept>

#include "Iterator.h"
#include "ManagedBuffer.h"
#include "UtilsBase.h"

namespace gge { namespace utils {
	
	template<class T_, class P_>
	struct elementsorterfrompointer {
		elementsorterfrompointer(const P_ &predicate) : predicate(predicate) {}

		bool operator ()(T_ *&left, T_*&right) const {
			if(left==NULL || right==NULL)
				return false;

			return predicate(*left,*right);
		}

		const P_ &predicate;
	};

	template <class T_, int growth=50>
	class OrderedCollection {
		template<class T__, int g__>
		friend class OrderedCollection;

		//		Iterators
		template<class O_, class C_, int g_>
		class Iterator_ : public IteratorBase<Iterator_<O_, C_, g_>, O_> {
			friend class IteratorBase<Iterator_, O_>;
			friend class OrderedCollection;

		public:
			Iterator_() : Col(NULL), Offset(-1) {
			}
			Iterator_(const Iterator_ &it) : Col(it.Col), Offset(it.Offset) {
			}

		protected:
			Iterator_(C_ &c, int offset=0) : Col(&c), Offset(offset) {
			}

		protected:
			O_ &current() const {
				if(!isvalid())
					throw std::out_of_range("Iterator is not valid.");

				return *Col->list[Offset];
			}

			O_ &current() {
				if(!isvalid())
					throw std::out_of_range("Iterator is not valid.");

				return *Col->list[Offset];
			}

			bool isvalid() const {
				return Offset>=0 && Offset<(*Col->count);
			}

			bool isinrange() const {
				return Offset>=0 || Offset<(*Col->count);
			}

			bool moveby(int amount) {
				//sanity check
				if(amount==0)  return isvalid();

				Offset+=amount;

				return isvalid();
			}

			bool compare(const Iterator_ &it) const {
				return it.Offset==Offset;
			}

			void set(const Iterator_ &it) {
				Col=it.Col;
				Offset=it.Offset;
			}

			int distance(const Iterator_ &it) const {
				return it.Offset-Offset;
			}

			bool isbefore(const Iterator_ &it) const {
				return Offset<it.Offset;
			}

		public:
		//Template compatibility for collections
			//should not work for const iterators
			void Remove() {
				Col->removeat(Offset);
			}

		//Template compatibility for collections
			//should not work for const iterators
			void Delete() {
				Col->deleteat(Offset);
			}

			Iterator_ &operator =(const Iterator_ &iterator) {
				set(iterator);

				return *this;
			}

		protected:
			C_ *Col;
			int Offset;
		};

		template<class O_, class C_, int g_>
		class SearchIterator_ : public IteratorBase<SearchIterator_<O_, C_, g_>, O_> {
			friend class IteratorBase<SearchIterator_, O_>;
			friend class OrderedCollection;

		public:
			SearchIterator_(const SearchIterator_ &it) : Col(it.Col), Offset(it.Offset) {
				if(it.Search)
					Search=new T_(*it.Search);
				else
					Search=NULL;
			}

			SearchIterator_() : Col(NULL), Offset(-1), Search(NULL) {
			}

		protected:
			SearchIterator_(C_ &c, const T_ &search, int offset=0) : 
				 Col(&c), Offset(offset), Search(new T_(search)) {
					 if(offset==-1) {
						 moveby(-1);
					 }
					 else if(!(current()==search)) {
						 moveby(1);
					 }
				 }

		protected:
			O_ &current() const {
				if(!isvalid())
					throw std::out_of_range("Iterator is not valid.");

				return *Col->list[Offset];
			}

			bool isvalid() const {
				return Offset>=0 && Offset<(*Col->count);
			}

			bool moveby(int amount) {
				//sanity check
				if(amount==0)  return isvalid();

				int new_off=Offset;

				if(!isvalid()) {
					if(amount>0) {
						if(Search && Offset<0)
							new_off = 0;
						else //if search is not set then don't do anything
							return false;
					}
					else {
						if(Search && Offset>=*Col->count)
							new_off = *Col->count;
						else //if search is not set then don't do anything
							return false;
					}
				}


				if(amount>0) {
					int l=*Col->count;
					while(amount && ++new_off < l) {
						if(*Col->list[new_off]==*Search)
							amount--;
					}

					Offset=new_off;
					if(new_off == *Col->count) {
						return false;
					} else {
						return true;
					}
				} else {
					while(amount && --new_off >= 0) {
						if(*Col->list[new_off]==*Search)
							amount++;
					}

					Offset=new_off;
					if(new_off == -1) {
						return false;
					} else {
						return true;
					}
				}
			}

			bool compare(const SearchIterator_<O_,C_,g_> &it) const {
				return it.Offset==Offset;
			}

			Iterator_<O_,C_,g_> cast() {
				return Iterator_<O_,C_,g_>(Col, Offset);
			}

			bool isbefore(const SearchIterator_<O_,C_,g_> &it) const {
				return Offset<it.Offset;
			}

			int distance(const SearchIterator_ &it) const {
				return it.Offset-Offset;
			}

		public:
		//Template compatibility for collections
			void Remove() {
				Col->removeat(Offset);
			}

		//Template compatibility for collections
			void Delete() {
				Col->deleteat(Offset);
			}

			operator Iterator_<O_,C_,g_>() const {
				return Iterator_<O_,C_,g_>(Col, Offset);
			}

			~SearchIterator_() {
				if(Search)
					delete Search;

				Search=NULL;
			}
		protected:
			C_ *Col;
			int Offset;
			T_ *Search;
		};

	public:
		typedef Iterator_<      T_,      OrderedCollection, growth>	  Iterator;
		class ConstIterator : public Iterator_<const T_,const OrderedCollection, growth> {
			friend class OrderedCollection;
		public:
			ConstIterator(const Iterator &it) {
				this->Col=it.Col;
				this->Offset=it.Offset;
			}

		protected:
			ConstIterator(const OrderedCollection &c, int offset=0) : Iterator_<const T_,const OrderedCollection, growth>(c, offset) {
			}
		};
		typedef SearchIterator_<      T_,      OrderedCollection, growth>	    SearchIterator;
		class ConstSearchIterator : public SearchIterator_<const T_,const OrderedCollection, growth> {
			friend class OrderedCollection;
		public:
			ConstSearchIterator(const SearchIterator &it) {
				this->Col=it.Col;
				this->Offset=it.Offset;
			}

		protected:
			ConstSearchIterator(const OrderedCollection &c, const T_ &search, int offset=0) : SearchIterator_<const T_,const OrderedCollection, growth>(c, search, offset) {
			}
		};

		//#pragma endregion

	public:
		OrderedCollection() : count(new int(0)) {
			init();
		}

		~OrderedCollection() {
			if( list.getReferenceCount() <=1 ) {
				delete count;
			}
		}

		template <int g_>
		OrderedCollection(const OrderedCollection<T_, g_> &col) : 
		list(col.list), count(col.count)
		{ 	}

	//Template compatibility for collections
		////Returns number of elements
		int GetCount() const {
			return *count;
		}

		////Adds a new item to the end of the list
		void Add(T_* Data) {
			if(*count==list.GetSize())
				grow();

			list[*count]=Data;

			(*count)++;
		}

	//Template compatibility for collections
		////Adds a new item to the end of the list
		void Add(T_& data) {
			Add(&data);
		}

		//this method adds the given object in front of the reference
		void Insert(T_* data, int before) {
			if(before<0)
				return;
			if(before>=*count)
				Add(data);

			if(*count==list.GetSize())
				grow();

			for(int i=*count;i>before;i--)
				list[i]=list[i-1];

			list[before]=data;

			(*count)++;
		}

		//this method adds the given object in front of the reference
		void Insert(T_& data, int before) {
			Insert(&data, before);
		}

		//this method adds the given object in front of the reference
		void Insert(T_* data, const T_ *before) {
			Insert(data, FindLocation(before));
		}

		//this method adds the given object in front of the reference
		void Insert(T_& data, const T_ *before) {
			Insert(&data, FindLocation(before));
		}

		//this method adds the given object in front of the reference
		void Insert(T_* data, const T_ &before) {
			Insert(data, FindLocation(before));
		}

		//this method adds the given object in front of the reference
		void Insert(T_& data, const T_ &before) {
			Insert(&data, FindLocation(before));
		}

		//this method moves the given object in the collection in front of the reference
		void MoveBefore(int index, int before) {
			if(index<0 && index>=*count)
				return;
			if(before<0)
				return;
			if(before>*count)
				before=*count;
			if(index==before)
				return;

			if(index>before) {
				T_ *t=list[index];
				for(int i=index;i>before;i--)
					list[i]=list[i-1];

				list[before]=t;
			}
			else if(before==*count) {
				T_ *t=list[index];

				for(int i=index;i<*count-1;i++)
					list[i]=list[i+1];

				list[(*count-1)]=t;
			}
			else {
				T_ *t=list[index];
				for(int i=index;i<before;i++)
					list[i]=list[i+1];

				list[before]=t;
			}
		}

		void MoveBefore(int index, const T_ *before) {
			MoveBefore(index, FindLocation(before));
		}

		void MoveBefore(int index, const T_ &before) {
			MoveBefore(index, FindLocation(before));
		}

		void MoveBefore(const T_ *index, int before) {
			MoveBefore(FindLocation(index), before);
		}

		void MoveBefore(const T_ *index, const T_ *before) {
			MoveBefore(FindLocation(index), FindLocation(before));
		}

		void MoveBefore(const T_ *index, const T_ &before) {
			MoveBefore(FindLocation(index), FindLocation(before));
		}

		void MoveBefore(const T_ &index, int before) {
			MoveBefore(FindLocation(index), before);
		}

		void MoveBefore(const T_ &index, const T_ *before) {
			MoveBefore(FindLocation(index), FindLocation(before));
		}

		void MoveBefore(const T_ &index, const T_ &before) {
			MoveBefore(FindLocation(index), FindLocation(before));
		}

		template <int g_>
		OrderedCollection &operator =(const OrderedCollection<T_, g_> &col) {
			if(list.getReferenceCount()<=1)
				delete count;

			list=col.list;
			count=col.count;
		}

		////Adds a new item to the end of the list
		void operator +=(T_* Data) {
			Add(Data);
		}
		////Adds a new item to the end of the list
		void operator +=(T_& data) {
			Add(&data);
		}

		////Removes an item from the collection using its index
		void Remove(int Index) {
			int i;

			for(i=Index;i<*count-1;i++)
				list[i]=list[i+1];

			(*count)--;
		}

		////Removes an item from the collection using its pointer
		void Remove(T_ *Item) {
			int i;

			for(i=0;i<list.GetSize();i++) {
				if(list[i]==Item)
					break;
			}

			if(i==*count)
				return;
			
			for(;i<*count-1;i++)
				list[i]=list[i+1];

			(*count)--;
		}

		////Removes an item from the collection using its reference
		void Remove(T_& data) {
			Remove(&data);
		}

		////Removes an item from the collection using its index
		OrderedCollection &operator -=(int Index) {
			Remove(Index);

			return *this;
		}

		////Removes an item from the collection using its pointer
		OrderedCollection &operator -=(T_ *Item) {
			Remove(Item);

			return *this;
		}

		////Removes an item from the collection using its reference
		OrderedCollection &operator -=(T_& data) {
			Remove(&data);

			return *this;
		}

		////Deletes an item from the collection using its index.
		/// Deleting both removes the item from the list and free the item itself.
		void Delete(int Index) {
			int i;

			delete list[Index];

			for(i=Index;i<*count-1;i++)
				list[i]=list[i+1];

			(*count)--;
		}

		////Deletes an item from the collection using its pointer.
		/// Deleting both removes the item from the list and free the item itself.
		void Delete(T_ *Item) {
			int i;

			for(i=0;i<list.GetSize();i++) {
				if(list[i]==Item)
					break;
			}

			if(i==*count)
				return;

			delete list[i];

			for(;i<*count-1;i++)
				list[i]=list[i+1];

			(*count)--;
		}

		////Deletes an item from the collection using its reference.
		/// Deleting both removes the item from the list and free the item itself.
		void Delete(T_& data) {
			Delete(&data);
		}

		////Searches the position of a given item, if not found invalid iterator returned
		Iterator Find(T_ *Item) {
			int i;
			for(i=0;i<list.GetSize();i++) {
				if(Item==list[i])
					return Iterator(*this, i);
			}

			return Iterator(*this, -1);
		}

		////Searches the position of a given item, if not found invalid iterator returned
		Iterator Find(T_ &Item) {
			return Find(&Item);
		}

		////Searches the position of a given item, if not found invalid iterator returned
		ConstIterator Find(const T_ *Item) const {
			int i;
			for(i=0;i<list.GetSize();i++) {
				if(Item==list[i])
					return ConstIterator(*this, i);
			}

			return ConstIterator(*this, -1);
		}

		////Searches the position of a given item, if not found invalid iterator returned
		ConstIterator Find(const T_ &Item) const {
			return Find(&Item);
		}

		////Searches the position of a given item, if not found -1 is returned
		int FindLocation(const T_ *Item) const {
			int i;
			for(i=0;i<list.GetSize();i++) {
				if(Item==list[i])
					return i;
			}

			return -1;
		}

		////Searches the position of a given item, if not found -1 is returned
		int FindLocation(const T_ &Item) const {
			return FindLocation(&Item);
		}

		////Searches the collection for the item that is equal to the given
		/// parameter. If there is more than one, first one is returned.
		/// Start parameter can be used to discover more items. For this function
		/// to compile, you should use a type that supports comparison
		SearchIterator Search(const T_ &Item, Iterator start)  {
			return SearchIterator(*this, Item, start.Offset);
		}

		SearchIterator Search(const T_ &Item)  {
			return SearchIterator(*this, Item, 0);
		}

		ConstSearchIterator Search(const T_ &Item, Iterator start) const {
			return ConstSearchIterator(*this, Item, start.Offset);
		}

		ConstSearchIterator Search(const T_ &Item) const {
			return ConstSearchIterator(*this, Item, 0);
		}

		SearchIterator ReverseSearch(const T_ &Item)  {
			return SearchIterator(*this, Item, -1);
		}

		ConstSearchIterator ReverseSearch(const T_ &Item) const {
			return ConstSearchIterator(*this, Item, -1);
		}

		template<class P_>
		void Sort(P_ predicate=std::less<T_>()) {
			std::sort(list.begin(), list.begin()+*count, elementsorterfrompointer<T_, P_>(predicate));
		}

		void Sort() {
			std::sort(list.begin(), list.begin()+*count, elementsorterfrompointer<T_, std::less<T_>>(std::less<T_>()));
		}

		T_ &Get(int Index) {
			T_ *r=get_(Index);

			if(r==NULL)
				throw std::out_of_range("Index out of range");

			return *r;
		}

		const T_ &Get(int Index) const {
			const T_ *r=get_(Index);

			if(r==NULL)
				throw std::out_of_range("Index out of range");

			return *r;
		}

		////Returns the item at a given index
		T_& operator [] (int Index) {
			return Get(Index);
		}

		//Returns the item at a given index
		const T_& operator [] (int Index) const  {
			return Get(Index);
		}

		////Returns the item at a given index
		T_* operator () (int Index) {
			return get_(Index);
		}

		//Returns the item at a given index
		const T_* operator () (int Index) const {
			return get_(Index);
		}

		Iterator begin() {
			return Iterator(*this, 0);
		}

		Iterator end() {
			return Iterator(*this, *count);
		}

		SearchIterator send() {
			return SearchIterator();
		}

	//Template compatibility for collections, lists
		Iterator First() {
			return Iterator(*this, 0);
		}

	//Template compatibility for collections, lists
		Iterator Last() {
			return Iterator(*this, *count-1);
		}

		ConstIterator begin() const {
			return ConstIterator(*this, 0);
		}

		ConstIterator end() const {
			return ConstIterator(*this, *count);
		}

		ConstSearchIterator send() const {
			return ConstSearchIterator(*this,T_());
		}

	//Template compatibility for collections, lists
		ConstIterator First() const {
			return ConstIterator(*this, 0);
		}

	//Template compatibility for collections, lists
		ConstIterator Last() const {
			return ConstIterator(*this, *count-1);
		}

	//Template compatibility for collections
		////Removes all items from the list, allocated memory for the
		/// list stays
		void Clear() {
			std::memset(list.GetBuffer(),0,sizeof(T_*)*list.GetSize());
			*count=0;
		}

		////Clears the contents of the collection and releases the memory
		/// used for the list. Items are not freed.
		void Collapse() {
			*count=0;

			list.Resize(0);
		}

	//Template compatibility for collections
		////Destroys the entire collection, effectively deleting the contents
		/// and the list {
		void Destroy() {
			int i;
			for(i=0;i<*count;i++)
				if(list[i])
					delete list[i];

			list.Resize(0);
			*count=0;
		}

	//Template compatibility for lists
		////Allocates memory for the given amount of items
		void AllocateFor(int amount) {
			grow(amount);
		}

	protected:
		ManagedBuffer<T_*> list;
		int *count;

		void init() {
			list.Resize(growth);
			std::memset(list.GetBuffer(),0,sizeof(T_*)*list.GetSize());

			*count=0;
		}

		void grow(int amount=growth) {
			list.Resize(list.GetSize()+amount);

			std::memset(list.GetBuffer()+(list.GetSize()-amount),0,sizeof(T_*)*amount);
		}

		void removeat(int absolutepos) {
			Remove(absolutepos);
		}

		void deleteat(int absolutepos) {
			Delete(absolutepos);
		}

		T_ *get_(int Index) {
			if(Index<0 || Index>*count)
				return NULL;

			return list[Index];
		}
	};

} }

