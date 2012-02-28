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
			if(WR.Combobox)
				setblueprint(*WR.Combobox);
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
			OrderedCollection::Remove(item);
		}

		void Delete(ListItem &item) {
			listbox.Remove(item);
			OrderedCollection::Delete(item);
		}

		void DeleteAll(const T_ &value) {
			for(auto it=First();it.IsValid();it.Next()) {
				if(it->Value==value) {
					listbox.Remove(*it);
					it.Delete();
				}
			}
		}

		ListItem *Find(const T_ &value) {
			for(auto it=First();it.IsValid();it.Next()) {
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
			return Find(getvalue());
		}

		//returns last selected if listbox is in multi select
		ListItem *GetSelectedItem() {
			return Find(getvalue());
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

		virtual bool KeyboardEvent(input::keyboard::Event::Type event, input::keyboard::Key Key) {
			if(event==input::keyboard::Event::Up) {
				//if(!isextended) {
					if(!input::keyboard::Modifier::IsModified()) {
						if(Key==input::keyboard::KeyCodes::Down) {
							auto it=OrderedCollection::Find(GetSelectedItem());

							if(!it.IsValid()) {
								if(GetCount())
									setvalue(OrderedCollection::get_(0)->Value);
							}
							else {
								it.Next();
								if(it.IsValid()) {
									setvalue(it->Value);
								}
							}

							return true;
						}
						else if(Key==input::keyboard::KeyCodes::Up) {
							auto it=OrderedCollection::Find(GetSelectedItem());
							it.Previous();
							if(it.IsValid()) {
								setvalue(it->Value);
							}

							return true;
						}
					}
				//}
			}

			return Base::KeyboardEvent(event, Key);
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

		virtual void wr_loaded() {
			if(WR.Combobox && !blueprintmodified)
				setblueprint(*WR.Combobox);
		}
	};


}}
