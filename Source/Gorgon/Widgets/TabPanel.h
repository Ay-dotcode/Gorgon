#pragma once

#include "Composer.h"
#include "../Containers/Hashmap.h"
#include "../Containers/Collection.h"

#include "Panel.h"
#include "Checkbox.h"
#include "../UI/RadioControl.h"
#include "../UI/Organizers/Flow.h"

namespace Gorgon { namespace Widgets {
    template <class Key_>
    class basic_TabPanel;

    /**
     * This class represents a tab in Tab TabPanel
     */
    template<class Key_>
    class Tab : public Panel {
        friend class basic_TabPanel<Key_>;
    public:
        //TODO: icon
        
        Key_ GetKey() const {
            return key;
        }

        void SetKey(const Key_ &value);

        std::string GetTitle() const {
            return title;
        }

        void SetTitle(const std::string &value);
        
        PROPERTY_GETSET(Tab, , Key_, Key);
        
        PROPERTY_GETSET(Tab, , std::string, Title);

    private:
        Tab(basic_TabPanel<Key_> &parent, const UI::Template &temp, 
            const Key_ &key, const std::string &title
        ) : 
            Panel(temp), parent(&parent), 
            key(key), title(title)
        {
        }

        basic_TabPanel<Key_> *parent;
        Key_ key;
        std::string title;
    };

    /**
     * This widget is a tabbed container. While the widget itself is not
     * a container, its members are. Each tab is associated with a key.
     * Creating another with the same key will erase the tab, removing its
     * contents.
     */
    template <class Key_>
    class basic_TabPanel : public ComponentStackComposer {
        friend class Tab<Key_>;
    public:

        /// Enumeration that controls how tab buttons are sized
        enum ButtonSizing {
            /// Size is automatic depending on the title length
            Auto,
            /// Size is the smallest integer unit size that the title will fit into
            AutoUnit,
            /// Only the given size is used
            Fixed,
            /// Fills the full width of the tab bar
            Fill,
            /// Fills the full width of the container but will the given size
            /// is used as maximum
            Adaptive,
        };

        /// Enumeration that controls how the overflow in buttons is treated
        enum ButtonOverflow {
            /// Any excess buttons will be hidden
            HideExcess,
            /// Buttons can be scrolled vertically
            VScroll,
            /// Buttons can be scrolled horizontally
            HScroll,
            /// Buttons are shown in multiple lines
            ExpandLines,
            /// Buttons are forced to a scale that will fit the bar
            Scale,
            /// Excess buttons are grouped in a list
            List
        };

        /// Construct a new panel
        explicit basic_TabPanel(const UI::Template &temp) : 
            ComponentStackComposer(temp, {
                {UI::ComponentTemplate::ButtonTag, {}},
                {UI::ComponentTemplate::PanelTag, {}},
            })
        {
            stack.AddGenerator(UI::ComponentTemplate::ButtonsTag, std::bind(&basic_TabPanel::getbtns, this, std::placeholders::_1));
            radio.ChangedEvent.Register(*this, &basic_TabPanel::Activate);

            Refresh();
        }

        /// Construct a new panel
        explicit basic_TabPanel(Registry::TemplateType type = Registry::TabPanel_Regular) : basic_TabPanel(Registry::Active()[type]) {
        }

        ~basic_TabPanel() {
            delete buttonspnl;
            buttons.Destroy();
            tabs.Destroy();
        }

        /// Create a new tab with the given key and title
        Tab<Key_> &New(const Key_ &key, const std::string &title) {
            auto layertemp = stack.GetTemplate(UI::ComponentTemplate::PanelTag);
            if(!layertemp)
                layertemp = &Registry::Active()[Registry::Panel_Blank];
            
            auto buttontemp = stack.GetTemplate(UI::ComponentTemplate::ButtonTag);
            if(!buttontemp)
                buttontemp = &Registry::Active()[Registry::Checkbox_Button];

            auto &tab = *new Tab<Key_>(
                *this,
                *layertemp,
                key, title
            );
            tabs.Add(tab);
            mapping.Add(key, tab);

            auto &btn = *new Checkbox(*buttontemp, title);
            btn.SetHorizonalAutosize(UI::Autosize::Unit);
            buttons.Add(btn);
            radio.Add(key, btn);

            if(buttonspnl)
                buttonspnl->Add(btn);

            if(tabs.GetSize() == 1) {
                Activate(key);
            }

            Refresh();

            return tab;
        }

        /// Create a new tab with the given key, title will be determined from the key
        Tab<Key_> &New(const Key_ &key) {
            return New(key, String::From(key));
        }

        /// Create a new tab with the given title, key will be determined from the title
        template<typename K_ = Key_>
        typename std::enable_if<!std::is_same<K_, std::string>::value, Tab<Key_> &>::type
        New(const std::string &title) {
            return New(String::To<Key_>(title), title);
        }

        /// Create a new tab with the given key and title. The tab will be inserted before the given key.
        /// If the key does not exist, new tab will be appended to the end.
        Tab<Key_> &Insert(const Key_ &before, const Key_ &key, const std::string &title);

        /// Create a new tab with the given key, title will be determined from the title. The tab will be
        /// inserted before the given key. If the key does not exist, new tab will be appended to the end.
        Tab<Key_> &Insert(const Key_ &before, const Key_ &key) {
            return Insert(before, key, String::From(key));
        }

        /// Create a new tab with the given title, key will be determined from the title. The tab will be
        /// inserted before the given key. If the key does not exist, new tab will be appended to the end.
        template<typename K_ = Key_>
        typename std::enable_if<!std::is_same<K_, std::string>::value, Tab<Key_> &>::type Insert(const Key_ &before, const std::string &title) {
            return Insert(before, String::To<Key_>(title), title);
        }

        /// Remove the tab at the given key
        void Remove(const Key_ &key);

        /// Remove all tabs with the supplied title
        void RemoveAllOf(const std::string &title);

        /// Moves the tab at the given key before another tab. If the before tab does not exit, the tab
        /// will be moved to the end.
        void MoveBefore(const Key_ &before, const Key_ &tab);

        /// Return the tab with the supplied key
        Tab<Key_> &operator [](const Key_ &key) {
            return mapping[key];
        }

        /// Return the tab with the supplied key
        const Tab<Key_> &operator [](const Key_ &key) const {
            return mapping[key];
        }

        /// Returns true if the tab with the given key exist
        bool Exists(const Key_ &key) const {
            return mapping.Exists(key);
        }

        bool Activate() override {
            return true;
        }

        /// Activates the tab with the key. If key does not exist nothing is done.
        void Activate(const Key_ &key) {
            if(mapping.Exists(key)) {
                if(radio.Get() != key)
                    radio.Set(key);

                mapping[key].Resize(stack.GetTagSize(UI::ComponentTemplate::PanelTag));
                stack.SetWidget(UI::ComponentTemplate::PanelTag, &mapping[key]);

                hasactive = true;
            }
        }

        /// Deactivate the currently active tab.
        void Deactivate() {
            radio.Set(Key_{});
            stack.SetWidget(UI::ComponentTemplate::PanelTag, nullptr);
            hasactive = false;
        }

        /// Activates the next tab
        void ActivateNext();

        /// Activates the previous tab
        void ActivatePrevious();

        /// Returns if there is an active tab.
        bool HasActiveTab() const {
            return hasactive && radio.Exists(radio.Get()) && mapping.Exists(radio.Get());
        }

        /// Returns the currently active tab. Throws is there is no active tab.
        Tab<Key_> &GetActiveTab() {
            if(!HasActiveTab())
                throw std::runtime_error("No active tab");

            return mapping[radio.Get()];
        }

        /// Returns the currently active tab. Throws is there is no active tab.
        const Tab<Key_> &GetActiveTab() const {
            if(!HasActiveTab())
                throw std::runtime_error("No active tab");

            return mapping[radio.Get()];
        }

        /// Set how the tab buttons will be resized. Default is AutoUnit
        void SetButtonSizing(ButtonSizing value);

        /// Returns how the tab buttons will be resized
        ButtonSizing GetButtonSizing() const {
            return sizing;
        }

        /// Sets if the button text would be wrapped. If true, current tab size
        /// will be used to determine wrap width. Default value is false.
        void SetButtonTextWrap(bool value) {
            if(value == buttontextwrap)
                return;

            buttontextwrap = value;
            Refresh();
        }

        /// Returns if the button text would wrapped.
        bool GetButtonTextWrap() const {
            return buttontextwrap;
        }

        /// Sets how the overflowing tab buttons are managed. Default is Scale.
        /// Some options are not implemented yet and will default to Scale.
        void SetButtonOverflow(ButtonOverflow value);

        /// Returns how the overflowing tab buttons are managed.
        ButtonOverflow GetButtonOverflow() const {
            return overflow;
        }

        /// Sets the size of the tab buttons. The size will be used according
        /// to ButtonSizing.
        void SetButtonSize(int w, int h) {
            SetButtonSize({w, h});
        }

        /// Sets the size of the tab buttons. The size will be used according
        /// to ButtonSizing.
        void SetButtonSize(const Geometry::Size &size);

        /// Returns the size of the buttons
        Geometry::Size GetButtonSize() const {
            return buttonsize;
        }

        /// Sets if next or previous tab switches will rollover. Default is false
        void SetTabRollover(bool value);

        /// Returns if next or previous tab switches will rollover.
        bool GetTabRollover() const {
            return rollover;
        }

        /// Refreshes the tab buttons and their locations. This function is called automatically.
        void Refresh() {
            int x = 0;
            for(int i=0; i<tabs.GetCount(); i++) {
                buttons[i].SetText(tabs[i].GetTitle());
                buttons[i].Location.X = x;
                buttons[i].SetTextWrap(buttontextwrap);
                x = buttons[i].GetBounds().Right;
                tabs[i].Resize(stack.BoundsOf(stack.IndexOfTag(UI::ComponentTemplate::PanelTag)).GetSize());
            }

            if(buttonspnl) {
                if(buttons.GetSize()) {
                    buttonspnl->Resize((Geometry::Size)buttons.Last()->GetBounds().BottomRight());
                }
                else {
                    buttonspnl->Resize({0, GetUnitWidth()});
                }

                stack.SetTagSize(UI::ComponentTemplate::ButtonsTag, buttonspnl->GetSize());
            }
        }

        /// Used to enumerate tabs
        auto begin() {
            return tabs.begin();
        }

        /// Used to enumerate tabs
        auto begin() const {
            return tabs.begin();
        }

        /// Used to enumerate tabs
        auto end() {
            return tabs.end();
        }

        /// Used to enumerate tabs
        auto end() const {
            return tabs.end();
        }

    private:
        //for key mapping
        Containers::Hashmap<Key_, Tab<Key_>> mapping;

        //ordered list
        Containers::Collection<Tab<Key_>>    tabs;

        ButtonSizing    sizing          = AutoUnit;
        bool            buttontextwrap  = false;
        ButtonOverflow  overflow        = Scale;
        Geometry::Size  buttonsize; //constructor will initialize to 3x1U
        bool            rollover        = false;
        bool            hasactive       = false;

        Panel *buttonspnl = nullptr;
        Containers::Collection<Checkbox> buttons;

        UI::RadioControl<Key_, Checkbox> radio;

        UI::Widget* getbtns(const UI::Template& temp) {
            buttonspnl = new Panel(temp);
            //buttonspnl->CreateOrganizer<UI::Organizers::Flow>();
            buttonspnl->EnableScroll(false, false);

            return buttonspnl;
        }
    };

    template <class Key_>
    void Tab<Key_>::SetKey(const Key_ &value) {
        parent->mapping.Remove(key);
        parent->mapping.Add(value, this);
        parent->radio.ChangeValue(key, value);
        key = value;
    }

    template<class Key_>
    void Tab<Key_>::SetTitle(const std::string &value) {
        title = value;
        parent->Refresh();
    }

    using TabPanel = basic_TabPanel<std::string>;


} }
