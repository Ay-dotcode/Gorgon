#pragma once


#include "Common.h"
#include "../UI/ComponentStackWidget.h"
#include "../Property.h"
#include "Registry.h"
#include "ListItem.h"
#include <vector>

namespace Gorgon { namespace Widgets {
    
    //TODO how to deal with pointers and references
    
    /**
     * This is the abstract base of listboxes. It is mainly used to allow list
     * mixins to access list items.
     */
    template<class T_, class W_>
    class ListBase {
    public:
        virtual ~ListBase() {
        }
        
        //TODO add, remove, insert, move, push_back (for back inserter)
        
        /// Returns the item at the given point. This operator will not perform
        /// bounds checking.
        virtual T_ &operator[](long index) = 0;

        /// Returns the item at the given point. This operator will not perform
        /// bounds checking.
        virtual const T_ &operator[](long index) const = 0;
        
        /// Returns the number of elements in the list.
        virtual long GetCount() const = 0;
        
        /// For internal use.
        /// Returns the widget used to represent the item at the given index. This
        /// function will return nullptr if the index does not currently have a
        /// visual representation. This is not an edge case, any item that is not
        /// in view will most likely not have a representation.
        virtual W_ *getrepresentation(long index) = 0;
        
        /// For internal use.
        /// Returns the first widget used to represent any item at within the 
        /// listbox. This function will return nullptr if there are no items in the
        /// list.
        virtual W_ *getrepresentation() = 0;
    };
    
    /// @cond internal
    namespace internal {
        // blank traits has no data associated with the items.
        // It is good for very long lists. Should be used with
        // useisvisible = false.
        template<class T_, class W_>
        class LBTR_blank {
        public:
            void Apply(W_ &, const T_ &, Geometry::Point, Geometry::Size) { }
            UI::ComponentTemplate::Tag Tag(const T_ &, Geometry::Point, Geometry::Size) {
                return UI::ComponentTemplate::UnknownTag;
            }
        };
        
        template <class T_, class W_>
        class LBTRF_blank {
            using TR_ = LBTR_blank<T_, W_>;
        protected:
            LBTRF_blank() { }
            ~LBTRF_blank() { }
            
            TR_ access(long) {
                return TR_();
            }
            
            void prepare(W_ &) { }
            void insert(long, long) { }
            void move(long, long) { }
            void remove(long, long) { }
        };
        
        template<class T_, class W_>
        void SetTextUsingFrom(const T_ &val, W_ &w) {
            w.SetText(String::From(val));
        }
        
        template<class T_, class W_>
        void GetTextUsingTo(W_ &w, T_ &val) {
            val = String::To<T_>(w.GetText());
        }
        
        //This class allows single selection. The selected item will
        //follow the focus by default. If desired, this can be changed
        template<class T_, class W_>
        class LBSELTR_Single {
        public:
            /// Returns true if this listbox has a selected item.
            bool HasSelectedItem() const {
                return selectedindex != -1;
            }
            
            /// Returns the selected item. If nothing is selected this function
            /// will throw. You may check if there is a selection using 
            /// HasSelectedItem function.
            T_ GetSelectedItem() const {
                if(selectedindex == -1)
                    throw std::runtime_error("Nothing is selected.");
                
                return dynamic_cast<ListBase<T_, W_>&>(*this)[selectedindex];
            }
            
            /// Returns the index of the selected item. -1 will be returned if 
            /// nothing is selected.
            long GetSelectedIndex() const {
                return selectedindex;
            }
            
            /// Sets the selection to the given index. If index in not within the
            /// bounds this function will throw std::out_of_range exception. -1
            /// can be used to remove selected item.
            void SetSelectedIndex(long index) {
                if(index < -1 || index >= dynamic_cast<ListBase<T_, W_>&>(*this).GetSize())
                    throw std::out_of_range("Selected index does not exits");
                
                if(index == selectedindex)
                    return;
                
                if(focusonly) {
                    if(index == -1) {
                        W_ *elm = dynamic_cast<ListBase<T_, W_>&>(*this).getrepresentation();
                        if(elm != nullptr && elm->HasParent()) {
                            elm->GetParent().RemoveFocus();
                        }
                    }
                    else {
                        W_ *elm = dynamic_cast<ListBase<T_, W_>&>(*this).getrepresentation(index);
                        if(elm != nullptr) {
                            elm->Focus();
                        }
                        else {
                            elm = dynamic_cast<ListBase<T_, W_>&>(*this).getrepresentation();
                            
                            if(elm != nullptr && elm->HasParent()) {
                                elm->GetParent().RemoveFocus();
                            }
                        }
                    }
                    
                    focusindex = index;
                }
                
                if(selected)
                    selected->SetSelected(false);
                
                if(index != -1) {
                    W_ *elm = dynamic_cast<ListBase<T_, W_>&>(*this).getrepresentation(index);
                    
                    if(elm)
                        elm->SetSelected(true);
                }
                
                selectedindex = index;
            }
            
            void RemoveSelection() {
                SetSelectedIndex(-1);
            }
            
            /// Selects the first item that has the given value. If item does
            /// not exists, this function will remove the selection
            void SetSelection(T_ item) {
                auto &me = dynamic_cast<ListBase<T_, W_>&>(*this);
                
                for(long i=0; i<me.GetSize(); i++) {
                    if(me[i] == item) {
                        SetSelectedIndex(i);
                        return;
                    }
                }
                
                SetSelectedIndex(-1);
            }
            
            /// Returns true if the selected item is the focused item. Default
            /// is true. When set to false, an item can be focused without being
            /// selected. A listbox with radio buttons should have this value
            /// set to false.
            bool IsSelectionFollowingFocus() const {
                return focusonly;
            }
            
            /// Sets whether the selected item is the focused item. Default
            /// is true. When set to false, an item can be focused without being
            /// selected. A listbox with radio buttons should have this value
            /// set to false.
            void SetSelectionFollowsFocus(bool value) {
                if(!focusonly && value) {
                    if(focusindex != -1) {
                        if(selectedindex != focusindex) {
                            selectedindex = focusindex;
                            SelectionChanged(selectedindex);
                        }
                    }
                    else {
                        focusindex = selectedindex;
                    }
                }
                
                focusonly = value;
            }
            
            Event<LBSELTR_Single, long> SelectionChanged = Event<LBSELTR_Single, long>{this};
            
        protected:
            LBSELTR_Single() {
            }
            
            void clicked(long index, W_ &w) {
                if(focusonly) {
                    if(selectedindex == index)
                        return;
                    
                    if(selected)
                        selected->SetSelected(false);
                    
                    w.SetSelected(true);
                    w.Focus();
                    
                    selected = &w;
                    selectedindex = index;
                    focusindex    = index;
                }
                else {
                    if(focusindex == index)
                        return;
                    
                    w.Focus();
                    focusindex = index;
                }
            }
            
            void toggled(long index, W_ &w) {
                if(selectedindex == index)
                    return;
                
                if(selected)
                    selected->SetSelected(false);
                
                w.SetSelected(true);
                selectedindex = index;
                selected = &w;
                
                if(focusonly) {
                    w.Focus();
                    focusindex = index;
                }
            }
            
            void apply(long index, W_ &w, const T_ &) {
                if(index == focusindex) {
                    w.Focus();
                }
                else if(w.IsFocused()) {
                    w.RemoveFocus();
                }
                
                if(index == selectedindex) {
                    w.SetSelected(true);
                    w = &w;
                }
                else {
                    w.SetSelected(false);
                    
                    if(&w == selected)
                        w = nullptr;
                }
            }
            
            void prepare(W_ &) {
                //nothing to be done
            }
            
            void insert(long index, long count) { 
                if(index <= focusindex)
                    focusindex += count;
                
                if(index <= selectedindex)
                    selectedindex += count;
            }
            
            void move(long index, long target) { 
                //move triggers apply to both indexes
                
                if(index == focusindex) {
                    focusindex = target;
                }
                else if(index > focusindex && target <= focusindex) {
                    focusindex++;
                }
                else if(index < focusindex && target > focusindex) {
                    focusindex--;
                }
                
                if(index == selectedindex) {
                    selectedindex = target;
                }
                else if(index > selectedindex && target <= selectedindex) {
                    selectedindex++;
                }
                else if(index < selectedindex && target > selectedindex) {
                    selectedindex--;
                }
            }
            
            void remove(long index, long count) { 
                //removed items will be repurposed using apply,
                if(index <= focusindex) {
                    if(index+count > focusindex) {
                        focusindex = -1;
                    }
                    else {
                        focusindex -= count;
                    }
                }
                
                if(index <= selectedindex) {
                    if(index+count > selectedindex) {
                        selectedindex = -1;
                    }
                    else {
                        selectedindex -= count;
                    }
                }
            }
            
            void destroy(W_ &w) {
                if(&w == selected) {
                    selected = nullptr;
                    selectedindex = -1;
                }
                if(w.IsFocused()) {
                    focusindex = -1;
                    w.RemoveFocus();
                }
            }
            
            bool focusonly = true;
            
            long focusindex = -1, selectedindex = -1;
            
            W_ *selected = nullptr;
        };
        
        template<class S_>
        typename std::enable_if<TMP::FunctionTraits<decltype(&S_::size)>::IsMember, long>::type 
        getsize(const S_ &storage) {
            return storage.size();
        }
        
        template<class S_>
        typename std::enable_if<TMP::FunctionTraits<decltype(&S_::GetCount)>::IsMember, long>::type
        getsize(const S_ &storage) {
            return storage.GetCount();
        }
    }
    /// @endcond
    
    /**
     * This is the base class for all listbox like widgets. It is not intended
     * to be used directly but you may use it to create your own listboxes. The
     * solid listboxes should be derived from this object and should make 
     * desired functionality public.
     * 
     * Listbox will maintain the necessary number widgets to fill its area. It
     * does not create widgets for elements falling outside the range. If traits
     * can return different tags, the widgets to cover its area will be created
     * separately for these tags. 
     * 
     * Listbox has multi-column functionality. However, this is not to be used
     * for tables. Table systems should insert rows as listbox items, which then
     * should have cells.
     * 
     * *Template parameters*
     * 
     * T_ is the stored data type. 
     * 
     * S_ is the storage for T_.
     * 
     * W_ is the widget that will be used to represent items.
     * 
     * TR_ is widget traits class that allows additional properties for each
     * element. They should contain minimal number of data members. There should
     * be a function Apply taking `W_ &`, `const T_ &`, Point index, Size total
     * as parameters applying traits to W_. Additionally, TR_ should have Tag
     * function that has `const T_ &`, Point index, Size total as parameters and
     * should return UI::ComponentTemplate::Tag to determine the template for
     * the widget. The tags should match with the listbox template. If the given
     * tag is not found, ItemTag will be used.
     * 
     * TRF_ is the trait functions that will return/set the traits of items. 
     * TRF_ should contain access function that should take the index of an item
     * to return TR_ associated with it.
     * It should contain `insert(before, count)` to add count elements before
     * the given index. These elements must be default initialized to a normal
     * state. 
     * `move(index, before)` should move the indexed item before the given 
     * index. 
     * `remove(index, count)` should remove count number of items 
     * starting from the given index. 
     * `prepare(W_ &)` function should prepare the given widget for the first
     * usage. This is only called once on a newly created widget.
     * `destroy(W_ &)` function is called when a widget is about to be 
     * destroyed.
     * Index parameters should be long int. These functions should be protected
     * as ListboxBase will be derived from TRF_. Finally, TRF_ should have a 
     * protected constructor/destructor.
     * 
     * useisvisible controls if elements can be hidden. If this is true, TR_
     * should have IsVisible function returning bool. This parameter will
     * slowdown listbox considerably when many items are stored as each item
     * will be checked whether they are visible until to the point of display.
     * This will not affect the performance on short lists.
     * 
     * SELTR_ is the class that will control selection traits of the Listbox.
     * Defult traits allow single select. It is possible to construct list of 
     * particular widgets that has no concept of selection if selection traits
     * is set to internal::LBSELTR_None.
     * This class should support click and toggle functions both taking long 
     * index, const T_ & value, W_ &widget. Click function will be called when
     * the user uses arrow keys to select an item.
     * apply function should apply selection related traits to the widget taking
     * long index, W_ &widget, const T_ & value. Any item that is benched will
     * be applied to as well with an index of -1.
     * This class should contain `insert(before, count)` to add count elements
     * before the given index. These elements must be default initialized to a
     * non-selected normal state. 
     * `move(index, before)` should move the indexed item before the given 
     * index. 
     * `remove(index, count)` should remove count number of items 
     * starting from the given index. 
     * `prepare(W_ &)` function should prepare the given widget for the first
     * usage. This is only called once on a newly created widget.
     * `destroy(W_ &)` function is called when a widget is about to be 
     * destroyed.
     * Index parameters should be long int. These functions should be protected
     * as ListboxBase will be derived from SELTR_. Finally, SELTR_ should have a 
     * protected constructor/destructor.
     * 
     * TW_ function should take a T_ and set this to its W_ representation. 
     * 
     * WT_ should read the data from W_ and set it to T_.
     */
    template<
        class T_, class S_, class W_, class TR_, class TRF_, 
        bool useisvisible = false, class SELTR_ = internal::LBSELTR_Single<T_, W_>,
        void (*TW_)(const T_ &, W_ &) = internal::SetTextUsingFrom<T_, W_>,
        void (*WT_)(W_ &, T_ &)       = internal::GetTextUsingTo  <T_, W_>
    >
    class ListboxBase : public UI::ComponentStackWidget, public ListBase<T_, W_>, public TRF_, public SELTR_ {
    public:
        
        /// Returns the item at the given point. This operator will not perform
        /// bounds checking.
        virtual T_ &operator[](long index) override {
            return storage[index];
        }

        /// Returns the item at the given point. This operator will not perform
        /// bounds checking.
        virtual const T_ &operator[](long index) const override {
            return storage[index];
        }
        
        /// Returns the number of elements in the list.
        virtual long GetCount() const override {
            return internal::getsize(storage);
        }
        
        /// For internal use.
        /// Returns the widget used to represent the item at the given index. This
        /// function will return nullptr if the index does not currently have a
        /// visual representation. This is not an edge case, any item that is not
        /// in view will most likely not have a representation.
        virtual W_ *getrepresentation(long index) override {
            return nullptr;
        }
        
        /// For internal use.
        /// Returns the first widget used to represent any item at within the 
        /// listbox. This function will return nullptr if there are no items in the
        /// list.
        virtual W_ *getrepresentation() override {
            return nullptr;
        }
        
        auto begin() {
            return storage.begin();
        }
        
        auto begin() const {
            return storage.begin();
        }
        
        auto end() {
            return storage.end();
        }
        
        auto end() const {
            return storage.end();
        }
        
    protected:
        ListboxBase(const UI::Template &temp) : 
            ComponentStackWidget(temp)
        {
            stack.HandleMouse();
        }
        
        ~ListboxBase() { }
        
        S_ storage;
    };
    
    /**
     * This is a simple listbox. It does not store any additional information
     * about each item, therefore, can store large amounts of items. Items are
     * stored in a std::vector. Only one element can be selected at a time.
     */
    template<class T_>
    class SimpleListbox : 
        public ListboxBase<T_, 
            std::vector<T_>, ListItem, 
            internal::LBTR_blank <T_, ListItem>,
            internal::LBTRF_blank<T_, ListItem>
        >
    {
        using Base = ListboxBase<T_, 
            std::vector<T_>, ListItem, 
            internal::LBTR_blank <T_, ListItem>,
            internal::LBTRF_blank<T_, ListItem>
        >;
    public:
        SimpleListbox() : Base(Registry::Active()[Registry::Checkbox_Regular]) {
        }
        
        virtual bool Activate() {
            return false;
        }
    };
    
} }

