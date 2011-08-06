// THE CHANGE IS IN THE WIND

//DESCRIPTION
//	SortedCollection class is an sorted add/remove list with keys for sort
//	order. Allows iteration using std methods. This collection is a
//	doubly linked list, and sorting is done at insertion time.
//	Also hosts search iteration. This Collection is preferable for small
//	number of objects, it does not host any optimizations for access speed;
//	however, it has a very small memory overhead. Total size of the object
//	is 16 bytes. It can be passed by value, contents will not be duplicated
//	instead it will ref count the object list.
//	
//	IMPORTANT: Stored objects are not duplicated nor destroyed at the end
//	of the collection's life time. To destruct objects that are contained
//	within use .Destroy() method. This applies for removed objects, use
//	delete to destruct and remove them

//REQUIRES:
//	gge::utils::IteratorBase
//	gge::utils::RefCount

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

#include <cstdlib>
#include <cstring>
#include <stdexcept>

#include "Iterator.h"
#include "ManagedBuffer.h"
#include "UtilsBase.h"

#define SORTEDCOLLECTION_EXISTS

namespace gge { namespace utils {
	//SortedCollection is a linked list and cannot
	//employ object pools
	template<class T_, class K_=int>
	class SortedCollection : RefCounter<SortedCollection<T_, K_> > {
		friend class Iterator_;
		friend class RefCounter<SortedCollection>;
	public:
		class Wrapper {
			friend class SortedCollection;
		protected:
			T_ *item;
			K_ key;

			Wrapper *next;
			Wrapper *previous;
			SortedCollection *parent;

			Wrapper() {}

		public:
			operator T_ &() {
				return *item;
			}

			operator T_ *() {
				return *item;
			}

			T_ *operator ->() {
				return item;
			}

			operator const T_ &() const {
				return *item;
			}

			operator const T_ *() const {
				return *item;
			}

			const T_ *operator ->() const {
				return item;
			}

			Wrapper *Next() { return next; }
			const Wrapper *Next() const { return next; }

			Wrapper *Previous() { return previous; }
			const Wrapper *Previous() const { return previous; }

			K_ GetKey() {
				return key;
			}

			T_ &Get() {
				return *item;
			}

			T_ *GetPtr() {
				return item;
			}

			void SetItem(T_ &item) {
				this->item=&item;
			}

			void SetItem(T_ *item) {
				this->item=item;
			}

			void Reorder(const K_ &key) {
				parent->Reorder(*this, key);
			}

			void Remove() {
				parent->Remove(*this);
			}

			void Delete() {
				parent->Delete(*this);
			}

			int GetLocation() {
				return parent->FindLocation(*this);
			}

			SortedCollection &GetParent() {
				return *parent;
			}
		};

	protected:
		template<class O_>
		class Iterator_ : public IteratorBase<Iterator_<O_>, O_> {
			friend class IteratorBase<Iterator_, O_>;
			friend class ConstIterator;
			friend class SortedCollection;
		public:
			Iterator_() {
			}

			Iterator_(Wrapper *item) : Current(item), parent(NULL) {
				if(item) {
					parent=item->parent;
				}
			}

			Iterator_(Wrapper *item, SortedCollection *parent) : Current(item), parent(parent) {
			}

			Iterator_(Wrapper &item) : Current(item), parent(item.parent) {
			}

			Iterator_(const Iterator_&it) : Current(it.Current), parent(it.parent) {
			}

			Wrapper &GetWrapper() {
				if(!Current)
					throw std::out_of_range("Current item does not exists");

				return *Current;
			}

			void Reorder(const K_ &key)  {
				if(Current) {
					Current->parent->Reorder(*this, key);
				}
			}

			void Remove() {
				if(Current) {
					Current->parent->Remove(*this);
				}
			}

			void Delete() {
				if(Current) {
					Current->parent->Delete(*this);
				}
			}

			int GetLocation() {
				if(Current)
					return Current->parent->FindLocation(*this);
				else
					return -1;
			}

			K_ GetKey() {
				if(Current) {
					return Current->key;
				} else {
					throw std::runtime_error("Invalid location");
				}
			}

		protected:
			O_& current() const {
				if(!Current)
					throw std::runtime_error("Invalid location");
				return *(Current->item);
			}

			bool moveby(int amount) {
				if(Current==NULL && parent==NULL)
					return false;

				if(Current==NULL) {
					if(amount>0)
						return false;
					else {
						Current=*parent->tail;
						amount++;
					}
				}				

				while(amount>0) {
					Current=Current->next;
					amount--;

					if(Current==NULL) return false;
				}
				while(amount<0) {
					Current=Current->previous;
					amount++;

					if(Current==NULL) return false;
				}

				return true;
			}

			//compare parents? yes
			bool compare(const Iterator_ &it) const {
				if(Current==NULL && it.Current==NULL) {
					return true;
				}
				return parent == it.parent && Current==it.Current;
			}

			bool isvalid() const {
				return Current!=NULL;
			}

			void set(const Iterator_ &it) {
				Current=it.Current;
				parent=it.parent;
			}

			int distance(const Iterator_ &it) const {
				int dist=0;
				Wrapper *c;
				if(it.Current==NULL) {
					c=Current;
					while(c) {
						dist++;
						c=c->next;
					}

					return dist;
				}

				if(Current==NULL) return it.distance(*this);

				//or return 0?
				if(*(Current->parent->head) != *(it.Current->parent->head))
					throw new std::runtime_error("Iterators are not from the same container.");

				if(it.Current==*(it.Current->parent->head)) {
					c=it.Current;
					while(c && c!=Current) {
						dist--;
						c=c->next;
					}

					return dist;
				}

				//we have no idea about if it is forward or not,
				//so we will move randomly and if we fail, we
				//will try to other direction
				c=Current;
				while(c && c!=it.Current) {
					dist++;
					c=c->next;
				}

				if(c==NULL) {
					dist=0;
					c=it.Current;
					while(c && c!=Current) {
						dist--;
						c=c->next;
					}
				}


				return dist;
			}

			bool isbefore(const Iterator_ &it) const {
				Wrapper *c;
				if(it.Current==Current)
					return false;

				if(Current==NULL)
					return false;

				if(it.Current==NULL)
					return true;

				//or return exception?
				if(*(Current->parent->head) != *(it.Current->parent->head))
					return false;

				if(it.Current==*(it.Current->parent->head))
					return true;

				//we have no idea about if it is forward or not,
				//so we will move randomly and if we fail, we
				//will try to other direction
				c=Current;
				while(c && c!=it.Current) {
					c=c->next;
				}

				if(c==NULL)
					return false;
				else
					return true;
			}

		protected:
			Wrapper *Current;
			SortedCollection *parent;
		};
	public:
		typedef Iterator_<T_> Iterator;
		class ConstIterator : public Iterator_<const T_> {
			friend class ConstIterator;
			friend class SortedCollection;
		public:
			ConstIterator(const Iterator &it) {
				Current=it.Current;
			}
		};

		SortedCollection() : count(new int(0)), head(new Wrapper*(NULL)), tail(new Wrapper*(NULL)) {
		}

		SortedCollection(const SortedCollection &c) : head(c.head), tail(c.tail), count(c.count) {

		}

		Wrapper &Add(T_ *item, const K_ &key = K_()) {
			Wrapper *w;

			if(*head==NULL) {
				*head=new Wrapper();
				(*head)->item=item;
				(*head)->key=key;
				(*head)->previous=NULL;
				(*head)->next=NULL;
				(*head)->parent=this;
				w=*tail=*head;
			} else if((*head)->key >= key) {
				w=new Wrapper();
				w->item=item;
				w->key=key;
				w->previous=NULL;
				w->next=*head;
				w->parent=this;

				(*head)->previous=w;
				*head=w;
			} else if((*tail)->key < key) {
				w=new Wrapper();
				w->item=item;
				w->key=key;
				w->previous=*tail;
				w->next=NULL;
				w->parent=this;

				(*tail)->next=w;
				*tail=w;
			} else {
				Wrapper *c=(*head)->next;
				while(c) {
					if(c->key >= key) {
						w=new Wrapper();
						w->item=item;
						w->key=key;
						w->previous=c->previous;
						w->next=c;
						w->parent=this;

						c->previous->next=w;
						c->previous=w;

						break;
					}

					c=c->next;
				}
			}

			(*count)++;

			return *w;
		}

		Wrapper &Add(T_ &item, const K_ &key = K_()) {
			return Add(&item, key);
		}

		SortedCollection &operator +=(T_ &item) {
			Add(item);
		}

		SortedCollection &operator +=(T_ *item) {
			Add(item);
		}

		void Reorder(Wrapper &item, const K_ &key) {
			Wrapper *w=&item;
			w->key=key;
			
			if(w==*head && w==*tail) {
				return; //no need to do anything
			} 
			else if(w==*head) {
				if(w->next->key >= key) {
					return; //next item is larger than key, no need to do sorting
				}
				else {
					//moving forwards
					Wrapper *c=w->next;

					//detach
					w->next->previous=NULL;
					*head=w->next;

					//find suitable
					while(c) {
						if(c->key >= key) {
							w->next=c;
							w->previous=c->previous;

							c->previous->next=w;
							c->previous=w;

							return; //finished
						}

						c=c->next;
					}

					//if nowhere is suitable
					if(c==NULL) { //add as tail
						w->next	    = NULL;
						w->previous = *tail;

						(*tail)->next=w;
						*tail=w;
					}
				}
			}
			else if(w==*tail) {
				if(w->previous->key < key) {
					return; //prev item smaller than key, no need to do sorting
				}
				else {
					//moving backwards
					Wrapper *c=w->previous;

					//detach
					w->previous->next=NULL;
					*tail=w->previous;

					//find suitable
					while(c) {
						if(c->key < key) {
							w->previous=c;
							w->next=c->next;

							c->next->previous=w;
							c->next=w;

							return; //finished
						}

						c=c->previous;
					}

					//if nowhere is suitable
					if(c==NULL) { //add as head
						w->previous	    = NULL;
						w->next = *head;

						(*head)->previous=w;
						*head=w;
					}
				}
			} 
			else {
				if(w->next->key >= key) {
					if(w->previous->key < key) {
						return; //prev item smaller than key, no need to do sorting
					}
					else {
						Wrapper *c=w->previous;

						//detach
						w->previous->next=NULL;
						*tail=w->previous;

						//find suitable
						while(c) {
							if(c->key < key) {
								w->previous=c;
								w->next=c->next;

								c->next->previous=w;
								c->next=w;

								return; //finished
							}

							c=c->previous;
						}

						//if nowhere is suitable
						if(c==NULL) { //add as head
							w->previous	    = NULL;
							w->next = *head;

							(*head)->previous=w;
							*head=w;
						}
					}
				}
				else {
					//moving forwards
					Wrapper *c=w->next;

					//detach
					w->next->previous=NULL;
					*head=w->next;

					//find suitable
					while(c) {
						if(c->key >= key) {
							w->next=c;
							w->previous=c->previous;

							c->previous->next=w;
							c->previous=w;

							return; //finished
						}

						c=c->next;
					}

					//if nowhere is suitable
					if(c==NULL) { //add as tail
						w->next	    = NULL;
						w->previous = *tail;

						(*tail)->next=w;
						*tail=w;
					}
				}
			}
		}

		void Reorder(T_ &item, const K_ &key) {
			Reorder(Find(item), key);
		}

		void Reorder(T_ *item, const K_ &key) {
			Reorder(Find(item), key);
		}

		void Reorder(const Iterator &item, const K_ &key) {
			if(item.Current)
				Reorder(*item.Current, key);
		}

		void Remove(Iterator &item) {
			if(item.Current==NULL)
				return;

			Wrapper &w=*item.Current;
			item.Current=item.Current->next;

			Remove(w);

			return;
		}

		void Delete(Iterator &item) {
			if(item.Current==NULL)
				return;

			Wrapper &w=*item.Current;
			item.Current=item.Current->next;

			Remove(w);

			return;
		}

		void Remove(Wrapper &item) {
			if(item.previous) {
				item.previous->next=item.next;
			}
			else {
				*head=item.next;
			}
			if(item.next) {
				item.next->previous=item.previous;
			}
			else {
				*tail=item.previous;
			}

			delete &item;
			(*count)--;
		}

		void Delete(Wrapper &item) {
			delete item.item;
			Remove(item);
		}

		SortedCollection &operator -=(Wrapper &item) {
			Remove(item);

			return *this;
		}

		void Remove(T_ *item) {
			Iterator it=Find(item);
			Remove(it);
		}

		void Delete(T_ *item) {
			Iterator it=Find(item);
			Delete(it);
		}

		SortedCollection &operator -=(T_ *item) {
			Remove(item);

			return *this;
		}

		void Remove(T_ &item) {
			Iterator it=Find(item);
			Remove(it);
		}

		void Delete(T_ &item) {
			Iterator it=Find(item);
			Delete(it);
		}

		SortedCollection &operator -=(T_ &item) {
			Remove(item);

			return *this;
		}

		void Remove(int item) {
			Remove(GetItem(item));
		}

		void Delete(int item) {
			Delete(GetItem(item));
		}

		SortedCollection &operator -=(int item) {
			Remove(item);

			return *this;
		}

		SortedCollection &operator -=(Iterator &item) {
			return Remove(item);
		}

		void Destroy() {
			Wrapper *c=*head;

			while(c) {
				Wrapper *t=c->next;
				
				if(c->item)
					delete c->item;

				delete c;
				c=t;
			}

			*head=NULL;
			*tail=NULL;

			*count=0;
		}

		void Clear() {
			dealloc();
		}

		Iterator Find(const T_ *item) {
			Wrapper *c=*head;
			while(c) {
				if(c->item==item)
					return Iterator(c);

				c=c->next;
			}

			return Iterator(NULL);
		}

		Iterator Find(const T_ &item) {
			return Find(&item);
		}

		ConstIterator Find(const T_ *item) const {
			Wrapper *c=*head;
			while(c) {
				if(c->item==item)
					return ConstIterator(c);

				c=c->next;
			}

			return ConstIterator(NULL);
		}

		ConstIterator Find(const T_ &item) const {
			return Find(&item);
		}

		int FindLocation(const T_ *item) const {
			int loc=0;
			Wrapper *c=*head;
			while(c) {
				if(c->item==item)
					return loc;

				loc++;
				c=c->next;
			}
			
			return -1;
		}

		int FindLocation(const T_ &item) const {
			return FindLocation(item);
		}

		int FindLocation(const Wrapper &item) const {
			int loc=0;
			Wrapper *c=*head;
			while(c) {
				if(c==item)
					return loc;

				loc++;
				c=c->next;
			}

			return -1;
		}

		int getCount() const {
			return *count;
		}

		T_ &Get(int Index) {
			Wrapper *r=get_(Index);

			if(r==NULL)
				throw std::out_of_range("Index out of range");

			return *r;
		}
		
		const T_ &Get(int Index) const {
			const Wrapper *r=get_(Index);

			if(r==NULL)
				throw std::out_of_range("Index out of range");

			return *r;
		}

		Wrapper &GetItem(int Index) {
			
		}

		const Wrapper &GetItem(int Index) const {

		}

		////Returns the item at a given index
		T_& operator [] (int Index) {
			return Get(Index);
		}

		////Returns the item at a given index
		const T_& operator [] (int Index) const  {
			return Get(Index);
		}

		////Returns the item at a given index
		T_* operator () (int Index) {
			return get_(Index);
		}

		////Returns the item at a given index
		const T_* operator () (int Index) const {
			return get_(Index);
		}

		Iterator begin() {
			return Iterator(*head, this);
		}

		Iterator end() {
			return Iterator(NULL, this);
		}

		Iterator First() {
			return Iterator(*head, this);
		}

		Iterator Last() {
			return Iterator(*tail, this);
		}

		ConstIterator begin() const {
			return ConstIterator(*head, this);
		}

		ConstIterator end() const {
			return ConstIterator(NULL, this);
		}

		ConstIterator First() const {
			return ConstIterator(*head, this);
		}

		ConstIterator Last() const {
			return ConstIterator(*tail, this);
		}

		//This function returns an empty K_ if no items found
		K_ HighestOrder() {
			if(!(*tail))
				return K_();

			return (*tail)->key;
		}

		//This function returns an empty K_ if no items found
		K_ LowestOrder() {
			if(!(*head))
				return K_();

			return (*head)->key;
		}

		//throws no items runtime_error
		Wrapper &FirstItem() {
			if(!(*head))
				throw std::out_of_range("No items");

			return *(*head);
		}

		//throws no items runtime_error
		Wrapper &LastItem() {
			if(!(*tail))
				throw std::out_of_range("No items");

			return *(*tail);
		}
	protected:
		int *count;
		Wrapper **head, **tail;

		Wrapper *get_(int Index) {
			if(Index<0 || Index>*count)
				return NULL;

			Wrapper *c=*head;

			while(c) {
				if(!Index)
					return c;

				Index--;
				c=c->next;
			}


			return NULL;
		}

		const Wrapper *get_(int Index) const {
			if(Index<0 || Index>*count)
				return NULL;

			const Wrapper *c=*head;

			while(c) {
				if(!Index)
					return c;

				Index--;
				c=c->next;
			}


			return NULL;
		}

		void dealloc() {
			Wrapper *c=*head;

			while(c) {
				Wrapper *t=c;
				c=c.next;
				delete t;
			}

			*head=NULL;
			*tail=NULL;

			*count=0;
		}

		void destroy() {
			dealloc();

			delete head;
			delete tail;
			delete count;
		}

	};

} }
