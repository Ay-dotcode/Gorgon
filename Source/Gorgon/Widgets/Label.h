#pragma once

#include "../UI/ComponentStackWidget.h"
#include "../Property.h"

namespace Gorgon { namespace Graphics { class Bitmap; } }

namespace Gorgon { namespace Widgets {

    class Label : public UI::ComponentStackWidget {
    public:
        Label(const Label &) = delete;
        
        Label(Label &&) = default;

        explicit Label(const UI::Template &temp, std::string text = "");

        Label(const UI::Template &temp, const char *text) : Label(temp, std::string(text)) { }
        
        Label &operator =(Label &&) = default;

        virtual ~Label();

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

        TextualProperty<Label, std::string, &Label::GetText, &Label::SetText> Text;

        ObjectProperty<Label, const Graphics::Animation, &Label::GetIcon, &Label::SetIcon> Icon;
        
    private:
        std::string text;
        const Graphics::Animation *icon = nullptr;
        bool ownicon    = false;
        
    protected:
        virtual bool allowfocus() const override;

    };
    
} }
