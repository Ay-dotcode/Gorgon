#pragma once

#include "../UI/ComponentStackWidget.h"
#include "../UI/Helpers.h"
#include "../Property.h"
#include "Registry.h"

namespace Gorgon { namespace Widgets {
    
    template<class T_>
    float FloatDivider(T_ left, T_ min, T_ max) {
        if(min == max) return 0.f;
        
        return float(left - min) / float(max - min);
    }

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

        explicit Progressor(const UI::Template &temp, T_ cur = T_{}) : Progressor(temp, cur, T_{}) { }

        Progressor(const UI::Template &temp, T_ cur, T_ max) : 
            ComponentStackWidget(temp),
            PropType(&helper), value(cur), max(max)
        { 
            refreshprogress();
        }
        
        /// Sets the maximum value that this progressor reaches up to. If equal to minimum,
        /// progress will display 0. Progressor will always keep the value between minimum
        /// and maximum. If maximum is less than minimum, this function will automatically
        /// exchange these values.
        void SetMaximum(T_ value) {
            if(value < min) {
                max = min;
                min = value;
            }
            else {
                max = value;
            }
            
            if(!setval(value)) //if returns true, refresh is already called
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
        void SetMinimum(T_ value) {
            if(value > max) {
                min = max;
                max = value;
            }
            else {
                min = value;
            }
            
            if(!setval(value)) //if returns true, refresh is already called
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
            
            if(setval(value)) //if returns true, refresh is already called
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
        
        
        Event<Progressor, T_> ProgressChanged = Event<Progressor, T_>(this);
        
        
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
        }
        
        T_ value;
        T_ min = T_{};
        T_ max;
        
        
    private:
        struct UI::internal::prophelper<Progressor<T_, DIV_, P_>, T_> helper = UI::internal::prophelper<Progressor<T_, DIV_, P_>, T_>(this);

    };
    
    using Progressbar = Progressor<int>;
    
} }
