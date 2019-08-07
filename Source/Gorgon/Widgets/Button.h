#pragma once

#include "../UI/ComponentStackWidget.h"
#include "../Property.h"

namespace Gorgon { namespace Graphics { class Bitmap; } }

namespace Gorgon { namespace Widgets {

    class Button : public UI::ComponentStackWidget {
    public:
        Button(const Button &) = delete;
        
        Button(Button &&) = default;

        explicit Button(const UI::Template &temp, std::string text = "");

		Button(const UI::Template &temp, const char *text) : Button(temp, std::string(text)) { }

		template<class F_>
		Button(const UI::Template &temp, F_ clickfn) : Button(temp, "") {
			ClickEvent.Register(clickfn);
		}

		template<class F_>
		Button(const UI::Template &temp, std::string text, F_ clickfn) : Button(temp, text) {
			ClickEvent.Register(clickfn);
		}

		template<class F_>
		Button(const UI::Template &temp, const char *text, F_ clickfn) : Button(temp, std::string(text), clickfn) { }
        
        Button &operator =(Button &&) = default;

		virtual ~Button();

        void SetText(const std::string &value);

        std::string GetText() const { return text; }

        void SetIcon(const Graphics::Animation &value);

		void RemoveIcon();

        const Graphics::Animation &GetIcon() const {
			if(!HasIcon())
				throw std::runtime_error("This widget has no icon.");

			return *icon;
		}

        bool HasIcon() const { return icon != nullptr; }

        void OwnIcon();

        void OwnIcon(const Graphics::Animation &value);
        
        void OwnIcon(Graphics::Bitmap &&value);
        
        virtual bool Activate() override;
        
        bool KeyEvent(Input::Key key, float state) override;

        TextualProperty<Button, std::string, &Button::GetText, &Button::SetText> Text;

        ObjectProperty<Button, const Graphics::Animation, &Button::GetIcon, &Button::SetIcon> Icon;

        Event<Button> ClickEvent;
        
    private:
        std::string text;
		const Graphics::Animation *icon = nullptr;
		bool ownicon    = false;
        bool spacedown  = false;
        
	protected:
		virtual bool allowfocus() const override;

	};
    
} }
