#pragma once

#include "Panel/PanelBase.h"
#include "../Utils/Property.h"

namespace gge { namespace widgets { namespace tabpanel {
	class Panel : public panel::Base {

	public:

		Panel() : 
		  INIT_PROPERTY(tabpanel::Panel, Padding),
		  INIT_PROPERTY(tabpanel::Panel, Title),
		  TitleChangedEvent("TitleChanged", this)
		{
			setallowmove(false);
			setupvscroll(true, true, true);
		}



		utils::Property<tabpanel::Panel, utils::Margins> Padding;
		utils::TextualProperty<tabpanel::Panel> Title;

		utils::EventChain<tabpanel::Panel> TitleChangedEvent;


	protected:
		void setPadding(const utils::Margins &value) {
			Base::setpadding(value);
		}
		utils::Margins getPadding() const {
			return Base::getpadding();
		}

		void setTitle(const std::string &value) {
			if(gettitle()!=value) {
				Base::settitle(value);
				TitleChangedEvent();
			}
		}
		std::string getTitle() const {
			return Base::gettitle();
		}

	};

}}}
