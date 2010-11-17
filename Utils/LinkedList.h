#pragma once

#include <malloc.h>
#include <memory.h>
#include "Iterator.h"

#ifndef NULL
#define NULL	0
#endif

template <class T_>
class LinkedList;
template <class T_>
class LinkedListIterator;
template <class T_>
class LinkedListOrderedIterator;

////This class is a wrapper to all objects that will be added
/// to a linked list. This wrapper contains linked list structure,
/// order, the item and few other useful functions.
template <class T_>
class LinkedListItem {
	friend class LinkedList<T_>;
	friend class LinkedListIterator<T_>;
	friend class LinkedListOrderedIterator<T_>;
protected:
	LinkedListItem<T_> *prev,*next,*order_prev,*order_next;
	LinkedList<T_> *parent;
	float Order;
	
	LinkedListItem(LinkedList<T_> *parent) {
		prev=next=order_prev=order_next=NULL;
		this->parent=parent;
	}

public:
	////The item
	T_ *Item;
	////Returns the next element in the list,
	/// if this is the last item returns NULL
	LinkedListItem<T_> *getNext() {
		return next;
	}
	////Returns the previous element in the list,
	/// if this is the first item returns NULL
	LinkedListItem<T_> *getPrevious() {
		return prev;
	}
	////Returns the next element in the ordered list,
	/// if this is the last item returns NULL
	LinkedListItem<T_> *getOrderedNext() {
		return order_next;
	}
	////Returns the previous element in the ordered list,
	/// if this is the first item returns NULL
	LinkedListItem<T_> *getOrderedPrevious() {
		return order_prev;
	}
	LinkedList<T_> *getParent() { return parent; }

	////Returns the order of this list item
	float getOrder() { return Order; }
	////Changes the order of this item
	void setOrder(float Order) {
		if(parent!=NULL)
			parent->setOrderOf(this,Order);
	}
};


template <class T_>
////This iterator class iterates through unsorted linked list
class LinkedListIterator : public IIterator<T_> {
	friend class LinkedList<T_>;
protected:
	LinkedListItem<T_> *start,*current;
	bool reverse;

	LinkedListIterator(LinkedListItem<T_> *start,bool reverse) {
		current=start;
		this->start=start;
		this->reverse=reverse;
	}

public:
	////This method resets the iterator
	virtual void reset() {
		current=start;
	}

	////This method returns whether iterator is at the last item
	virtual bool eof() {
		return (current==NULL);
	}

	////This method gets the next item and moves item pointer to the next one
	virtual T_* get() {
		T_ *item;

		if(current==NULL)
			return NULL;

		item=current->Item;

		if(reverse)
			current=current->prev;
		else
			current=current->next;

		return item;
	}

	////This method gets the next item and moves item pointer to the next one
	virtual T_* peek() {
		return current->Item;
	}

	////This method gets the next list item and moves item pointer to the next one
	LinkedListItem<T_>* getitem() {
		if(current==NULL)
			return NULL;

		LinkedListItem<T_>* item=current;

		if(reverse)
			current=current->prev;
		else
			current=current->next;

		return item;
	}
	////This method gets the next item and moves item pointer to the next one
	virtual operator LinkedListItem<T_>*() {
		return getitem();
	}
};
////This iterator class iterates through sorted linked list
template <class T_>
class LinkedListOrderedIterator : public IIterator<T_> {
	friend class LinkedList<T_>;
protected:
	LinkedListItem<T_> *start,*current;
	bool reverse;

	LinkedListOrderedIterator(LinkedListItem<T_>* start,bool reverse) {
		current=this->start=start;
		this->reverse=reverse;
	}

public:
	////This method resets the iterator
	virtual void reset() {
		current=start;
	}

	////This method returns whether iterator is at the last item
	virtual bool eof() {
		return (current==NULL);
	}

	////This method gets the next item and moves item pointer to the next one
	virtual T_* get() {
		T_ *item;

		if(current==NULL)
			return NULL;

		item=current->Item;

		if(reverse)
			current=current->order_prev;
		else
			current=current->order_next;

		return item;
	}

	////This method gets the next item and moves item pointer to the next one
	virtual T_* peek() {
		return current->Item;
	}

	////This method gets the next list item and moves item pointer to the next one
	LinkedListItem<T_>* getitem() {
		if(current==NULL)
			return NULL;

		LinkedListItem<T_>* item=current;

		if(reverse)
			current=current->order_prev;
		else
			current=current->order_next;

		return item;
	}
	////This method gets the next item and moves item pointer to the next one
	virtual operator LinkedListItem<T_>*() {
		return getitem();
	}
};



////This class is a linked list structure.
/// It allows insert and remove operations
/// as well as sorted item query and iterators.
template <class T_>
class LinkedList {

	friend class LinkedListIterator<T_>;

protected:
	int poolid;
	int count;
	LinkedListItem<T_> **Pool;
	LinkedListItem<T_> *first,*last,*order_first,*order_last;

	void grow() {
		Pool=new LinkedListItem<T_>*[growth];
		int i;
		for(i=0;i<growth;i++)
			Pool[i]=new LinkedListItem<T_>(this);

		poolid=growth-1;
	}

	LinkedListItem<T_>* obtain() {
		if(poolid<0)
			grow();

		return Pool[poolid--];
	}
public:
	////The amount of growth for each step
	int growth;
	////Returns number of elements
	int getCount() {
		return count;
	}

	////Basic constructor
	LinkedList() {
		count=0;
		poolid=-1,
		growth=10;
		first=last=NULL;
		order_first=order_last=NULL;
	}

	////Changes the order of an item
	///@Position: position of the item to be reordered
	///@Order	: The new order
	void setOrderOf(int Position,float Order) {
		setOrderOf(ItemAt(Position),Order);
	}

	////Changes the order of an item
	///@Item	: The item to be reordered
	///@Order	: The new order
	void setOrderOf(LinkedListItem<T_> *Item,float Order) {
		if(Item==NULL)
			return;

		if(Item->Order==Order)
			return;

		if(Item->order_prev!=NULL)
			Item->order_prev->order_next=Item->order_next;

		if(Item->order_next!=NULL)
			Item->order_next->order_prev=Item->order_prev;

		if(order_first==Item)
			order_first=Item->order_next;

		if(order_last==Item)
			order_last=Item->order_prev;

		Item->Order=Order;

		bool done=false;

		LinkedListOrderedIterator<T_> i=GetOrderedIterator();
		LinkedListItem<T_> *item;
		LinkedListItem<T_> *ret=Item;
		while(item=i) {
			if(item->Order>=ret->Order) {
				if(item->order_prev!=NULL)
					item->order_prev->order_next=ret;
				else
					order_first=ret;

				ret->order_prev=item->order_prev;
				ret->order_next=item;

				item->order_prev=ret;

				done=true;

				break;
			}
		}

		if(!done) {
			if(order_last)
				order_last->order_next=ret;
			else
				order_first=ret;

			ret->order_prev=order_last;
			ret->order_next=NULL;
			order_last=ret;
		}

	}

	////Removes an item from the list
	void Remove(LinkedListItem<T_> *item) {
		if(item==NULL)
			return;

		if(item->prev!=NULL)
			item->prev->next=item->next;

		if(item->next!=NULL)
			item->next->prev=item->prev;

		if(first==item)
			first=item->next;

		if(last==item)
			last=item->prev;


		if(item->order_prev!=NULL)
			item->order_prev->order_next=item->order_next;

		if(item->order_next!=NULL)
			item->order_next->order_prev=item->order_prev;

		if(order_first==item)
			order_first=item->order_next;

		if(order_last==item)
			order_last=item->order_prev;

		count--;
		delete item;
	}

	////Gets the first item in list.
	/// Returns item itself
	T_ *getFirstItem() {
		if(first)
			return first->Item;
		else
			return NULL;
	}

	////Gets the last item in list.
	/// Returns item itself
	T_ *getLastItem() {
		if(last)
			return last->Item;
		else
			return NULL;
	}

	////Gets the first item in ordered list.
	/// Returns item itself
	T_ *getOrderedFirstItem() {
		if(order_first)
			return order_first->Item;
		else
			return NULL;
	}

	////Gets the last item in ordered list.
	/// Returns item itself
	T_ *getOrderedLastItem() {
		if(order_last)
			return order_last->Item;
		else
			return NULL;
	}

	////Gets the first item in list.
	/// Returns item wrapper
	LinkedListItem<T_> *getFirst() {
		return first;
	}

	////Gets the last item in list.
	/// Returns item wrapper
	LinkedListItem<T_> *getLast() {
		return last;
	}

	////Gets the first item in ordered list.
	/// Returns item wrapper
	LinkedListItem<T_> *getOrderedFirst() {
		return order_first;
	}

	////Gets the last item in ordered list.
	/// Returns item wrapper
	LinkedListItem<T_> *getOrderedLast() {
		return order_last;
	}

	float HighestOrder() {
		if(order_last)
			return order_last->Order;

		return 0;
	}

	float LowestOrder() {
		if(order_first)
			return order_first->Order;

		return 0;
	}

	////Removes an item from the list
	void Remove(T_ *Item) {
		if(Item==NULL)
			return;
		
		LinkedListItem<T_>* item=FindListItem(Item);
		if(item==NULL)
			return;


		if(item->prev!=NULL)
			item->prev->next=item->next;

		if(item->next!=NULL)
			item->next->prev=item->prev;

		if(first==item)
			first=item->next;

		if(last==item)
			last=item->prev;


		if(item->order_prev!=NULL)
			item->order_prev->order_next=item->order_next;

		if(item->order_next!=NULL)
			item->order_next->order_prev=item->order_prev;

		if(order_first==item)
			order_first=item->order_next;

		if(order_last==item)
			order_last=item->order_prev;

		count--;
		delete item;
	}

	////Removes an item from the list
	void Remove(int Position) {
		Remove(ItemAt(Position));
	}

	void Remove(T_ &item) { Remove(&item); }


	void Delete(T_ *Item) {
		if(Item==NULL)
			return;

		Remove(Item);
		delete Item;
	}
			
	////Removes an item from the list
	void Delete(int Position) {
		Delete(ItemAt(Position));
	}

	void Delete(T_ &item) { Delete(&item); }

	////Clears the entire list
	void Clear() {
		while(getCount())
			Remove(0);
	}

	////Returns an item from a given position
	T_ *operator [](int Position) {
		LinkedListItem<T_>* item=ListItemAt(Position);
		if(item)
			return item->Item;
		else
			return NULL;
	}

	////Returns an item from a given position
	T_ *ItemAt(int Position) {
		LinkedListItem<T_>* item=ListItemAt(Position);
		if(item)
			return item->Item;
		else
			return NULL;
	}

	////Returns a list item from a given position
	LinkedListItem<T_> *ListItemAt(int Position) {
		LinkedListIterator<T_> i=(*this);
		LinkedListItem<T_> *item;

		while(item=i)
			if(Position--==0)
				return item;
	
		return NULL;
	}

	////Returns an item from a given ordered position
	T_ *OrderedItemAt(int Position) {
		LinkedListItem<T_>* item=OrderedListItemAt(Position);
		if(item)
			return item->Item;
		else
			return NULL;
	}

	////Returns a list item from a given ordered position
	LinkedListItem<T_> *OrderedListItemAt(int Position) {
		LinkedListOrderedIterator<T_> i=(*this);
		LinkedListItem<T_> *item;

		while(item=i)
			if(Position--==0)
				return item;
	
		return NULL;
	}

	////Searches a specific item in the list and returns the list item.
	/// If item is not found NULL is returned
	LinkedListItem<T_> *FindListItem(T_ *Item) {
		LinkedListIterator<T_> it=*this;
		LinkedListItem<T_> *item;
		while(item=it) {
			if(item->Item==Item)
				return item;
		}

		return NULL;
	}
	LinkedListItem<T_> &FindListItem(T_ &Item) {
		LinkedListIterator<T_> it=*this;
		LinkedListItem<T_> *item;
		while(item=it) {
			if(*item->Item==Item)
				return *item;
		}

		return NULL;
	}
	////Creates an ordered iterator object
	operator LinkedListOrderedIterator<T_>() {
		return GetOrderedIterator();
	}

	////Destroys the list by freeing every member and removing them
	void Destroy() {
		while(getCount()) {
			delete ItemAt(0);
			Remove(0);
		}
		int i;
		for(i=0;i<poolid;i++)
			delete Pool[i];
		delete Pool;
		poolid=-1;
	}

	////Creates an iterator object
	operator LinkedListIterator<T_>() {
		return GetIterator();
	}
	////Creates an iterator object
	operator IIterator<T_>*() {
		IIterator<T_>* it= new LinkedListOrderedIterator<T_>(order_first,false);

		return it;
	}

	////Creates an iterator object
	LinkedListIterator<T_> GetIterator() {
		return LinkedListIterator<T_>(first,false);
	}

	////Creates a reverse iterator object
	LinkedListIterator<T_> GetReverseIterator() {
		return LinkedListIterator<T_>(last,true);
	}

	////Creates an ordered iterator object
	LinkedListOrderedIterator<T_> GetOrderedIterator() {
		return LinkedListOrderedIterator<T_>(order_first,false);
	}

	////Creates an ordered reverse iterator object
	LinkedListOrderedIterator<T_> GetReverseOrderedIterator() {
		return LinkedListOrderedIterator<T_>(order_last,true);
	}

	LinkedListItem<T_> *AddItem(T_ *Item) { return AddItem(Item, 0); }

	LinkedListItem<T_> *Add(T_ *Item) { return AddItem(Item, 0); }

	LinkedListItem<T_> *Add(T_& Item) { return AddItem(&Item, 0);}

	LinkedListItem<T_> *AddItem(T_& Item) { return AddItem(&Item, 0);}

	LinkedListItem<T_> *AddItem(T_& Item,float Order) { return AddItem(&Item, Order);}


	////Adds a new item to the list. This function returns the list item
	/// that surrounds the newly added item
	///@Item	: The item to be added to the list
	///@Order	: The order of the item to be added
	LinkedListItem<T_> *AddItem(T_ *Item,float Order) {
		LinkedListItem<T_> *ret=obtain();
		count++;

		ret->Order=Order;
		ret->Item=Item;

		if(first==NULL) {
			first=last=order_first=order_last=ret;
			ret->prev=NULL;
			ret->next=NULL;
			ret->order_prev=NULL;
			ret->order_next=NULL;
		}
		else {
			ret->prev=last;
			ret->next=NULL;

			last->next=ret;
			last=ret;

			LinkedListOrderedIterator<T_> i=GetOrderedIterator();
			LinkedListItem<T_> *item;

			bool done=false;

			while( (item=i) ) {
				if(item->Order>=ret->Order) {
					if(item->order_prev!=NULL)
						item->order_prev->order_next=ret;
					else
						order_first=ret;

					ret->order_prev=item->order_prev;
					ret->order_next=item;

					item->order_prev=ret;

					done=true;

					break;
				}
			}

			if(!done) {
				if(order_last)
					order_last->order_next=ret;
				else
					order_first=ret;

				ret->order_prev=order_last;
				order_last=ret;
			}
		}

		return ret;
	}
};
