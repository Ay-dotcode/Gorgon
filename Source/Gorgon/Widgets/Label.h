#pragma once

#include "../UI/ComponentStackWidget.h"
#include "../Property.h"
#include "Registry.h"

namespace Gorgon { namespace Graphics { class Bitmap; } }

namespace Gorgon { namespace Widgets {

    class Label : public UI::ComponentStackWidget {
    public:
        Label(const Label &) = delete;
        
        explicit Label(std::string text = "", Registry::TemplateType type = Registry::Label_Regular) : 
            Label(Registry::Active()[type], text) 
        {
        }


        explicit Label(const UI::Template &temp, std::string text = "");

        Label(const UI::Template &temp, const char *text) : Label(temp, std::string(text)) { }

        virtual ~Label();

        void SetText(const std::string &value);

        std::string GetText() const { return text; }
        
        /// Changes the icon on the button. The ownership of the bitmap
        /// is not transferred. If you wish the bitmap to be destroyed
        /// with the button, use OwnIcon instead.
        void SetIcon(const Graphics::Bitmap &value);
        
        /// Changes the icon on the button. The ownership of the animation
        /// is not transferred. If you wish the animation to be destroyed
        /// with the button, use OwnIcon instead.
        void SetIcon(const Graphics::Animation &value);
        
        /// Changes the icon on the button. This will create a new animation
        /// from the given provider and will own the resultant animation.
        void SetIconProvider(const Graphics::AnimationProvider &value);
        
        /// Changes the icon on the button. This will move in the provider,
        /// create a new animation and own both the provider and the animation
        void SetIconProvider(Graphics::AnimationProvider &&provider);
        
        /// Removes the icon on the button
        void RemoveIcon();
        
        /// Returns if the button has an icon
        bool HasIcon() const { return icon != nullptr; }
        
        /// Returns the icon on the button. If the button does not have an
        /// icon, this function will throw
        const Graphics::Animation &GetIcon() const {
            if(!HasIcon())
                throw std::runtime_error("This widget has no icon.");
            
            return *icon;
        }
        
        /// Transfers the ownership of the current icon.
        void OwnIcon();
        
        /// Sets the icon while transferring the ownership
        void OwnIcon(const Graphics::Animation &value);
        
        /// Moves the given animation to the icon of the button
        void OwnIcon(Graphics::Bitmap &&value);
        
        virtual bool Activate() override;

        TextualProperty<Label, std::string, &Label::GetText, &Label::SetText> Text;

        ObjectProperty<Label, const Graphics::Animation, &Label::GetIcon, &Label::SetIcon> Icon;
        
    private:
        std::string text;
        const Graphics::Animation          *icon     = nullptr;
        const Graphics::AnimationProvider  *iconprov = nullptr;
        
        bool ownicon = false;
        
    protected:
        virtual bool allowfocus() const override;

    };
    
} }
