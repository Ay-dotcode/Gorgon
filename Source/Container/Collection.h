/// @file Collection.h contains collection, a vector of references.

#pragma once

#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <algorithm>

namespace Gorgon { 
	namespace Container {

		///	Collection class is an unsorted add/remove list with consistent IDs
		///	for its members. Allows iteration using std methods. 
		///	Also hosts search iteration. This Collection is preferable for small
		///	number of objects, it does not host any optimizations for access speed;
		///	however, it has a very small memory overhead. Total size of the object
		///	is 16 bytes. It can be passed by value, contents will not be duplicated
		///	instead it will ref count the object list. 
		///	
		///	@warning Stored objects are not duplicated nor destroyed at the end
		///	of the collection's life time. To destruct objects that are contained
		///	within use .Destroy() method. This applies for removed objects, use
		///	delete to destruct and remove them	
		template <class T_>
		class Collection {
	
			template<class T_, class P_>
			struct sorter {
				sorter(const P_ &predicate) : predicate(predicate) {}

				bool operator ()(const T_ *left, const T_*right) const {
					if(left==NULL || right==NULL)
						return false;

					return predicate(*left,*right);
				}

				const P_ &predicate;
			};

			//		Iterators
			template<class O_, class C_, int g_>
			class Iterator_ : public IteratorBase<Iterator_<O_, C_>, O_> {
				friend class IteratorBase<Iterator_, O_>;
				friend class Collection;

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

		public:
			typedef Iterator_<      T_,      Collection>	  Iterator;
			class ConstIterator : public Iterator_<const T_,const Collection> {
				friend class Collection;
			public:
				ConstIterator(const Iterator &it) {
					this->Col=it.Col;
					this->Offset=it.Offset;
				}

			protected:
				ConstIterator(const Collection &c, int offset=0) : Iterator_<const T_,const Collection>(c, offset) {
				}
			};

		public:
			Collection() { }

			~Collection() { }

			//Template compatibility for collections
			////Returns number of elements
			int GetCount() const {
				return list.size();
			}

			////Adds a new item to the end of the list
			void Add(T_* Data) {
				list.push_back(Data);
			}

			//Template compatibility for collections
			////Adds a new item to the end of the list
			void Add(T_& data) {
				Add(&data);
			}

			//this method adds the given object infront of the reference
			void Insert(T_* data, int before) {
				if(before<0)
					return;
				if(before>=list.size())
					Add(data);

				for(int i=list.size()-1;i>before;i--)
					list[i]=list[i-1];

				list[before]=data;
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
				if(index<0 && index>=list.size())
					return;
				if(before<0)
					before=list.size();
				if(before>list.size())
					before=list.size();
				if(index==before)
					return;

				if(index>before) {
					T_ *t=list[index];
					for(int i=index;i>before;i--)
						list[i]=list[i-1];

					list[before]=t;
				}
				else if(before==list.size()) {
					T_ *t=list[index];

					for(int i=index;i<list.size()-1;i++)
						list[i]=list[i+1];

					list[list.size()-1]=t;
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

			/// TODO Return an adder interface
			////Adds a new item to the end of the list
			void operator +=(T_* Data) {
				Add(Data);
			}
			////Adds a new item to the end of the list
			void operator +=(T_& data) {
				Add(&data);
			}

			////Removes an item from the collection using its index
			void Remove(int index) {
				list.erase(index);
			}

			////Removes an item from the collection using its pointer
			void Remove(const T_ *Item) {
				int i;

				//TODO use stl find
				for(i=0;i<list.size();i++) {
					if(list[i]==Item)
						break;
				}

				Remove(i);
			}

			////Removes an item from the collection using its reference
			void Remove(const T_ &data) {
				Remove(&data);
			}

			////Removes an item from the collection using its index
			Collection &operator -=(const int Index) {
				Remove(Index);

				return *this;
			}

			////Removes an item from the collection using its pointer
			Collection &operator -=(const T_ *Item) {
				Remove(Item);

				return *this;
			}

			////Removes an item from the collection using its reference
			Collection &operator -=(const T_& data) {
				Remove(&data);

				return *this;
			}

			////Deletes an item from the collection using its index.
			/// Deleting both removes the item from the list and free the item itself.
			void Delete(int index) {
				delete list[index];

				list.erase(list.begin()+index);
			}

			////Deletes an item from the collection using its pointer.
			/// Deleting both removes the item from the list and free the item itself.
			/// If given item does not exists, it is not modified.
			void Delete(const T_ *Item) {
				int i;

				for(i=0;i<list.size();i++) {
					if(list[i]==Item)
						break;
				}

				if(i==list.size())
					return;

				Delete(i);
			}

			////Deletes an item from the collection using its reference.
			/// Deleting both removes the item from the list and free the item itself.
			void Delete(T_& data) {
				Delete(&data);
			}

			////Searches the position of a given item, if not found end iterator returned
			Iterator Find(const T_ *Item) {
				int i;
				for(i=0;i<list.GetSize();i++) {
					if(Item==list[i])
						return Iterator(*this, i);
				}

				return Iterator(*this, -1);
			}

			////Searches the position of a given item, if not found end iterator returned
			Iterator Find(const T_ &Item) {
				return Find(&Item);
			}

			////Searches the position of a given item, if not found end iterator returned
			ConstIterator Find(const T_ *Item) const {
				int i;
				for(i=0;i<list.GetSize();i++) {
					if(Item==list[i])
						return ConstIterator(*this, i);
				}

				return ConstIterator(*this, -1);
			}

			////Searches the position of a given item, if not found end iterator returned
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

			template<class P_>
			void Sort(P_ predicate=P_()) {
				std::sort(list.begin(), list.end(), elementsorterfrompointer<T_, P_>(predicate));
			}

			void Sort() {
				std::sort(list.begin(), list.end(), elementsorterfrompointer<T_, std::less<T_>>(std::less<T_>()));
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
				return Iterator(*this, list.size());
			}

			//Template compatibility for collections, lists
			Iterator First() {
				return Iterator(*this, 0);
			}

			//Template compatibility for collections, lists
			Iterator Last() {
				return Iterator(*this, list.size()-1);
			}

			ConstIterator begin() const {
				return ConstIterator(*this, 0);
			}

			ConstIterator end() const {
				return ConstIterator(*this, list.size());
			}

			//Template compatibility for collections, lists
			ConstIterator First() const {
				return ConstIterator(*this, 0);
			}

			//Template compatibility for collections, lists
			ConstIterator Last() const {
				return ConstIterator(*this, list.size()-1);
			}

			//Template compatibility for collections
			////Removes all items from the list, allocated memory for the
			/// list stays
			void Clear() {
				list.clear();
			}

			////Clears the contents of the collection and releases the memory
			/// used for the list. Items are not freed.
			void Collapse() {
				std::vector<T_*> newlist;
				using std::swap;
				
				swap(newlist, list);	
			}

			//Template compatibility for collections
			////Destroys the entire collection, effectively deleting the contents
			/// and the list {
			void Destroy() {
				int i;
				for(auto e : list)
					delete e;

				std::vector<T_*> newlist;
				using std::swap;
				
				swap(newlist, list);
			}

			//Template compatibility for lists
			////Allocates memory for the given amount of items
			void Reserve(int amount) {
				list.reserve(amount);
			}

		protected:
			std::vector<T_ *> list;

			void removeat(int absolutepos) {
				Remove(absolutepos);
			}

			void deleteat(int absolutepos) {
				Delete(absolutepos);
			}

			T_ *get_(int Index) {
				if(Index<0 || Index>=list.size())
					return NULL;

				return list[Index];
			}

			const T_ *get_(int Index) const {
				if(Index<0 || Index>list.size())
					return NULL;

				return list[Index];
			}
		};

	} 
}

