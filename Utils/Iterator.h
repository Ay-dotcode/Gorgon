#pragma once

#define foreach(type, name, container)	for(IIterator<type > &name##_it(*(IIterator<type >*)container); type *name=getIteratorItem(name##_it);)

#ifndef NULL
#define NULL	0
#endif

////Generic iterator interface. 
template <class T_>
class IIterator {
public:
	////This method resets the iterator
	virtual void reset()=0;
	////This method gets the next item and moves item pointer to the next one
	virtual T_* get()=0;
	virtual T_* peek()=0;

	////This method returns whether iterator is at the last item
	virtual bool eof()=0;

	virtual T_& getReference() { return *get(); }

	////This method gets the next item and moves item pointer to the next one
	virtual operator T_*() {
		return get();
	}

	////This method gets the next item and moves item pointer to the next one
	virtual operator T_&() {
		return *get();
	}
	////This method returns whether iterator is at the last item
	virtual operator bool() {
		return !eof();
	}
	////This method resets the iterator
	virtual bool operator !() {
		return eof();
	}

	virtual ~IIterator() {  }

};

template<class T_>
T_ *getIteratorItem(IIterator<T_> &iterator) {
	if(!iterator) { delete &iterator; return NULL; }
	else { return (T_*) iterator; }
}

