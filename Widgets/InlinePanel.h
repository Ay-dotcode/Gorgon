#pragma once

#include "Panel/PanelBase.h"

namespace gge { namespace widgets {

	class InlinePanel : public panel::Base {
	public:

		InlinePanel() : INIT_PROPERTY(InlinePanel, Padding)
		{
			setallowmove(false);
			setupvscroll(true, true, true);

			if(WR.Panel)
				setblueprint(*WR.Panel);

			setdisplay(false);
		}

		void FillContainer() {
			if(Container) {
				Move(0,0);
				Resize(Container->GetUsableWidth()-1, Container->GetUsableHeight()-1);
			}
		}

		utils::Property<InlinePanel, utils::Margins> Padding;

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
