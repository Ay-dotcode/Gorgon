#pragma once


//#include "Interfaces\ILabel.h"
#include "Listbox\ListboxBase.h"
#include "..\Utils\Property.h"
#include "Listbox\ListItem.h"
#include "..\Utils\OrderedCollection.h"


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

		typedef ListItem<T_, CF_> ListItem;
		typedef OrderedCollection<ListItem>::Iterator Iterator;
		typedef OrderedCollection<ListItem>::ConstIterator ConstIterator;
		typedef OrderedCollection<ListItem>::SearchIterator SearchIterator;
		typedef OrderedCollection<ListItem>::ConstSearchIterator ConstSearchIterator;

		Listbox() : selectiontype(SingleSelect), active(NULL),
			INIT_PROPERTY(Listbox, SelectionType),
			INIT_PROPERTY(Listbox, AutoHeight),
			ItemClickedEvent("ItemClicked", this)
		{
			if(WR.Listbox)
				setblueprint(*WR.Listbox);
		}

		ListItem &Add(const T_ &value=T_()) {
			ListItem *li=new ListItem(this, &Listbox::togglenotify);
			li->Value=value;

			Add(*li);

			return *li;
		}

		void Add(ListItem &item) {
			if(bp && bp->Item)
				item.SetBlueprint(*bp->Item);

			callsettoggle(item, this, &Listbox::togglenotify);
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
			ListItem *li=new ListItem(this, &Listbox::togglenotify);
			li->Value=value;

			Insert(*li, before);

			return *li;
		}

		void Insert(ListItem &item, const ListItem *before) {
			if(bp && bp->Item)
				item.SetBlueprint(*bp->Item);

			callsettoggle(item, this, &Listbox::togglenotify);
			OrderedCollection::Insert(item, before);
			add(item);
			reorganize();
		}

		ListItem &Insert(const T_ &value, const  T_ &before) {
			return Insert(value, Find(before));
		}

		void Insert(ListItem &item, const T_ &before) {
			Insert(item, Find(before));
		}

		void Remove(ListItem &item) {
			remove(item);
			OrderedCollection::Remove(item);
		}

		void Delete(ListItem &item) {
			remove(item);
			OrderedCollection::Delete(item);
		}

		void DeleteAll(const T_ &value) {
			for(auto it=First();it.IsValid();it.Next()) {
				if(it->Value==value) {
					remove(*it);
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
			panel.Widgets.Clear();
			adjustheight();
			OrderedCollection::Destroy();
		}

		void Clear() {
			panel.Widgets.Clear();
			adjustheight();
			OrderedCollection::Clear();
		}

		int GetCount() const {
			return OrderedCollection::GetCount();
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

		template<class P_>
		void Sort(P_ predicate=std::less<T_>) {
			OrderedCollection::Sort(predicate);

			reorganize();
		}

		void Sort() {
			OrderedCollection::Sort();

			reorganize();
		}

		utils::Property<Listbox, SelectionTypes> SelectionType;

		using WidgetBase::SetBlueprint;

		virtual void SetBlueprint(const widgets::Blueprint &bp) {
			Base::SetBlueprint(bp);

			for(auto it=First();it.IsValid();it.Next())
				it->SetBlueprint(*Base::bp->Item);

			if(GetCount())
				panel.SmallScroll=2*First()->GetHeight();
			else
				panel.SmallScroll=2*Base::bp->Item->DefaultSize.Height;

			panel.LargeScroll=panel.GetUsableSize().Height-panel.SmallScroll;
			if(panel.LargeScroll<panel.GetUsableSize().Height/2)
				panel.LargeScroll=panel.GetUsableSize().Height/2;

			adjustheight();
		}

		void ClearSelection() {
			clearall();
		}

		//!Keyboard handling

		utils::EventChain<Listbox, ListItem&> ItemClickedEvent;
		utils::BooleanProperty<Listbox> AutoHeight;

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

		utils::Collection<ListItem> selected;
		ListItem *active;

		void clearall(ListItem *item=NULL) {
			for(auto it=First();it.IsValid();it.Next())
				if(it.CurrentPtr()!=item) callclear(*it);
		}

		void togglenotify(IListItem<T_, CF_> *li, bool raise) {
			ListItem* item=dynamic_cast<ListItem*>(li);
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
			//panel.Widgets.Clear();

			for(auto it=First();it.IsValid();it.Next()) {
				panel.RemoveWidget(*it);
			}

			for(auto it=First();it.IsValid();it.Next()) {
				add(*it);
			}
		}

		void setAutoHeight(const bool &value) {
			setautoheight(value);
		}
		bool getAutoHeight() const {
			return getautoheight();
		}

		virtual void wr_loaded() {
			if(WR.Listbox && !blueprintmodified)
				setblueprint(*WR.Listbox);
		}
	};


}}
