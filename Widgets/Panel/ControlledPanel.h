#pragma once

#include "PanelBase.h"

namespace gge { namespace widgets {

	class ControlledPanel : public panel::Base {
	public:

		ControlledPanel() : INIT_PROPERTY(ControlledPanel, Padding),
			INIT_PROPERTY(ControlledPanel, SmallScroll),
			INIT_PROPERTY(ControlledPanel, LargeScroll),
			INIT_PROPERTY(ControlledPanel, LogicalHeight),
			INIT_PROPERTY(ControlledPanel, AllowTabSwitch),
			Scrolled(this),
			logicalheight(0)
		{
			setallowmove(false);
			setupvscroll(true, true, true);
			panel::Base::getvscroller().ChangeEvent.LinkTo(Scrolled);
		}

		utils::Property<ControlledPanel, utils::Margins> Padding;
		utils::NumericProperty<ControlledPanel, int> SmallScroll;
		utils::NumericProperty<ControlledPanel, int> LargeScroll;
		utils::NumericProperty<ControlledPanel, int> LogicalHeight;
		utils::BooleanProperty<ControlledPanel> AllowTabSwitch;

		gge::utils::EventChain<ControlledPanel> Scrolled;

		int GetVScroll() const {
			return panel::Base::getvscroll();
		}

		void SetVScroll(int scroll) {
			return panel::Base::vscrollto(scroll);
		}

		Scrollbar<> &GetVScroller() { return getvscroller(); }

		void Freeze() {
			freeze();
		}
		void Unfreeze() {
			unfreeze();
		}

	protected:
		int logicalheight;

		virtual int calculatevscrollback(int min) {
			if(logicalheight==0) {
				return panel::Base::calculatevscrollback(min);
			}
			else {
				return logicalheight;
			}
		}

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

		void setLogicalHeight(const int &value) {
			logicalheight=value;
			adjustscrolls();
		}
		int getLogicalHeight() const {
			return logicalheight;
		}

		void setAllowTabSwitch(const bool &value) {
			Base::setallowtabswitch(value);
		}
		bool getAllowTabSwitch() const {
			return Base::getallowtabswitch();
		}
	};

}}
