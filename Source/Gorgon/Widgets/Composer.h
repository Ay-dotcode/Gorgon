#pragma once

#include "../UI/Widget.h"
#include "../UI/WidgetContainer.h"

namespace Gorgon { namespace Widgets {

    /**
     * This object allows its users to compose widgets using other widgets. It is
     * designed to be inherited. Derived classes can be use this object as a
     * container, which allows widgets on top of it. However, container side is
     * not public. Therefore, users of the derived widget cannot add more widgets
     * to the derived widget. Widget Composer handles focus, keyboard and all
     * necessary functions to build a widget container and a widget. It does not
     * require any templates to build, however, you may request templates to use
     * in widgets in the composer. This object does not support scrolling, however,
     * you may place a panel in it to have that functionality.
     */
    class Composer : public UI::Widget, protected UI::WidgetContainer {
    public:
        ~Composer() { }
        
        using Widget::Resize;
       
        using Widget::Move;

        using Widget::Remove;
        
        using WidgetContainer::Remove;

        
        virtual bool Activate() override;

        virtual bool IsVisible() const override {
            return base.IsVisible();
        }

        virtual Geometry::Size GetInteriorSize() const override {
            return base.GetSize();
        }
        
        virtual Geometry::Size GetSize() const override {
            return base.GetSize();
        }

        virtual bool ResizeInterior(Geometry::Size size) override {
            Resize(size);
            
            return true;
        }
        
        virtual void Resize(const Geometry::Size &size) override;

        virtual Geometry::Point GetLocation() const override {
            return base.GetLocation();
        }
        
        virtual void Move(const Geometry::Point &location) override;

        using Widget::EnsureVisible;
        
        bool EnsureVisible(const UI::Widget &widget) override {
            return true;
        }

        using Widget::Enable;
        using Widget::Disable;
        using Widget::ToggleEnabled;

        virtual void SetEnabled(bool value) override {
            if(value != IsEnabled()) {
                enabled = value;
                distributeparentenabled(value);
            }
        }

        virtual bool IsEnabled() const override {
            return enabled;
        }
        
    protected:
        //ensure this object is derived
        Composer() {
        }
        
        virtual bool allowfocus() const override;
        
        virtual void focused() override;

        virtual void focuslost() override;

        virtual Layer &getlayer() override {
            return base;
        }
        
        void focuschanged() override;
        
        virtual void addto(Layer &layer) override  {
            layer.Add(base);
        }
        
        virtual void removefrom(Layer &layer) override {
            layer.Remove(base);
        }
        
        virtual void setlayerorder(Layer &, int order) override {
            base.PlaceBefore(order);
        }
        
    private:
        bool enabled = true;
        
        virtual void hide() override;

        virtual void show() override;
        
        Layer base;
    };
    
} }
