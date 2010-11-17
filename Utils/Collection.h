#pragma once

#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include "Iterator.h"

#ifndef NULL
#define NULL	0
#endif

template <class T_>
class CollectionIterator;

template <class T_>
class CollectionIterator : public IIterator<T_> {
public:
	CollectionIterator(T_ **list, int count, int alloc, bool reverse=false) : list(list), count(count), alloc(alloc), reverse(reverse) {
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
		if(l+1>=alloc)
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
		if(count==0)
			return NULL;

	getnext:
		if(lastservedindex+1>=alloc)
			return NULL;

		lastservedindex++;
		if(!list[lastservedindex])
			goto getnext;

		return list[lastservedindex];
	}

	T_* previous() {
		if(count==0)
			return NULL;

getprev:
		if(lastservedindex-1<0)
			return NULL;

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
	T_ **list;
	int count;
	int alloc;
	bool reverse;
	int lastservedindex;

};


////This class is a generic collection that
/// supports add, remove, find and iterate methods
template <class _T, int growth=50>
class Collection {
public:
	////The amount of growth for each step
	
	Collection() {
		init();
	}
	
	~Collection() {
		if(list)
			free(list);
	}
	
	////Returns number of elements
	int getCount() {
		return count;
	}
	
	////Adds a new item to the list and returns its index.
	/// When an item is removed its index is released and given
	/// to the next object. However, when an index is given it
	/// is not changed until the object is removed
	int Add(_T* Data) {
		if(count==alloc)
			grow();

		int i;
		for(i=0;i<alloc;i++)
			if(list[i]==0) {
				list[i]=Data;
				break;
			}
		
		//OrderedList[count]=i;
		count++;

		return i;
	}

	////Adds a new item to the list and returns its index.
	/// When an item is removed its index is released and given
	/// to the next object. However, when an index is given it
	/// is not changed until the object is removed
	int Add(_T& data) {
		return Add(&data);
	}

	////Adds a new item to the list and returns its index.
	/// When an item is removed its index is released and given
	/// to the next object. However, when an index is given it
	/// is not changed until the object is removed
	int operator +=(_T* Data) {
		return Add(Data);
	}
	////Adds a new item to the list and returns its index.
	/// When an item is removed its index is released and given
	/// to the next object. However, when an index is given it
	/// is not changed until the object is removed
	int operator +=(_T& data) {
		return Add(&data);
	}

	////Removes an item from the collection using its index
	void Remove(int Index) {
		int i,j;
		j=0;

		for(i=0;i<alloc;i++) {
			if(list[i]) j++;
			if((j-1)==Index) {
				list[i]=0;
				break;
			}
		}

		count--;
	}

	////Removes an item from the collection using its pointer
	void Remove(_T *Item) {
		int i,j;
		j=0;

		for(i=0;i<alloc;i++) {
			if(list[i]) j++;
			if(list[i]==Item) {
				list[i]=0;
				break;
			}
		}

		count--;
	}

	////Removes an item from the collection using its reference
	void Remove(_T& data) {
		Remove(&data);
	}

	////Removes an item from the collection using its index
	Collection &operator -=(int Index) {
		Remove(Index);

		return *this;
	}

	////Removes an item from the collection using its pointer
	Collection &operator -=(_T *Item) {
		Remove(Item);

		return *this;
	}

	////Removes an item from the collection using its reference
	Collection &operator -=(_T& data) {
		Remove(&data);

		return *this;
	}

	////Deletes an item from the collection using its index.
	/// Deleting both removes the item from the list and free the item itself.
	void Delete(int Index) {
		int i,j;
		j=0;

		for(i=0;i<alloc;i++) {
			if(list[i]) j++;
			if((j-1)==Index) {
				delete (list[i]);
				list[i]=0;
				break;
			}
		}
		count--;
	}

	////Deletes an item from the collection using its pointer.
	/// Deleting both removes the item from the list and free the item itself.
	void Delete(_T *Item) {
		int i,j;
		j=0;

		for(i=0;i<alloc;i++) {
			if(list[i]) j++;
			if(list[i]==Item) {
				delete (list[i]);
				list[i]=0;
				break;
			}
		}

		count--;
	}

	////Deletes an item from the collection using its reference.
	/// Deleting both removes the item from the list and free the item itself.
	void Delete(_T& data) {
		Delete(&data);
	}

	////Searches the position of a given item, if not found -1 is returned
	int Find(_T *Item) {
		int i, j=0;
		for(i=0;i<alloc;i++) {
			if(Item==list[i])
				return j;

			if(list[i]) j++;
		}

		return -1;
	}
	////Searches the position of a given item, if not found -1 is returned
	int Find(_T &Item) {
		return Find(&Item);
	}

	////Searches the collection for the item that is equal to the given
	/// parameter. If there is more than one, first one is returned.
	/// Start parameter can be used to discover more items. For this function
	/// to compile, you should use a type that supports comparison
	int Search(_T &Item, int start=0) {
		int i, j=0;
		for(i=start;i<alloc;i++) {
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
	int ReverseSearch(_T &Item, int start=-1) {
		int i, j=count-1;

		if(start==-1)	
			start=count-1;

		for(i=alloc;i>=0;i--) {
			if(j<=start && Item==*list[i])
				return j;

			if(list[i]) j--;
		}

		return -1;
	}

	
	////Returns the item at a given index
	_T* operator [] (int Index)
	{
		if(Index<0 || Index>alloc)
			return NULL;

		int i,j;
		j=0;

		for(i=0;i<alloc;i++) {
			if(list[i]) j++;
			if((j-1)==Index) {
				return list[i];
			}
		}
		
		return NULL;
	}

	////Returns the item at a given index
	_T& operator () (int Index)
	{
		if(Index<0 || Index>alloc)
			throw std::out_of_range("Index requested is out of range");

		int i,j;
		j=0;

		for(i=0;i<alloc;i++) {
			if(list[i]) j++;
			if((j-1)==Index) {
				return *(list[i]);
			}
		}
		
		throw std::out_of_range("Index requested is out of range");
	}

	CollectionIterator<_T> &ForwardIterator() {
		return CollectionIterator<_T>(list, alloc);
	}

	CollectionIterator<_T> &BackwardIterator() {
		return CollectionIterator<_T>(list, count, alloc, true);
	}

	operator IIterator<_T> *() {
		CollectionIterator<_T> *it=new CollectionIterator<_T>(list, count, alloc);

		return it;
	}

	////Restarts the iteration, this function should be called before
	/// using next and previous functions.
	///@reverse: This parameter denotes the direction of the iteration
	void ResetIteration(bool reverse=false) {
		if(reverse)
			lastservedindex=count;
		else
			lastservedindex=-1;
	}

	////Returns the next item in the collection, moving
	/// iteration pointer to the next item. ResetIteration 
	/// should be called prior to this function
	_T* next() {
		if(count==0)
			return NULL;

getnext:
		if(lastservedindex+1>=alloc)
			return NULL;

		lastservedindex++;
		if(!list[lastservedindex])
			goto getnext;

		return list[lastservedindex];
	}

	////Returns the previous item in the collection, moving
	/// iteration pointer to the previous item. ResetIteration 
	/// should be called prior to this function
	_T* previous() {
		if(count==0)
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
	void Clear()
	{
		std::memset(list,0,sizeof(_T*)*alloc);
		count=0;
	}

	////Clears the contents of the collection and releases the memory
	/// used for the list. Items are not freed.
	void Collapse() {
		Clear();

		free(list);
		list=NULL;
	}
	
	////Destroys the entire collection, effectively deleting the contents
	/// and the list
	void Destroy()
	{
		int i;
		for(i=0;i<alloc;i++)
			if(list[i])
				delete list[i];
		
		free(list);
		list=NULL;
		count=0;
		alloc=0;
	}


	////Allocates memory for the given amount of items
	void AllocateFor(int amount) {
		list=(_T**)realloc(list,sizeof(_T*)*(alloc+amount));
		std::memset(list+sizeof(_T*)*alloc,0,sizeof(_T*)*amount);
		alloc+=amount;
	}

private:
	_T **list;
	int alloc,count;
	int lastservedindex;
	
	void init()
	{
		list=(_T**)std::malloc(sizeof(_T*)*growth);
		alloc=growth;
		std::memset(list,0,sizeof(_T*)*alloc);

		count=0;
		lastservedindex=-1;
	}
	
	void grow()
	{
		int i;

		if(!list){
			init();
			return;
		}

		list=(_T**)std::realloc(list,sizeof(_T*)*(alloc+growth));
		std::memset(list+alloc,0,sizeof(_T*)*growth);

		alloc+=growth;
	}
};

