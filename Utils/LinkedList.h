//DESCRIPTION
//	This file contains LinkedList class which supports fast
//	item insertion, sorted and unsorted iteration, fast item
//	removal. Indexed item retrieval requires traversing the
//	list, therefore might be slow.

//REQUIRES:
//	-

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

#include <malloc.h>
#include <memory.h>
#include "UtilsBase.h"
#include "Iterator.h"


#define LINKEDLIST_EXIST


namespace gge { namespace utils {
	template <class T_, class SK_=int, int growth=10>
	class LinkedList;
	template <class T_, class SK_=int, int growth=10>
	class LinkedListIterator;
	template <class T_, class SK_=int, int growth=10>
	class LinkedListOrderedIterator;
	template <class T_, class SK_=int, int growth=10>
	class LinkedListItemIterator;
	template <class T_, class SK_=int, int growth=10>
	class LinkedListOrderedItemIterator;

	////This class is a wrapper to all objects that will be added
	/// to a linked list. This wrapper contains linked list structure,
	/// order, the item and few other useful functions.
	template <class T_, class SK_=int, int growth=10>
	class LinkedListItem {
		friend class LinkedList<T_,SK_,growth>;
		friend class LinkedListIterator<T_,SK_,growth>;
		friend class LinkedListOrderedIterator<T_,SK_,growth>;
		friend class LinkedListItemIterator<T_,SK_,growth>;
		friend class LinkedListOrderedItemIterator<T_,SK_,growth>;
	protected:
		LinkedListItem<T_, SK_, growth> *prev,*next,*order_prev,*order_next;
		LinkedList<T_,SK_,growth> *parent;
		SK_ Order;
	
		LinkedListItem(LinkedList<T_, SK_,growth> *parent) {
			prev=next=order_prev=order_next=NULL;
			this->parent=parent;
		}

	public:
		////The item
		T_ *Item;
		////Returns the next element in the list,
		/// if this is the last item returns NULL
		LinkedListItem<T_, SK_,growth> *getNext() {
			return next;
		}
		////Returns the previous element in the list,
		/// if this is the first item returns NULL
		LinkedListItem<T_, SK_,growth> *getPrevious() {
			return prev;
		}
		////Returns the next element in the ordered list,
		/// if this is the last item returns NULL
		LinkedListItem<T_, SK_,growth> *getOrderedNext() {
			return order_next;
		}
		////Returns the previous element in the ordered list,
		/// if this is the first item returns NULL
		LinkedListItem<T_, SK_,growth> *getOrderedPrevious() {
			return order_prev;
		}
		LinkedList<T_, SK_,growth> *getParent() { return parent; }

		////Returns the order of this list item
		SK_ getOrder() { return Order; }
		////Changes the order of this item
		void setOrder(SK_ Order) {
			if(parent!=NULL)
				parent->setOrderOf(this,Order);
		}
		operator T_*() { return Item; }
		operator T_&() { return Item; }
		T_ &operator *() { return Item; }
		T_ &operator ->() { return Item; }
	};


	////This iterator class iterates through unsorted linked list
	template <class T_, class SK_, int growth>
	class LinkedListIterator : public IIterator<T_> {
		friend class LinkedList<T_, SK_,growth>;
	protected:
		LinkedListItem<T_, SK_,growth> *start,*current;
		bool reverse, autodestroy;

		LinkedListIterator(LinkedListItem<T_, SK_, growth> *start,bool reverse,bool autodestroy=false) {
			current=start;
			this->start=start;
			this->reverse=reverse;
			this->autodestroy=autodestroy;
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

			if(current==NULL) {
				if(autodestroy)
					delete this;
				return NULL;
			}

			item=current->Item;

			if(reverse)
				current=current->prev;
			else
				current=current->next;

			return item;
		}

		virtual T_* peek() {

			if(current==NULL) {
				if(autodestroy)
					delete this;
				return NULL;
			}

			return current->Item;
		}

		operator T_*() { return get(); }
		operator T_&() { return *get(); }
		T_ *operator ->() { return get(); }
		T_ &operator *() { return *get(); }

		////This method gets the next list item and moves item pointer to the next one
		LinkedListItem<T_, SK_, growth>* getitem() {
			if(current==NULL) {
				if(autodestroy)
					delete this;
				return NULL;
			}

			LinkedListItem<T_, SK_, growth>* item=current;

			if(reverse)
				current=current->prev;
			else
				current=current->next;

			return item;
		}
		////This method gets the next item and moves item pointer to the next one
		virtual operator LinkedListItem<T_, SK_, growth>*() {
			return getitem();
		}
	};

	////This iterator class iterates through sorted linked list
	template <class T_, class SK_, int growth>
	class LinkedListOrderedIterator : public IIterator<T_> {
		friend class LinkedList<T_, SK_, growth>;
	protected:
		LinkedListItem<T_, SK_, growth> *start,*current;
		bool reverse, autodestroy;

		LinkedListOrderedIterator(LinkedListItem<T_, SK_, growth>* start,bool reverse,bool autodestroy=false) {
			current=this->start=start;
			this->reverse=reverse;
			this->autodestroy=autodestroy;
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

			if(current==NULL) {
				if(autodestroy)
					delete this;
				return NULL;
			}

			item=current->Item;

			if(reverse)
				current=current->order_prev;
			else
				current=current->order_next;

			return item;
		}

		virtual T_* peek() {

			if(current==NULL) {
				if(autodestroy)
					delete this;
				return NULL;
			}

			return current->Item;
		}

		operator T_*() { return get(); }
		operator T_&() { return *get(); }
		T_ *operator ->() { return get(); }
		T_ &operator *() { return *get(); }


		////This method gets the next list item and moves item pointer to the next one
		LinkedListItem<T_, SK_, growth>* getitem() {
			if(current==NULL) {
				if(autodestroy)
					delete this;
				return NULL;
			}

			LinkedListItem<T_, SK_, growth>* item=current;

			if(reverse)
				current=current->order_prev;
			else
				current=current->order_next;

			return item;
		}
		////This method gets the next item and moves item pointer to the next one
		virtual operator LinkedListItem<T_, SK_, growth>*() {
			return getitem();
		}
	};


	////This iterator class iterates through unsorted linked list
	template <class T_, class SK_, int growth>
	class LinkedListItemIterator : public IIterator<LinkedListItem<T_, SK_, growth> > {
		friend class LinkedList<T_, SK_,growth>;
	protected:
		LinkedListItem<T_, SK_,growth> *start,*current;
		bool reverse, autodestroy;

		LinkedListItemIterator(LinkedListItem<T_, SK_, growth> *start,bool reverse,bool autodestroy=false) {
			current=start;
			this->start=start;
			this->reverse=reverse;
			this->autodestroy=autodestroy;
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
		virtual LinkedListItem<T_, SK_,growth> *get() {
			LinkedListItem<T_, SK_,growth> *item;

			if(current==NULL) {
				if(autodestroy)
					delete this;
				return NULL;
			}

			item=current;

			if(reverse)
				current=current->prev;
			else
				current=current->next;

			return item;
		}

		virtual LinkedListItem<T_, SK_,growth> * peek() {

			if(current==NULL) {
				if(autodestroy)
					delete this;
				return NULL;
			}

			return current;
		}

		operator LinkedListItem<T_, SK_,growth>*() { return get(); }
		operator LinkedListItem<T_, SK_,growth>&() { return *get(); }
		LinkedListItem<T_, SK_,growth> *operator ->() { return get(); }
		LinkedListItem<T_, SK_,growth> &operator *() { return *get(); }


		////This method gets the next list item and moves item pointer to the next one
		T_ *getobject() {
			if(current==NULL) {
				if(autodestroy)
					delete this;
				return NULL;
			}

			T_* item=current->Item;

			if(reverse)
				current=current->prev;
			else
				current=current->next;

			return item;
		}
		////This method gets the next item and moves item pointer to the next one
		operator T_*() {
			return getobject();
		}
	};

	////This iterator class iterates through sorted linked list
	template <class T_, class SK_, int growth>
	class LinkedListOrderedItemIterator : public IIterator<LinkedListItem<T_, SK_, growth> > {
		friend class LinkedList<T_, SK_, growth>;
	protected:
		LinkedListItem<T_, SK_, growth> *start,*current;
		bool reverse, autodestroy;

		LinkedListOrderedItemIterator(LinkedListItem<T_, SK_, growth>* start,bool reverse,bool autodestroy=false) {
			current=this->start=start;
			this->reverse=reverse;
			this->autodestroy=autodestroy;
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
		virtual LinkedListItem<T_, SK_, growth>* get() {
			LinkedListItem<T_, SK_, growth> *item;

			if(current==NULL) {
				if(autodestroy)
					delete this;
				return NULL;
			}

			item=current;

			if(reverse)
				current=current->order_prev;
			else
				current=current->order_next;

			return item;
		}

		virtual LinkedListItem<T_, SK_, growth>* peek() {

			if(current==NULL) {
				if(autodestroy)
					delete this;
				return NULL;
			}

			return current;
		}

		operator LinkedListItem<T_, SK_,growth>*() { return get(); }
		operator LinkedListItem<T_, SK_,growth>&() { return *get(); }
		LinkedListItem<T_, SK_,growth> *operator ->() { return get(); }
		LinkedListItem<T_, SK_,growth> &operator *() { return *get(); }


		////This method gets the next list item and moves item pointer to the next one
		T_* getobject() {
			if(current==NULL) {
				if(autodestroy)
					delete this;
				return NULL;
			}

			T_* item=current->Item;

			if(reverse)
				current=current->order_prev;
			else
				current=current->order_next;

			return item;
		}
		////This method gets the next item and moves item pointer to the next one
		operator T_*() {
			return getobject();
		}
	};


	////This class is a linked list structure.
	/// It allows insert and remove operations
	/// as well as sorted item query and iterators.
	template <class T_, class SK_, int growth>
	class LinkedList {

		friend class LinkedListIterator<T_, SK_, growth>;

	protected:
		int poolid;
		int count;
		LinkedListItem<T_, SK_, growth> **Pool;
		LinkedListItem<T_, SK_, growth> *first,*last,*order_first,*order_last;

		void grow() {
			Pool=new LinkedListItem<T_, SK_, growth>*[growth];
			int i;
			for(i=0;i<growth;i++)
				Pool[i]=new LinkedListItem<T_, SK_, growth>(this);

			poolid=growth-1;
		}

		LinkedListItem<T_, SK_, growth>* obtain() {
			if(poolid<0)
				grow();

			return Pool[poolid--];
		}
	public:

		enum {
			Ordered,
			OrderedReverse,
			Normal,
			Reverse
		} IterationType;

		////Returns number of elements
		int getCount() {
			return count;
		}

		////Basic constructor
		LinkedList() : count(0), poolid(-1), Pool(NULL), first(NULL), last(NULL), order_first(NULL), order_last(NULL), IterationType(Ordered)
		{ }

		////Changes the order of an item
		///@Position: position of the item to be reordered
		///@Order	: The new order
		void setOrderOf(int Position,SK_ Order) {
			setOrderOf(ItemAt(Position),Order);
		}

		////Changes the order of an item
		///@Item	: The item to be reordered
		///@Order	: The new order
		void setOrderOf(LinkedListItem<T_, SK_, growth> *Item,SK_ Order) {
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

			LinkedListOrderedIterator<T_, SK_, growth> i=GetOrderedIterator();
			LinkedListItem<T_, SK_, growth> *item;
			LinkedListItem<T_, SK_, growth> *ret=Item;
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
		void Remove(LinkedListItem<T_, SK_, growth> *item) {
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
		LinkedListItem<T_, SK_, growth> *getFirst() {
			return first;
		}

		////Gets the last item in list.
		/// Returns item wrapper
		LinkedListItem<T_, SK_, growth> *getLast() {
			return last;
		}

		////Gets the first item in ordered list.
		/// Returns item wrapper
		LinkedListItem<T_, SK_, growth> *getOrderedFirst() {
			return order_first;
		}

		////Gets the last item in ordered list.
		/// Returns item wrapper
		LinkedListItem<T_, SK_, growth> *getOrderedLast() {
			return order_last;
		}

		SK_ HighestOrder() {
			if(order_last)
				return order_last->Order;

			return 0;
		}

		SK_ LowestOrder() {
			if(order_first)
				return order_first->Order;

			return 0;
		}

		////Removes an item from the list
		void Remove(T_ *Item) {
			if(Item==NULL)
				return;
		
			LinkedListItem<T_, SK_, growth>* item=FindListItem(Item);
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
			LinkedListItem<T_, SK_, growth>* item=ListItemAt(Position);
			if(item)
				return item->Item;
			else
				return NULL;
		}

		////Returns an item from a given position
		T_ *ItemAt(int Position) {
			LinkedListItem<T_, SK_, growth>* item=ListItemAt(Position);
			if(item)
				return item->Item;
			else
				return NULL;
		}

		////Returns a list item from a given position
		LinkedListItem<T_, SK_, growth> *ListItemAt(int Position) {
			LinkedListIterator<T_, SK_, growth> i=(*this);
			LinkedListItem<T_, SK_, growth> *item;

			while(item=i)
				if(Position--==0)
					return item;
	
			return NULL;
		}

		////Returns an item from a given ordered position
		T_ *OrderedItemAt(int Position) {
			LinkedListItem<T_, SK_, growth>* item=OrderedListItemAt(Position);
			if(item)
				return item->Item;
			else
				return NULL;
		}

		////Returns a list item from a given ordered position
		LinkedListItem<T_, SK_, growth> *OrderedListItemAt(int Position) {
			LinkedListOrderedIterator<T_, SK_, growth> i=(*this);
			LinkedListItem<T_, SK_, growth> *item;

			while(item=i)
				if(Position--==0)
					return item;

			return NULL;
		}

		////Returns an item from a given ordered position
		T_ *ReverseOrderedItemAt(int Position) {
			LinkedListItem<T_, SK_, growth>* item=OrderedListItemAt((getCount()-1) - Position);
			if(item)
				return item->Item;
			else
				return NULL;
		}

		////Returns a list item from a given ordered position
		LinkedListItem<T_, SK_, growth> *ReverseOrderedListItemAt(int Position) {
			LinkedListOrderedIterator<T_, SK_, growth> i=GetReverseOrderedIterator();
			LinkedListItem<T_, SK_, growth> *item;

			while(item=i)
				if(Position--==0)
					return item;

			return NULL;
		}

		////Searches a specific item in the list and returns the list item.
		/// If item is not found NULL is returned
		LinkedListItem<T_, SK_, growth> *FindListItem(T_ *Item) {
			LinkedListIterator<T_, SK_, growth> it=*this;
			LinkedListItem<T_, SK_, growth> *item;
			while(item=it) {
				if(item->Item==Item)
					return item;
			}

			return NULL;
		}
		LinkedListItem<T_, SK_, growth> &FindListItem(T_ &Item) {
			LinkedListIterator<T_, SK_, growth> it=*this;
			LinkedListItem<T_, SK_, growth> *item;
			while(item=it) {
				if(*item->Item==Item)
					return *item;
			}

			return NULL;
		}
		////Creates an ordered iterator object
		operator LinkedListOrderedIterator<T_, SK_, growth>() {
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
		operator LinkedListIterator<T_, SK_, growth>() {
			return GetIterator();
		}
		////Creates an iterator object
		operator IIterator<T_>*() {
			switch(IterationType) {
			case Ordered:
				return new LinkedListOrderedIterator<T_, SK_, growth>(order_first,false, true);
			case OrderedReverse:
				return new LinkedListOrderedIterator<T_, SK_, growth>(order_last,true, true);
			case Reverse:
				return new LinkedListIterator<T_, SK_, growth>(last,true, true);
			case Normal:
			default:
				return new LinkedListIterator<T_, SK_, growth>(first,false, true);
			}
		}
		////Creates an iterator object
		operator IIterator<LinkedListItem<T_, SK_, growth> >*() {
			switch(IterationType) {
			case Ordered:
				return new LinkedListOrderedItemIterator<T_, SK_, growth>(order_first,false, true);
			case OrderedReverse:
				return new LinkedListOrderedItemIterator<T_, SK_, growth>(order_last,true, true);
			case Reverse:
				return new LinkedListItemIterator<T_, SK_, growth>(last,true, true);
			case Normal:
			default:
				return new LinkedListItemIterator<T_, SK_, growth>(first,false, true);
			}
		}

		////Creates an iterator object
		virtual LinkedListIterator<T_, SK_, growth> GetIterator() {
			return LinkedListIterator<T_, SK_, growth>(first,false);
		}

		////Creates a reverse iterator object
		virtual LinkedListIterator<T_, SK_, growth> GetReverseIterator() {
			return LinkedListIterator<T_, SK_, growth>(last,true);
		}

		////Creates an ordered iterator object
		virtual LinkedListOrderedIterator<T_, SK_, growth> GetOrderedIterator() {
			return LinkedListOrderedIterator<T_, SK_, growth>(order_first,false);
		}

		////Creates an ordered reverse iterator object
		virtual LinkedListOrderedIterator<T_, SK_, growth> GetReverseOrderedIterator() {
			return LinkedListOrderedIterator<T_, SK_, growth>(order_last,true);
		}

		LinkedListItem<T_, SK_, growth> *Add(T_ *Item, SK_ Order=SK_()) { return AddItem(Item, Order); }

		LinkedListItem<T_, SK_, growth> *Add(T_& Item, SK_ Order=SK_()) { return AddItem(&Item, Order);}

		LinkedListItem<T_, SK_, growth> *AddItem(T_& Item,SK_ Order=SK_()) { return AddItem(&Item, Order);}


		////Adds a new item to the list. This function returns the list item
		/// that surrounds the newly added item
		///@Item	: The item to be added to the list
		///@Order	: The order of the item to be added
		LinkedListItem<T_, SK_, growth> *AddItem(T_ *Item,SK_ Order=SK_()) {
			LinkedListItem<T_, SK_, growth> *ret=obtain();
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

				LinkedListOrderedIterator<T_, SK_, growth> i=GetOrderedIterator();
				LinkedListItem<T_, SK_, growth> *item;

				bool done=false;

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
					order_last=ret;
				}
			}

			return ret;
		}
	};
}}