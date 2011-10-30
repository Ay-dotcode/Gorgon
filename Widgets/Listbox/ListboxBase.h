#pragma once


#include "..\Base\Widget.h"
#include "..\Interfaces\IScroller.h"
#include "ListboxBlueprint.h"
#include <map>
#include <queue>
#include "..\Main.h"
#include "..\Basic\PetContainer.h"
#include "..\Interfaces\IScroller.h"
#include "..\Scrollbar.h"
#include "..\Panel\ControlledPanel.h"
#include "IListItem.h"
#include "Organizers/ListOrganizer.h"

namespace gge { namespace widgets {
	namespace listbox {

		template<class T_>
		void CastToString(const T_ &v, std::string &str) {
			str=(std::string)v;
		}

		template<class T_, void(*CF_)(const T_ &, std::string &)=CastToString<T_> >
		class Base : public WidgetBase {
		public:

			Base() : bp(NULL),  controls(*this)
			{
				controls.AddWidget(panel);
				panel.Move(0,0);
				panel.SetOrganizer(organizer);
				panel.AllowTabSwitch=false;
			}


			virtual bool IsEnabled() {
				return WidgetBase::IsEnabled();
			}

			virtual void Enable() {
				controls.Enable();
				WidgetBase::Enable();
			}

			virtual void Disable() {
				controls.Disable();
				WidgetBase::Disable();
			}

			virtual void Draw() {
				panel.Draw();
				WidgetBase::Draw();
			}

			virtual bool Focus() {
				if(IsEnabled() && IsVisible())
					panel.Focus();

				return WidgetBase::Focus();
			}

			virtual void SetBlueprint(const widgets::Blueprint &bp);

			virtual bool MouseEvent(input::mouse::Event::Type event, utils::Point location, int amount) {
				return panel.MouseEvent(event, location, amount);
			}

			virtual bool KeyboardEvent(input::keyboard::Event::Type event, input::keyboard::Key Key) {
				return panel.KeyboardEvent(event, Key);
			}

			virtual void Resize(utils::Size Size) {
				WidgetBase::Resize(Size);
				panel.Resize(Size);
				controls.Resize(Size);
			}
			
			

		protected:

			virtual void add(IListItem<T_, CF_> &item) {
				panel.AddWidget(item.GetWidget());
			}

			virtual void draw() {}

			virtual bool loosefocus(bool force) {
				if(force) {
					panel.ForceRemoveFocus();
					return true;
				}
				else
					return panel.RemoveFocus();
			}

			virtual bool detach(ContainerBase *container) {
				controls.BaseLayer.parent=NULL;

				return true;
			}

			virtual void containerenabledchanged(bool state) {
				controls.InformEnabledChange();
			}

			virtual void located(ContainerBase* container, utils::SortedCollection<WidgetBase>::Wrapper *w, int Order) {
				WidgetBase::located(container, w, Order);

				BaseLayer->Add(controls);
			}


			const Blueprint *bp;

			PetContainer<Base> controls;

		//TODO Need a new panel that allows removal of tab function
			ControlledPanel panel;
			

			ListOrganizer organizer;
		private:
		};

		template<class T_, void(*CF_)(const T_ &, std::string &)>
		void Base<T_, CF_>::SetBlueprint(const widgets::Blueprint &bp) {
			if(this->bp==&bp)
				return;

			this->bp=static_cast<const Blueprint*>(&bp);

			if(this->bp->Panel)
				panel.SetBlueprint(*this->bp->Panel);

			if(WidgetBase::size.Width==0)
				Resize(this->bp->DefaultSize);
		}

	}
}}

