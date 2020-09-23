#pragma once

#include "Common.h"
#include "../UI/ComponentStackWidget.h"
#include "../UI/Helpers.h"
#include "../Property.h"
#include "Registry.h"

namespace Gorgon { namespace Widgets {
    

    /**
     * This widget displays a progress bar to track progress of an on going 
     * process. For regular integer based progressor use Progressbar. This
     * is the base widget that can be used with any type, including types
     * that are not inherintly numeric. The completion is calculated using
     * DIV_ function which takes current, minimum and maximum values and
     * should return the progress between 0 and 1. Property type should be
     * adjusted if numeric operators does not exist for the given type.
     * 
     * If the type is numeric, using NumericProperty will allow you to use
     * the widget as a regular numeric variable with operators like =, +=,
     * ++, --. 
     */
    template<
        class T_ = int, 
        float(*DIV_)(T_, T_, T_) = FloatDivider<T_>, 
        template<class C_, class PT_, PT_(C_::*Getter_)() const, void(C_::*Setter_)(const PT_ &)> class P_ = Gorgon::NumericProperty
    >
    class Progressor : 
        public UI::ComponentStackWidget,
        public P_<
            UI::internal::prophelper<Progressor<T_, DIV_, P_>, T_>, 
            T_, 
            &UI::internal::prophelper<Progressor<T_, DIV_, P_>, T_>::get_, 
            &UI::internal::prophelper<Progressor<T_, DIV_, P_>, T_>::set_
        >
    {
    public:
        using Type     = T_;
        using PropType = P_<
            UI::internal::prophelper<Progressor<T_, DIV_, P_>, T_>, 
            T_, 
            &UI::internal::prophelper<Progressor<T_, DIV_, P_>, T_>::get_, 
            &UI::internal::prophelper<Progressor<T_, DIV_, P_>, T_>::set_
        >;
        
        friend class P_<
            UI::internal::prophelper<Progressor<T_, DIV_, P_>, T_>, 
            T_, 
            &UI::internal::prophelper<Progressor<T_, DIV_, P_>, T_>::get_, 
            &UI::internal::prophelper<Progressor<T_, DIV_, P_>, T_>::set_
        >;
        
        template<
            class T1_, 
            float(*DIV1_)(T1_, T1_, T1_), 
            template<
                class C_, 
                class PT_, 
                PT_(C_::*Getter_)() const, 
                void(C_::*Setter_)(const PT_&)
            > class P1_
        >
        friend class Progressor;
        
        friend struct UI::internal::prophelper<Progressor<T_, DIV_, P_>, T_>;

        Progressor(const Progressor &) = delete;
        
        explicit Progressor(T_ cur, T_ max, Registry::TemplateType type = Registry::Progress_Regular) : 
            Progressor(Registry::Active()[type], cur, max) 
        {
        }
        
        explicit Progressor(T_ cur = T_{}, Registry::TemplateType type = Registry::Progress_Regular) : 
            Progressor(Registry::Active()[type], cur) 
        {
        }

        explicit Progressor(const UI::Template &temp, T_ cur = T_{}) : Progressor(temp, cur, T_{100}) { }

        Progressor(const UI::Template &temp, T_ cur, T_ max) : 
            ComponentStackWidget(temp),
            PropType(&helper), 
            Maximum(this),
            Minimum(this),
            value(cur), max(max)
        { 
            stack.SetValueTransitionSpeed({progressspeed, 0, 0, 0});
            refreshprogress();
        }
        
        /// Sets the maximum value that this progressor reaches up to. If equal to minimum,
        /// progress will display 0. Progressor will always keep the value between minimum
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
                refreshprogress();
        }
        
        /// Returns the current maximum value.
        T_ GetMaximum() const {
            return max;
        }
        
        /// Sets the minimum value that this progressor reaches up to. If equal to maximum,
        /// progress will display 0. Progressor will always keep the value between minimum
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
                refreshprogress();
        }
        
        /// Sets minimum and maximum limits. If minimum is equal to maximum,
        /// progress will display 0. Progressor will always keep the value between minimum
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
                refreshprogress();
        }
        
        /// Returns the current minimum value. 
        T_ GetMinimum() const {
            return min;
        }
        
        /// Sets the current value of the progressor
        Progressor &operator =(T_ value) {
            set(value);
            
            return *this;
        }
        
        /// Returns the current value of the progressor
        operator T_() const {
            return get();
        }

        virtual bool Activate() override {
            return false;
        }
        
        /// Disables smooth progress
        void DisableSmoothProgress() {
            SetSmoothProgressSpeed(0);
        }
        
        /// Adjusts the smooth progress speed. Given value is in values per second, 
        /// default value is max and will be sync to the maximum value.
        void SetSmoothProgressSpeed(T_ value) {
            SetSmoothProgressSpeedRatio(DIV_(value, min, max));
        }
        
        /// Adjusts the smooth progress speed. Given value is in values per second, 
        /// default value is 1 and will be sync to the maximum value.
        void SetSmoothProgressSpeedRatio(float value) {
            progressspeed = value;
            stack.SetValueTransitionSpeed({value, 0, 0, 0});
        }
        
        /// Returns the smooth progress speed. If smooth progress is disabled, this 
        /// value will be 0.
        T_ GetSmoothProgressSpeed() const {
            return progressspeed * (max - min);
        }
        
        /// Returns the smooth progress in ratio to the maximum. 1 means full progress
        /// will be done in 1 second.
        float GetSmoothProgressSpeedRatio() const {
            return progressspeed;
        }
        
        /// Returns if the smooth progress is enabled.
        bool IsSmoothProgressEnabled() const {
            return progressspeed != 0;
        }
        
        /// This event is fired when the progress is changed
        Event<Progressor, T_> ProgressChanged = Event<Progressor, T_>(this);
        
        NumericProperty<Progressor, T_, &Progressor::GetMaximum, &Progressor::SetMaximum> Maximum;
        NumericProperty<Progressor, T_, &Progressor::GetMinimum, &Progressor::SetMinimum> Minimum;
        
        
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
                
                ProgressChanged(value);
            
                refreshprogress();
                
                return true;
            }
            
            return false;
        }
        
        void refreshprogress() {
            float progress = DIV_(value, min, max);
            
            stack.SetValue(progress);
            
            auto v = std::round(progress*1000);
            
            if(v == 0) {
                stack.AddCondition(UI::ComponentCondition::Ch1V0);
            }
            else {
                stack.RemoveCondition(UI::ComponentCondition::Ch1V0);
            }
            if(v == 500) {
                stack.AddCondition(UI::ComponentCondition::Ch1V05);
            }
            else {
                stack.RemoveCondition(UI::ComponentCondition::Ch1V05);
            }
            if(v == 1000) {
                stack.AddCondition(UI::ComponentCondition::Ch1V1);
            }
            else {
                stack.RemoveCondition(UI::ComponentCondition::Ch1V1);
            }
        }
        
        T_ value = T_{};
        T_ min = T_{};
        T_ max = T_{};
        
        float progressspeed = 1;
        
        
    private:
        struct UI::internal::prophelper<Progressor<T_, DIV_, P_>, T_> helper = UI::internal::prophelper<Progressor<T_, DIV_, P_>, T_>(this);

    };
    
    /**
     * This widget displays the progress. It can be used as an integer variable through
     * NumericProperty<int> with operators like =, +=, ++, --. Also this widget can be
     * implicitly casted to integer. If you need a type other than int, you may use
     * Progressor<T_> for any type, even non-numeric.
     */
    using Progressbar = Progressor<int>;
    
} }
