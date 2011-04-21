#pragma once

#include <malloc.h>
#include <memory.h>

////This template class holds preallocated classes or
/// structures. When a new object is requested from this
/// class, a previously created object is returned. Any
/// class that is planned to be used with this system should
/// have empty constructor and a resetting filler.
/// This class reduces costs for reallocation when the list
/// is cleared and built again. Should be used with care. This
/// list does not have support for item removal, it can only be
/// cleared.
template <class T_>
class PAClassList {
public:
	////Amount of growth when this object runs out of space
	int growth;
	
	////Default constructor sets growth to 50
	PAClassList() {
		growth=50;
		init();
	}
	
	////Constructor allowing custom growth rate
	PAClassList(int growth) {
		this->growth=growth;
		init();
	}
	
	////Destructor
	~PAClassList() {
		Destroy();
	}
	
	////Returns current number of objects
	int getCount() {
		return count;
	}
	
	////Designates a new item and returns it. This function
	/// does not make any initialization unless a growth is 
	/// required. The returned object should be considered
	/// dirty.
	T_* Add() {
		if(count==alloc)
			grow();
		
		return list[count++];
	}
	
	////Indexes an item, does not do bound check
	T_* operator [] (int Index) {
		return list[Index];
	}
	
	////Destroys the list with every allocated element
	void Destroy() {
		int i;
		for(i=0;i<count;i++)
			delete list[i];
		
		free(list);
	}

	////Clears the list. This function does not free any
	/// used resources. It only starts positioning items
	/// from the beginning of the list
	void Clear() {
		count=0;
	}

	////This function can be used to allocate a given 
	/// amount of items. It also initializes the newly
	/// created objects
	void AllocateFor(int amount) {
		list=(T_**)realloc(list,sizeof(T_*)*(alloc+amount));
		for(int i=alloc;i<alloc+growth;i++)
			list[i]=new T_;

		alloc+=amount;
	}

private:
	T_ **list;
	int alloc,count;
	
	void init() {
		list=(T_**)malloc(sizeof(T_*)*growth);
		alloc=growth;
		for(int i=0;i<alloc;i++)
			list[i]=new T_;

		count=0;
	}
	
	void grow() {
		list=(T_**)realloc(list,sizeof(T_*)*(alloc+growth));
		for(int i=alloc;i<alloc+growth;i++)
			list[i]=new T_;

		alloc+=growth;
	}
};
