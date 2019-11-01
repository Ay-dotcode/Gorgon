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
        
        /// Changes the text displayed on the button
        void SetText(const std::string &value);

        /// Returns the text displayed on the button
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
        
        /// Activates click repeat. All parameters are in milliseconds. delay is the
        /// time before first repeat. repeat is the initial time between repeats.
        /// accelerationtime is time to reach minimum repeat interval. maxrepeat is the
        /// minimum time between the repeats.
        void ActivateClickRepeat(int delay = 500, int repeat = 400, int accelerationtime = 2000, int minrepeat = 200);
        
        
        /// Deactivates click repeat.
        void DeactivateClickRepeat();
        
        virtual bool Activate() override;
        
        bool KeyEvent(Input::Key key, float state) override;

        TextualProperty<Button, std::string, &Button::GetText, &Button::SetText> Text;

        ObjectProperty<Button, const Graphics::Animation, &Button::GetIcon, &Button::SetIcon> Icon;
        
        Event<Button> ClickEvent    = Event<Button>(this);
        Event<Button> PressEvent    = Event<Button>(this);
        Event<Button> ReleaseEvent  = Event<Button>(this);
        
    private:
        std::string text;
        const Graphics::Animation          *icon     = nullptr;
        const Graphics::AnimationProvider  *iconprov = nullptr;
        bool ownicon    = false;
        bool spacedown  = false;
        bool mousedown  = false;
        
        enum repeatstate {
            repeatdisabled,
            repeatstandby,
            repeatondelay,
            repeating,
        };
        
        int             repeatdelay = 500;
        int             repeatinit  = 400;
        int             repeatacc   = 2000;
        int             repeatfin   = 400;
        int             repeatdiff  = -200;
        float           repeatcur   = 0;
        int             repeatleft  = -1;
        repeatstate     repeaten    = repeatdisabled;
        
    protected:
        virtual bool allowfocus() const override;

        void repeattick();
    };
    
} }
