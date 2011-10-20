#pragma once


//#include "Interfaces\ILabel.h"
#include "Listbox\ListboxBase.h"
#include "..\Utils\Property.h"
#include "Listbox\ListItem.h"
#include "..\Utils\OrderedCollection.h"


namespace gge { namespace widgets {

	//multi select, direction keys, insert

	//This listbox is not for 100s of items
	template<class T_=std::string, void(*CF_)(const T_ &, std::string &)=listbox::CastToString<T_> >
	class Listbox : public listbox::Base<T_, CF_>, ListItemModifier<T_, CF_>, utils::OrderedCollection<ListItem<T_, CF_> > {
	public:

		enum SelectionTypes {
			SingleSelect,
			ToggleSelect, //click toggles
			MultiSelect, //control click adds to selected, support for shift multi select and drag multi select
		};

		typedef ListItem<T_, CF_> ListItem;
		typedef OrderedCollection<ListItem>::Iterator Iterator;
		typedef OrderedCollection<ListItem>::ConstIterator ConstIterator;
		typedef OrderedCollection<ListItem>::SearchIterator SearchIterator;
		typedef OrderedCollection<ListItem>::ConstSearchIterator ConstSearchIterator;

		Listbox() : selectiontype(SingleSelect), active(NULL),
			INIT_PROPERTY(Listbox, SelectionType)
		{
			notifyevent.Register(this, &Listbox::togglenotify);
		}

		ListItem &Add(const T_ &value=T_()) {
			ListItem *li=new ListItem(notifyevent);
			if(bp->Item)
				li->SetBlueprint(*bp->Item);

			li->Value=value;
			add(*li);
			OrderedCollection::Add(li);

			return *li;
		}

		ListItem &Insert(const ListItem &before, const T_ &value=T_()) {

		}

		void Add(ListItem &item) {

		}

		void Insert(const ListItem &before, ListItem &item) {

		}

		void Remove(ListItem &item) {
			item.Detach();
			items.Remove(item);
		}

		void Delete(ListItem &item) {
			item.Detach();
			items.Delete(item);
		}

		void DeleteAll(const T_ &value) {
			for(auto it=First();it.isValid();it.Next()) {
				if(it->Value==value) {
					it->Detach();
					it.Delete();
				}
			}
		}

		Iterator First() {
			return OrderedCollection::First();
		}

		ConstIterator First() const {
			return OrderedCollection::First();
		}

		Iterator Last() {
			return OrderedCollection::Last();
		}

		ConstIterator Last() const {
			return OrderedCollection::Last();
		}

		Iterator begin() {
			return OrderedCollection::begin();
		}

		ConstIterator begin() const {
			return OrderedCollection::begin();
		}

		Iterator end() {
			return OrderedCollection::end();
		}

		ConstIterator end() const {
			return OrderedCollection::end();
		}

		SearchIterator send() {
			return OrderedCollection::send();
		}

		ConstSearchIterator send() const {
			return OrderedCollection::send();
		}

		void DeleteAll() {
			Destroy();
		}

		void Destroy() {
			panel.Widgets.Clear();
			OrderedCollection::Destroy();
		}

		void Clear() {
			panel.Widgets.Clear();
			OrderedCollection::Clear();
		}

		int GetCount() const {
			return OrderedCollection::getCount();
		}

		T_ GetValue(int Index) {
			if(OrderedCollection::Get(Index))
				return OrderedCollection::Get(Index).Value;

			return T_();
		}

		//returns selected item value
		//returns last selected if listbox is in multi select
		T_ GetValue() {
			return active->Value;
		}

		ListItem *GetItem(int Index) {
			return OrderedCollection::Get(Index);
		}

		//returns selected item
		//returns last selected if listbox is in multi select
		ListItem *GetItem() {
			return active;
		}

		//returns last selected if listbox is in multi select
		ListItem *GetSelectedItem() {
			return active;
		}
		
		//works only for multi select
		utils::ConstCollection<ListItem> GetSelectedItems() {
			return selected;
		}

		template<class C_>
		void operator +=(const C_ &values) {
			for(auto it=values.begin();it!=values.end();++it)
				Add(*it);
		}

		template<class I_>
		void AddRange(const I_ &begin, const I_ &end) {
			for(auto it=begin;it!=end;++it)
				Add(*it);
		}

		utils::Property<Listbox, SelectionTypes> SelectionType;

	protected:
		utils::EventChain<Base, IListItem<T_, CF_>*> notifyevent;

		void setSelectionType(const SelectionTypes &value) {
			if(selectiontype==MultiSelect && value!=MultiSelect) {
				selected.Clear();
			}
			selectiontype=value;
		}
		SelectionTypes getSelectionType() const {
			return selectiontype;
		}

		SelectionTypes selectiontype;

		utils::Collection<ListItem> selected;
		ListItem *active;

		void clearall(ListItem *item=NULL) {
			for(auto it=First();it.isValid();it.Next())
				if(it!=item) callclear(*it);
		}

		void togglenotify(IListItem<T_, CF_> *li) {
			ListItem* item=dynamic_cast<ListItem*>(li);
			if(!item) return;

			if(selectiontype==SingleSelect) {
				callclear(*active);
				active=item;
				callcheck(*active);
			}
			else if(selectiontype==ToggleSelect) {
				if(item->IsSelected()) {
					active=NULL;
					callclear(*item);
				}
				else {
					if(active)
						callclear(*active);

					active=item;
					callcheck(*active);
				}
			}
			else if(selectiontype==MultiSelect) {
				using namespace input::keyboard;
				
				if(Modifier::Current==Modifier::Ctrl) {
					if(selected.FindLocation(item)==-1) {
						selected.Add(item);
						callcheck(*item);

						active=item;
					}
					else {
						selected.Remove(item);
						callclear(*item);

						active=selected(selected.getCount()-1);
					}
				}
				else {
					clearall();
					selected.Clear();
					selected.Add(item);
					callcheck(*item);
					active=item;
				}
			}
		}
	};


}}
