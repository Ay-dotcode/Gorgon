#pragma once

#include "PanelBase.h"

namespace gge { namespace widgets {

	class ControlledPanel : public panel::Base {
	public:

		ControlledPanel() : INIT_PROPERTY(ControlledPanel, Padding),
			INIT_PROPERTY(ControlledPanel, SmallScroll),
			INIT_PROPERTY(ControlledPanel, LargeScroll),
			INIT_PROPERTY(ControlledPanel, AllowTabSwitch)
		{
			setallowmove(true);
			setupvscroll(true, true, false);
		}

		utils::Property<ControlledPanel, utils::Margins> Padding;
		utils::NumericProperty<ControlledPanel, int> SmallScroll;
		utils::NumericProperty<ControlledPanel, int> LargeScroll;
		utils::BooleanProperty<ControlledPanel> AllowTabSwitch;

	protected:
		void setPadding(const utils::Margins &value) {
			Base::setpadding(value);
		}
		utils::Margins getPadding() const {
			return Base::getpadding();
		}

		void setSmallScroll(const int &value) {
			getvscroller().SmallChange=value;
		}
		int getSmallScroll() const {
			return getvscroller().SmallChange;
		}

		void setLargeScroll(const int &value) {
			getvscroller().LargeChange=value;
		}
		int getLargeScroll() const {
			return getvscroller().LargeChange;
		}

		void setAllowTabSwitch(const bool &value) {
			Base::setallowtabswitch(value);
		}
		bool getAllowTabSwitch() const {
			return Base::getallowtabswitch();
		}
	};

}}
