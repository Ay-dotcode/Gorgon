#pragma once
#include "Interfaces/IProgressor.h"
#include "Slider/SliderBase.h"

namespace gge { namespace widgets {

	template<class T_=int, class floattype=typename slider::floattype<T_>::Type>
	class Percentbar : public IProgressor<T_>, public slider::Base<float> {
	public:

		enum Alignment {
			Left=TextAlignment::Left,
			Center=TextAlignment::Center,
			Right=TextAlignment::Right
		};

		Percentbar() : mymin(0), mymax(100),
			INIT_PROPERTY(Percentbar, AnimationDuration),
			INIT_PROPERTY(Percentbar, Decimals),
			INIT_PROPERTY(Percentbar, IndicatorAlignment),
			INIT_PROPERTY(Percentbar, ValueAlignment),
			INIT_PROPERTY(Percentbar, ShowTicks),
			INIT_PROPERTY(Percentbar, TickDistance)
		{
			Base::setorientation(slider::Blueprint::Horizontal);
			Base::setupdisplay(false, true, true, false, true);
			Base::setsmoothingmode(false, true, false, true, 100);
			Base::setmarkers(true, false, false);
			setvalueformat(numberformat(0,"%"));
			setpassive();

			if(WR.Progressbars.Percent)
				setblueprint(*WR.Progressbars.Percent);
		}

		virtual WidgetBase *GetWidget() {
			return this;
		}

		template <class O_>
		Percentbar &operator =(const O_ &value) { 
			(this->Object.*this->setter)(value);

			return *this;
		}

		utils::NumericProperty<Percentbar, int> AnimationDuration;
		utils::NumericProperty<Percentbar, int> Decimals;
		utils::Property<Percentbar, Alignment> IndicatorAlignment;
		utils::Property<Percentbar, Alignment> ValueAlignment;
		utils::BooleanProperty<Percentbar> ShowTicks;
		utils::NumericProperty<Percentbar, floattype> TickDistance;

	protected:
		//REQUIRED
		virtual T_ getValue() const {
			if(typeid(T_)!=typeid(floattype))
				return (T_)Round((Base::getvalue()/100)*(mymax-mymin)+mymin);
			else
				return T_((Base::getvalue()/100)*(mymax-mymin)+mymin);
		}
		void setValue(const T_ &value) {
			Base::setvalue(100*float(value-mymin)/(mymax-mymin));
		}
		T_ getMin() const {
			return mymin;
		}
		void setMin(const T_ &value) {
			T_ v=getValue();
			mymin=value;
			setValue(v);
		}
		T_ getMax() const {
			return mymax;
		}
		void setMax(const T_ &value) {
			T_ v=getValue();
			mymax=value;
			setValue(v);
		}

		T_ mymax,mymin;


		int getAnimationDuration() const {
			return int(100000/Base::getsmoothingspeed());
		}
		void setAnimationDuration(const int &value) {
			Base::setsmoothingspeed(100000.f/value);
		}

		void setDecimals(const int &value) {
			numberformat n=getvalueformat();
			n.decimals=value;
			setvalueformat(n);
		}
		int getDecimals() const {
			return getvalueformat().decimals;
		}

		void setIndicatorAlignment(const Alignment &value) {
			if(value==getIndicatorAlignment())
				return;

			if(value==Center) {
				setcenterindicator(true);
				setaxisinverse(false);
			}
			else if(value==Right) {
				setcenterindicator(false);
				setaxisinverse(true);
			}
			else {
				setcenterindicator(false);
				setaxisinverse(false);
			}

		}
		Alignment getIndicatorAlignment() const {
			if(getcenterindicator())
				return Center;
			else if(getaxisinverse())
				return Right;
			else
				return Left;
		}

		void setValueAlignment(const Alignment &value) {
			if(value==Center) {
				setvaluelocation(gge::Alignment::Middle_Center);
			}
			else if(value==Right) {
				setvaluelocation(gge::Alignment::Middle_Right);
			}
			else {
				setvaluelocation(gge::Alignment::Middle_Left);
			}
		}
		Alignment getValueAlignment() const {
			return (Alignment)TextAlignment::GetHorizontal(getvaluelocation());
		}

		void setShowTicks(const bool &value) {
			Base::setshowticks(value);
		}
		bool getShowTicks() const {
			return Base::getshowticks();
		}

		void setTickDistance(const floattype &value) {
			Base::settickdistance(100*float(value-mymin)/(mymax-mymin));
		}
		floattype getTickDistance() const {
			return floattype((Base::gettickdistance()/100)*(mymax-mymin)+mymin);
		}

		virtual void wr_loaded() {
			if(WR.Progressbars.Percent && !blueprintmodified)
				setblueprint(*WR.Progressbars.Percent);
		}

	};

}}
