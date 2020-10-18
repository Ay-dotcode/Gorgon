#pragma once


#include "Common.h"
#include "../UI/ComponentStackWidget.h"
#include "../Property.h"
#include "Registry.h"
#include "ListItem.h"
#include <vector>
#include "Scrollbar.h"

namespace Gorgon { namespace Widgets {
    
    /**
     * This is the abstract base of listboxes. It is mainly used to allow list
     * mixins to access list items.
     */
    template<class T_, class W_>
    class ListBase {
    public:
        virtual ~ListBase() {
        }
        
        /// Returns the item at the given point. This operator will not perform
        /// bounds checking.
        virtual T_ &operator[](long index) = 0;

        /// Returns the item at the given point. This operator will not perform
        /// bounds checking.
        virtual const T_ &operator[](long index) const = 0;
        
        /// Returns the number of elements in the list.
        virtual long GetCount() const = 0;
        
        /// For internal use.
        /// Returns the first widget used to represent any item at within the 
        /// listbox. This function will return nullptr if there are no items in the
        /// list.
        virtual long getindex(const W_ &widget) = 0;
        
        /// This function should refresh the contents of the listbox. Normally, 
        /// calling this function is not necessary as it is handled internally.
        /// This function may defer refresh to next frame.
        virtual void Refresh() = 0;
        
    protected:

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
                return UI::ComponentTemplate::ItemTag;
            }
        };
        
        template <class T_, class W_, class F_>
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
        template<class T_, class W_, class F_>
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
                
                return dynamic_cast<const F_&>(*this)[selectedindex];
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
                if(index < -1 || index >= dynamic_cast<F_&>(*this).GetSize())
                    throw std::out_of_range("Selected index does not exits");
                
                if(index == selectedindex)
                    return;
                
                if(focusonly) {
                    if(index == -1) {
                        W_ *elm = dynamic_cast<F_&>(*this).getrepresentation();
                        if(elm != nullptr && elm->HasParent()) {
                            elm->GetParent().RemoveFocus();
                        }
                    }
                    else {
                        W_ *elm = dynamic_cast<F_&>(*this).getrepresentation(index);
                        if(elm != nullptr) {
                            elm->Focus();
                        }
                        else {
                            elm = dynamic_cast<F_&>(*this).getrepresentation();
                            
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
                    W_ *elm = dynamic_cast<F_&>(*this).getrepresentation(index);
                    
                    if(elm)
                        elm->SetSelected(true);
                }
                
                selectedindex = index;
                ChangedEvent(selectedindex);
            }
            
            void RemoveSelection() {
                SetSelectedIndex(-1);
            }
            
            /// Selects the first item that has the given value. If item does
            /// not exists, this function will remove the selection
            void SetSelection(T_ item) {
                auto &me = dynamic_cast<F_&>(*this);
                
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
                            ChangedEvent(selectedindex);
                        }
                    }
                    else {
                        focusindex = selectedindex;
                    }
                }
                
                focusonly = value;
            }
            
            operator T_() {
                return GetSelectedItem();
            }
            
            operator T_() const {
                return GetSelectedItem();
            }
            
            Event<LBSELTR_Single, long> ChangedEvent = Event<LBSELTR_Single, long>{this};
            
        protected:
            LBSELTR_Single() {
            }
            
            virtual ~LBSELTR_Single() { }
            
            void clicked(long index, W_ &w) {
                if(focusonly) {
                    if(selectedindex == index)
                        return;
                    
                    if(selected)
                        selected->SetSelected(false);
                    
                    w.SetSelected(true);
                    
                    if(dynamic_cast<UI::Widget*>(this)->IsFocused())
                        w.Focus();
                    
                    selected = &w;
                    selectedindex = index;
                    focusindex    = index;
                    
                    ChangedEvent(index);
                }
                else {
                    if(focusindex == index)
                        return;
                    
                    if(dynamic_cast<UI::Widget*>(this)->IsFocused())
                        w.Focus();
                    
                    focusindex = index;
                }
                
                dynamic_cast<UI::Widget*>(this)->Focus();
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
                    if(dynamic_cast<UI::Widget*>(this)->IsFocused())
                        w.Focus();
                    
                    focusindex = index;
                }
                
                ChangedEvent(index);
            }
            
            void apply(long index, W_ &w, const T_ &) {
                if(index == focusindex) {
                    w.Focus();
                }
                else if(w.IsFocused()) {
                    w.Defocus();
                }
                
                if(index == selectedindex) {
                    w.SetSelected(true);
                    selected = &w;
                }
                else {
                    w.SetSelected(false);
                    
                    if(&w == selected)
                        selected = nullptr;
                }
            }
            
            void prepare(W_ &w) {
                w.ClickEvent.Register([&w, this] {
                    clicked(dynamic_cast<F_*>(this)->getindex(w), w);
                });
                w.ToggleEvent.Register([&w, this] {
                    toggled(dynamic_cast<F_*>(this)->getindex(w), w);
                });
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
                        ChangedEvent(-1);
                    }
                    else {
                        focusindex -= count;
                    }
                }
                
                if(index <= selectedindex) {
                    if(index+count > selectedindex) {
                        selectedindex = -1;
                        ChangedEvent(-1);
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
                    ChangedEvent(-1);
                }
                if(w.IsFocused()) {
                    focusindex = -1;
                    w.RemoveFocus();
                }
            }
            
            void reapplyfocus() {
                if(focusindex != -1) {
                    auto w = dynamic_cast<F_*>(this)->getrepresentation(focusindex);
                    if(w)
                        w->Focus();
                }
            }
            
            bool focusonly = true;
            
            long focusindex = -1, selectedindex = -1;
            
            W_ *selected = nullptr;
        };
        
        template<class T_, class W_, class S_, class F_>
        class LBSTR_STLVector {
        public:
            using Storage = S_;
            
            void Add(T_ val) {
                storage.push_back(val);
                
                dynamic_cast<F_*>(this)->Refresh();
            }
            
            template<class... A_>
            void Add(T_ val, A_&& ...rest) {
                storage.push_back(val);
                
                Add(std::forward<A_>(rest)...);
            }
            
            //insert, remove, move
            
            /**
             * @name Iteration 
             * 
             * These functions allow iteration of the listbox. If 
             * the contents are changed through these functions, 
             * you must call Refresh manually.
             * 
             * @{
             */
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
            
            /// @}
        
        protected:
            LBSTR_STLVector() {}
            virtual ~LBSTR_STLVector() {}
            
            long getsize() const {
                return storage.size();
            }
            
            T_ &getelm(long index) {
                return storage[index];
            }
            
            const T_ &getelm(long index) const {
                return storage[index];
            }
            
            Storage storage;
        };
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
     * STR_ is the storage traits for T_. This class should contain typedef
     * Storage which is the storage. It should have functions that allows it
     * to handle Add/Remove/Insert/Move/getsize/getelm functions. Only 
     * getsize and getelm functions are mandatory. getelm should take a long
     * index and should return the stored value. GetSize should return long.
     * Additionally, this function should have a protected storage member
     * named storage. After every modifying operation, this function should
     * call Refresh. Ideally, this class should contain 
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
     * the user uses arrow keys to select an item. It is this classes
     * responsibility to handle actual click and toggle functions.
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
     * reapplyfocus function should apply the focus to the focused element. This
     * function is called after listbox receives focus.
     * 
     * TW_ function should take a T_ and set this to its W_ representation. 
     * 
     * WT_ should read the data from W_ and set it to T_.
     */
    template<
        class T_, class W_, class TR_, class TRF_, 
        class STR_, class SELTR_, 
        bool useisvisible = false,
        void (*TW_)(const T_ &, W_ &) = internal::SetTextUsingFrom<T_, W_>,
        void (*WT_)(W_ &, T_ &)       = internal::GetTextUsingTo  <T_, W_>
    >
    class ListboxBase : public UI::ComponentStackWidget, 
                        public ListBase<T_, W_>, 
                        public TRF_, 
                        public SELTR_, public STR_ 
    {
    public:
        
        /// Returns the item at the given point. This operator will not perform
        /// bounds checking.
        virtual T_ &operator[](long index) override {
            return this->getelm(index);
        }

        /// Returns the item at the given point. This operator will not perform
        /// bounds checking.
        virtual const T_ &operator[](long index) const override {
            return this->getelm(index);
        }
        
        /// Returns the number of elements in the list.
        virtual long GetCount() const override {
            return this->STR_::getsize();
        }
        
        /// For internal use.
        /// Returns the widget used to represent the item at the given index. This
        /// function will return nullptr if the index does not currently have a
        /// visual representation. This is not an edge case, any item that is not
        /// in view will most likely not have a representation.
        virtual W_ *getrepresentation(long index) override {
            return widgetlist.count(index) ? widgetlist[index] : nullptr;
        }
        
        /// For internal use.
        /// Returns the first widget used to represent any item at within the 
        /// listbox. This function will return nullptr if there are no items in the
        /// list.
        virtual W_ *getrepresentation() override {
            return nullptr;
        }
        
        virtual long getindex(const W_ &widget) override {
            return indexes.count(&widget) ? indexes[&widget] : -1;
        }
        
        virtual void Refresh() override {
            if(!contents.IsReady())
                return;

            auto elms = this->STR_::getsize();
            
            std::map<UI::ComponentTemplate::Tag, int> tagcounts;
            
            //remove all first
            for(auto &p : widgetlist) {
                p.second->Remove();
            }
            indexes.clear();
            widgetlist.clear();
            
            auto b = stack.TagBounds(UI::ComponentTemplate::ViewPortTag);
            if(b.Width() == 0 || b.Height() == 0)
                b = stack.TagBounds(UI::ComponentTemplate::ContentsTag);
            
            int y = 0;
            long i = int(scrolloffset), c = 0;
            int totalh = 0;
            while(y < b.Height() && i < elms) {
                auto &v   = this->getelm(i);
                auto  tr  = this->access(i);
                auto  tag = tr.Tag(v, {0, (int)i}, {1, (int)elms});
                
                auto w = getwidget(tag, tagcounts[tag]++);
                
                if(w == nullptr)
                    return;

                TW_(v, *w);
                
                contents.Add(*w);
                tr.Apply(*w, v, {0, (int)i}, {1, (int)elms});
                this->SELTR_::apply(i, *w, v);
                
                if(y == 0) {
                    y = -int(std::round(w->GetHeight() * (scrolloffset - int(scrolloffset))));
                }
                
                indexes.insert({w, i});
                widgetlist.insert({i, w});
                
                w->Move(0, y);
                
                //to be moved to resize
                w->SetWidth(b.Width());
                auto advance = w->GetHeight() + stack.GetTemplate().GetSpacing() / 2;
                y += advance;
                totalh += advance;
                i++;
                c++;
            }
            
            auto scroller = dynamic_cast<VScroller<float>*>(stack.GetWidget(UI::ComponentTemplate::VScrollTag));
            if(scroller) {
                scroller->SetMaximum(elms);
                
                if(totalh == 0)
                    scroller->Range = 0;
                else
                    scroller->Range = b.Height() / (float(totalh) / c);
            }
        }
        
        
    protected:
        ListboxBase(const UI::Template &temp) : 
            ComponentStackWidget(temp, { 
                { UI::ComponentTemplate::ItemTag   , {}},
                { UI::ComponentTemplate::HeaderTag , {}},
                { UI::ComponentTemplate::SpacerTag , {}},
                { UI::ComponentTemplate::VScrollTag, 
                    std::bind(&ListboxBase::createvscroll, this, std::placeholders::_1)
                },
            } )
        {
            stack.HandleMouse();
            stack.SetClickEvent([&](auto, auto, auto) {
                Focus();
            });
            contents.SetLayer(stack.GetLayerOf(stack.IndexOfTag(UI::ComponentTemplate::ContentsTag)));
            stack.AddCondition(UI::ComponentCondition::VScroll);
        }
        
        ~ListboxBase() {
            for(auto &p : widgets) {
                p.second.Destroy();
            }
        }
        
        //this will return the widget with the tag and the order (not index). If the requested
        //widget does not exists, it will be created
        W_ *getwidget(UI::ComponentTemplate::Tag tag, int order) {
            auto temp = stack.GetTemplate(tag);
            
            if(temp == nullptr)
                return nullptr;
            
            auto &ws = widgets[tag];
            
            while(ws.GetSize() <= order) {
                auto &w = ws.AddNew(*temp);
                this->TRF_::prepare(w);
                this->SELTR_::prepare(w);
            }
            
            return &ws[order];
        }
        
        virtual void focused() override {
            ComponentStackWidget::focused();
            this->reapplyfocus();
        }

        virtual void focuslost() override {
            ComponentStackWidget::focuslost();
            contents.RemoveFocus();
        }
        
        UI::Widget *createvscroll(const UI::Template &temp) {
            auto vscroller = new VScroller<float>(temp);
            vscroller->Maximum = this->STR_::getsize();
            vscroller->Range   = 1;
            *vscroller         = scrolloffset;
            //vscroller->SetSmoothChangeSpeed(scrollspeed);
            vscroller->ValueChanged.Register(*this, &ListboxBase::scrollto);
            
            return vscroller;
        }
        
        void scrollto(float target) {
            if(scrolloffset == target)
                return;
            
            scrolloffset = target;
            Refresh();
        }
        
        std::map<UI::ComponentTemplate::Tag, Containers::Collection<W_>> widgets;
        std::map<const W_*, long> indexes;
        std::map<long, W_*> widgetlist;
        UI::LayerAdapter contents;
        float scrolloffset = 0.f;
    };
    
    /**
     * This is a simple listbox. It does not store any additional information
     * about each item, therefore, can store large amounts of items. Items are
     * stored in a std::vector. Only one element can be selected at a time.
     */
    template<class T_>
    class SimpleListbox : 
        public ListboxBase<T_, ListItem, 
            internal::LBTR_blank <T_, ListItem>,
            internal::LBTRF_blank<T_, ListItem, SimpleListbox<T_>>,
            internal::LBSTR_STLVector<T_, ListItem, std::vector<T_>, SimpleListbox<T_>>,
            internal::LBSELTR_Single<T_, ListItem, SimpleListbox<T_>>
        >
    {
        using Base = ListboxBase<T_, ListItem, 
            internal::LBTR_blank <T_, ListItem>,
            internal::LBTRF_blank<T_, ListItem, SimpleListbox<T_>>,
            internal::LBSTR_STLVector<T_, ListItem, std::vector<T_>, SimpleListbox<T_>>,
            internal::LBSELTR_Single<T_, ListItem, SimpleListbox<T_>>
        >;
        
        friend internal::LBTR_blank <T_, ListItem>;
        friend internal::LBTRF_blank<T_, ListItem, SimpleListbox<T_>>;
        friend internal::LBSTR_STLVector<T_, ListItem, std::vector<T_>, SimpleListbox<T_>>;
        friend internal::LBSELTR_Single<T_, ListItem, SimpleListbox<T_>>;
        
    public:
        explicit SimpleListbox(Registry::TemplateType temp = Registry::Listbox_Regular) : Base(Registry::Active()[temp]) {
        }
        
        virtual bool Activate() {
            return false;
        }
    };
    
} }

