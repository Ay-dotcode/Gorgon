#pragma once


#include "Combobox/ComboboxBase.h"
#include "Listbox/ListItem.h"


namespace gge { namespace widgets {


	template<class T_, void(*CS_)(const T_ &, std::string &)=combobox::CastToString<T_> >
	class Combobox : public combobox::Base<T_, CS_>, ListItemModifier<T_, CS_>, utils::OrderedCollection<ListItem<T_, CS_> > {
	public:
		typedef ListItem<T_, CS_> ListItem;
		typedef utils::OrderedCollection<ListItem>::Iterator Iterator;
		typedef utils::OrderedCollection<ListItem>::ConstIterator ConstIterator;
		typedef utils::OrderedCollection<ListItem>::SearchIterator SearchIterator;
		typedef utils::OrderedCollection<ListItem>::ConstSearchIterator ConstSearchIterator;


		Combobox() {
		}

		ListItem &Add(const T_ &value=T_()) {
			ListItem *li=new ListItem(this, &Combobox::togglenotify);
			li->Value=value;

			Add(*li);

			return *li;
		}

		void Add(ListItem &item) {
			callsettoggle(item, this, &Combobox::togglenotify);
			OrderedCollection::Add(item);
			add(item);
		}

		ListItem &Insert(const T_ &value, const ListItem &before) {
			return Insert(value, &before);
		}

		void Insert(ListItem &item, const ListItem &before) {
			Insert(item, &before);
		}

		ListItem &Insert(const T_ &value, const ListItem *before) {
			ListItem *li=new ListItem(this, &Combobox::togglenotify);
			li->Value=value;

			Insert(*li, before);

			return *li;
		}

		void Insert(ListItem &item, const ListItem *before) {
			callsettoggle(item, this, &Combobox::togglenotify);
			OrderedCollection::Insert(item, before);
			insert(item,before);
		}

		ListItem &Insert(const T_ &value, const  T_ &before) {
			return Insert(value, Find(before));
		}

		void Insert(ListItem &item, const T_ &before) {
			Insert(item, Find(before));
		}

		void Remove(ListItem &item) {
			listbox.Remove(item);
			this->Remove(item);
		}

		void Delete(ListItem &item) {
			listbox.Remove(item);
			this->Delete(item);
		}

		void DeleteAll(const T_ &value) {
			for(auto it=First();it.isValid();it.Next()) {
				if(it->Value==value) {
					listbox.Remove(*it);
					it.Delete();
				}
			}
		}

		ListItem *Find(const T_ &value) {
			for(auto it=First();it.isValid();it.Next()) {
				if(it->Value==value)
					return it.CurrentPtr();
			}

			return NULL;
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
			listbox.Clear();
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
			
		utils::EventChain<Combobox> ValueChanged;

	protected:
		virtual void valuechanged() {
			ValueChanged();
		}
		void togglenotify(IListItem<T_, CS_> *li, bool raise) {
			ListItem* item=dynamic_cast<ListItem*>(li);
			if(!item) return;
		}
	};


}}