#pragma once

#include "IScroller.h"
#include "SliderBP.h"
#include "SliderBase.h"
#include "IWidgetContainer.h"

namespace gge { namespace widgets {
	enum ScrollbarAlignments {
		SA_Verticle=SS_Verticle,
		SA_Horizontal=SS_Horizontal
	};

	class Scrollbar : public IScroller, public SliderBase {
	public:
		Scrollbar(SliderBP *BP, gge::widgets::IWidgetContainer &container, ScrollbarAlignments Alignment);
		Scrollbar(SliderBP *BP, ScrollbarAlignments Alignment);

		virtual void setValue(int value) { 
			if(alignment==SS_Verticle) value=(int)(maximum-value); SliderBase::setValue((float)value); 
		}
		virtual int getValue() { 
			if(alignment==SS_Verticle) return (int)(maximum-value); else return (int)value; 
		}
		virtual void setValue(float value);
		virtual void	Resize(int Cx,int Cy) { 
			if(alignment==SS_Verticle) 
				SliderBase::Resize(StyleGroup->PreferredWidth, Cy);
			else
				SliderBase::Resize(Cx, StyleGroup->PreferredHeight);
		}

		void setMaximum(int Max);
		int getMaximum() { return (int)maximum; }
		virtual void setLargeChange(int val) { if(val>0) { largeincrement=(float)val; ruleaction=SRA_LargeIncrement; } else ruleaction=SRA_JumpTo; }
		int getLargeChange() { if(ruleaction==SRA_JumpTo) return -1; return (int)largeincrement; }
		virtual IWidgetObject *GetWidgetObject() { return this; }

		virtual void ScrollUp(int Amount=1) { setValue(value-buttonincrement*Amount); }
		virtual void ScrollDown(int Amount=1) { setValue(value+buttonincrement*Amount); }

	protected:
		ScrollbarAlignments alignment;

		void slider_changed();
		void init();
	};
} }
