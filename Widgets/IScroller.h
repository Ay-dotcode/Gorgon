#pragma once

#include "IWidgetObject.h"


namespace gorgonwidgets {
	class IScroller {
	public:
		IScroller() : ChangeEvent("Change", this) { }

		virtual void setMaximum(int Max)=0;
		virtual void setValue(int value)=0;
		virtual int getValue()=0;
		virtual void setLargeChange(int val)=0;

		virtual IWidgetObject *GetWidgetObject()=0;
		virtual void ScrollUp(int Amount=1)=0;
		virtual void ScrollDown(int Amount=1)=0;

		utils::EventChain<IScroller> ChangeEvent;
	};
}