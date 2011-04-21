#pragma once

#include <malloc.h>
#include <memory.h>

////This class holds a list of buffers managing them as required
template <typename T_> 
class BufferList {
public:
	////Growth amount
	int growth;

	////Default constructor setting growth to 10
	BufferList() {
		growth=10;
		init();
	}
	
	////Filling constructor allows growth to be modified
	BufferList(int growth) {
		this->growth=growth;
		init();
	}
	
	////Releases used resources
	~BufferList() {
		Destroy();
	}
	
	////Returns elements in this list
	int getCount() {
		return count;
	}

	////Add a new buffer and sets buffer data and size
	void Add(T_ *Data,int size) {
		if(count==alloc)
			grow();
		
		list[count]=(T_*)malloc(size*sizeof(T_));
		buffersizes[count]=size;
		memcpy(list[count++],Data,size);
	}
	
	////Returns the requested item
	const T_* operator [] (int Index) {
		if(Index<0 || Index>count)
			return NULL;
		
		return list[Index];
	}

	////Returns the size of a buffer
	int getSize(int Index) {
		if(Index<0 || Index>count)
			return NULL;
		
		return buffersizes[Index];
	}
	
	////Destroys entire list including the buffers
	void Destroy() {
		int i;
		for(i=0;i<count;i++)
			free(list[i]);
		
		free(list);
	}

	////Allocates memory for the given amount of items
	void AllocateFor(int amount) {
		alloc+=amount;
		list=(T_**)realloc(list,sizeof(T_*)*alloc);
		buffersizes=(int*)realloc(buffersizes,sizeof(int)*alloc);
	}
	
private:
	T_ **list;
	int *buffersizes;
	int alloc,count;
	
	void init() {
		list=(T_**)malloc(sizeof(T_*)*growth);
		buffersizes=(int*)malloc(sizeof(int)*growth);
		alloc=growth;
		count=0;
	}
	
	void grow() {
		alloc+=growth;
		list=(T_**)realloc(list,sizeof(T_*)*alloc);
		buffersizes=(int*)realloc(buffersizes,sizeof(int)*alloc);
	}
	
};
