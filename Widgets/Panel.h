#pragma once

#include "Panel/PanelBase.h"

namespace gge { namespace widgets {

	class Panel : public panel::Base {
	public:

		Panel() : INIT_PROPERTY(Panel, Padding)
		{
			setallowmove(false);
			setupvscroll(true, true, true);

			if(WR.Panel)
				setblueprint(*WR.Panel);
		}

		void FillContainer() {
			if(Container) {
				Move(0,0);
				Resize(Container->GetUsableWidth()-1, Container->GetUsableHeight()-1);
			}
		}

		utils::Property<Panel, utils::Margins> Padding;

	protected:
		void setPadding(const utils::Margins &value) {
			Base::setpadding(value);
		}
		utils::Margins getPadding() const {
			return Base::getpadding();
		}
		void wr_loaded() {
			if(!blueprintmodified && WR.Panel)
				setblueprint(*WR.Panel);
		}
	};

}}
