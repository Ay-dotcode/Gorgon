#pragma once

#include "WidgetContainer.h"

namespace Gorgon { namespace UI {
   
    /**
     * This class turns a layer into a widget container. This class does not
     * receive keyboard events automatically. Default focus strategy of
     * LayerAdapter is Deny.
     */
    class LayerAdapter : public WidgetContainer {
    public:
        /// Constructor requires the base layer
        LayerAdapter(Layer &base) : base(base) { 
            SetFocusStrategy(Deny);
        }
        
        virtual bool EnsureVisible(const Gorgon::UI::WidgetBase &widget) override {
            return false;
        }
        
        virtual Geometry::Size GetInteriorSize() const override {
            return base.GetSize();
        }
        
        virtual bool ResizeInterior(Geometry::Size size) override {
            base.Resize(size);
            
            return true;
        }
        
        virtual bool IsVisible() const override {
            return base.IsVisible();
        }
        
    protected:
        virtual Gorgon::Layer &getlayer() override {
            return base;
        }
        
        Layer &base;
    };
    
} }
