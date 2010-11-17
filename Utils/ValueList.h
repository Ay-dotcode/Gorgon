
#ifndef __VALUELIST__
#define __VALUELIST__

#include <cstring>
#include "ManagedBuffer.h"

template <class _T>
class ValueList
{
public:
	int growth;
	
	ValueList() : buffer(50), growth(50), count(new int(0))
	{ }
	
	ValueList(int growth) : buffer(growth), growth(growth), count(new int(0))
	{ }

	ValueList(ValueList &list) : buffer(list.buffer), count(list.count), growth(list.growth) {
		buffer++;
	}
	
	~ValueList()
	{
		if(buffer.getReferenceCount()==1)
			delete count;
	}
	
	int getCount()
	{
		return *count;
	}
	
	void Add(_T data)
	{
		if(*count==buffer.GetSize())
			grow();
		
		buffer[(*count)++]=data;
	}

	void Trim(int items) {
		*count-=items;
		if(*count<0) *count=0;
	}
	
	_T &operator [] (int Index)
	{
#ifdef DEBUG
		_T t;
		if(Index<0 || Index>*count)
			return t;
#endif
		
		return buffer[Index];
	}

	ValueList &operator = (ValueList &list) {
		if(!buffer.getReferenceCount())
			delete count;

		buffer=list.buffer;
		count=list.count;

		buffer++;

		return *this;
	}
	
	void Destroy()
	{
		buffer--;
		if(!buffer.getReferenceCount())
			delete count;
	}

	void Clear()
	{
		*count=0;
	}

	void AllocateFor(int amount) {
		buffer.Resize(buffer.GetSize()+amount);
	}

	_T *getList() {
		return buffer.GetBuffer();
	}

private:
	ManagedBuffer<_T> buffer;
	int *count;
	
	void grow()
	{
		buffer.Resize(buffer.GetSize()+growth);
	}
};

#endif
