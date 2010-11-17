#pragma once

#include <malloc.h>
#include <memory.h>

////This class holds a list of buffers managing them as required
template <typename _T> 
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
	void Add(_T *Data,int size) {
		if(count==alloc)
			grow();
		
		list[count]=(_T*)malloc(size*sizeof(_T));
		buffersizes[count]=size;
		memcpy(list[count++],Data,size);
	}
	
	////Returns the requested item
	const _T* operator [] (int Index) {
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
		list=(_T**)realloc(list,sizeof(_T*)*alloc);
		buffersizes=(int*)realloc(buffersizes,sizeof(int)*alloc);
	}
	
private:
	_T **list;
	int *buffersizes;
	int alloc,count;
	
	void init() {
		list=(_T**)malloc(sizeof(_T*)*growth);
		buffersizes=(int*)malloc(sizeof(int)*growth);
		alloc=growth;
		count=0;
	}
	
	void grow() {
		alloc+=growth;
		list=(_T**)realloc(list,sizeof(_T*)*alloc);
		buffersizes=(int*)realloc(buffersizes,sizeof(int)*alloc);
	}
	
};
