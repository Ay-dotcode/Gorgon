#pragma once

#include "Combobox/ComboboxBase.h"
#include "Listbox/ListItem.h"


namespace gge { namespace widgets {

	template<class T_, void(*CS_)(const T_ &, std::string &)=combobox::CastToString<T_> >
	class Combobox : public combobox::Base<T_, CS_>, ListItemModifier<T_, CS_>, utils::OrderedCollection<ListItem<T_, CS_> > {
	public:
		typedef ListItem<T_, CS_> LI;
		typedef typename utils::OrderedCollection<LI>::Iterator Iterator;
		typedef typename utils::OrderedCollection<LI>::ConstIterator ConstIterator;
		typedef typename utils::OrderedCollection<LI>::SearchIterator SearchIterator;
		typedef typename utils::OrderedCollection<LI>::ConstSearchIterator ConstSearchIterator;
		typedef utils::OrderedCollection<LI> OC;


		Combobox() {
			if(WR.Combobox)
				this->setblueprint(*WR.Combobox);
		}

		LI &Add(const T_ &value=T_()) {
			LI *li=new LI(this, &Combobox::togglenotify);
			li->Value=value;

			Add(*li);

			return *li;
		}

		void Add(LI &item) {
			callsettoggle(item, this, &Combobox::togglenotify);
			OC::Add(item);
			add(item);
		}

		LI &Insert(const T_ &value, const LI &before) {
			return Insert(value, &before);
		}

		void Insert(LI &item, const LI &before) {
			Insert(item, &before);
		}

		LI &Insert(const T_ &value, const LI *before) {
			LI *li=new LI(this, &Combobox::togglenotify);
			li->Value=value;

			Insert(*li, before);

			return *li;
		}

		void Insert(LI &item, const LI *before) {
			callsettoggle(item, this, &Combobox::togglenotify);
			OC::Insert(item, before);
			insert(item,before);
		}

		LI &Insert(const T_ &value, const  T_ &before) {
			return Insert(value, Find(before));
		}

		void Insert(LI &item, const T_ &before) {
			Insert(item, Find(before));
		}

		void Remove(LI &item) {
			this->listbox.Remove(item);
			OC::Remove(item);
		}

		void Delete(LI &item) {
			this->listbox.Remove(item);
			OC::Delete(item);
		}

		void DeleteAll(const T_ &value) {
			for(auto it=First();it.IsValid();it.Next()) {
				if(it->Value==value) {
					this->listbox.Remove(*it);
					it.Delete();
				}
			}
		}

		LI *Find(const T_ &value) {
			for(auto it=First();it.IsValid();it.Next()) {
				if(it->Value==value)
					return it.CurrentPtr();
			}

			return NULL;
		}

		Iterator First() {
			return OC::First();
		}

		ConstIterator First() const {
			return OC::First();
		}

		Iterator Last() {
			return OC::Last();
		}

		ConstIterator Last() const {
			return OC::Last();
		}

		Iterator begin() {
			return OC::begin();
		}

		ConstIterator begin() const {
			return OC::begin();
		}

		Iterator end() {
			return OC::end();
		}

		ConstIterator end() const {
			return OC::end();
		}

		SearchIterator send() {
			return OC::send();
		}

		ConstSearchIterator send() const {
			return OC::send();
		}

		void DeleteAll() {
			Destroy();
		}

		void Destroy() {
			this->listbox.Clear();
			OC::Destroy();
		}

		void Clear() {
			this->panel.Widgets.Clear();
			OC::Clear();
		}

		int GetCount() const {
			return OC::GetCount();
		}

		T_ GetValue(int Index) {
			if(OC::Get(Index))
				return OC::Get(Index).Value;

			return T_();
		}

		//returns selected item value
		//returns last selected if listbox is in multi select
		T_ GetValue() {
			return this->getvalue();
		}

		void SetValue(const T_ &value) {
			setvalue(value);
		}

		LI *GetItem(int Index) {
			return &OC::Get(Index);
		}

		//returns selected item
		//returns last selected if listbox is in multi select
		LI *GetItem() {
			return Find(this->getvalue());
		}

		//returns last selected if listbox is in multi select
		LI *GetSelectedItem() {
			return Find(this->getvalue());
		}

		template<class P_>
		void Sort(P_ predicate=std::less<T_>()) {
			OC::Sort(predicate);
			this->listbox.Sort(predicate);
		}

		void Sort() {
			OC::Sort();
			this->listbox.Sort();
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

		virtual bool KeyboardHandler(input::keyboard::Event::Type event, input::keyboard::Key Key) {
			if(event==input::keyboard::Event::Up) {
				//if(!isextended) {
					if(!input::keyboard::Modifier::IsModified()) {
						if(Key==input::keyboard::KeyCodes::Down) {
							auto it=OC::Find(GetSelectedItem());

							if(!it.IsValid()) {
								if(GetCount())
									setvalue(OC::get_(0)->Value);
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
							auto it=OC::Find(GetSelectedItem());
							it.Previous();
							if(it.IsValid()) {
								setvalue(it->Value);
							}

							return true;
						}
					}
				//}
			}

			return combobox::Base<T_, CS_>::KeyboardHandler(event, Key);
		}
			
		utils::EventChain<Combobox> ChangeEvent;

	protected:
		virtual void valuechanged() {
			ChangeEvent();
		}
		void togglenotify(IListItem<T_, CS_> *li, bool raise) {
			LI* item=dynamic_cast<LI*>(li);
			if(!item) return;
		}

		virtual void wr_loaded() {
			if(WR.Combobox && !this->blueprintmodified)
				this->setblueprint(*WR.Combobox);
		}
	};


}}
