#pragma once

#include "Base/Widget.h"
#include "Basic/Placeholder.h"
#include "Checkbox/CheckboxBlueprint.h"
#include "Panel/PanelBlueprint.h"
#include "../Resource/ResourceBase.h"
#include "Panel.h"
#include "RadioButton.h"
#include "../Utils/OrderedCollection.h"
#include "TabPanelPanel.h"

namespace gge { namespace widgets {
	
	namespace tabpanel {
		class Blueprint;

		Blueprint *Load(resource::File& File, std::istream &Data, int Size);

		class Blueprint : public widgets::Blueprint, public resource::ResourceBase {
			friend Blueprint *Load(resource::File& File, std::istream &Data, int Size);
		public:

			widgets::Placeholder Placeholder;
			checkbox::Blueprint *Radio;
			widgets::panel::Blueprint *Panel;

			void Prepare(GGEMain &main, resource::File &file);

			resource::GID::Type getGID() const { return GID::Tabpanel; }

		protected:
			utils::SGuid panel, radio, placeholder;
		};
	}

	class Tabpanel : public WidgetBase, utils::OrderedCollection<tabpanel::Panel> {
	public:


		typedef OrderedCollection<tabpanel::Panel>::Iterator Iterator;
		typedef OrderedCollection<tabpanel::Panel>::ConstIterator ConstIterator;
		typedef OrderedCollection<tabpanel::Panel>::SearchIterator SearchIterator;
		typedef OrderedCollection<tabpanel::Panel>::ConstSearchIterator ConstSearchIterator;

		Tabpanel() : bp(NULL), controls(*this), active(NULL) {

		}



		
		using WidgetBase::SetBlueprint;
		virtual void SetBlueprint(const Blueprint &bp);

		virtual void Draw() {
			for(auto it=First();it.isValid();it.Next()) {
				it->Draw();
			}
		}

		virtual bool Focus();

		virtual void Disable();

		virtual void Enable();

		virtual void Resize(utils::Size Size);

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

		void Activate(tabpanel::Panel &panel) {
			Activate(&panel);
		}

		void Activate(tabpanel::Panel *panel);

		tabpanel::Panel *GetActive() {
			return active;
		}

		tabpanel::Panel &Find(const std::string &value) {
			for(auto it=First();it.isValid();it.Next()) {
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

		virtual bool detach(ContainerBase *container) {
			controls.BaseLayer.parent=NULL;

			return WidgetBase::detach(container);
		}

		virtual bool loosefocus(bool force);

		virtual void located(ContainerBase* container, utils::SortedCollection<WidgetBase>::Wrapper *w, int Order) {
			WidgetBase::located(container, w, Order);

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
