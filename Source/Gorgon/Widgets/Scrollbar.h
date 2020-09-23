#pragma once

#include "Slider.h"


namespace Gorgon { namespace Widgets {
    
    /**
     * This widget is a scrollbar. It is basically a slider without ticks and 
     * potentially has buttons to adjust value. This widget has no ability to
     * provide scroll capability to another widget automatically. Therefore, it
     * has no additional capability on top of slider.
     * 
     * Scrollbar orientation is controlled by its template. You may use 
     * HScrollbar and VScrollbar to denote the orientation. Additionally, these
     * variants have their type fixed to int.
     */
    template<
        class T_, 
        float(*DIV_)(T_, T_, T_) = FloatDivider<T_>, 
        T_(*VAL_)(float, T_, T_) = FloatToValue<T_>,
        template<class C_, class PT_, PT_(C_::*Getter_)() const, void(C_::*Setter_)(const PT_ &)> class P_ = Gorgon::NumericProperty
    >
    class Scrollbar : public SliderBase<T_, DIV_, VAL_, P_, true> {
        using Base = SliderBase<T_, DIV_, VAL_, P_, true>;
    public:
        
        Scrollbar(const Scrollbar &) = delete;
        
        explicit Scrollbar(T_ cur, T_ max, Registry::TemplateType type = Registry::Scrollbar_Horizontal) : 
            Scrollbar(Registry::Active()[type], cur, max) 
        {
        }
        
        explicit Scrollbar(T_ cur = T_{}, Registry::TemplateType type = Registry::Scrollbar_Horizontal) : 
            Scrollbar(Registry::Active()[type], cur) 
        {
        }

        explicit Scrollbar(const UI::Template &temp, T_ cur = T_{}) : Scrollbar(temp, cur, T_{100}) { }

        Scrollbar(const UI::Template &temp, T_ cur, T_ max) : 
            Base(temp, cur, max)
        {
            this->stack.HandleMouse();
        }
        
        using Base::SetMaximum;
        using Base::GetMaximum;
        
        using Base::SetRange;
        using Base::GetRange;
        
        using Base::operator =;
        using Base::operator T_;
        
        using Base::DisableSmoothChange;
        using Base::SetSmoothChangeSpeed;
        using Base::SetSmoothChangeSpeedRatio;
        using Base::GetSmoothChangeSpeed;
        using Base::GetSmoothChangeSpeedRatio;
        using Base::IsSmoothChangeEnabled;
        
        using Base::Maximum;
        using Base::Range;
        
        /// This event is fired when the scroll is changed
        Event<Scrollbar, T_> ValueChanged = Event<Scrollbar, T_>(this);
        
    protected:
        virtual void valuechanged(T_ value) override {
            ValueChanged(value);
        }
        
        virtual void refreshvalue() override {
            Base::refreshvalue();
            
        }
    };
    
} }
