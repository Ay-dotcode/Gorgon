#pragma once

#include "Panel/PanelBase.h"
#include "../Utils/Property.h"

namespace gge { namespace widgets {
	class NamedPanel : public panel::Base {

	public:

		NamedPanel() : 
		  INIT_PROPERTY(NamedPanel, Padding),
		  INIT_PROPERTY(NamedPanel, Title),
		  TitleChangedEvent("TitleChanged", this)
		{
			setallowmove(false);
			setupvscroll(true, true, true);
		}



		utils::Property<NamedPanel, utils::Margins> Padding;
		utils::TextualProperty<NamedPanel> Title;

		utils::EventChain<NamedPanel> TitleChangedEvent;


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

}}
