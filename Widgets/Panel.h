#pragma once

#include "Panel/PanelBase.h"

namespace gge { namespace widgets {

	class Panel : public panel::Base {
	public:

		Panel() : INIT_PROPERTY(Panel, Padding)
		{
			setallowmove(true);
			setupvscroll(true, true, false);
		}

		utils::Property<Panel, utils::Margins> Padding;

	protected:
		void setPadding(const utils::Margins &value) {
			Base::setpadding(value);
		}
		utils::Margins getPadding() const {
			return Base::getpadding();
		}
	};

}}
