#pragma once

#include "../UI/ComponentStackWidget.h"
#include "../Property.h"

namespace Gorgon { namespace Widgets {

    class Checkbox : public UI::ComponentStackWidget {
    public:
        Checkbox(const Checkbox &) = delete;
        
        Checkbox(Checkbox &&) = default;

        explicit Checkbox(const UI::Template &temp, std::string text = "") : Checkbox(temp, text, false) { }

        Checkbox(const UI::Template &temp, const char *text) : Checkbox(temp, text, false) { }

        Checkbox(const UI::Template &temp, bool state) : Checkbox(temp, "", state) { }

        Checkbox(const UI::Template &temp, std::string text, bool state);

        Checkbox(const UI::Template &temp, std::function<void()> changed) : Checkbox(temp, "", false, changed) { }

        Checkbox(const UI::Template &temp, std::string text, std::function<void()> changed) : Checkbox(temp, text, false, changed) { }

        Checkbox(const UI::Template &temp, const char *text, std::function<void()> changed) : Checkbox(temp, text, false, changed) { }

        Checkbox(const UI::Template &temp, bool state, std::function<void()> changed) : Checkbox(temp, "", state, changed) { }

        Checkbox(const UI::Template &temp, std::string text, bool state, std::function<void()> changed);
        
        Checkbox &operator =(Checkbox &&) = default;
        
        Checkbox &operator =(bool value) { SetState(value); return *this; }

		virtual ~Checkbox();

        void SetText(const std::string &value);

        std::string GetText() const { return text; }
		
		/// Returns the state of the checkbox
		explicit operator bool() const { return GetState(); }
        
        /// Returns the inverse state of the checkbox
        bool operator !() const { return !GetState(); }
        
        /// Returns the state of the checkbox
        bool GetState() const { return state; }
        
        /// Changes the state of the checkbox
        void SetState(bool value);
        
        /// Changes the state of the checkbox
        void Check() { SetState(true); }
        
        /// Changes the state of the checkbox
        void Clear() { SetState(false); }
        
        /// Changes the state of the checkbox
        void Toggle() { SetState(!GetState()); }
        
        virtual bool Activate() override;
        
        bool KeyEvent(Input::Key key, float state) override;

        TextualProperty<Checkbox, std::string, &Checkbox::GetText, &Checkbox::SetText> Text;

        Event<Checkbox> ChangedEvent;
        
    private:
        std::string text;
        bool spacedown  = false;
        bool state = false;
        
	protected:
		virtual bool allowfocus() const override;

	};
    
} }

