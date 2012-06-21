#pragma once


#include "Listbox/ListboxBase.h"
#include "../Utils/Property.h"
#include "Listbox/ListItem.h"
#include "../Utils/OrderedCollection.h"


namespace gge { namespace widgets {

	//multi select, direction keys

	//This listbox is not for 100s of items
	template<class T_=std::string, void(*CF_)(const T_ &, std::string &)=listbox::CastToString<T_> >
	class Listbox : public listbox::Base<T_, CF_>, ListItemModifier<T_, CF_>, utils::OrderedCollection<ListItem<T_, CF_> > {
	public:

		enum SelectionTypes {
			SingleSelect,
			ToggleSelect, //click toggles
			MultiSelect, //control click adds to selected, support for shift multi select and drag multi select
		};


		typedef ListItem<T_, CF_> ItemType;
		typedef utils::OrderedCollection<ItemType> CollectionType;
		typedef typename CollectionType::Iterator Iterator;
		typedef typename CollectionType::ConstIterator ConstIterator;
		typedef typename CollectionType::SearchIterator SearchIterator;
		typedef typename CollectionType::ConstSearchIterator ConstSearchIterator;

		Listbox() : selectiontype(SingleSelect), active(NULL),
			INIT_PROPERTY(Listbox, SelectionType),
			INIT_PROPERTY(Listbox, AutoHeight),
			INIT_PROPERTY(Listbox, Columns),
			INIT_PROPERTY(Listbox, AllowReorder),
			ItemClickedEvent("ItemClicked", this)
		{
			if(WR.Listbox)
				this->setblueprint(*WR.Listbox);
		}

		virtual ~Listbox() {
			
		}

		ItemType &Add(const T_ &value=T_()) {
			ItemType *li=new ItemType(this, &Listbox::togglenotify);
			li->Value=value;

			Add(*li);

			return *li;
		}

		void Add(ItemType &item) {
			if(this->bp && this->bp->Item)
				item.SetBlueprint(*this->bp->Item);

			callsettoggle(item, this, &Listbox::togglenotify);
			CollectionType::Add(item);
			this->add(item);
		}

		ItemType &Insert(const T_ &value, const ItemType &before) {
			return Insert(value, &before);
		}

		void Insert(ItemType &item, const ItemType &before) {
			Insert(item, &before);
		}

		ItemType &Insert(const T_ &value, const ItemType *before) {
			ItemType *li=new ItemType(this, &Listbox::togglenotify);
			li->Value=value;

			Insert(*li, before);

			return *li;
		}

		void Insert(ItemType &item, const ItemType *before) {
			if(this->bp && this->bp->Item)
				item.SetBlueprint(*this->bp->Item);

			callsettoggle(item, this, &Listbox::togglenotify);
			CollectionType::Insert(item, before);
			this->add(item);
			this->reorganize();
		}

		ItemType &Insert(const T_ &value, const  T_ &before) {
			return Insert(value, Find(before));
		}

		void Insert(ItemType &item, const T_ &before) {
			Insert(item, Find(before));
		}

		void MoveBefore(ItemType &item, const ItemType *before) {
			CollectionType::MoveBefore(item, before);
			this->reorganize();
		}

		void MoveBefore(const T_ &value, const ItemType *before) {
			CollectionType::MoveBefore(Find(value), before);
			this->reorganize();
		}

		void MoveBefore(ItemType &value, const T_ &before) {
			CollectionType::MoveBefore(value, Find(before));
			this->reorganize();
		}

		void MoveBefore(const T_ &value, const T_ &before) {
			CollectionType::MoveBefore(Find(value), Find(before));
			this->reorganize();
		}

		void Remove(ItemType &item) {
			remove(item);
			CollectionType::Remove(item);
		}

		void Delete(ItemType &item) {
			remove(item);
			CollectionType::Delete(item);
		}

		void DeleteAll(const T_ &value) {
			for(auto it=this->First();it.IsValid();it.Next()) {
				if(it->Value==value) {
					remove(*it);
					it.Delete();
				}
			}
		}

		ItemType *Find(const T_ &value) {
			for(auto it=this->First();it.IsValid();it.Next()) {
				if(it->Value==value)
					return it.CurrentPtr();
			}

			return NULL;
		}

		Iterator First() {
			return CollectionType::First();
		}

		ConstIterator First() const {
			return CollectionType::First();
		}

		Iterator Last() {
			return CollectionType::Last();
		}

		ConstIterator Last() const {
			return CollectionType::Last();
		}

		Iterator begin() {
			return CollectionType::begin();
		}

		ConstIterator begin() const {
			return CollectionType::begin();
		}

		Iterator end() {
			return CollectionType::end();
		}

		ConstIterator end() const {
			return CollectionType::end();
		}

		SearchIterator send() {
			return CollectionType::send();
		}

		ConstSearchIterator send() const {
			return CollectionType::send();
		}

		void DeleteAll() {
			Destroy();
		}

		void Destroy() {
			this->panel.Widgets.Clear();
			this->adjustheight();
			CollectionType::Destroy();
		}

		void Clear() {
			this->panel.Widgets.Clear();
			this->adjustheight();
			CollectionType::Clear();
		}

		int GetCount() const {
			return CollectionType::GetCount();
		}

		T_ GetValue(int Index) {
			if(CollectionType::Get(Index))
				return CollectionType::Get(Index).Value;

			return T_();
		}

		//returns selected item value
		//returns last selected if listbox is in multi select
		T_ GetValue() {
			if(active)
				return active->Value;
			else
				return T_();
		}

		void SetSelected(ItemType &list) {
			togglenotify(&list, false);
		}

		void SetSelected(ItemType *list) {
			if(list==NULL) return;
			SetSelected(*list);
		}

		ItemType *GetItem(int Index) {
			return CollectionType::Get(Index);
		}

		//returns selected item
		//returns last selected if listbox is in multi select
		ItemType *GetItem() {
			return active;
		}

		//returns last selected if listbox is in multi select
		ItemType *GetSelectedItem() {
			return active;
		}
		
		//works only for multi select
		utils::ConstCollection<ItemType> GetSelectedItems() {
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

		template<class P_>
		void Sort(P_ predicate=std::less<T_>()) {
			CollectionType::Sort(predicate);

			reorganize();
		}

		void Sort() {
			CollectionType::Sort();

			reorganize();
		}

		utils::Property<Listbox, SelectionTypes> SelectionType;

		using WidgetBase::SetBlueprint;

		virtual void SetBlueprint(const widgets::Blueprint &bp) {
			listbox::Base<T_, CF_>::SetBlueprint(bp);

			for(auto it=First();it.IsValid();it.Next())
				it->SetBlueprint(*listbox::Base<T_, CF_>::bp->Item);

			if(GetCount())
				this->panel.SmallScroll=2*this->First()->GetHeight();
			else
				this->panel.SmallScroll=2*listbox::Base<T_, CF_>::bp->Item->DefaultSize.Height;

			this->panel.LargeScroll=this->panel.GetUsableSize().Height-this->panel.SmallScroll;
			if(this->panel.LargeScroll<this->panel.GetUsableSize().Height/2)
				this->panel.LargeScroll=this->panel.GetUsableSize().Height/2;

			this->adjustheight();
		}

		void ClearSelection() {
			clearall();
		}

		//!Keyboard handling

		utils::EventChain<Listbox, ItemType&> ItemClickedEvent;
		utils::BooleanProperty<Listbox> AutoHeight;
		utils::NumericProperty<Listbox, int> Columns;
		utils::BooleanProperty<Listbox> AllowReorder;

	protected:
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

		utils::Collection<ItemType> selected;
		ItemType *active;

		void clearall(ItemType *item=NULL) {
			for(auto it=this->First();it.IsValid();it.Next())
				if(it.CurrentPtr()!=item) callclear(*it);
		}

		void togglenotify(IListItem<T_, CF_> *li, bool raise) {
			ItemType* item=dynamic_cast<ItemType*>(li);
			if(!item) return;

			if(selectiontype==SingleSelect) {
				if(active)
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

						active=selected(selected.GetCount()-1);
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

			if(raise)
				ItemClickedEvent(*item);
		}

		void reorganize() {
			for(auto it=this->First();it.IsValid();it.Next()) {
				this->panel.RemoveWidget(*it);
			}

			for(auto it=this->First();it.IsValid();it.Next()) {
				add(*it);
			}
		}

		virtual void movebefore(IListItem<T_, CF_> &item, IListItem<T_, CF_> *before) {
			MoveBefore(dynamic_cast<ItemType&>(item), dynamic_cast<ItemType*>(before));
		}

		void setAutoHeight(const bool &value) {
			this->setautoheight(value);
		}
		bool getAutoHeight() const {
			return this->getautoheight();
		}
		void setColumns(const int &value) {
			this->setcolumns(value);
		}
		int getColumns() const {
			return this->getcolumns();
		}
		void setAllowReorder(const bool &value) {
			this->setallowreorder(value);
		}
		bool getAllowReorder() const {
			return this->getallowreorder();
		}

		virtual void wr_loaded() {
			if(WR.Listbox && !this->blueprintmodified)
				this->setblueprint(*WR.Listbox);
		}
	};


}}
