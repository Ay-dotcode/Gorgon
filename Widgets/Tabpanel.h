#pragma once

#include "Base/Widget.h"
#include "Basic/Placeholder.h"
#include "Panel.h"
#include "RadioButton.h"
#include "../Utils/OrderedCollection.h"
#include "TabPanelPanel.h"
#include "TabPanelBlueprint.h"
#include "WidgetRegistry.h"

namespace gge { namespace widgets {
	
	class Tabpanel : public WidgetBase, utils::OrderedCollection<tabpanel::Panel> {
	public:


		typedef OrderedCollection<tabpanel::Panel>::Iterator Iterator;
		typedef OrderedCollection<tabpanel::Panel>::ConstIterator ConstIterator;
		typedef OrderedCollection<tabpanel::Panel>::SearchIterator SearchIterator;
		typedef OrderedCollection<tabpanel::Panel>::ConstSearchIterator ConstSearchIterator;

		Tabpanel() : bp(NULL), controls(*this), active(NULL), blueprintmodified(false) {
			if(WR.Panels.Tabpanel)
				setblueprint(*WR.Panels.Tabpanel);
		}



		
		using WidgetBase::SetBlueprint;
		virtual void SetBlueprint(const Blueprint &bp) {
			blueprintmodified=true;
			setblueprint(bp);
		}

		virtual void Draw() {
			for(auto it=First();it.IsValid();it.Next()) {
				it->Draw();
			}
		}

		virtual bool Focus();

		virtual void Disable();

		virtual void Enable();

		using WidgetBase::Resize;
		virtual void Resize(utils::Size Size);

		virtual utils::Size GetSize() const {
			if(!bp)
				return size;

			return utils::Size(size.Width ? size.Width : bp->DefaultSize.Width, size.Height ? size.Height : bp->DefaultSize.Height);
		}

		virtual void draw() { }

		virtual bool KeyboardEvent(input::keyboard::Event::Type event, input::keyboard::Key Key);


		tabpanel::Panel &Add(const std::string &title) {
			tabpanel::Panel *panel=new tabpanel::Panel;
			panel->Title=title;

			Add(*panel);

			return *panel;
		}

		void Add(tabpanel::Panel &item) {
			if(bp)
				item.SetBlueprint(bp->Panel);

			controls.AddWidget(item);

			OrderedCollection::Add(item);

			if(GetCount())
				item.Hide();

			reorganize();

			if(!active)
				Activate(item);
		}

		tabpanel::Panel &Insert(const std::string &title, const tabpanel::Panel &before) {
			return Insert(title, &before);
		}

		void Insert(tabpanel::Panel &item, const tabpanel::Panel &before) {
			Insert(item, &before);
		}

		tabpanel::Panel &Insert(const std::string &value, const tabpanel::Panel *before) {
			tabpanel::Panel *panel=new tabpanel::Panel;
			panel->Title=value;

			Insert(*panel, before);

			return *panel;
		}

		void Insert(tabpanel::Panel &item, const tabpanel::Panel *before) {
			if(bp)
				item.SetBlueprint(*bp->Panel);

			OrderedCollection::Insert(item, before);

			if(GetCount())
				item.Hide();


			reorganize();
			if(!active)
				Activate(item);
		}

		void Remove(tabpanel::Panel &item);

		void Delete(tabpanel::Panel &item) {
			Remove(item);
			delete &item;
		}

		void Activate(tabpanel::Panel &panel, bool setfocus=false) {
			Activate(&panel,setfocus);
		}

		void Activate(tabpanel::Panel *panel, bool setfocus=false);

		tabpanel::Panel *GetActive() {
			return active;
		}

		tabpanel::Panel &Find(const std::string &value) {
			for(auto it=First();it.IsValid();it.Next()) {
				if(it->Title==value)
					return *it;
			}

			throw std::runtime_error("Cannot find the tab specified");
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
		void DeleteAll() {
			Destroy();
		}

		void Destroy() {
			controls.Widgets.Clear();
			reorganize();
			OrderedCollection::Destroy();
		}

		void Clear() {
			controls.Widgets.Clear();
			reorganize();
			OrderedCollection::Clear();
		}

		int GetCount() const {
			return OrderedCollection::getCount();
		}
		void reorganize();

	protected:
		RadioGroup<tabpanel::Panel*> buttons;
		ExtendedPetContiner<Tabpanel> controls;
		const tabpanel::Blueprint *bp;

		tabpanel::Panel *active;
		bool blueprintmodified;

		void setblueprint(const widgets::Blueprint & bp);

		virtual void wr_change() {
			if(WR.Panels.Tabpanel && !blueprintmodified)
				setblueprint(*WR.Panels.Tabpanel);
		}

		virtual bool detach(ContainerBase *container) {
			controls.BaseLayer.parent=NULL;

			return WidgetBase::detach(container);
		}

		virtual bool loosefocus(bool force);

		virtual void located(ContainerBase* container, utils::SortedCollection<WidgetBase>::Wrapper *w, int Order) {
			WidgetBase::located(container, w, Order);

			if(BaseLayer)
				BaseLayer->Resize(controls.GetSize());

			if(container)
				controls.AttachTo(BaseLayer, &container->CreateExtenderLayer());
			else
				controls.AttachTo(NULL,NULL);
		}

		virtual void containerenabledchanged(bool state) {
			controls.InformEnabledChange(state);
		}

		void tab_click(RadioButton<tabpanel::Panel*> &object);
	};

}}
