#pragma once


//#include "Interfaces\ILabel.h"
#include "Listbox\ListboxBase.h"
#include "..\Utils\Property.h"
#include "Listbox\ListItem.h"


namespace gge { namespace widgets {

	//Selection style, focus, insert

	template<class T_=std::string, void(*CF_)(const T_ &, std::string &)=listbox::CastToString<T_> >
	class Listbox : public listbox::Base<T_, CF_>, ListItemModifier<T_, CF_> {
	public:

		typedef ListItem<T_, CF_> ListItem;

		Listbox() {
			notifyevent.Register(this, &Listbox::togglenotify);
		}

		ListItem &Add(const T_ &value=T_()) {
			ListItem *li=new ListItem(notifyevent);
			if(bp->Item)
				li->SetBlueprint(*bp->Item);

			li->Value=value;
			add(*li);
			items.Add(li);

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
			for(auto it=items.First();it.isValid();it.Next()) {
				if(it->Value==value) {
					it->Detach();
					it.Delete();
				}
			}
		}

		void DeleteAll() {
			Destroy();
		}

		void Destroy() {
			panel.Widgets.Clear();
			items.Destroy();
		}

		void Clear() {
			panel.Widgets.Clear();
			items.Clear();
		}

		int GetCount() const {
			return items.getCount();
		}

		T_ GetValue(int Index) {
			if(items[Index])
				return items[Index]->Value;

			return T_();
		}

		T_ GetValue() {
			//!selected item
		}

		ListItem *GetItem(int Index) {
			return items[Index];
		}

		ListItem *GetItem() {
			//!selected item
		}

		ListItem *GetSelectedItem() {
			//!selected item
		}

		ListItem *GetActiveItem() {
			//!focus
		}

		std::vector<ListItem> GetSelectedItems() {
			//!selected item
		}

		void GetSelectedItems(std::vector<ListItem> &list) {
			//!selected item
		}

	protected:
		utils::EventChain<Base, IListItem<T_, CF_>*> notifyevent;
		utils::Collection<ListItem> items;

		void togglenotify(IListItem<T_, CF_> *li) {
			ListItem* item=dynamic_cast<ListItem*>(li);
			if(!item) return;

			for(auto it=items.First();it.isValid();it.Next())
				callclear(*it);

			callcheck(*item);
		}

	};


}}
