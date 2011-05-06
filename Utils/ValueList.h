
//Depreciated, use std::vector instead
// might be useful if fixed. not much different 
// from vector tough

#ifndef __VALUELIST__
#define __VALUELIST__

#include <cstring>
#include "ManagedBuffer.h"

template <class T_>
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
		if(buffer.getReferenceCount()<=1)
			delete count;
	}
	
	int getCount()
	{
		return *count;
	}
	
	void Add(T_ data)
	{
		if(*count==buffer.GetSize())
			grow();
		
		buffer[(*count)++]=data;
	}

	void Trim(int items) {
		*count-=items;
		if(*count<0) *count=0;
	}
	
	T_ &operator [] (int Index)
	{
#ifdef _DEBUG
		T_ t;
		if(Index<0 || Index>*count)
			return t;
#endif
		
		return buffer[Index];
	}

	ValueList &operator = (ValueList &list) {
		if(buffer.getReferenceCount()<=1)
			delete count;

		buffer=list.buffer;
		count=list.count;

		return *this;
	}
	
	void Destroy()
	{
		buffer.Resize(0);
	}

	void Clear()
	{
		*count=0;
	}

	void AllocateFor(int amount) {
		buffer.Resize(buffer.GetSize()+amount);
	}

	T_ *getList() {
		return buffer.GetBuffer();
	}

private:
	ManagedBuffer<T_> buffer;
	int *count;
	
	void grow()
	{
		buffer.Resize(buffer.GetSize()+growth);
	}
};

#endif
