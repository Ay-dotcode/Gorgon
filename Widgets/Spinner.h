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
			attachedto(NULL), margin(0), Base(value)
		{
			Base::setorientation(slider::Blueprint::Vertical);
			Base::setupdisplay(false, false, false, true, false, true);
			Base::setsmoothingmode(false, false, true, false, 100);
			Base::setmarkers(false, false, false, 10, 5);
			Base::setactions(false, Base::Goto, true);
			Base::setactive();

			ISlider::ChangeEvent.DoubleLink(ChangeEvent);

			if(WR.Sliders.NumberSpinner)
				setblueprint(*WR.Sliders.NumberSpinner);
		}

		virtual WidgetBase *GetWidget() {
			return this;
		}

		template <class O_>
		Spinner &operator =(const O_ &value) { 
			(Object.*setter)(value);

			return *this;
		}

		utils::NumericProperty<Spinner, int> AnimationDuration;
		utils::NumericProperty<Spinner, T_> Stepsize;

		utils::EventChain<Spinner> ChangeEvent;

		void Reposition() {
			if(!attachedto) return;

			if(attachedto->GetWidget()->GetContainer() != GetContainer())
				SetContainer(attachedto->GetWidget()->GetContainer());
				

			utils::Bounds target=attachedto->GetWidget()->GetBounds();

			WidgetBase::Move(utils::Point(target.Right+margin,target.Top));
			SetHeight(target.Height());
		}

		void AttachTo(INumberbox<T_> &numberbox) {
			if(!numberbox.GetWidget()) return;

			DetachFrom();

			attachedto=&numberbox;

			numberbox.Value=getvalue();
			numberbox.GetWidget()->BoundsChanged.Register(this, &Spinner::Reposition);
			numberbox.ChangeEvent.Register(this, &Spinner::updatevalue);
			numberbox.GetWidget()->LostFocus.Register(this, &Spinner::validate);
			numberbox.KeyEvent.Register(this, &Spinner::attached_keyboard);

			Reposition();
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
			Reposition();
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

			if(Value!=attachedto->Value) {
				Base::instantsetvalue(attachedto->Value);
			}
		}

		void validate() {
			if(!attachedto) return;
			if(Base::getvalue()!=attachedto->Value)
				attachedto->Value=Base::getvalue();
		}

		bool attached_keyboard(input::keyboard::Event event) {
			if(event.keycode==input::keyboard::KeyCodes::Left || event.keycode==input::keyboard::KeyCodes::Right)
				return false;

			return Base::KeyboardHandler(event.event, event.keycode);
		}


		//REQUIRED
		virtual T_ getValue() const {
			return Base::getvalue();
		}
		void setValue(const T_ &value) {
			Base::setvalue(value);
		}
		T_ getMin() const {
			return Base::getmin();
		}
		void setMin(const T_ &value) {
			Base::setmin(value);
		}
		T_ getMax() const {
			return Base::getmax();
		}
		void setMax(const T_ &value) {
			Base::setmax(value);
		}


		int getAnimationDuration() const {
			return int(100000/Base::getsmoothingspeed());
		}
		void setAnimationDuration(const int &value) {
			Base::setsmoothingspeed(100000.f/value);
		}

		void setStepsize(const T_ &value) {
			Base::setsteps(value);
			Base::setsmallchange(std::max(value,1));
		}
		T_ getStepsize() const {
			return Base::getsteps();
		}

		virtual void wr_loaded() {
			if(WR.Sliders.NumberSpinner && !blueprintmodified)
				setblueprint(*WR.Sliders.NumberSpinner);
		}


		virtual void value_changed() {
			if(attachedto && Value!=attachedto->Value)
				attachedto->Value=getvalue();

			if(attachedto)
				attachedto->GetWidget()->Focus();

			ChangeEvent();
		}

	};

}}
