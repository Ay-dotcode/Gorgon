#pragma once

#include "Combobox/ComboboxBase.h"
#include "Listbox/ListItem.h"


namespace gge { namespace widgets {

	template<class T_=std::string, void(*CF_)(const T_ &, std::string &)=listbox::CastToString<T_>,  graphics::RectangularGraphic2D*(*GetIcon)(const T_&)=listbox::GetIcon<T_> >
	class Combobox : public combobox::Base<T_, listbox::valueaccessor<T_>, Listbox<T_, CF_, GetIcon>, CF_> {
	public:

		typedef typename Listbox<T_, CF_, GetIcon>::Iterator/*<T_, listbox::valueaccessor<T_> >*/ Iterator;


		Combobox() : 
			INIT_PROPERTY(Combobox, Value)
		{
			if(WR.Combobox)
				this->setblueprint(*WR.Combobox);
		}

		Combobox &operator=(const T_ &value) {
			this->SetValue(value);

			return *this;
		}

		void Add(const T_ &value=T_()) {
			this->add(value);
		}

		void Insert(const T_ &value, unsigned before) {
			return this->insert(value, before);
		}
		
		void MoveBefore(unsigned index, unsigned before) {
			this->listbox.MoveBefore(index, before);
		}
		
		void Remove(unsigned item) {
			this->remove(item);
		}

		Iterator First() {
			return this->listbox.First();
		}

		Iterator Last() {
			return this->listbox.Last();
		}

		Iterator begin() {
			return this->listbox.begin();
		}

		Iterator end() {
			return this->listbox.end();
		}

		void Clear() {
			this->listbox.Clear();
		}

		int GetCount() const {
			return this->listbox.GetCount();
		}

		T_ Get(int Index) const {
			return this->listbox.Get(index);
		}

		T_ Get() const {
			return this->value;
		}

		int SelectedIndex() const {
			return this->listbox.ActiveIndex();
		}

		void SetSelection(int index) {
			this->setValue(this->listbox[index]);
		}

		void SelectFirst() {
			SetSelection(0);
		}

		T_ operator[](int Index) const {
			return Get(Index);
		}

		T_ &operator[](int Index) {
			return Get(Index);
		}

		template<class P_>
		void Sort(P_ predicate=std::less<T_>()) {
			this->listbox.Sort(predicate);
		}

		void Sort() {
			this->listbox.Sort();
		}

		template<class C_>
		void operator +=(const C_ &values) {
			for(auto it=values.begin();it!=values.end();++it)
				Add(*it);
		}

		template<class I_>
		void AddRange(const I_ &begin, const I_ &end) {
			this->list.AddRange(begin,end);
		}

		template<class C_>
		void AddAll(const C_ &container) {
			this->list.AddAll(container);
		}

		bool IsValid() const {
			for(auto it=this->listbox.First(); it.IsValid(); it.Next()) {
				if(*it==this->value) return true;
			}

			return false;
		}
			
		utils::EventChain<Combobox> ChangeEvent;
		utils::Property<Combobox, T_> Value;

	protected:
		virtual void valuechanged() {
			ChangeEvent();
		}

		void setValue(const T_ &value) {
			this->setvalue(value);
		}
		T_ getValue() const {
			return Get();
		}

		virtual void wr_loaded() {
			if(WR.Combobox && !this->blueprintmodified)
				this->setblueprint(*WR.Combobox);
		}
	};


	template<class T_=std::string, void(*CF_)(const T_ &, std::string &)=listbox::CastToString<T_>,  graphics::RectangularGraphic2D*(*GetIcon)(const T_&)=listbox::GetIcon<T_> >
	class Selectbox : public combobox::Base<T_, listbox::ptraccessor<T_>, Collectionbox<T_, CF_, GetIcon>, CF_> {
	public:

		typedef typename Collectionbox<T_, CF_, GetIcon>::Iterator/*<T_, listbox::ptraccessor<T_> >*/ Iterator;


		Selectbox() : 
			INIT_PROPERTY(Selectbox, Value)
		{
			if(WR.Combobox)
				this->setblueprint(*WR.Combobox);
		}

		Selectbox &operator=(const T_ &value) {
			this->SetValue(value);

			return *this;
		}

		void Add(T_ &value) {
			this->add(value);
		}

		void Add(T_ *value) {
			if(value)
				this->add(value);
		}

		T_ &AddNew() {
			T_ &New=*new T_;
			Add(New);
			return New;
		}

		void Insert(T_ &value, const  T_ *before) {
			this->insert(value, this->listbox.Find(before));
		}

		void Insert(T_ &value, const  T_ &before) {
			Insert(value, this->listbox.Find(before));
		}

		void Insert(T_ &value, unsigned before) {
			this->insert(value,before);
		}

		void Insert(T_ *value, const  T_ *before) {
			if(value)
				Insert(*value, before);
		}

		void Insert(T_ *value, const  T_ &before) {
			if(value)
				Insert(*value, &before);
		}

		void Insert(T_ *value, unsigned before) {
			if(value)
				Insert(*value, before);
		}

		void MoveBefore(T_ &value, const T_ &before) {
			this->listbox.movebefore(value, before);
		}

		void MoveBefore(T_ &value, unsigned before) {
			this->listbox.movebefore(value, before);
		}

		void MoveBefore(unsigned index, const T_ &before) {
			this->listbox.movebefore(index, before);
		}

		void MoveBefore(unsigned index, unsigned before) {
			this->listbox.movebefore(index, before);
		}

		void Remove(T_ &item) {
			this->Remove(&item);
		}

		void Remove(T_ *item) {
			this->remove(this->listbox.Find(item));
		}

		void Remove(unsigned index) {
			this->remove(index);
		}

		void Delete(T_ &item) {
			Delete(this->listbox.Find(item));
		}

		void Delete(T_ *item) {
			if(!item) return;

			Delete(this->listbox.Find(item));
		}

		void Delete(unsigned index) {
			T_ *item=&this->Get(index);

			this->remove(index);

			delete item;
		}

		void DeleteAll() {
			Destroy();
		}

		void Destroy() {
			for(auto it=this->listbox.items.begin();it!=this->listbox.items.end();++it) {
				delete it->item;
			}

			Clear();
		}


		Iterator First() {
			return this->listbox.First();
		}

		Iterator Last() {
			return this->listbox.Last();
		}

		Iterator begin() {
			return this->listbox.begin();
		}

		Iterator end() {
			return this->listbox.end();
		}

		void Clear() {
			this->listbox.Clear();
		}

		int GetCount() const {
			return this->listbox.GetCount();
		}

		const T_ &Get() const {
			return this->value;
		}

		const T_ &Get(int Index) const {
			return this->listbox.Get(index);
		}

		T_ &Get() {
			return this->value;
		}

		T_ &Get(int Index) {
			return this->listbox.Get(index);
		}

		int SelectedIndex() const {
			return this->listbox.ActiveIndex();
		}

		const T_ &operator[](int Index) const {
			return Get(Index);
		}

		T_ &operator[](int Index) {
			return Get(Index);
		}

		template<class P_>
		void Sort(P_ predicate=std::less<T_>()) {
			this->listbox.Sort(predicate);
		}

		void Sort() {
			this->listbox.Sort();
		}

		template<class C_>
		void operator +=(const C_ &values) {
			for(auto it=values.begin();it!=values.end();++it)
				Add(*it);
		}

		template<class I_>
		void AddRange(const I_ &begin, const I_ &end) {
			this->listbox.AddRange(begin,end);
		}

		template<class C_>
		void AddAll(const C_ &container) {
			this->listbox.AddAll(container);
		}

		bool IsValid() const {
			for(auto it=this->listbox.First(); it.IsValid(); it.Next()) {
				if(it.CurrentPtr()==this->value) return true;
			}

			return false;
		}

		utils::EventChain<Selectbox> ChangeEvent;
		utils::Property<Selectbox, T_> Value;

	protected:
		virtual void valuechanged() {
			ChangeEvent();
		}

		void setValue(const T_ &value) {
			this->setvalue(value);
		}
		T_ getValue() const {
			return Get();
		}

		virtual void wr_loaded() {
			if(WR.Combobox && !this->blueprintmodified)
				this->setblueprint(*WR.Combobox);
		}
	};


}}
