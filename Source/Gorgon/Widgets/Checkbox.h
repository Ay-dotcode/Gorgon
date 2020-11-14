#pragma once

#include "../UI/ComponentStackWidget.h"
#include "../UI/TwoStateControl.h"
#include "../Property.h"
#include "Registry.h"

namespace Gorgon { namespace Widgets {

    class Checkbox : public UI::ComponentStackWidget, public UI::TwoStateControl {
    public:
        Checkbox(const Checkbox &) = delete;
        
        
        explicit Checkbox(Registry::TemplateType type = Registry::Checkbox_Regular) : 
            Checkbox(Registry::Active()[type], "") 
        {
        }

        explicit Checkbox(bool state, Registry::TemplateType type = Registry::Checkbox_Regular) : 
            Checkbox(Registry::Active()[type], state) 
        {
        }

        explicit Checkbox(std::string text, Registry::TemplateType type = Registry::Checkbox_Regular) : 
            Checkbox(Registry::Active()[type], text) 
        {
        }

        explicit Checkbox(const char *text, Registry::TemplateType type = Registry::Checkbox_Regular) : 
            Checkbox(Registry::Active()[type], text)
        {
        }

        template<class F_>
        explicit Checkbox(F_ changed, Registry::TemplateType type = Registry::Checkbox_Regular) : 
            Checkbox(Registry::Active()[type], "", changed)
        {
        }

        template<class F_>
        explicit Checkbox(std::string text, F_ changed, Registry::TemplateType type = Registry::Checkbox_Regular):
            Checkbox(Registry::Active()[type], text, changed) {
        }

        template<class F_>
        explicit Checkbox(const char *text, F_ changed, Registry::TemplateType type = Registry::Checkbox_Regular):
            Checkbox(Registry::Active()[type], text, changed) {
        }

        explicit Checkbox(std::string text, bool state, Registry::TemplateType type = Registry::Checkbox_Regular) : 
            Checkbox(Registry::Active()[type], text, state) 
        {
        }

        explicit Checkbox(const char *text, bool state, Registry::TemplateType type = Registry::Checkbox_Regular) : 
            Checkbox(Registry::Active()[type], text, state)
        {
        }

        template<class F_>
        explicit Checkbox(bool state, F_ changed, Registry::TemplateType type = Registry::Checkbox_Regular) : 
            Checkbox(Registry::Active()[type], "", state, changed)
        {
        }
        
        template<class F_>
        explicit Checkbox(std::string text, bool state, F_ changed, Registry::TemplateType type = Registry::Checkbox_Regular) : 
            Checkbox(Registry::Active()[type], text, state, changed)
        {
        }

        explicit Checkbox(const UI::Template &temp, std::string text = "") : Checkbox(temp, text, false) {}

        Checkbox(const UI::Template &temp, const char *text) : Checkbox(temp, std::string(text), false) {}

        template<class F_>
        Checkbox(const UI::Template &temp, F_ changed) : Checkbox(temp, "", false, changed) { }

        template<class F_>
        Checkbox(const UI::Template &temp, std::string text, F_ changed) : Checkbox(temp, text, false, changed) { }

        template<class F_>
        Checkbox(const UI::Template &temp, const char *text, F_ changed) : Checkbox(temp, std::string(text), false, changed) { }

        template<class F_>
        Checkbox(const UI::Template &temp, bool state, F_ changed) : Checkbox(temp, "", state, changed) { }


        template<class F_>
        Checkbox(const UI::Template &temp, std::string text, bool state, F_ changed) : Checkbox(temp, text, state) {
            ChangedEvent.Register(changed);
        }

        template<class F_>
        Checkbox(const UI::Template &temp, const char *text, bool state, F_ changed) : Checkbox(temp, std::string(text), state, changed) { }

        Checkbox(const UI::Template &temp, bool state) : Checkbox(temp, "", state) {}

        Checkbox(const UI::Template &temp, std::string text, bool state);

        Checkbox(const UI::Template &temp, const char *text, bool state) : Checkbox(temp, std::string(text), state) { }
        
        Checkbox &operator =(bool value) { SetState(value); return *this; }
        
        operator bool() { return GetState(); }

        virtual ~Checkbox();

        void SetText(const std::string &value);

        std::string GetText() const { return text; }
        
        /// Returns the state of the checkbox
        virtual bool GetState() const override { return state; }
        
        /// Changes the state of the checkbox
        virtual bool SetState(bool value, bool force = false) override;
        
        virtual bool Activate() override;
        
        bool KeyEvent(Input::Key key, float state) override;

        TextualProperty<Checkbox, std::string, &Checkbox::GetText, &Checkbox::SetText> Text;

        Event<Checkbox, bool /*state*/> ChangedEvent;
        
    private:
        std::string text;
        bool spacedown  = false;
        bool state = false;
        
    protected:
        virtual bool allowfocus() const override;

    };
    
} }

