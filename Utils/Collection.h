//DESCRIPTION
//	Collection class is an unsorted add/remove list with consistent IDs
//	for its members. Allows iteration through foreach macro. Also hosts
//	search function

//REQUIRES:
//	gge::utils::private::Iterator
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
	namespace prvt {
		template <class T_>
		class CollectionIterator;
	}


	template <class T_, int growth=50>
	class Collection {
		template<class T__, int g__>
		friend class Collection;
	public:
		////The amount of growth for each step

		Collection() : count(new int(0)) {
			init();
		}

		~Collection() {
			if( list.getReferenceCount() <=1 ) {
				delete count;
			}
		}

		template <int g_>
		Collection(const Collection<T_, g_> &col) : 
		list(col.list), count(col.count)
		{ 	}

		////Returns number of elements
		int getCount() {
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

			(count)--;
		}

		////Deletes an item from the collection using its reference.
		/// Deleting both removes the item from the list and free the item itself.
		void Delete(T_& data) {
			Delete(&data);
		}

		////Searches the position of a given item, if not found -1 is returned
		int Find(T_ *Item) {
			int i, j=0;
			for(i=0;i<list.GetSize();i++) {
				if(Item==list[i])
					return j;

				if(list[i]) j++;
			}

			return -1;
		}
		////Searches the position of a given item, if not found -1 is returned
		int Find(T_ &Item) {
			return Find(&Item);
		}

		////Searches the collection for the item that is equal to the given
		/// parameter. If there is more than one, first one is returned.
		/// Start parameter can be used to discover more items. For this function
		/// to compile, you should use a type that supports comparison
		int Search(T_ &Item, int start=0) {
			int i, j=0;
			for(i=start;i<list.GetSize();i++) {
				if(Item==*list[i])
					return j;

				if(list[i]) j++;
			}

			return -1;
		}

		////Searches the collection for the item that is equal to the given
		/// parameter. If there is more than one, first one is returned.
		/// Start parameter can be used to discover more items. For this function
		/// to compile, you should use a type that supports comparison
		int ReverseSearch(T_ &Item, int start=-1) {
			int i, j=*count-1;

			if(start==-1)	
				start=*count-1;

			for(i=list.GetSize();i>=0;i--) {
				if(j<=start && Item==*list[i])
					return j;

				if(list[i]) j--;
			}

			return -1;
		}


		////Searches the collection for the item that is equal to the given
		/// parameter. If there is more than one, first one is returned.
		/// Start parameter can be used to discover more items. This variant 
		/// allows you to specify comparator function, UNTESTED
		template <bool (*F_)(T_ &,T_&)>
		int Search(T_ &Item, int start=0) {
			int i, j=0;
			for(i=start;i<list.GetSize();i++) {
				if(F(Item,*list[i]))
					return j;

				if(list[i]) j++;
			}

			return -1;
		}


		////Returns the item at a given index
		T_* operator [] (int Index) {
			if(Index<0 || Index>list.GetSize())
				return NULL;

			int i,j;
			j=0;

			for(i=0;i<list.GetSize();i++) {
				if(list[i]) j++;
				if((j-1)==Index) {
					return list[i];
				}
			}

			return NULL;
		}

		////Returns the item at a given index
		T_& operator () (int Index) {
			if(Index<0 || Index>list.GetSize())
				throw std::out_of_range("Index requested is out of range");

			int i,j;
			j=0;

			for(i=0;i<list.GetSize();i++) {
				if(list[i]) j++;
				if((j-1)==Index) {
					return *(list[i]);
				}
			}

			throw std::out_of_range("Index requested is out of range");
		}

		prvt::CollectionIterator<T_> &ForwardIterator() {
			return prvt::CollectionIterator<T_>(list, *count, false);
		}

		prvt::CollectionIterator<T_> &BackwardIterator() {
			return prvt::CollectionIterator<T_>(list, *count, true);
		}

		operator IIterator<T_>*() {
			prvt::CollectionIterator<T_> *it=new prvt::CollectionIterator<T_>(list, *count, false, true);

			return it;
		}

		////Restarts the iteration, this function should be called before
		/// using next and previous functions.
		///@reverse: This parameter denotes the direction of the iteration
		void ResetIteration(bool reverse=false) {
			if(reverse)
				lastservedindex=*count;
			else
				lastservedindex=-1;
		}

		////Returns the next item in the collection, moving
		/// iteration pointer to the next item. ResetIteration 
		/// should be called prior to this function
		T_* next() {
			if(*count==0)
				return NULL;

getnext:
			if(lastservedindex+1>=list.GetSize())
				return NULL;

			lastservedindex++;
			if(!list[lastservedindex])
				goto getnext;

			return list[lastservedindex];
		}

		////Returns the previous item in the collection, moving
		/// iteration pointer to the previous item. ResetIteration 
		/// should be called prior to this function
		T_* previous() {
			if(*count==0)
				return NULL;

getprev:
			if(lastservedindex-1<0)
				return NULL;

			lastservedindex--;
			if(!list[lastservedindex])
				goto getprev;

			return list[lastservedindex];
		}

		////Removes all items from the list, allocated memory for the
		/// list stays
		void Clear() {
			std::memset(list.GetBuffer(),0,sizeof(T_*)*list.GetSize());
			*count=0;
		}

		////Clears the contents of the collection and releases the memory
		/// used for the list. Items are not freed.
		void Collapse() {
			Clear();

			list.Resize(0);
		}

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


		////Allocates memory for the given amount of items
		void AllocateFor(int amount) {
			list.Resize(list.GetSize()+amount);
			std::memset(list.GetBuffer()+(list.GetSize()-amount)*sizeof(T_*), 0, sizeof(T_*)*amount);
		}

	private:
		ManagedBuffer<T_*> list;
		int *count;
		int lastservedindex;

		void init() {
			list.Resize(growth);
			std::memset(list.GetBuffer(),0,sizeof(T_*)*list.GetSize());

			*count=0;
			lastservedindex=-1;
		}

		void grow() {
			list.Resize(list.GetSize()+growth);

			std::memset(list.GetBuffer()+(list.GetSize()-growth)*sizeof(T_*),0,sizeof(T_*)*growth);
		}
	};


	namespace prvt {
		template <class T_>
		class CollectionIterator : public IIterator<T_> {
		public:
			CollectionIterator(ManagedBuffer<T_*> &list, int count, bool reverse=false, bool autodestroy=false) : 
			  list(list), count(count), reverse(reverse), autodestroy(autodestroy)
			  {
				  if(reverse)
					  lastservedindex=count;
				  else
					  lastservedindex=-1;
			  }

			  virtual T_ *get() {
				  if(reverse)
					  return previous();
				  else
					  return next();
			  }

			  virtual T_ *peek() {
				  if(reverse)
					  return peekprevious();
				  else
					  return peeknext();

			  }

			  T_ *peeknext() {
				  if(count==0)
					  return NULL;

				  int l=lastservedindex;

				getnext:
				  if(l+1>=list.GetSize())
					  return NULL;

				  l++;
				  if(!list[l])
					  goto getnext;

				  return list[l];
			  }

			  T_* peekprevious() {
				  if(count==0)
					  return NULL;

				  int l=lastservedindex;

				getprev:
				  if(l-1<0)
					  return NULL;

				  l--;
				  if(!list[l])
					  goto getprev;

				  return list[l];
			  }

			  T_ *next() {
				  if(count==0) {
					  if(autodestroy)
						  delete this;
					  return NULL;
				  }

getnext:
				  if(lastservedindex+1>=list.GetSize()) {
					  if(autodestroy)
						  delete this;
					  return NULL;
				  }

				  lastservedindex++;
				  if(!list[lastservedindex])
					  goto getnext;

				  return list[lastservedindex];
			  }

			  T_* previous() {
				  if(count==0) {
					  if(autodestroy)
						  delete this;
					  return NULL;
				  }

getprev:
				  if(lastservedindex-1<0) {
					  if(autodestroy)
						  delete this;
					  return NULL;
				  }

				  lastservedindex--;
				  if(!list[lastservedindex])
					  goto getprev;

				  return list[lastservedindex];
			  }


			  virtual bool eof() {
				  return !peek();
			  }

			  virtual void reset() {
				  if(reverse)
					  lastservedindex=count;
				  else
					  lastservedindex=-1;
			  }

			  virtual ~CollectionIterator() {}

		protected:
			ManagedBuffer<T_*> &list;
			int count;
			bool reverse;
			int lastservedindex;
			bool autodestroy;
		};
	}

} }
