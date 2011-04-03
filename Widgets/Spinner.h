#pragma once

#include "SliderBase.h"
#include "SliderBP.h"
#include "Textbox.h"

namespace gge {
	extern GGEMain Main;
}

namespace gge { namespace widgets {
	class Spinner : public SliderBase {
	public:
		Spinner(gge::widgets::SliderBP *BP, gge::widgets::IWidgetContainer &container);
		Spinner(gge::widgets::SliderBP *BP);

		void setValue(float value);
		float getValue() { return value; }

		void SetRange(float Min, float Max, float steps=-1) { setMinimum(Min); setMaximum(Max); if(steps>0) setSteps(steps); }
		void setMinimum(float Min) { minimum=Min; setSpeed(vps); }
		void setMaximum(float Max) { maximum=Max; setSpeed(vps); }
		void setDecimalPoints(int points) { char tmp[30]; sprintf_s<30>(tmp, "%%.%if", points); numberformat=tmp; setValue(value); }

		void setSteps(float steps) { buttonincrement=steps; this->steps=steps; setValue(this->value); setSpeed(vps); }

		void AttachTo(Textbox &textbox) { AttachTo(&textbox); }
		void AttachTo(Textbox *textbox);
		void Reposition();
		void DetachFrom();
		bool isAttached() { return attachedto!=NULL; }
		Textbox &getHost() { return *attachedto; }

		void setSpeed(float valuepersecond) { ButtonMoveTime=(int)((maximum-minimum)/valuepersecond); vps=valuepersecond; }

		virtual ~Spinner() { Detach(); SliderBase::~SliderBase(); }
		virtual void goUp() { if(buttonincrement<vps/Main.FPS) buttonincrement=vps/Main.FPS; SliderBase::goUp(); }
		virtual void stopUp() { buttonincrement=steps; SliderBase::stopUp(); }
		virtual void goDown() { if(buttonincrement<vps/Main.FPS) buttonincrement=vps/Main.FPS; SliderBase::goDown(); }
		virtual void stopDown() { buttonincrement=steps; SliderBase::stopDown(); }

				
		Spinner &operator =(float value) {
			setValue(value);

			return *this;
		}

		operator float() {
			return getValue();
		}

	protected:
		utils::EventChain<Spinner, slider_change_event_params> ChangeEvent;

		void attached_changed(Textbox &textbox);
		void attached_lostfocus(IWidgetObject &textbox);
		void attached_keyboard(keyboard_event_params p, Textbox &textbox);

		Textbox *attachedto;
		utils::EventChain<Textbox,			text_change_event_params>	::Token eventtoken;
		utils::EventChain<IWidgetObject>								::Token lostfocuseventtoken;
		utils::EventChain<Textbox,			keyboard_event_params>		::Token keyboardeventtoken;
		float vps;

		void init();

		void changeValue(float value) { SliderBase::setValue(value); if(this->value!=value) setValue(this->value); }
	};
} }
