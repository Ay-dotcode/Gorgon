#pragma once

#pragma once

#include "../UI/ComponentStackWidget.h"
#include "../UI/RadioControl.h"
#include "../Property.h"
#include "Checkbox.h"
#include "../UI/WidgetContainer.h"
#include "Registry.h"

namespace Gorgon { namespace Widgets {
    
    /**
     * Allows the use of radio buttons working together. This widget acts as a container for checkboxes
     * that work together. Manually adding/removing widgets will fail. Attaching an organizer to this
     * widget might cause unexpected behavior. All other container functionality should work as intended.
     */
    template<class T_, class W_ = Checkbox>
    class RadioButtons : public UI::WidgetBase, protected UI::RadioControl<T_, W_>, public UI::WidgetContainer {
        friend class UI::WidgetContainer;
    public:
        explicit RadioButtons(const UI::Template &temp) : temp(temp) { this->own = true; }
        
        explicit RadioButtons(Registry::TemplateType type = Registry::Radio_Regular) : temp(Registry::Active()[type]) {  this->own = true; }

        
        /// Radio buttons height is automatically adjusted. Only width will be used.
        virtual void Resize(const Geometry::Size &size) override {
            for(auto p : this->elements) {
                p.second.SetWidth(size.Width);
            }
            
            contents.SetWidth(size.Width);
        }


        virtual bool Activate() override {
            return false;
        }

        void SetSpacing(int value) {
            if(spacing == value)
                return;

            spacing = value;
            
            int total = spacing * (this->elements.GetSize() - 1);
            
            for(auto p : this->elements) {
                total += p.second.GetHeight();
            }
            
            if(total < 0) total = 0;
            
            SetHeight(total);
            
            this->PlaceIn((UI::WidgetContainer&)*this, {0, 0}, spacing);
        }

        void Add(const T_ value) {
            Add(value, String::From(value));
        }

        void Add(const T_ value, std::string text) {
            if(Exists(value)) {
                this->ForceRemove(this->elements[value]);
                this->elements.Delete(value);
            }
            
            auto &c = *new W_(temp, text);
            UI::RadioControl<T_, W_>::Add(value, c);
            
            if(value == this->Get())
                c.Check();
            
            if(GetWidth() < c.GetWidth())
                SetWidth(c.GetWidth());
            
            contents.SetHeight(GetHeight() + c.GetHeight() + spacing);
            
            if(IsVisible())
                this->PlaceIn((UI::WidgetContainer&)*this, {0, 0}, spacing);
            
            boundschanged();
            childboundschanged(&c);
        }
        
        /// Changes the value of the given element
        void ChangeValue(const T_ &before, const T_ &after) {
            if(before == after)
                return;
           
            if(!Exists(before))
                throw std::runtime_error("Element does not exist");

            auto &elm = this->elements[before];
            
            if(Exists(after)) {
                this->ForceRemove(this->elements[after]);
                this->elements.Delete(after);
                
                if(IsVisible())
                    this->PlaceIn((UI::WidgetContainer&)*this, {0, 0}, spacing);
                
                contents.SetHeight(this->widgets.Last()->GetBounds().Bottom + 1);
                
                boundschanged();
                childboundschanged(&elm);
            }

            if(before == this->Get())
                elm.Clear();

            this->elements.Remove(before);
            this->elements.Add(after, elm);
            this->reverse.erase(&elm);
            this->reverse.insert({&elm, after});
            
            if(after == this->Get())
                elm.Check();
        }

        using WidgetBase::Enable;

        /// Enables the given element
        void Enable(const T_ &value) {
            SetEnabled(value, true);
        }

        using WidgetBase::Disable;


        /// Disables the given element
        void Disable(const T_ &value) {
            SetEnabled(value, false);
        }

        using WidgetBase::ToggleEnabled;

        /// Toggles enabled state of the given element
        void ToggleEnabled(const T_ &value) {
            SetEnabled(value, !IsEnabled(value));
        }


        /// Sets the enabled state the given element
        void SetEnabled(const T_ &value, bool state) {
            for(auto p : this->elements) {
                if(p.first == value)
                    p.second.SetEnabled(state);
            }
        }
        
        /// Returns if given element is enabled. Returns false if the element
        /// is not found.
        bool IsEnabled(const T_ &value) const {
            for(auto p : this->elements)
                if(p.first == value)
                    return p.second.IsEnabled();

            return false;
        }


        Geometry::Size GetInteriorSize() const override {
            return GetSize();
        }
        
        bool ResizeInterior(Geometry::Size size) override {
            Resize(size);
            
            return size == GetSize();
        }
        
        Geometry::Point GetLocation() const override {
            return contents.GetLocation();
        }
        
        Geometry::Size GetSize() const override {
            return contents.GetSize();
        }
        
        bool IsVisible() const override {
            return contents.IsVisible();
        }
        
        bool EnsureVisible(const UI::WidgetBase &) override {
            return EnsureVisible();
        }

        using WidgetBase::Resize;

        using WidgetBase::Move;
        
        using WidgetBase::EnsureVisible;
        
        void Move(const Geometry::Point &location) override {
            contents.Move(location);
        }
        
        /// Assigns a new value to the radio control. If the specified value exists
        /// in the, it will be selected, if not, nothing will be selected.
        RadioButtons &operator =(const T_ value) {
            Set(value);
            
            return *this;
        }

        using UI::RadioControl<T_, W_>::ChangedEvent;

        using UI::RadioControl<T_, W_>::Exists;

        using UI::RadioControl<T_, W_>::Get;

        using UI::RadioControl<T_, W_>::Set;
        
        using WidgetBase::IsVisible;

        virtual void SetEnabled(bool value) override {
            if(enabled == value)
                return;

            enabled = value;

            if(!value) {
                ForceRemoveFocus();

                if(HasParent() && IsFocused()) {
                    GetParent().FocusNext();
                    if(IsFocused())
                        GetParent().ForceRemoveFocus();
                }
            }

            distributeparentenabled(value);
        }

        virtual bool IsEnabled() const override {
            return enabled;
        }
        
        bool KeyEvent(Input::Key key, float state) override {
            return UI::WidgetContainer::KeyEvent(key, state);
        }

    protected:
        virtual void addto(Layer &layer) override { 
            layer.Add(contents);
        }


        virtual void removefrom(Layer &layer) override { 
            layer.Remove(contents);
        }


        virtual void setlayerorder(Layer &, int order) override {
            contents.PlaceBefore(order);
        }


        virtual bool allowfocus() const override {
            return true;
        }
        
        virtual void focused() override {        
            if(!HasFocusedWidget())
                FocusFirst();
            
            WidgetBase::focused();
        }
        
        Gorgon::Layer &getlayer() override {
            return contents;
        }

        virtual void parentenabledchanged(bool state) override {
            if(!state && IsEnabled())
                distributeparentenabled(state);
            else if(state && IsEnabled())
                distributeparentenabled(state);
        }
        
        virtual bool addingwidget(WidgetBase &widget) override { 
            for(auto p : this->elements) {
                if(&p.second == &widget)
                    return true;
            }
            
            return false;
        }
        
        virtual bool removingwidget(WidgetBase &) override { return false; }

        Geometry::Point location = {0, 0};
        int spacing = 4;
        const UI::Template &temp;
        bool enabled = true;
        
    private:
        virtual void show() override {
            contents.Show();
        }
        
        virtual void hide() override {
            contents.Hide();
        }

        virtual void focuschanged() override {
            if(HasFocusedWidget() && !IsFocused())
                Focus();
            else if(!HasFocusedWidget() && IsFocused() && HasParent())
                GetParent().RemoveFocus();
        }

        virtual void focuslost() override {
            WidgetBase::focuslost();

            if(HasFocusedWidget()) {
                ForceRemoveFocus();
            }
        }
        
        Gorgon::Graphics::Layer contents;
    };
    
} }
