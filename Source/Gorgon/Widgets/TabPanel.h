#pragma once

#include "Composer.h"
#include "../Containers/Hashmap.h"
#include "../Containers/Collection.h"

#include "Panel.h"

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
        //TODO icon
        //blankpanel
        
        

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
                {UI::ComponentTemplate::ButtonTag, {}}
            }) 
        {
        }

        /// Construct a new panel
        explicit basic_TabPanel(Registry::TemplateType type = Registry::TabPanel_Regular) : basic_TabPanel(Registry::Active()[type]) {
        }

        /// Create a new tab with the given key and title
        Tab<Key_> &New(const Key_ &key, const std::string &title) {
            
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
        void Activate(const Key_ &key);

        /// Deactivate the currently active tab.
        void Deactivate();

        /// Activates the next tab
        void ActivateNext();

        /// Activates the previous tab
        void ActivatePrevious();

        /// Returns if there is an active tab.
        bool HasActiveTab() const;

        /// Returns the currently active tab. Throws is there is no active tab.
        Tab<Key_> &GetActiveTab();

        /// Returns the currently active tab. Throws is there is no active tab.
        const Tab<Key_> &GetActiveTab() const;

        /// Set how the tab buttons will be resized. Default is AutoUnit
        void SetButtonSizing(ButtonSizing value);

        /// Returns how the tab buttons will be resized
        ButtonSizing GetButtonSizing() const {
            return sizing;
        }

        /// Sets if the button text would be wrapped. If true, current tab size
        /// will be used to determine wrap width. Default value is false.
        void SetButtonTextWrap(bool value);

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
    };

    using TabPanel = basic_TabPanel<std::string>;

} }
