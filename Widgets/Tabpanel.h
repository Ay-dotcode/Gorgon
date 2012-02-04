#pragma once

#include "Base/Widget.h"
#include "Basic/Placeholder.h"
#include "Checkbox/CheckboxBlueprint.h"
#include "Panel/PanelBlueprint.h"
#include "../Resource/ResourceBase.h"
#include "Panel.h"
#include "RadioButton.h"
#include "../Utils/OrderedCollection.h"
#include "NamedPanel.h"

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

	class Tabpanel : public WidgetBase, utils::OrderedCollection<NamedPanel> {
	public:


		typedef OrderedCollection<NamedPanel>::Iterator Iterator;
		typedef OrderedCollection<NamedPanel>::ConstIterator ConstIterator;
		typedef OrderedCollection<NamedPanel>::SearchIterator SearchIterator;
		typedef OrderedCollection<NamedPanel>::ConstSearchIterator ConstSearchIterator;

		Tabpanel() : bp(NULL), controls(*this) {

		}



		
		using WidgetBase::SetBlueprint;
		virtual void SetBlueprint(const Blueprint &bp);

		virtual bool detach(ContainerBase *container);

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

		virtual void Detach() {
			controls.BaseLayer.parent=NULL;
		}

		virtual bool loosefocus(bool force);

		virtual bool locating(ContainerBase *container, int Order) {
			WidgetBase::locating(container, Order);
			
			if(BaseLayer)
				BaseLayer->Add(controls);
			else
				controls.BaseLayer.parent=NULL;
		}

		virtual void containerenabledchanged(bool state) {
			controls.InformEnabledChange(state);
		}


		NamedPanel &Add(const std::string &title) {
			NamedPanel *panel=new NamedPanel;
			panel->Title=title;

			Add(*panel);

			return *panel;
		}

		void Add(NamedPanel &item) {
			if(bp)
				item.SetBlueprint(bp->Panel);

			controls.AddWidget(item);

			OrderedCollection::Add(item);

			if(GetCount())
				item.Hide();

			reorganize();
		}

		NamedPanel &Insert(const std::string &title, const NamedPanel &before) {
			return Insert(title, &before);
		}

		void Insert(NamedPanel &item, const NamedPanel &before) {
			Insert(item, &before);
		}

		NamedPanel &Insert(const std::string &value, const NamedPanel *before) {
			NamedPanel *panel=new NamedPanel;
			panel->Title=value;

			Insert(*panel, before);

			return *panel;
		}

		void Insert(NamedPanel &item, const NamedPanel *before) {
			if(bp)
				item.SetBlueprint(*bp->Panel);

			OrderedCollection::Insert(item, before);

			if(GetCount())
				item.Hide();


			reorganize();
		}

		void Remove(NamedPanel &item) {
			controls.RemoveWidget(item);
			reorganize();
			OrderedCollection::Remove(item);
		}

		void Delete(NamedPanel &item) {
			controls.RemoveWidget(item);
			reorganize();
			OrderedCollection::Delete(item);
		}

		NamedPanel *Find(const std::string &value) {
			for(auto it=First();it.isValid();it.Next()) {
				if(it->Title==value)
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
		RadioGroup<Panel*> buttons;
		PetContainer<Tabpanel> controls;
		const tabpanel::Blueprint *bp;
	};

}}
