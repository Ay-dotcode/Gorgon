#pragma once

#include "Common.h"
#include "../UI/ComponentStackWidget.h"
#include "../Property.h"
#include "Registry.h"
#include "Listbox.h"

namespace Gorgon { namespace Widgets {
    
    /**
     * This is the dropdown base for all dropdown lists, including ones that are
     * not for item selection (e.g., DropdownChecklist).
     */
    template <class T_, void (*TW_)(const T_ &, ListItem &), class L_>
    class DropdownBase : public virtual UI::ComponentStackWidget, protected ListItem
    {
    public:
        using ListType = L_;
        
        template <class ...A_>
        DropdownBase(A_&& ... elms, Registry::TemplateType type = Registry::Dropdown_Regular) :
            DropdownBase(Registry::Active()[type])
        {
            List.Add(std::forward<A_>(elms)...);
        }
        
        explicit DropdownBase(Registry::TemplateType type = Registry::Dropdown_Regular) :
            DropdownBase(Registry::Active()[type])
        { }
        
        explicit DropdownBase(const UI::Template &temp) :
            ComponentStackWidget(temp, {
                {UI::ComponentTemplate::ListTag, {}}
            }),
            ListItem(temp),
            list(this->stack.GetTemplate(UI::ComponentTemplate::ListTag)  ? 
                 *this->stack.GetTemplate(UI::ComponentTemplate::ListTag) :
                 Registry::Active()[Registry::Listbox_Regular]
            ),
            List(list)
        { }
        
        template <class ...A_>
        explicit DropdownBase(const UI::Template &temp, A_&& ... elms) : DropdownBase(temp)
        {
            List.Add(std::forward<A_>(elms)...);
        }
        
    protected:
        ListType list;
        bool opened = false;
        bool refresh = false;
        
    public:
        typename ListType::ListBase &List;
        
        /// Refreshes the dropdown. Normally this is not required unless
        /// references of the data contained within the list is updated
        /// without any modification to the dropdown. This is intended to
        /// be overloaded.
        virtual void Refresh() {
            if(opened)
                list.Refresh();
            else
                refresh = true;
        }
        
        void Open() {
            //TODO
            
            if(refresh) {
                refresh = false;
                list.Refresh();
            }
        }
        
        void Close() {
            
        }
        
        /// Toggles open/close state of the dropdown
        void Toggle() {
            if(opened)
                Close();
            else
                Open();
        }
        
        /// Returns whether the dropdown is opened
        bool IsOpened() const {
            return opened;
        }
        
        /// Retuns if the list will be opened above the dropdown instead of
        /// below. This can happen if there is not enough space below.
        bool IsReversed() const {
            Utils::NotImplemented();
        }
    };
    
    /**
     * This is the dropdown base for single item selection dropdown lists.
     */
    template <class T_, void (*TW_)(const T_ &, ListItem &), class L_>
    class SingleSelectionDropdown : 
        public virtual UI::ComponentStackWidget, 
        public DropdownBase<T_, TW_, L_>
    {
        using Base = DropdownBase<T_, TW_, L_>;
    public:
        
        template <class ...A_>
        SingleSelectionDropdown(A_&& ... elms, Registry::TemplateType type = Registry::Dropdown_Regular) :
            SingleSelectionDropdown(Registry::Active()[type])
        {
            this->list.Add(std::forward<A_>(elms)...);
        }
        
        explicit SingleSelectionDropdown(Registry::TemplateType type = Registry::Dropdown_Regular) :
            SingleSelectionDropdown(Registry::Active()[type])
        { }
        
        explicit SingleSelectionDropdown(const UI::Template &temp) :
            ComponentStackWidget(temp, {
                {UI::ComponentTemplate::ListTag, {}}
            }),
            Base(temp)
        {
            this->list.ChangedEvent.Register([this]() {
                if(this->list.HasSelectedItem()) {
                    TW_(this->list.GetSelectedItem(), *this);
                }
                else {
                    this->SetText("");
                    this->RemoveIcon();
                }
            });
        }
        
        template <class ...A_>
        explicit SingleSelectionDropdown(const UI::Template &temp, A_&& ... elms) : SingleSelectionDropdown(temp)
        {
            this->ist.Add(std::forward<A_>(elms)...);
        }
        
        virtual void Refresh() override {
            Base::Refresh();
            
            if(this->list.HasSelectedItem()) {
                TW_(this->list.GetSelectedItem(), *this);
            }
            else {
                this->SetText("");
                this->RemoveIcon();
            }
        }
        
        /// Changes selection to the given item. If it is not found this function
        /// will throw.
        SingleSelectionDropdown &operator =(const T_ &value) {
            this->list.SetSelectedItem(value);
            
            return *this;
        }
        
        /// Changes the selection to the given index.
        void SetSelectedIndex(long index) {
            this->list.SetSelectedIndex(index);
        }
        
        /// Returns the currently selected item.
        operator const T_ &() const {
            return this->list.GetSelectedItem();
        }
        
        /// Returns the currently selected item. Changing the selected item does not update
        /// dropdown automatically.
        operator T_ &() {
            return this->list.GetSelectedItem();
        }
        
        Event<SingleSelectionDropdown, long> SelectionChanged = Event<SingleSelectionDropdown, long>{this};
    };
    
    
    template <class T_, void (*TW_)(const T_ &, ListItem &) = internal::SetTextUsingFrom<T_, ListItem>>
    using DropdownList = SingleSelectionDropdown<T_, TW_, SimpleListbox<T_, TW_>>;
    
    template <class T_, void (*TW_)(const T_ &, ListItem &) = internal::SetTextUsingFrom<T_, ListItem>>
    using DropdownCollection = SingleSelectionDropdown<T_, TW_, SimpleCollectionbox<T_, TW_>>;
    
} }
