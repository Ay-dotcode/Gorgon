#pragma once

#include "Interfaces/ISlider.h"
#include "Slider/SliderBase.h"
#include "Interfaces/INumberbox.h"

namespace gge { namespace widgets {

	template<class T_=int, class floattype=typename slider::floattype<T_>::Type>
	class Spinner : public ISlider<T_>, public slider::Base<T_> {
	public:

		Spinner(T_ value=T_()) : ChangeEvent("ChangeEvent", this),
			INIT_PROPERTY(Spinner, AnimationDuration),
			INIT_PROPERTY(Spinner, Stepsize),
			attachedto(NULL), margin(0), slider::Base<T_>(value)
		{
			slider::Base<T_>::setorientation(slider::Blueprint::Vertical);
			slider::Base<T_>::setupdisplay(false, false, false, true, false, true);
			slider::Base<T_>::setsmoothingmode(false, false, true, false, 100);
			slider::Base<T_>::setmarkers(false, false, false, 10, 5);
			slider::Base<T_>::setactions(false, slider::Base<T_>::Goto, true);
			slider::Base<T_>::setactive();

			ISlider<T_>::ChangeEvent.DoubleLink(ChangeEvent);

			if(WR.Sliders.NumberSpinner)
				this->setblueprint(*WR.Sliders.NumberSpinner);
		}

		virtual WidgetBase *GetWidget() {
			return this;
		}

		template <class O_>
		Spinner &operator =(const O_ &value) { 
			(this->Object.*this->setter)(value);

			return *this;
		}

		utils::NumericProperty<Spinner, int> AnimationDuration;
		utils::NumericProperty<Spinner, T_> Stepsize;

		utils::EventChain<Spinner> ChangeEvent;

		void Reposition() {
			if(!attachedto) return;

			if(attachedto->GetWidget()->GetContainer() != this->GetContainer())
				this->SetContainer(attachedto->GetWidget()->GetContainer());
				

			utils::Bounds target=attachedto->GetWidget()->GetBounds();

			WidgetBase::Move(utils::Point(target.Right+margin,target.Top));
			this->SetHeight(target.Height());
		}

		void AttachTo(INumberbox<T_> &numberbox) {
			if(!numberbox.GetWidget()) return;

			this->DetachFrom();

			attachedto=&numberbox;

			numberbox.Value=this->getvalue();
			numberbox.GetWidget()->BoundsChanged.Register(this, &Spinner::Reposition);
			numberbox.ChangeEvent.Register(this, &Spinner::updatevalue);
			numberbox.GetWidget()->LostFocus.Register(this, &Spinner::validate);
			numberbox.KeyEvent.Register(this, &Spinner::attached_keyboard);

			this->Reposition();
		}

		void DetachFrom() {
			if(!attachedto) return;

			attachedto->GetWidget()->BoundsChanged.Unregister(this, &Spinner::Reposition);
			attachedto->ChangeEvent.Unregister(this, &Spinner::updatevalue);
			attachedto->GetWidget()->LostFocus.Unregister(this, &Spinner::validate);
			attachedto->KeyEvent.Unregister(this, &Spinner::attached_keyboard);

			attachedto=NULL;
		}

		bool IsAttached() const {
			return attachedto!=NULL;
		}

		INumberbox<T_> &GetHost() {
			if(!attachedto) throw std::runtime_error("Spinner is not attached");
			return *attachedto;
		}

		void SetMargin(int margin) {
			this->margin=margin;
			this->Reposition();
		}

		int GetMargin() const {
			return margin;
		}

		virtual bool Focus() {
			return false;
		}
		
		using WidgetBase::Move;
		virtual void Move(utils::Point target) {
			if(!attachedto)
				WidgetBase::Move(target);
		}

	protected:

		INumberbox<T_> *attachedto;
		int margin;

		void updatevalue() {
			if(!attachedto) return;

			if(this->Value!=attachedto->Value) {
				slider::Base<T_>::instantsetvalue(attachedto->Value);
			}
		}

		void validate() {
			if(!attachedto) return;
			if(slider::Base<T_>::getvalue()!=attachedto->Value)
				attachedto->Value=slider::Base<T_>::getvalue();
		}

		bool attached_keyboard(input::keyboard::Event event) {
			if(event.keycode==input::keyboard::KeyCodes::Left || event.keycode==input::keyboard::KeyCodes::Right)
				return false;

			return slider::Base<T_>::KeyboardHandler(event.event, event.keycode);
		}


		//REQUIRED
		virtual T_ getValue() const {
			return slider::Base<T_>::getvalue();
		}
		void setValue(const T_ &value) {
			slider::Base<T_>::setvalue(value);
		}
		T_ getMin() const {
			return slider::Base<T_>::getmin();
		}
		void setMin(const T_ &value) {
			slider::Base<T_>::setmin(value);
		}
		T_ getMax() const {
			return slider::Base<T_>::getmax();
		}
		void setMax(const T_ &value) {
			slider::Base<T_>::setmax(value);
		}


		int getAnimationDuration() const {
			return int(100000/slider::Base<T_>::getsmoothingspeed());
		}
		void setAnimationDuration(const int &value) {
			slider::Base<T_>::setsmoothingspeed(100000.f/value);
		}

		void setStepsize(const T_ &value) {
			slider::Base<T_>::setsteps(value);
			slider::Base<T_>::setsmallchange(std::max(value,1));
		}
		T_ getStepsize() const {
			return slider::Base<T_>::getsteps();
		}

		virtual void wr_loaded() {
			if(WR.Sliders.NumberSpinner && !this->blueprintmodified)
				this->setblueprint(*WR.Sliders.NumberSpinner);
		}


		virtual void value_changed() {
			if(attachedto && this->Value!=attachedto->Value)
				attachedto->Value=this->getvalue();

			if(attachedto)
				attachedto->GetWidget()->Focus();

			ChangeEvent();
		}

	};

}}
