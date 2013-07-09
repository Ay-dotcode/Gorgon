//DESCRIPTION
//	Collection class is an unsorted add/remove list with consistent IDs
//	for its members. Allows iteration using std methods. 
//	Also hosts search iteration. This Collection is preferable for small
//	number of objects, it does not host any optimizations for access speed;
//	however, it has a very small memory overhead. Total size of the object
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

	template <class T_, int growth=50>
	class Collection {
		template<class T__, int g__>
		friend class Collection;

//		Iterators
		template<class O_, class C_, int g_>
		class Iterator_ : public IteratorBase<Iterator_<O_, C_, g_>, O_> {
			friend class IteratorBase<Iterator_, O_>;
			friend class Collection;

		public:
			Iterator_() : Col(NULL), Offset(-1) {}
			Iterator_(const Iterator_ &it) : Col(it.Col), Offset(it.Offset) {
			}

		protected:
			Iterator_(C_ &c, int offset=0) : Col(&c), Offset(offset) {
				if(offset==0 && !isvalid()) moveby(1);
			}

		protected:
			O_ &current() const {
				if(!isvalid())
					throw std::out_of_range("Iterator is not valid.");

				if(!Col->list[Offset])
					throw std::out_of_range("Iterator is not valid.");


				return *Col->list[Offset];
			}

			O_ &current() {
				if(!isvalid())
					throw std::out_of_range("Iterator is not valid.");

				if(!Col->list[Offset])
					if(!moveby(1))
						throw std::out_of_range("Iterator is not valid.");


				return *Col->list[Offset];
			}

			bool isvalid() const {
				return Offset!=-1 && (*Col->count)!=0 && Col->list[Offset];
			}

			bool isinrange() const {
				return Offset!=-1 && (*Col->count)!=0;
			}

			bool moveby(int amount) {
				//sanity check
				if(amount==0)  return isvalid();

				int new_off=Offset;

				if(!isinrange()) {
					if(amount>0)
						//new_off = -1;
						return false; //?
					else
						new_off = Col->list.GetSize();
				}


				if(amount>0) {
					int l=Col->list.GetSize();
					while(amount && ++new_off < l) {
						if(Col->list[new_off]) 
							amount--;
					}

					if(new_off == Col->list.GetSize()) {
						Offset=-1;
						return false;
					} else {
						Offset=new_off;
						return true;
					}
				} else {
					while(amount && --new_off >= 0) {
						if(Col->list[new_off]) 
							amount++;
					}

					if(new_off == -1) {
						Offset=-1;
						return false;
					} else {
						Offset=new_off;
						return true;
					}
				}
			}

			bool compare(const Iterator_ &it) const {
				return it.Offset==Offset;
			}

			void set(const Iterator_ &it) {
				Col=it.Col;
				Offset=it.Offset;
			}

			int distance(const Iterator_ &it) const {
				int new_off=Offset;
				int dist=0;
				int l=Col->list.GetSize();

				if(it.Offset==Offset)
					return 0;

				if(it.Offset==-1) {
					while(new_off < l) {
						if(Col->list[new_off]) 
							dist++;

						new_off++;
					}

					return dist;
				}


				if(it.Offset-Offset >= 0) {
					while(it.Offset-Offset && ++new_off < l) {
						if(Col->list[new_off]) 
							dist++;
					}
				}
				else {
					while(it.Offset-Offset && --new_off >= 0) {
						if(Col->list[new_off]) 
							dist--;
					}
				}

				return dist;
			}

			bool isbefore(const Iterator_ &it) const {
				if(Offset==-1)
					return false;

				if(it.Offset <= Offset) 
					return false;

				int new_off=Offset;
				int l=Col->list.GetSize();
				while(new_off < it.Offset && new_off < l) {
					if(Col->list[new_off])
						return true;
				}

				return false;
			}

		public:
		//TODO ! Template compatibility for collections
			//should not work for const iterators
			void Remove() {
				Col->removeat(Offset);
			}

		//TODO ! Template compatibility for collections
			//should not work for const iterators
			void Delete() {
				Col->deleteat(Offset);
			}

			Iterator_ &operator =(const Iterator_ &iterator) {
				set(iterator);

				return *this;
			}

			int Location() {
				int l=Col->GetCount();
				int location;

				if(Offset > l/2) {
					int newoff=Offset;
					while(newoff<l) {
						if(Col->list[newoff])
							location++;

						newoff++;
					}

					return l-location;
				} else {
					int newoff=Offset;
					while(--newoff>=0) {
						if(Col->list[newoff])
							location++;
					}

					return location;
				}
			}

		protected:
			C_ *Col;
			int Offset;
		};

		template<class O_, class C_, int g_>
		class SearchIterator_ : public IteratorBase<SearchIterator_<O_, C_, g_>, O_> {
			friend class IteratorBase<SearchIterator_, O_>;
			friend class Collection;

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
				return Offset!=-1;
			}

			bool moveby(int amount) {
				//sanity check
				if(amount==0)  return isvalid();

				int new_off=Offset;

				if(!isvalid()) {
					if(amount>0)
						//new_off = -1;
						return false; //?
					else if(Search)
						new_off = Col->list.GetSize();
					else //if search is not set then don't do anything
						return false;
				}


				if(amount>0) {
					int l=Col->list.GetCount();
					while(amount && ++new_off < l) {
						if(Col->list[new_off]) 
							if(*Col->list[new_off]==*Search)
								amount--;
					}

					if(new_off == Col->list.GetCount()) {
						Offset=-1;
						return false;
					} else {
						Offset=new_off;
						return true;
					}
				} else {
					while(amount && --new_off >= 0) {
						if(Col->list[new_off]) 
							if(*Col->list[new_off]==*Search)
								amount++;
					}

					if(new_off == -1) {
						Offset=-1;
						return false;
					} else {
						Offset=new_off;
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
				if(Offset==-1)
					return false;

				if(it.Offset <= Offset) 
					return false;

				int new_off=Offset;
				int l=Col->list.GetCount();
				while(new_off < it.Offset && new_off < l) {
					if(Col->list[new_off])
						return true;
				}

				return false;
			}

			int distance(const SearchIterator_ &it) const {
				int new_off=Offset;
				int dist=0;
				int l=Col->list.GetCount();

				if(it.Offset==Offset)
					return 0;

				if(Search==NULL)
					return 0;

				if(it.Offset==-1) {
					while(new_off < l) {
						if(Col->list[new_off] && Col->list[new_off]==Search) 
							dist++;

						new_off++;
					}

					return dist;
				}


				if(it.Offset-Offset >= 0) {
					while(it.Offset-Offset && ++new_off < l) {
						if(Col->list[new_off] && Col->list[new_off]==Search) 
							dist++;
					}
				}
				else {
					while(it.Offset-Offset && --new_off >= 0) {
						if(Col->list[new_off] && Col->list[new_off]==Search) 
							dist--;
					}
				}

				return dist;
			}

		public:
		//TODO ! Template compatibility for collections
			void Remove() {
				Col->removeat(Offset);
			}

		//TODO ! Template compatibility for collections
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
		typedef Iterator_<      T_,      Collection, growth>	  Iterator;
		class ConstIterator : public Iterator_<const T_,const Collection, growth> {
			friend class Collection;
		public:
			ConstIterator(const Iterator &it) {
				this->Col=it.Col;
				this->Offset=it.Offset;
			}

		protected:
			ConstIterator(const Collection &c, int offset=0) : Iterator_<const T_,const Collection, growth>(c, offset) {
			}
		};
		typedef SearchIterator_<      T_,      Collection, growth>	    SearchIterator;
		class ConstSearchIterator : public SearchIterator_<const T_,const Collection, growth> {
			friend class Collection;
		public:
			ConstSearchIterator(const SearchIterator &it) {
				this->Col=it.Col;
				this->Offset=it.Offset;
			}

		protected:
			ConstSearchIterator(const Collection &c, const T_ &search, int offset=0) : SearchIterator_<const T_,const Collection, growth>(c, search, offset) {
			}
		};

//#pragma endregion

	public:
		Collection() : count(new int(0)) {
			init();
		}

		~Collection() {
			if( list.getReferenceCount() <=1 ) {
				delete count;
			}
		}

		template <int g_>
		Collection(Collection<T_, g_> &col) : 
		list(col.list), count(col.count)
		{ 	}

	//TODO ! Template compatibility for collections
		////Returns number of elements
		int GetCount() const {
			return *count;
		}

		////Adds a new item to the list and returns its index.
		/// When an item is removed its index is released and given
		/// to the next object. However, when an index is given it
		/// is not changed until the object is removed
		int Add(T_* Data) {
			if(*count==list.GetSize())
				grow();

			int i;
			for(i=0;i<list.GetSize();i++)
				if(list[i]==0) {
					list[i]=Data;
					break;
				}

				(*count)++;

				return i;
		}

	//TODO ! Template compatibility for collections
		////Adds a new item to the list and returns its index.
		/// When an item is removed its index is released and given
		/// to the next object. However, when an index is given it
		/// is not changed until the object is removed
		int Add(T_& data) {
			return Add(&data);
		}

		template <int g_>
		Collection &operator =(const Collection<T_, g_> &col) {
			if(list.getReferenceCount()<=1)
				delete count;

			list=col.list;
			count=col.count;
		}

		////Adds a new item to the list and returns its index.
		/// When an item is removed its index is released and given
		/// to the next object. However, when an index is given it
		/// is not changed until the object is removed
		int operator +=(T_* Data) {
			return Add(Data);
		}
		////Adds a new item to the list and returns its index.
		/// When an item is removed its index is released and given
		/// to the next object. However, when an index is given it
		/// is not changed until the object is removed
		int operator +=(T_& data) {
			return Add(&data);
		}

		////Removes an item from the collection using its index
		void Remove(int Index) {
			int i,j;
			j=0;

			for(i=0;i<list.GetSize();i++) {
				if(list[i]) j++;
				if((j-1)==Index) {
					list[i]=0;
					break;
				}
			}

			(*count)--;
		}

		////Removes an item from the collection using its pointer
		void Remove(T_ *Item) {
			int i,j;
			j=0;

			for(i=0;i<list.GetSize();i++) {
				if(list[i]) j++;
				if(list[i]==Item) {
					list[i]=0;
					break;
				}
			}

			(*count)--;
		}

		////Removes an item from the collection using its reference
		void Remove(T_& data) {
			Remove(&data);
		}

		////Removes an item from the collection using its index
		Collection &operator -=(int Index) {
			Remove(Index);

			return *this;
		}

		////Removes an item from the collection using its pointer
		Collection &operator -=(T_ *Item) {
			Remove(Item);

			return *this;
		}

		////Removes an item from the collection using its reference
		Collection &operator -=(T_& data) {
			Remove(&data);

			return *this;
		}

		////Deletes an item from the collection using its index.
		/// Deleting both removes the item from the list and free the item itself.
		void Delete(int Index) {
			int i,j;
			j=0;

			for(i=0;i<list.GetSize();i++) {
				if(list[i]) j++;
				if((j-1)==Index) {
					delete (list[i]);
					list[i]=0;
					break;
				}
			}
			(*count)--;
		}

		////Deletes an item from the collection using its pointer.
		/// Deleting both removes the item from the list and free the item itself.
		void Delete(T_ *Item) {
			int i,j;
			j=0;

			for(i=0;i<list.GetSize();i++) {
				if(list[i]) j++;
				if(list[i]==Item) {
					delete (list[i]);
					list[i]=0;
					break;
				}
			}

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
		ConstIterator Find(T_ *Item) const {
			int i;
			for(i=0;i<list.GetSize();i++) {
				if(Item==list[i])
					return ConstIterator(*this, i);
			}

			return ConstIterator(*this, -1);
		}

		////Searches the position of a given item, if not found invalid iterator returned
		ConstIterator Find(T_ &Item) const {
			return Find(&Item);
		}

		////Searches the position of a given item, if not found -1 is returned
		int FindLocation(T_ *Item) const {
			int i, p=0;
			for(i=0;i<list.GetSize();i++) {
				if(Item==list[i])
					return p;
				else if(list[i])
					p++;
			}

			return -1;
		}

		////Searches the position of a given item, if not found -1 is returned
		int FindLocation(T_ &Item) const {
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
			return Iterator(*this, -1);
		}

		SearchIterator send() {
			return SearchIterator();
		}

	//TODO ! Template compatibility for collections, lists
		Iterator First() {
			return Iterator(*this, 0);
		}

	//TODO ! Template compatibility for collections, lists
		Iterator Last() {
			return --(Iterator(*this, -1));
		}

		ConstIterator begin() const {
			return ConstIterator(*this, 0);
		}

		ConstIterator end() const {
			return ConstIterator(*this, -1);
		}

		ConstSearchIterator send() const {
			return ConstSearchIterator();
		}

	//TODO ! Template compatibility for collections, lists
		ConstIterator First() const {
			return ConstIterator(*this, 0);
		}

	//TODO ! Template compatibility for collections, lists
		ConstIterator Last() const {
			return --(ConstIterator(*this, -1));
		}

	//TODO ! Template compatibility for collections
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

	//TODO ! Template compatibility for collections
		////Destroys the entire collection, effectively deleting the contents
		/// and the list {
		void Destroy() {
			int i;
			for(i=0;i<list.GetSize();i++)
				if(list[i])
					delete list[i];

			list.Resize(0);
			*count=0;
		}

	//TODO ! Template compatibility for lists
		////Allocates memory for the given amount of items
		void AllocateFor(int amount) {
			grow(amount);
		}

		ManagedBuffer<T_*> list;
		int *count;

	protected:

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
			list[absolutepos]=NULL;
			(*count)--;
		}

		void deleteat(int absolutepos) {
			delete list[absolutepos];
			list[absolutepos]=NULL;
			(*count)--;
		}

		T_ *get_(int Index) {
			if(Index<0 || Index>list.GetSize())
				return NULL;

			int i,j;
			j=0;

			if(Index>list.GetSize()) {
				for(i=list.GetSize()-1;i>=0;i--) {
					if(list[i]) j++;
					if((j-1)==Index) {
						return list[i];
					}
				}
			} 
			else {
				for(i=0;i<list.GetSize();i++) {
					if(list[i]) j++;
					if((j-1)==Index) {
						return list[i];
					}
				}
			}

			return NULL;
		}
	};

	template <class T_, int growth=50>
	class ConstCollection {
		template<class T__, int g__>
		friend class ConstCollection;

		//		Iterators
		template<class O_, class C_, int g_>
		class Iterator_ : public IteratorBase<Iterator_<O_, C_, g_>, O_> {
			friend class IteratorBase<Iterator_, O_>;
			friend class ConstCollection;

		public:
			Iterator_() : Col(NULL), Offset(-1) { }
			Iterator_(const Iterator_ &it) : Col(it.Col), Offset(it.Offset) {
			}

		protected:
			Iterator_(C_ &c, int offset=0) : Col(&c), Offset(offset) {
			}

		protected:
			O_ &current() const {
				if(!isvalid())
					throw std::out_of_range("Iterator is not valid.");

				if(!Col->list[Offset])
					throw std::out_of_range("Iterator is not valid.");


				return *Col->list[Offset];
			}

			O_ &current() {
				if(!isvalid())
					throw std::out_of_range("Iterator is not valid.");

				if(!Col->list[Offset])
					if(!moveby(1))
						throw std::out_of_range("Iterator is not valid.");


				return *Col->list[Offset];
			}

			bool isvalid() const {
				return Offset!=-1 && (*Col->count)!=0 && Col->list[Offset];
			}

			bool isinrange() const {
				return Offset!=-1 && (*Col->count)!=0;
			}

			bool moveby(int amount) {
				//sanity check
				if(amount==0)  return isvalid();

				int new_off=Offset;

				if(!isinrange()) {
					if(amount>0)
						//new_off = -1;
						return false; //?
					else
						new_off = Col->list.GetSize();
				}


				if(amount>0) {
					int l=Col->list.GetSize();
					while(amount && ++new_off < l) {
						if(Col->list[new_off]) 
							amount--;
					}

					if(new_off == Col->list.GetSize()) {
						Offset=-1;
						return false;
					} else {
						Offset=new_off;
						return true;
					}
				} else {
					while(amount && --new_off >= 0) {
						if(Col->list[new_off]) 
							amount++;
					}

					if(new_off == -1) {
						Offset=-1;
						return false;
					} else {
						Offset=new_off;
						return true;
					}
				}
			}

			bool compare(const Iterator_ &it) const {
				return it.Offset==Offset;
			}

			void set(const Iterator_ &it) {
				Col=it.Col;
				Offset=it.Offset;
			}

			int distance(const Iterator_ &it) const {
				int new_off=Offset;
				int dist=0;
				int l=Col->list.GetSize();

				if(it.Offset==Offset)
					return 0;

				if(it.Offset==-1) {
					while(new_off < l) {
						if(Col->list[new_off]) 
							dist++;

						new_off++;
					}

					return dist;
				}


				if(it.Offset-Offset >= 0) {
					while(it.Offset-Offset && ++new_off < l) {
						if(Col->list[new_off]) 
							dist++;
					}
				}
				else {
					while(it.Offset-Offset && --new_off >= 0) {
						if(Col->list[new_off]) 
							dist--;
					}
				}

				return dist;
			}

			bool isbefore(const Iterator_ &it) const {
				if(Offset==-1)
					return false;

				if(it.Offset <= Offset) 
					return false;

				int new_off=Offset;
				int l=Col->list.GetCount();
				while(new_off < it.Offset && new_off < l) {
					if(Col->list[new_off])
						return true;
				}

				return false;
			}

		public:
			Iterator_ &operator =(const Iterator_ &iterator) {
				set(iterator);

				return *this;
			}

			int Location() {
				int l=Col->GetCount();
				int location;

				if(Offset > l/2) {
					int newoff=Offset;
					while(newoff<l) {
						if(Col->list[newoff])
							location++;

						newoff++;
					}

					return l-location;
				} else {
					int newoff=Offset;
					while(--newoff>=0) {
						if(Col->list[newoff])
							location++;
					}

					return location;
				}
			}

		protected:
			C_ *Col;
			int Offset;
		};

		template<class O_, class C_, int g_>
		class SearchIterator_ : public IteratorBase<SearchIterator_<O_, C_, g_>, O_> {
			friend class IteratorBase<SearchIterator_, O_>;
			friend class ConstCollection;

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
				return Offset!=-1;
			}

			bool moveby(int amount) {
				//sanity check
				if(amount==0)  return isvalid();

				int new_off=Offset;

				if(!isvalid()) {
					if(amount>0)
						//new_off = -1;
						return false; //?
					else if(Search)
						new_off = Col->list.GetSize();
					else //if search is not set then don't do anything
						return false;
				}


				if(amount>0) {
					int l=Col->list.GetCount();
					while(amount && ++new_off < l) {
						if(Col->list[new_off]) 
							if(*Col->list[new_off]==*Search)
								amount--;
					}

					if(new_off == Col->list.GetCount()) {
						Offset=-1;
						return false;
					} else {
						Offset=new_off;
						return true;
					}
				} else {
					while(amount && --new_off >= 0) {
						if(Col->list[new_off]) 
							if(*Col->list[new_off]==*Search)
								amount++;
					}

					if(new_off == -1) {
						Offset=-1;
						return false;
					} else {
						Offset=new_off;
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
				if(Offset==-1)
					return false;

				if(it.Offset <= Offset) 
					return false;

				int new_off=Offset;
				int l=Col->list.GetCount();
				while(new_off < it.Offset && new_off < l) {
					if(Col->list[new_off])
						return true;
				}

				return false;
			}

			int distance(const SearchIterator_ &it) const {
				int new_off=Offset;
				int dist=0;
				int l=Col->list.GetCount();

				if(it.Offset==Offset)
					return 0;

				if(Search==NULL)
					return 0;

				if(it.Offset==-1) {
					while(new_off < l) {
						if(Col->list[new_off] && Col->list[new_off]==Search) 
							dist++;

						new_off++;
					}

					return dist;
				}


				if(it.Offset-Offset >= 0) {
					while(it.Offset-Offset && ++new_off < l) {
						if(Col->list[new_off] && Col->list[new_off]==Search) 
							dist++;
					}
				}
				else {
					while(it.Offset-Offset && --new_off >= 0) {
						if(Col->list[new_off] && Col->list[new_off]==Search) 
							dist--;
					}
				}

				return dist;
			}

		public:
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
		typedef Iterator_<      T_,      ConstCollection, growth>	  Iterator;
		class ConstIterator : public Iterator_<const T_,const ConstCollection, growth> {
			friend class ConstCollection;
		public:
			ConstIterator(const Iterator &it) {
				this->Col=it.Col;
				this->Offset=it.Offset;
			}

		protected:
			ConstIterator(const ConstCollection &c, int offset=0) : Iterator_<const T_,const ConstCollection, growth>(c, offset) {
			}
		};
		typedef SearchIterator_<      T_,      ConstCollection, growth>	    SearchIterator;
		class ConstSearchIterator : public SearchIterator_<const T_,const ConstCollection, growth> {
			friend class ConstCollection;
		public:
			ConstSearchIterator(const SearchIterator &it) {
				this->Col=it.Col;
				this->Offset=it.Offset;
			}

		protected:
			ConstSearchIterator(const ConstCollection &c, const T_ &search, int offset=0) : SearchIterator_<const T_,const ConstCollection, growth>(c, search, offset) {
			}
		};

		//#pragma endregion

	public:
		template <int g_>
		ConstCollection(const Collection<T_, g_> &col) : 
		list(col.list), count(col.count)
		{ 	}

		template <int g_>
		ConstCollection(const ConstCollection<T_, g_> &col) : 
		list(col.list), count(col.count)
		{ 	}

	//TODO ! Template compatibility for collections
		////Returns number of elements
		int GetCount() const {
			return *count;
		}

		template <int g_>
		ConstCollection &operator =(const Collection<T_, g_> &col) {
			if(list.getReferenceCount()<=1)
				delete count;

			list=col.list;
			count=col.count;
			
			return *this;
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
		ConstIterator Find(T_ *Item) const {
			int i;
			for(i=0;i<list.GetSize();i++) {
				if(Item==list[i])
					return ConstIterator(*this, i);
			}

			return ConstIterator(*this, -1);
		}

		////Searches the position of a given item, if not found invalid iterator returned
		ConstIterator Find(T_ &Item) const {
			return Find(&Item);
		}

		////Searches the position of a given item, if not found -1 is returned
		int FindLocation(T_ *Item) const {
			int i, p=0;
			for(i=0;i<list.GetSize();i++) {
				if(Item==list[i])
					return p;
				else if(list[i])
					p++;
			}

			return -1;
		}

		////Searches the position of a given item, if not found -1 is returned
		int FindLocation(T_ &Item) const {
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
			return Iterator(*this, -1);
		}

		SearchIterator send() {
			return SearchIterator();
		}

	//TODO ! Template compatibility for collections, lists
		Iterator First() {
			return Iterator(*this, 0);
		}

	//TODO ! Template compatibility for collections, lists
		Iterator Last() {
			return --(Iterator(*this, -1));
		}

		ConstIterator begin() const {
			return ConstIterator(*this, 0);
		}

		ConstIterator end() const {
			return ConstIterator(*this, -1);
		}

		ConstSearchIterator send() const {
			return ConstSearchIterator();
		}

	//TODO ! Template compatibility for collections, lists
		ConstIterator First() const {
			return ConstIterator(*this, 0);
		}

	//TODO ! Template compatibility for collections, lists
		ConstIterator Last() const {
			return --(ConstIterator(*this, -1));
		}

		~ConstCollection() {
			if(list.getReferenceCount()<=1)
				delete count;
		}

	protected:
		const ManagedBuffer<T_*> list;
		int *count;

		T_ *get_(int Index) {
			if(Index<0 || Index>list.GetSize())
				return NULL;

			int i,j;
			j=0;

			if(Index>list.GetSize()) {
				for(i=list.GetSize()-1;i>=0;i--) {
					if(list[i]) j++;
					if((j-1)==Index) {
						return list[i];
					}
				}
			} 
			else {
				for(i=0;i<list.GetSize();i++) {
					if(list[i]) j++;
					if((j-1)==Index) {
						return list[i];
					}
				}
			}

			return NULL;
		}

		const T_ *get_(int Index) const {
			if(Index<0 || Index>list.GetSize())
				return NULL;

			int i,j;
			j=0;

			if(Index>list.GetSize()) {
				for(i=list.GetSize()-1;i>=0;i--) {
					if(list[i]) j++;
					if((j-1)==Index) {
						return list[i];
					}
				}
			} 
			else {
				for(i=0;i<list.GetSize();i++) {
					if(list[i]) j++;
					if((j-1)==Index) {
						return list[i];
					}
				}
			}

			return NULL;
		}
	};
} }

