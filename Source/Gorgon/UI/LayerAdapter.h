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
        
        /// This constructor will leave the LayerAdapter in an invalid state.
        /// You must use SetLayer before calling any other functions.
        LayerAdapter() {
        }
        
        /// Constructor requires the base layer
        LayerAdapter(Layer &base) : base(&base) { 
            SetFocusStrategy(Deny);
        }
        
        virtual bool EnsureVisible(const Gorgon::UI::Widget &widget) override {
            return false;
        }
        
        virtual Geometry::Size GetInteriorSize() const override {
            return base->GetCalculatedSize();
        }
        
        virtual bool ResizeInterior(Geometry::Size size) override {
            base->Resize(size);
            
            return true;
        }
        
        virtual bool IsVisible() const override {
            return base->IsVisible();
        }
        
        /// Returns true if this adapter can be used.
        bool IsReady() const {
            return base != nullptr;
        }
        
        /// Sets the base layer
        void SetLayer(Gorgon::Layer &value) {
            base = &value;
        }
        
        /// Returns the base layer
        Gorgon::Layer &GetLayer() const {
            return *base;
        }
        
        virtual ExtenderRequestResponse RequestExtender(const Gorgon::Layer &self) override;
        
        /// The spacing should be left between widgets
        virtual int GetSpacing() const override;
        
        /// Returns the unit width for a widget. This size is enough to
        /// have a bordered icon. Widgets should be sized according to unit
        /// width and spacing. A single unit width would be too small for
        /// most widgets.
        virtual int GetUnitWidth() const override;
        
        /// Overrides default spacing and unitwidth
        void SetSizes(int spacing, int unitwidth) {
            this->spacing = spacing;
            this->unitwidth = unitwidth;
            issizesset = true;
        }
        
        /// Sets the unit size automatically. Full width will be at least
        /// given units wide. Returns remaining size.
        int AutomaticUnitSize(int spacing, int units = 6) {
            ASSERT(base, "Base layer is not set");
            
            this->spacing   = spacing;
            issizesset      = true;
            this->unitwidth = ( base->GetWidth() - spacing * (units-1) ) / units;
            
            return base->GetWidth() - (this->unitwidth * units + this->spacing * (units-1));
        }
        
        /// Return to use default sizes
        void UseDefaultSizes() {
            issizesset = false;
        }

        void SetFocusChangedHandler(std::function<void()> fn) {
            focushandler = fn;
        }
        
    protected:
        virtual Gorgon::Layer &getlayer() override {
            return *base;
        }

        virtual void focuschanged() override {
            if(focushandler)
                focushandler();
        }

        std::function<void()> focushandler;
        
        Layer *base = nullptr;
        
        int spacing   = 0;
        int unitwidth = 0;
        bool issizesset = false;
    };
    
} }
