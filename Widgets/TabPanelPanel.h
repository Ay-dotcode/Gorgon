#pragma once

#include "Panel/PanelBase.h"
#include "Panel.h"
#include "../Utils/Property.h"

namespace gge { namespace widgets { namespace tabpanel {
	class Panel : public widgets::Panel {
	public:

		Panel() : 
		  INIT_PROPERTY(tabpanel::Panel, Title),
		  TitleChangedEvent("TitleChanged", this),
		  inactive(true)
		{
			setallowmove(false);
			setupvscroll(true, true, true);
		}

		virtual void Show(bool setfocus=true) {
			panel::Base::Show(setfocus);
			adjustvisibility();
		}

		virtual void Hide() {
			panel::Base::Hide();
			adjustvisibility();
		}

		void HideScroll() { setupvscroll(true, false, true); }
		void ShowScroll() { setupvscroll(true, true, true); }

		virtual bool Focus() {
			if(inactive)
				return false;
			return panel::Base::Focus();
		}

		virtual void Enable() {
			panel::Base::Enable();
			if(Container)
				Container->WidgetBoundsChanged();
		}

		virtual void Disable() {
			panel::Base::Disable();
			if(Container)
				Container->WidgetBoundsChanged();
		}

		void setinactive(bool v) {
			inactive=v;
			adjustvisibility();
		}

		utils::TextualProperty<tabpanel::Panel> Title;

		utils::EventChain<tabpanel::Panel> TitleChangedEvent;


	protected:
		void setTitle(const std::string &value) {
			if(gettitle()!=value) {
				Base::settitle(value);
				TitleChangedEvent();
			}
		}
		std::string getTitle() const {
			return Base::gettitle();
		}

		void adjustvisibility() {
			if(!BaseLayer) return;
			BaseLayer->IsVisible=!inactive && WidgetBase::isvisible;
		}

		bool inactive;

	};

}}}
