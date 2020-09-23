#pragma once

#include "Common.h"
#include "../UI/ComponentStackWidget.h"
#include "../UI/Helpers.h"
#include "../Property.h"
#include "Registry.h"

namespace Gorgon { namespace Widgets {
    
    /**
     * This is an internal basis for slider. It is not very useful by its
     * own as many of its functions are protected. It is used as a base
     * for Slider, Progressbar (not yet), and Scrollbar widgets.
     */
    template<
        class T_ = int, 
        float(*DIV_)(T_, T_, T_) = FloatDivider<T_>, 
        T_(*VAL_)(float, T_, T_) = FloatToValue<T_>, 
        template<class C_, class PT_, PT_(C_::*Getter_)() const, void(C_::*Setter_)(const PT_ &)> class P_ = Gorgon::NumericProperty,
        bool interactive = false
    >
    class SliderBase: 
        public UI::ComponentStackWidget,
        public P_<
            UI::internal::prophelper<SliderBase<T_, DIV_, VAL_, P_, interactive>, T_>, 
            T_, 
            &UI::internal::prophelper<SliderBase<T_, DIV_, VAL_, P_, interactive>, T_>::get_, 
            &UI::internal::prophelper<SliderBase<T_, DIV_, VAL_, P_, interactive>, T_>::set_
        >
    {
    public:
        using Type     = T_;
        using PropType = P_<
            UI::internal::prophelper<SliderBase<T_, DIV_, VAL_, P_, interactive>, T_>, 
            T_, 
            &UI::internal::prophelper<SliderBase<T_, DIV_, VAL_, P_, interactive>, T_>::get_, 
            &UI::internal::prophelper<SliderBase<T_, DIV_, VAL_, P_, interactive>, T_>::set_
        >;
        
        friend class P_<
            UI::internal::prophelper<SliderBase<T_, DIV_, VAL_, P_, interactive>, T_>, 
            T_, 
            &UI::internal::prophelper<SliderBase<T_, DIV_, VAL_, P_, interactive>, T_>::get_, 
            &UI::internal::prophelper<SliderBase<T_, DIV_, VAL_, P_, interactive>, T_>::set_
        >;
        
        template<
            class T1_, 
            float(*DIV1_)(T1_, T1_, T1_), 
            T1_(*VAL1_)(float, T1_, T1_),
            template<
                class C_, 
                class PT_, 
                PT_(C_::*Getter_)() const, 
                void(C_::*Setter_)(const PT_&)
            > class P1_,
            bool I2_
        >
        friend class SliderBase;
        
        friend struct UI::internal::prophelper<SliderBase<T_, DIV_, VAL_, P_, interactive>, T_>;

        SliderBase(const SliderBase &) = delete;
        
        explicit SliderBase(T_ cur, T_ max, Registry::TemplateType type = Registry::Progress_Regular) : 
            SliderBase(Registry::Active()[type], cur, max) 
        {
        }
        
        explicit SliderBase(T_ cur = T_{}, Registry::TemplateType type = Registry::Progress_Regular) : 
            SliderBase(Registry::Active()[type], cur) 
        {
        }

        explicit SliderBase(const UI::Template &temp, T_ cur = T_{}) : SliderBase(temp, cur, T_{100}) { }

        SliderBase(const UI::Template &temp, T_ cur, T_ max) : 
            ComponentStackWidget(temp),
            PropType(&helper), 
            Maximum(this),
            Minimum(this),
            Range(this),
            value(cur), max(max)
        {
            refreshvalue();
            stack.SetValueTransitionSpeed({changespeed, 0, 0, 0});
            
            if(interactive)
                stack.HandleMouse();
        }
        
    protected: //these methods are here to be elevated to public if necessary.
        
        /// Sets the maximum value that this slider reaches up to. If equal to minimum,
        /// progress will display 0. SliderBase will always keep the value between minimum
        /// and maximum. If maximum is less than minimum, this function will automatically
        /// exchange these values.
        void SetMaximum(const T_ &value) {
            if(value < min) {
                max = min;
                min = value;
            }
            else {
                max = value;
            }
            
            if(!setval(this->value)) //if returns true, refresh is already called
                refreshvalue();
        }
        
        /// Returns the current maximum value.
        T_ GetMaximum() const {
            return max;
        }
        
        /// Sets the minimum value that this slider reaches up to. If equal to maximum,
        /// progress will display 0. SliderBase will always keep the value between minimum
        /// and maximum. If maximum is less than minimum, this function will automatically
        /// exchange these values.
        void SetMinimum(const T_ &value) {
            if(value > max) {
                min = max;
                max = value;
            }
            else {
                min = value;
            }
            
            if(!setval(this->value)) //if returns true, refresh is already called
                refreshvalue();
        }
        
        /// Sets minimum and maximum limits. If minimum is equal to maximum,
        /// progress will display 0. SliderBase will always keep the value between minimum
        /// and maximum. If maximum is less than minimum, this function will automatically
        /// exchange these values if exchange is set. If exchange is not set, they will both
        /// be set to T_{}, effectively locking progress at 0.
        void SetLimits(T_ min, T_ max, bool exchange = true) {
            if(min > max) {
                if(exchange) {
                    using std::swap;
                    
                    swap(min, max);
                }
                else {
                    min = T_{};
                    max = T_{};
                }
            }
            
            this->min = min;
            this->max = max;
            
            if(setval(this->value)) //if returns true, refresh is already called
                refreshvalue();
        }
        
        /// Returns the current minimum value. 
        T_ GetMinimum() const {
            return min;
        }
        
        /// Sets the current value of the slider
        SliderBase &operator =(T_ value) {
            set(value);
            
            return *this;
        }
        
        /// Sets the range the container can display. This is used to show how 
        /// much more the scroller can be scrolled.
        void SetRange(const T_ &value) {
            range = value;
            
            if(range < this->min)
                range = this->min;
            
            if(range > this->max)
                range = this->max;
            
            this->refreshvalue();
        }
        
        /// Returns the range the container can display. This is used to show 
        /// how much more the scroller can be scrolled.
        T_ GetRange() const {
            return range;
        }
        
        /// Returns the current value of the slider
        operator T_() const {
            return get();
        }

        virtual bool Activate() override {
            return Focus();
        }
        
        /// Disables smooth change
        void DisableSmoothChange() {
            SetSmoothChangeSpeed(0);
        }
        
        /// Adjusts the smooth change speed. Given value is in values per second, 
        /// default value is max and will be sync to the maximum value.
        void SetSmoothChangeSpeed(T_ value) {
            SetSmoothChangeSpeedRatio(DIV_(value, min, max));
        }
        
        /// Adjusts the smooth change speed. Given value is in values per second, 
        /// default value is 1 and will be sync to the maximum value.
        void SetSmoothChangeSpeedRatio(float value) {
            changespeed = value;
            stack.SetValueTransitionSpeed({value, 0, 0, 0});
        }
        
        /// Returns the smooth change speed. If smooth change is disabled, this 
        /// value will be 0.
        T_ GetSmoothChangeSpeed() const {
            return changespeed * (max - min);
        }
        
        /// Returns the smooth change in ratio to the maximum. 1 means full progress
        /// will be done in 1 second.
        float GetSmoothChangeSpeedRatio() const {
            return changespeed;
        }
        
        /// Returns if the smooth change is enabled.
        bool IsSmoothChangeEnabled() const {
            return changespeed != 0;
        }
        
        /// This property controls the maximum value that the slider can have
        NumericProperty<SliderBase, T_, &SliderBase::GetMaximum, &SliderBase::SetMaximum> Maximum;
        
        /// This property controls the minimum value that the slider can have
        NumericProperty<SliderBase, T_, &SliderBase::GetMinimum, &SliderBase::SetMinimum> Minimum;
        
        /// This is used to show how much more the scroller can be scrolled.
        NumericProperty<SliderBase, T_, &SliderBase::GetRange, &SliderBase::SetRange> Range;
        
        
    protected:
        virtual bool allowfocus() const override { return false; }
        
        /// Returns the value in the box
        T_ get() const {
            return value;
        }
        
        /// Changes the value in the box
        void set(const T_ &val) {
            setval(val);
        }
        
        bool setval(T_ val) {
            if(val > max)
                val = max;
            
            if(val < min)
                val = min;
            
            if(value != val) {
                value = val;
                
                valuechanged(value);
            
                refreshvalue();
                
                return true;
            }
            
            return false;
        }
        
        virtual void refreshvalue() {
            float val = DIV_(this->value, min, max);
            float val2 = DIV_(this->value+this->range, this->min, this->max);
            float r = DIV_(this->range, this->min, this->max);
            
            stack.SetValue(val, val2, r);
            
            val = std::round(val*1000);
            
            if(val == 0) {
                stack.AddCondition(UI::ComponentCondition::Ch1V0);
            }
            else {
                stack.RemoveCondition(UI::ComponentCondition::Ch1V0);
            }
            if(val == 500) {
                stack.AddCondition(UI::ComponentCondition::Ch1V05);
            }
            else {
                stack.RemoveCondition(UI::ComponentCondition::Ch1V05);
            }
            if(val == 1000) {
                stack.AddCondition(UI::ComponentCondition::Ch1V1);
            }
            else {
                stack.RemoveCondition(UI::ComponentCondition::Ch1V1);
            }
            
            val2 = std::round(val2*1000);
            
            if(val2 == 0) {
                this->stack.AddCondition(UI::ComponentCondition::Ch2V0);
            }
            else {
                this->stack.RemoveCondition(UI::ComponentCondition::Ch2V0);
            }
            if(val2 == 500) {
                this->stack.AddCondition(UI::ComponentCondition::Ch2V05);
            }
            else {
                this->stack.RemoveCondition(UI::ComponentCondition::Ch2V05);
            }
            if(val2 == 1000) {
                this->stack.AddCondition(UI::ComponentCondition::Ch2V1);
            }
            else {
                this->stack.RemoveCondition(UI::ComponentCondition::Ch2V1);
            }
            
            r = std::round(r*1000);
            
            if(r == 0) {
                this->stack.AddCondition(UI::ComponentCondition::Ch3V0);
            }
            else {
                this->stack.RemoveCondition(UI::ComponentCondition::Ch3V0);
            }
            if(r == 500) {
                this->stack.AddCondition(UI::ComponentCondition::Ch3V05);
            }
            else {
                this->stack.RemoveCondition(UI::ComponentCondition::Ch3V05);
            }
            if(r == 1000) {
                this->stack.AddCondition(UI::ComponentCondition::Ch3V1);
            }
            else {
                this->stack.RemoveCondition(UI::ComponentCondition::Ch3V1);
            }
        }
        
        T_ value = T_{};
        T_ min = T_{};
        T_ max = T_{};
        
        //this is range that is covered by the scrollbar
        T_ range = T_{};
        
        float changespeed = 1;
        
        virtual void valuechanged(T_) = 0;
        
    private:
        struct UI::internal::prophelper<SliderBase<T_, DIV_, VAL_, P_, interactive>, T_> helper = UI::internal::prophelper<SliderBase<T_, DIV_, VAL_, P_, interactive>, T_>(this);

    };
    
    
} }
