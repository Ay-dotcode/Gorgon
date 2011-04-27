#pragma once

#define foreach(type, name, container)			for(IIterator<type > &name##_it(*(IIterator<type >*)container); type *name=getIteratorItem(name##_it);)
#define const_foreach(type, name, container)	for(IIterator<type > &name##_it(*(const IIterator<type >*)container); type *name=getIteratorItem(name##_it);)

#ifndef NULL
#define NULL	0
#endif

////Generic iterator interface. 
template <class T_>
class IIterator {
public:
	////This method resets the iterator
	virtual void reset() const = 0;
	////This method gets the next item and moves item pointer to the next one
	virtual T_* get() = 0;
	virtual const T_* get() const = 0;
	virtual T_* peek() = 0;
	virtual const T_* peek() const = 0;

	////This method returns whether iterator is at the last item
	virtual bool eof() const = 0;

	virtual T_& getReference() { return *get(); }
	virtual const T_& getReference() const { return *get(); }

	////This method gets the next item and moves item pointer to the next one
	virtual operator T_*() {
		return get();
	}

	////This method gets the next item and moves item pointer to the next one
	virtual operator T_&() {
		return *get();
	}

	////This method gets the next item and moves item pointer to the next one
	virtual operator const T_*() const {
		return get();
	}
	virtual T_* operator ->() {
		return peek();
	}

	////This method gets the next item and moves item pointer to the next one
	virtual operator const T_&() const {
		return *get();
	}
	////This method returns whether iterator is at the last item
	virtual operator bool() const {
		return !eof();
	}
	////This method resets the iterator
	virtual bool operator !() const {
		return eof();
	}

	virtual ~IIterator() {  }
};

template<class T_>
T_ *getIteratorItem(IIterator<T_> &iterator) {
	if(!iterator) { delete &iterator; return NULL; }
	else { return (T_*) iterator; }
}

template<class T_>
T_ *getIteratorItem(const IIterator<T_> &iterator) {
	if(!iterator) { delete &iterator; return NULL; }
	else { return (const T_*) iterator; }
}

