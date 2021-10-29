#pragma once


#include "Common.h"
#include "../UI/ComponentStackWidget.h"
#include "../Property.h"
#include "../Input/KeyRepeater.h"
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
        /// Returns the first widget used to represent any item within the 
        /// listbox. This function will return nullptr if there are no items in the
        /// list.
        virtual W_ *getrepresentation() = 0;
        
    };
    
    /// @cond internal
    namespace internal { 
        /// Contains no extra data and does not assume anything about W_
        template <class T_, class W_, class F_>
        class LBTRF_blank {
        protected:
            LBTRF_blank() { }
            ~LBTRF_blank() { }
            
            void apply(long, W_ &, const T_ &, Geometry::Point p, Geometry::Size) { }
            
            UI::ComponentTemplate::Tag tag(long, const T_ &, Geometry::Point, Geometry::Size) {
                return UI::ComponentTemplate::ItemTag;
            }
            
            void prepare(W_ &) { }
            void insert(long, long) { }
            void move(long, long) { }
            void remove(long, long) { }
        }; 
        
        /// Contains no extra data, W_ must be compatible with ListItem
        template <class T_, class W_, class F_>
        class LBTRF_ListItem {
        public:
            
            /// Sets if Odd/even styling should be used. Default is on.
            void SetOddEven(bool value) {
                if(oddeven == value)
                    return;
                
                oddeven = value;
                dynamic_cast<F_*>(this)->Refresh();
            }
            
            /// Returns wether Odd/even styling is in effect
            bool GetOddEven() const {
                return oddeven;
            }
            
        protected:
            LBTRF_ListItem() { }
            virtual ~LBTRF_ListItem() { }
            
            void apply(long, W_ &w, const T_ &, Geometry::Point p, Geometry::Size) { 
                if(oddeven) {
                    w.SetParity(p.Y%2 ? Parity::Odd : Parity::Even);
                }
                else {
                    w.SetParity(Parity::None);
                }
            }
            
            UI::ComponentTemplate::Tag tag(long, const T_ &, Geometry::Point, Geometry::Size) {
                return UI::ComponentTemplate::ItemTag;
            }
            
            void prepare(W_ &) { }
            void insert(long, long) { }
            void move(long, long) { }
            void remove(long, long) { }
            
            bool oddeven = true;
        };
        
        template<class T_, class W_>
        void SetTextUsingFrom(const T_ &val, W_ &w) {
            w.SetText(String::From(val));
        }
        
        template<class T_, class W_, class F_, F_ f>
        void SetTextUsingFn(const T_ &val, W_ &w) {
            w.SetText(f(val));
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
            const T_ &GetSelectedItem() const {
                if(selectedindex == -1)
                    throw std::runtime_error("Nothing is selected.");
                
                return dynamic_cast<const F_&>(*this)[selectedindex];
            }
            
            /// Returns the selected item. If nothing is selected this function
            /// will throw. You may check if there is a selection using 
            /// HasSelectedItem function. Changing the returned value will not
            /// automatically refresh the contents.
            T_ &GetSelectedItem() {
                if(selectedindex == -1)
                    throw std::runtime_error("Nothing is selected.");
                
                return dynamic_cast<F_&>(*this)[selectedindex];
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
                if(index < -1 || index >= dynamic_cast<F_&>(*this).GetCount())
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
                    
                    selected = elm;
                }
                
                selectedindex = index;
                ChangedEvent(selectedindex);
            }

            /// Sets the index of the focused element. If set to -1, nothing is
            /// focused
            void SetFocusIndex(long value) {
                if(focusindex == value)
                    return;
                
                if(focusonly) {
                    SetSelectedIndex(value);
                    return;
                }
                
                if(value != -1) {
                    auto w = dynamic_cast<F_*>(this)->getrepresentation(value);
                    if(w)
                        w->Focus();
                }
                else {
                    auto w = dynamic_cast<F_*>(this)->getrepresentation(focusindex);
                    if(w)
                        w->Defocus();
                }
                
                focusindex = value;
            }
            
            /// Returns the index of the focused element. If nothing is focused,
            /// returns -1.
            long GetFocusIndex() const {
                return focusindex;
            }
            
            void RemoveSelection() {
                SetSelectedIndex(-1);
            }
            
            /// Selects the first item that has the given value. If item does
            /// not exists, this function will remove the selection
            void SetSelection(const T_ &item) {
                auto &me = dynamic_cast<F_&>(*this);
                
                SetSelectedIndex(me.Find(item));
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
            
            operator T_&() {
                return GetSelectedItem();
            }
            
            operator T_&() const {
                return GetSelectedItem();
            }
            
            Event<LBSELTR_Single, long> ChangedEvent = Event<LBSELTR_Single, long>{this};
            
        protected:
            LBSELTR_Single() {
            }
            
            virtual ~LBSELTR_Single() { }
            
            void sel_clicked(long index, W_ &w) {
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
            
            void sel_toggled(long index, W_ &w) {
                if(selectedindex == index || focusonly)
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
            
            void sel_apply(long index, W_ &w, const T_ &) {
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
            
            void sel_prepare(W_ &w) {
                w.ClickEvent.Register([&w, this] {
                    sel_clicked(dynamic_cast<F_*>(this)->getindex(w), w);
                });
                w.ToggleEvent.Register([&w, this] {
                    sel_toggled(dynamic_cast<F_*>(this)->getindex(w), w);
                });
            }
            
            void sel_insert(long index, long count) { 
                if(index <= focusindex)
                    focusindex += count;
                
                if(index <= selectedindex)
                    selectedindex += count;
            }
            
            void sel_move(long index, long target) { 
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
            
            void sel_remove(long index, long count) { 
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
                        ChangedEvent(-1);
                    }
                    else {
                        selectedindex -= count;
                    }
                }
            }
            
            void sel_destroy(W_ &w) {
                if(&w == selected) {
                    selected = nullptr;
                }
                if(w.IsFocused()) {
                    focusindex = -1;
                    w.Defocus();
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
        
        //This class allows single selection. The selected item will
        //follow the focus by default. If desired, this can be changed
        template<class T_, class W_, class F_>
        class LBSELTR_Multi {
            template <class F1_> friend class ItemIterator_;
            friend class SelectionHelper;

            class SelectionIndexHelper {
            public:
                SelectionIndexHelper(const std::vector<long> &v) : v(v) {
                }
                
                auto begin() {
                    return v.begin();
                }
                
                auto end() {
                    return v.end();
                }
                
            private:
                const std::vector<long> &v;
            };

            class SelectionHelper {
            public:
                SelectionHelper(LBSELTR_Multi &l) : l(l) {
                }

                auto begin() {
                    auto &me = dynamic_cast<F_ &>(l);
                    return ItemIterator(me, l.selected, 0);
                }

                auto end() {
                    auto &me = dynamic_cast<F_ &>(l);
                    return ItemIterator(me, l.selected, (long)l.selected.size());
                }

                auto begin() const {
                    auto &me = dynamic_cast<F_ &>(l);
                    return ConstItemIterator(me, l.selected, 0);
                }

                auto end() const {
                    auto &me = dynamic_cast<F_ &>(l);
                    return ConstItemIterator(me, l.selected, (long)l.selected.size());
                }

            private:
                LBSELTR_Multi &l;
            };


            template <class O_, class F1_>
            class ItemIterator_ : public Containers::Iterator<ItemIterator_<O_, F1_>, O_> {
                friend class SelectionHelper;
                friend class ConstItemIterator;
                friend class Containers::Iterator<ItemIterator_<O_, F1_>, O_>;
            public:
                /// Default constructor, creates an iterator pointing to invalid location
                ItemIterator_(): list(NULL), offset(-1) {
                }
                /// Copies another iterator
                ItemIterator_(const ItemIterator_ &it): list(it.list), offset(it.offset) {
                }

            protected:
                ItemIterator_(F1_ &list, const std::vector<long> &inds, long offset = 0): list(&list), offset(offset), inds(&inds) {
                }

                /// Satisfies the needs of Iterator
                O_ &current() const {
                    if(!isvalid())
                        throw std::out_of_range("Iterator is not valid.");

                    return (*list)[(*inds)[offset]];
                }

                bool isvalid() const {
                    return list && inds && offset>=0 && offset<(inds->size());
                }

                bool moveby(long amount) {
                    //sanity check
                    if(amount==0)  return isvalid();

                    offset += amount;

                    return isvalid();
                }

                bool compare(const ItemIterator_ &it) const {
                    if(!it.isvalid() && !isvalid()) return true;
                    return it.offset==offset;
                }

                void set(const ItemIterator_ &it) {
                    list = it.list;
                    offset = it.offset;
                }

                long distance(const ItemIterator_ &it) const {
                    return it.offset-offset;
                }

                bool isbefore(const ItemIterator_ &it) const {
                    return offset<it.offset;
                }

                F1_ *list = nullptr;
                long offset = 0;
                const std::vector<long> *inds = nullptr;
            };
        public:
            /// Regular iterator. @see Container::Iterator
            typedef ItemIterator_<T_, F_> ItemIterator;

            /// Const iterator allows iteration of const collections
            class ConstItemIterator : public ItemIterator_<const T_, const F_> {
                friend class SelectionHelper;
                friend class Containers::Iterator<ItemIterator_<const T_, const F_>, T_>;

            public:
                ///Regular iterators can be converted to const iterators
                ConstItemIterator(const ItemIterator &it) {
                    this->list = it.list;
                    this->offset = it.offset;
                }

            private:
                ConstItemIterator(const F_ &c, const std::vector<long> &inds, long offset = 0): ItemIterator_<const T_, const F_>(c, inds, offset) {
                }
            };

            enum SelectionMethod {
                /// Clicking on the item will toggle its selected state
                Toggle,

                /// Clicking on the item will set the selection to be
                /// that item only. Ctrl clicking toggles
                UseCtrl,
            };
            
            enum EventMethod {
                /// Event will be fired only once per action
                Once,
                
                /// Even will be fired per changed item
                ForEachItem
            };
            
            /// Changes selection method. Default method is toggle. In toggle
            /// method, clicking on an item will toggle its state without
            /// effecting other items. In UseCtrl method, if an item is clicked
            /// with Ctrl key pressed, it will be toggled. If Ctrl is not pressed,
            /// it will become the only selected item. 
            void SetSelectionMethod(SelectionMethod value) {
                method = value;
            }
            
            /// Returns the selection method.
            SelectionMethod GetSelectionMethod() const {
                return method;
            }
            
            /// Changes event method. Default method is ForEachItem. In ForEachItem
            /// method, ChangedEvent will be fired for each item that is affected.
            /// index and status parameters will be set. If event method is set to
            /// Once, ChangedEvent will be fired once per action. index and status
            /// parameter will be set only if one item is affected. Otherwise,
            /// index will be set to -1 and status is set to false.
            void SetEventMethod(EventMethod value) {
                event = value;
            }
            
            /// Returns the event method.
            EventMethod GetEventMethod() const {
                return event;
            }
            
            /// Returns the index of the focused element. If nothing is focused,
            /// returns -1.
            long GetFocusIndex() const {
                return focusindex;
            }
            
            /// Sets the index of the focused element. If set to -1, nothing is
            /// focused
            void SetFocusIndex(long value) {
                if(focusindex == value)
                    return;
                
                if(value != -1) {
                    auto w = dynamic_cast<F_*>(this)->getrepresentation(value);
                    if(w)
                        w->Focus();
                }
                else {
                    auto w = dynamic_cast<F_*>(this)->getrepresentation(focusindex);
                    if(w)
                        w->Defocus();
                }
                
                focusindex = value;
            }
            
            /// Removes all items from the selection
            void ClearSelection() {
                if(event == Once) {
                    selected.clear();
                    ChangedEvent(-1, false);
                }
                else {
                    std::vector<long> old;
                    std::swap(old, selected);
                    
                    for(auto ind : old) {
                        ChangedEvent(ind, false);
                    }
                }
                
                dynamic_cast<F_*>(this)->Refresh();
            }
            
            /// Replaces the selection by the given index
            void SetSelection(long ind) {
                SetSelection(std::vector<long>{ind});
            };
            
            /// Replaces the selection by the given indices
            void SetSelection(std::vector<long> indices) {
                if(event == ForEachItem)
                    ClearSelection();
                
                selected = std::move(indices);
                std::sort(selected.begin(), selected.end());
                
                //ensure nothing is duplicated
                selected.erase(std::unique(selected.begin(), selected.end()), selected.end());
                
                if(event == ForEachItem) {
                    for(auto ind : selected) {
                        ChangedEvent(ind, true);
                    }
                }
                else {
                    ChangedEvent(-1, false);
                }
                
                dynamic_cast<F_*>(this)->Refresh();
            };
            
            /// Replaces the selection by the given indices
            template<class ...P_>
            void SetSelection(P_&&... elms) {
                SetSelection({elms...});
            }
            
            /// Adds the given index to the selection
            void AddToSelection(long ind) {
                AddToSelection(std::vector<long>{ind});
            };
            
            /// Adds the given indices to the selection
            void AddToSelection(std::vector<long> indices) {
                std::sort(indices.begin(), indices.end());
                
                selected.resize(selected.size() + indices.size());
                auto sel = selected.rbegin();
                auto cur = selected.rbegin() + indices.size();
                auto ind = indices.rbegin();
                
                while(ind != indices.rend()) {
                    if(cur == selected.rend() || *cur < *ind) {
                        *sel = *ind;
                        
                        if(event == ForEachItem)
                            ChangedEvent(*ind, true);
                        
                        ++ind;
                    }
                    else {
                        *sel = *cur;
                        ++cur;
                    }
                    ++sel;
                }
                
                //ensure nothing is duplicated
                selected.erase(std::unique(selected.begin(), selected.end()), selected.end());
                
                if(event == Once) {
                    ChangedEvent(-1, false);
                }
                
                dynamic_cast<F_*>(this)->Refresh();
            };
            
            /// Adds the given indices to the selection
            template<class ...P_>
            void AddToSelection(P_&&... elms) {
                AddToSelection({elms...});
            }
            
            /// Returns if the item in the given index is selected.
            bool IsSelected(long index) const {
                return std::binary_search(selected.begin(), selected.end(), index);
            }
            
            /// Returns how many items are selected
            long GetSelectionCount() const {
                return long(selected.size());
            }
            
            /// Removes the given index from the selection
            void RemoveFromSelection(long index) {
                auto item = std::lower_bound(selected.begin(), selected.end(), index);
                
                if(item != selected.end() && *item == index) {
                    selected.erase(item);
                    ChangedEvent(index, false);
                }
                
                dynamic_cast<F_*>(this)->Refresh();
            }
            
            /// Selects all items
            void SelectAll() {
                auto elms = dynamic_cast<F_*>(this)->GetCount();
                
                std::vector<long> old;
                old.reserve(elms);
                
                std::swap(old, selected);
                
                auto sel = old.begin();
                for(int i=0; i<elms; i++) {
                    if(sel != old.end() && *sel == i)
                        ++sel;
                    else if(event == ForEachItem)
                        ChangedEvent(i, true);
                        
                    selected.push_back(i);
                }
                
                if(event == Once)
                    ChangedEvent(-1, false);
                
                
                dynamic_cast<F_*>(this)->Refresh();
            }
            
            /// Inverts the selection.
            void InvertSelection() {
                std::vector<long> old;
                
                auto elms = dynamic_cast<F_*>(this)->GetCount();
                old.reserve(elms-selected.size());
                
                std::swap(selected, old);
                
                auto sel = old.begin();
                for(int i=0; i<elms; i++) {
                    if(sel != old.end() && *sel == i) {
                        ++sel;
                        
                        if(event == ForEachItem)
                            ChangedEvent(i, false);
                    }
                    else {
                        selected.push_back(i);
                        
                        if(event == ForEachItem)
                            ChangedEvent(i, true);
                    }
                }
                
                if(event == Once)
                    ChangedEvent(-1, false);
                
                dynamic_cast<F_*>(this)->Refresh();
            }
            
            /// Allows iteration of selected indices
            SelectionIndexHelper SelectedIndices;
            
            /// Allows iteration of selected items. If the iterated items is
            /// changed, list will not be automatically updated.
            SelectionHelper Selection;
            
            Event<LBSELTR_Multi, long, bool> ChangedEvent = Event<LBSELTR_Multi, long, bool>{this};
            
        protected:
            LBSELTR_Multi() : 
                SelectedIndices(selected),
                Selection(*this)
            {
            }
            
            virtual ~LBSELTR_Multi() { }
            
            void sel_clicked(long index, W_ &w) {
                if(focusindex == index)
                    return;
                
                if(dynamic_cast<UI::Widget*>(this)->IsFocused())
                    w.Focus();
                
                focusindex = index;
                
                dynamic_cast<UI::Widget*>(this)->Focus();
            }
            
            void sel_toggled(long index, W_ &w, bool forcetoggle = true) {
                if(method == Toggle || forcetoggle) {
                    auto item = std::lower_bound(selected.begin(), selected.end(), index);
                    
                    if(item != selected.end() && *item == index) {
                        w.SetSelected(false);
                        selected.erase(item);
                        
                        ChangedEvent(index, false);
                    }
                    else {
                        w.SetSelected(true);
                        selected.insert(item, index);
                        
                        ChangedEvent(index, true);
                    }
                }
                else {
                    if(Input::Keyboard::CurrentModifier == Input::Keyboard::Modifier::Ctrl) {
                        auto item = std::lower_bound(selected.begin(), selected.end(), index);
                        
                        if(item != selected.end() && *item == index) {
                            w.SetSelected(false);
                            selected.erase(item);
                            
                            ChangedEvent(index, false);
                        }
                        else {
                            w.SetSelected(true);
                            selected.insert(item, index);
                            
                            ChangedEvent(index, true);
                        }
                    }
                    else {
                        SetSelection(index);
                    }
                }
            }
            
            void sel_apply(long index, W_ &w, const T_ &) {
                if(index == focusindex) {
                    w.Focus();
                }
                else if(w.IsFocused()) {
                    w.Defocus();
                }
                
                if(std::binary_search(selected.begin(), selected.end(), index)) {
                    w.SetSelected(true);
                }
                else {
                    w.SetSelected(false);
                }
            }
            
            void sel_prepare(W_ &w) {
                w.ClickEvent.Register([&w, this] {
                    sel_clicked(dynamic_cast<F_*>(this)->getindex(w), w);
                });
                w.ToggleEvent.Register([&w, this] {
                    sel_toggled(dynamic_cast<F_*>(this)->getindex(w), w, false);
                });
            }
            
            void sel_insert(long index, long count) { 
                if(index <= focusindex)
                    focusindex += count;
                
                auto item = std::lower_bound(selected.begin(), selected.end(), index);
                for(; item != selected.end(); ++item) {
                    *item += count;
                }
            }
            
            void sel_move(long index, long target) { 
                if(index == target) 
                    return;
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
                
                if(index < target) {
                    auto from = std::lower_bound(selected.begin(), selected.end(), index);
                    auto to = std::lower_bound(selected.begin(), selected.end(), target+1);
                
                    auto shifted = from;
                    
                    if(*from == index)
                        ++shifted;
                    
                    while(from < to) {
                        *from = *shifted - 1;
                        
                        ++from;
                        ++shifted;
                    }
                    
                    if(shifted != from) {
                        *shifted = target;
                    }
                }
                else {
                    auto from = std::lower_bound(selected.begin(), selected.end(), index-1);
                    auto to = std::upper_bound(selected.begin(), selected.end(), target);
                
                    auto shifted = from;
                    
                    if(*from == index)
                        --shifted;
                    
                    while(from > to) {
                        *from = *shifted + 1;
                        
                        --from;
                        --shifted;
                    }
                    
                    if(shifted != from) {
                        *shifted = target;
                    }
                }
            }
            
            void sel_remove(long index, long count) { 
                //removed items will be repurposed using apply,
                if(index <= focusindex) {
                    if(index+count > focusindex) {
                        focusindex = -1;
                    }
                    else {
                        focusindex -= count;
                    }
                }
                
                //move and update items that are after the point of removal
                auto item = std::lower_bound(selected.begin(), selected.end(), index);
                auto shifted = item;
                while(shifted != selected.end()) {
                    if(*item < index+count) {
                        shifted++;
                        continue;
                    }
                    
                    *item = *shifted - count;
                    
                    ++shifted;
                    ++item;
                }
                
                //remove the items to be removed, they are already moved to the end
                if(item != selected.end())
                    selected.erase(item, selected.end());
            }
            
            void sel_destroy(W_ &w) {
                if(w.IsFocused()) {
                    focusindex = -1;
                    w.Defocus();
                }
            }
            
            void reapplyfocus() {
                if(focusindex != -1) {
                    auto w = dynamic_cast<F_*>(this)->getrepresentation(focusindex);
                    if(w)
                        w->Focus();
                }
            }
            
            long focusindex = -1;
            std::vector<long> selected; //this list will be kept sorted
            SelectionMethod method = Toggle;
            EventMethod event      = ForEachItem;
        };
        
        template<class T_, class W_, class S_, class F_>
        class LBSTR_STLVector {
        public:
            using Storage = S_;
            
            /// Adds the given item to the listbox
            void Add(T_ val) {
                storage.push_back(val);
                
                dynamic_cast<F_*>(this)->Refresh();
            }
            
            /// Adds the given items to the listbox
            template<class... A_>
            void Add(T_ val, A_&& ...rest) {
                storage.push_back(val);
                
                Add(std::forward<A_>(rest)...);
            }
            
            /// Inserts the given item before the given location. You may use
            /// Find to find the location of a specific item. If location is
            /// -1, then item is added at the end.
            void Insert(long before, T_ val) {
                if(before == -1) {
                    Add(val);
                    return;
                }
                
                storage.insert(storage.begin()+before, val);
                
                dynamic_cast<F_*>(this)->sel_insert(before, 1);
                dynamic_cast<F_*>(this)->Refresh();
            }
            
            /// Inserts the given items before the given location. You may use
            /// Find to find the location of a specific item. If location is
            /// -1, then items are added at the end.
            template<class... A_>
            void Insert(long before, T_ val, A_&& ...rest) {
                if(before == -1) {
                    Add(val, rest...);
                    return;
                }
                
                //TODO optimize
                Insert(std::forward<A_>(rest)...);
                
                storage.insert(storage.begin()+before, val);
                dynamic_cast<F_*>(this)->sel_insert(before, 1);
            }
            
            /// Removes the item at the given index.
            void Remove(long index) {
                storage.erase(storage.begin()+index);
                
                dynamic_cast<F_*>(this)->sel_remove(index, 1);
                dynamic_cast<F_*>(this)->Refresh();
            }
            
            /// Moves the item at the given index before the given location.
            /// If before is -1 or equal or larger than the number of items, 
            /// then the item is moved to the end.
            void MoveBefore(long index, long before) {
                if(index==before)
                    return;

                if(before >= storage.size() || before == -1) {
                    auto t = storage[index];

                    for(unsigned i=index; i<storage.size()-1; i++)
                        storage[i] = storage[i+1];

                    storage[storage.size()-1] = t;
                }
                else if(index>before) {
                    auto t = storage[index];
                    for(unsigned i=index; i>before; i--)
                        storage[i] = storage[i-1];

                    storage[before]=t;
                }
                else {
                    auto t = storage[index];
                    for(unsigned i=index; i<before; i++)
                        storage[i] = storage[i+1];

                    storage[before] = t;
                }
                
                dynamic_cast<F_*>(this)->sel_move(index, before);
                dynamic_cast<F_*>(this)->Refresh();
            }
            
            /// Return the index of the first item that has the given value.
            /// Returns -1 if item not found.
            long Find(const T_ &item, long start = 0) {
                auto it = std::find(storage.begin() + start, storage.end(), item);
                if(it == storage.end())
                    return -1;
                else
                    return it-storage.begin();
            }
            
            /// Allocates memory for the given amount of items
            void Reserve(long amount) {
                storage.reserve(amount);
            }
            
            /// Clears all items from the listbox
            void Clear() {
                long len = long(storage.size());
                storage.clear();
                
                dynamic_cast<F_*>(this)->sel_remove(0, len);
                
                dynamic_cast<F_*>(this)->Refresh();
            }
            
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
                return (long)storage.size();
            }
            
            T_ &getelm(long index) {
                return storage[index];
            }
            
            const T_ &getelm(long index) const {
                return storage[index];
            }
            
            Storage storage;
        };
        
        template<class T_, class W_, class F_>
        class LBSTR_Collection {
        public:
            using Storage = Containers::Collection<T_>;
            
            /// Adds the given item to the list 
            void Add(T_ &val) {
                storage.Push(val);
                
                dynamic_cast<F_*>(this)->Refresh();
            }
            
            template<class... A_>
            T_ &AddNew(A_&& ...args) {
                auto &t = storage.AddNew(args...);
                
                dynamic_cast<F_*>(this)->Refresh();
                
                return t;
            }
            
            template<class... A_>
            void Add(T_ &val, A_& ...rest) {
                storage.Push(val);
                
                Add(std::forward<A_>(rest)...);
            }
            
            void Insert(long before, T_ &val) {
                if(storage.Insert(before, val)) {
                    dynamic_cast<F_*>(this)->sel_insert(before, 1);
                    dynamic_cast<F_*>(this)->Refresh();
                }
            }
            
            void Insert(const T_ &before, T_ &val) {
                Insert(FindLocation(before), val);
            }
            
            template<class... A_>
            T_ &InsertNew(const T_ &before, A_&& ...args) {
                auto &t = storage.InsertNew(FindLocation(before), args...);
                
                dynamic_cast<F_*>(this)->sel_insert(before, 1);
                dynamic_cast<F_*>(this)->Refresh();
                
                return t;
            }
            
            void Remove(long index) {
                storage.Remove(index);
                
                dynamic_cast<F_*>(this)->sel_remove(index, 1);
                dynamic_cast<F_*>(this)->Refresh();
            }
            
            void Remove(const T_ &item) {
                long index = storage.FindLocation(item);
                if(index == -1)
                    return;
                
                storage.Remove(index);
                
                dynamic_cast<F_*>(this)->sel_remove(index, 1);
                dynamic_cast<F_*>(this)->Refresh();
            }
            
            /// Deletes an item from the listbox. Deleting both removes the item from the list and 
            /// free the item itself. If given item does not exists, this function deletes the item
            /// and does nothing else
            void Delete(const T_ &item) {
                long index = storage.FindLocation(item);
                if(index == -1) {
                    delete &item;
                    
                    return;
                }
                
                storage.Delete(index);
                
                dynamic_cast<F_*>(this)->sel_remove(index, 1);
                dynamic_cast<F_*>(this)->Refresh();
            }
            
            /// Deletes the item with the given index
            void Delete(long index) {
                storage.Delete(index);
                
                dynamic_cast<F_*>(this)->sel_remove(index, 1);
                dynamic_cast<F_*>(this)->Refresh();
            }
            
            void MoveBefore(long index, long before) {
                if(index==before)
                    return;
                
                storage.MoveBefore(index, before);
                
                dynamic_cast<F_*>(this)->sel_move(index, before);
                dynamic_cast<F_*>(this)->Refresh();
            }
            
            /// Return the location of the given item.
            /// Returns -1 if item not found.
            long Find(const T_ &item) {
                return storage.FindLocation(item);
            }
            
            /// Removes all elements without destroying them
            void Clear() {
                auto s = storage.GetSize();
                
                storage.Clear();
                
                dynamic_cast<F_*>(this)->sel_remove(0, s);
                dynamic_cast<F_*>(this)->Refresh();
            }
            
            /// Deletes and removes all elements in the listbox
            void DeleteAll() {
                auto s = storage.GetSize();
                
                storage.DeleteAll();
                
                dynamic_cast<F_*>(this)->sel_remove(0, s);
                dynamic_cast<F_*>(this)->Refresh();
            }
            
            /// Deletes and removes all elements in the listbox. This function
            /// also clears the memory associated with the storage.
            void Destroy() {
                auto s = storage.GetSize();
                
                storage.Destroy();
                
                dynamic_cast<F_*>(this)->sel_remove(0, s);
                dynamic_cast<F_*>(this)->Refresh();
            }
            
            /// Allocates memory for the given amount of items
            void Reserve(long amount) {
                storage.Reserve(amount);
            }
            
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
            LBSTR_Collection() {}
            virtual ~LBSTR_Collection() {}
            
            long getsize() const {
                return storage.GetSize();
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
     * This class should support sel_click and sel_toggle functions both taking
     * long index, const T_ & value, W_ &widget. Click function will be called 
     * when the user uses arrow keys to select an item. It is this classes 
     * responsibility to handle actual click and toggle functions.
     * apply function should apply selection related traits to the widget taking
     * long index, W_ &widget, const T_ & value. Any item that is benched will
     * be applied to as well with an index of -1.
     * This class should contain `insert(before, count)` to add count elements
     * before the given index. These elements must be default initialized to a
     * non-selected normal state. 
     * `sel_move(index, before)` should move the indexed item before the given 
     * index. 
     * `sel_remove(index, count)` should remove count number of items 
     * starting from the given index. 
     * `sel_prepare(W_ &)` function should prepare the given widget for the first
     * usage. This is only called once on a newly created widget.
     * `sel_destroy(W_ &)` function is called when a widget is about to be 
     * destroyed.
     * Index parameters should be long int. These functions should be protected
     * as ListboxBase will be derived from SELTR_. Finally, SELTR_ should have a 
     * protected constructor/destructor.
     * reapplyfocus function should apply the focus to the focused element. This
     * function is called after listbox receives focus.
     * GetFocusIndex and SetFocusIndex functions should exist to handle focus
     * changes through keyboard keys.
     * 
     * TW_ function should take a T_ and set this to its W_ representation. 
     */
    template<
        class T_, class W_, class TRF_, 
        class STR_, class SELTR_, 
        bool useisvisible,
        void (*TW_)(const T_ &, W_ &)
    >
    class ListboxBase : 
        public ListBase<T_, W_>, 
        public TRF_, 
        public SELTR_, 
        public STR_ 
    {
    public:
        using SelectionType = SELTR_;
        using TypeTraits    = TRF_;
        using StorageType   = STR_;
        
        /// Returns the item at the given point. This operator will not perform
        /// bounds checking. Changing the returned value will not automatically
        /// refresh the contents.
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
        
        

    protected:
        ListboxBase()
        { }
    };
    
    template<
        class T_, class W_, class TRF_, 
        class STR_, class SELTR_, 
        bool useisvisible,
        void (*TW_)(const T_ &, W_ &)
    >
    class ListboxWidgetBase : 
        public UI::ComponentStackWidget,
        public ListboxBase<T_, W_, TRF_, STR_, SELTR_, useisvisible, TW_> 
    {
    public:
        using ListBase = ListboxBase<T_, W_, TRF_, STR_, SELTR_, useisvisible, TW_>;
        
        virtual void Refresh() override {
            if(!contents.IsReady())
                return;

            auto elms = this->STR_::getsize();
            
            std::map<UI::ComponentTemplate::Tag, int> tagcounts;
            
            //TODO improve performance by rolling
            
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
            if(i < 0)
                i = 0;
            while(y < b.Height() && i < elms) {
                auto &v   = this->getelm(i);
                auto  tag = this->tag(i, v, {0, (int)i}, {1, (int)elms});
                
                auto w = getwidget(tag, tagcounts[tag]++);
                
                if(w == nullptr)
                    return;

                TW_(v, *w);
                
                contents.Add(*w);
                this->apply(i, *w, v, {0, (int)i}, {1, (int)elms});
                this->sel_apply(i, *w, v);
                
                if(y == 0) {
                    y = -int(std::round(w->GetCurrentHeight() * (scrolloffset - int(scrolloffset))));
                }
                
                indexes.insert({w, i});
                widgetlist.insert({i, w});
                
                w->Move(Pixels(0, y));
                
                //to be moved to resize
                w->SetWidth(Pixels(b.Width()));
                auto advance = w->GetCurrentHeight() + stack.GetTemplate().GetSpacing() / 2;
                y += advance;
                totalh += advance;
                i++;
                c++;
            }
                
            if(totalh == 0)
                maxdisplay = 0;
            else
                maxdisplay = b.Height() / (float(totalh) / c);
            
            if(maxdisplay < elms+overscroll && !stack.HasCondition(UI::ComponentCondition::VScroll)) {
                stack.AddCondition(UI::ComponentCondition::VScroll);
                stack.Update(true);
            }
            
            auto scroller = dynamic_cast<VScroller<float>*>(stack.GetWidget(UI::ComponentTemplate::VScrollTag));
            if(scroller) {
                if(elms == 0)
                    scroller->SetMaximum(0);
                else
                    scroller->SetMaximum(elms + overscroll);
                
                scroller->Range = maxdisplay;
                scroller->LargeChange = maxdisplay * 0.8f;
                
                if(scroller->Range >= scroller->Maximum) {
                    stack.RemoveCondition(UI::ComponentCondition::VScroll);
                }
            }
            
            contents.Displaced();
        }

        virtual void SetVisible(bool value) override {
            ComponentStackWidget::SetVisible(value);
            contents.Displaced();
        }
        
        /// Scrolls the contents of the listbox so that the it will start displaying
        /// items from the given location in list items. This function takes columns
        /// into account.
        void ScrollTo(float y) { 
            float max = this->STR_::getsize() + overscroll - maxdisplay;
            
            if(y < 0)
                y = 0;
            
            if(y > max)
                y = max;
            
            if(target == y)
                return;
            
            target = y;
            
            if(scrollspeed == 0) {
                scrolloffset = y;
                Refresh();
            }
            else {
                if(!isscrolling) {
                    stack.SetFrameEvent(std::bind(&ListboxWidgetBase::updatescroll, this));
                    isscrolling = true;
                }
            }
            
            auto scroller = dynamic_cast<VScroller<float>*>(stack.GetWidget(UI::ComponentTemplate::VScrollTag));
            if(scroller)
                *scroller = target;
            
            Refresh();
        }
        
        /// Scrolls the contents an additional amount of items.
        void ScrollBy(float y) {
            ScrollTo(ScrollOffset() + y);
        }
        
        /// Returns the current scroll offset.
        float ScrollOffset() const {
            return scrolloffset;
        }
        
        /// Sets the amount of extra scrolling distance after the bottom-most
        /// widget is completely visible in pixels. Default is 0. Does not
        /// apply if everything is visible.
        void SetOverscroll(float value) {
            if(overscroll == value)
                return;
            
            overscroll = value;
            
            auto scroller = dynamic_cast<VScroller<float>*>(stack.GetWidget(UI::ComponentTemplate::VScrollTag));
            if(scroller) {
                if(this->STR_::getsize() == 0)
                    scroller->SetMaximum(0);
                else
                    scroller->SetMaximum(this->STR_::getsize() + overscroll);
            }
        }
        
        /// Returns the amount of extra scrolling distance after the bottom-most
        /// widget is completely visible in pixels.
        float GetOverscroll() const {
            return overscroll;
        }
        
        /// Sets the scroll distance per click in pixels. Default depends
        /// on the default size of the listbox.
        void SetScrollDistance(int value) {
            scrolldist = value;
        }
        
        /// Returns the scroll distance per click
        int GetScrollDistance() const {
            return scrolldist;
        }
        
        /// Disables smooth scrolling of the panel
        void DisableSmoothScroll() {
            SetSmoothScrollSpeed(0);
        }
        
        /// Adjusts the smooth scrolling speed of the listbox. Given value is
        /// in items per second, default value is 20.
        void SetSmoothScrollSpeed(int value) {
            scrollspeed = value;
            
            auto scroller = dynamic_cast<VScroller<float>*>(stack.GetWidget(UI::ComponentTemplate::VScrollTag));
            if(scroller)
                scroller->SetSmoothChangeSpeed(scrollspeed);
            
            auto elms = this->STR_::getsize();
            
            
            stack.SetValueTransitionSpeed({elms > 0 ? float(value)/elms : 0, 0, 0, 0});
        }
        
        /// Returns the smooth scrolling speed of the listbox. If smooth scroll
        /// is disabled, this value will be 0.
        int GetSmoothScrollSpeed() const {
            return scrollspeed;
        }
        
        /// Returns if the smooth scroll is enabled.
        bool IsSmoothScrollEnabled() const {
            return scrollspeed != 0;
        }
        
        /// Sets the the duration that scrolling can take. This speeds up scrolling
        /// if the distance is too much. This value is not exact and scrolling will
        /// slow down as it gets close to the target. However, total scroll duration 
        /// cannot exceed twice this value. The time is in milliseconds and default 
        /// value is 500. 
        void SetMaximumScrollDuration(int value) {
            maxscrolltime = value;
        }
        
        /// Returns how long a scrolling operation can take.
        int GetMaximumScrollDuration() const {
            return maxscrolltime;
        }
        
        /// This may not be a perfect number
        float GetMaximumDisplayedElements() const {
            return maxdisplay;
        }
        
        Geometry::Size GetInteriorSize() const {
            auto b = stack.TagBounds(UI::ComponentTemplate::ViewPortTag);
            if(b.Width() == 0 || b.Height() == 0)
                b = stack.TagBounds(UI::ComponentTemplate::ContentsTag);
            
            return b.GetSize();
        }
        
        using UI::Widget::EnsureVisible;
        
        /// Ensures the given index is completely visible on the screen.
        void EnsureVisible(long index) {
            float targetind = float(index);
            
            auto md = maxdisplay;
            
            if(targetind >= target && targetind < floor(target + md)) {
                return; //already visible
            }
            else if(targetind >= floor(target + md)) {
                targetind -= md - 1.5f; //show the next item a little bit
            }
            else {
                targetind -= 0.5f; //show the next item a little bit
            }
            
            auto elms = this->GetCount();
            
            if(targetind < 0)
                targetind = 0;
            if(targetind > overscroll + elms)
                targetind = overscroll + elms;
            
            ScrollTo(targetind);
            
            //if max display changes, call ensure visible again
            if(md != maxdisplay)
                EnsureVisible(index);
        }
        
        bool KeyPressed (Input::Key key, float state) override {
            namespace Keycodes = Input::Keyboard::Keycodes;
            
            if(repeater.KeyEvent(key, state))
                return true;
            
            if(key == Keycodes::Space) {
                if(contents.HasFocusedWidget() && state != 0) {
                    auto w = dynamic_cast<W_*>(&contents.GetFocus());
                    if(w) {
                        this->sel_toggled(indexes.count(w) ? indexes[w] : -1, *w);
                    }
                }
                
                return true;
            }

            return false;
        }

        /// This function will try to set the height of the listbox to contain
        /// all its elements. If the size necessary to do so is larger than
        /// maxsize, then maxsize will be used and this function will return
        /// false. This function may give up before fully exceeding maxsize if
        /// elements have different heights. But it will never surpass maxsize.
        bool FitHeight(const UI::UnitDimension &maxsize) {
            return fitheight(Convert(maxsize, true, true));
        }

        /// This function will try to set the height of the listbox to contain
        /// all its elements. If the size necessary to do so is larger than
        /// maxsize, then maxsize will be used and this function will return
        /// false. This function may give up before fully exceeding maxsize if
        /// elements have different heights. But it will never surpass maxsize.
        bool FitHeight(float elms) {
            if(widgetlist.empty())
                return fitheight(Convert(Units(elms), true, true));
            else {
                int sp = stack.GetTemplate().GetSpacing() / 2;
                int border = 0;

                auto innersize = stack.TagBounds(UI::ComponentTemplate::ViewPortTag).GetSize();
                border = GetCurrentSize().Height - innersize.Height;
                if(border < 0)
                    border = 0;

                return fitheight((int)std::round((widgetlist.begin()->second->GetCurrentHeight() + sp)*elms - sp + border));
            }
        }

    protected:
        ListboxWidgetBase(const UI::Template &temp) : 
            ComponentStackWidget(temp, { 
                { UI::ComponentTemplate::ItemTag   , {}},
                { UI::ComponentTemplate::HeaderTag , {}},
                { UI::ComponentTemplate::SpacerTag , {}},
                { UI::ComponentTemplate::VScrollTag, 
                    std::bind(&ListboxWidgetBase::createvscroll, this, std::placeholders::_1)
                }
            })
        {
            stack.HandleMouse();
            
            stack.SetClickEvent([&](auto, auto, auto) {
                Focus();
            });
            
            stack.SetOtherMouseEvent([this](UI::ComponentTemplate::Tag tag, Input::Mouse::EventType type, Geometry::Point, float amount) {
                if(type == Input::Mouse::EventType::Scroll_Vert && stack.HasCondition(UI::ComponentCondition::VScroll)) {
                    ScrollBy(-amount*scrolldist);
                    
                    return true;
                }
                
                return false;
            });
            
            contents.SetLayer(stack.GetLayerOf(stack.IndexOfTag(UI::ComponentTemplate::ContentsTag)));
            stack.AddCondition(UI::ComponentCondition::VScroll);
            
            repeater.Register(Input::Keyboard::Keycodes::Up);
            repeater.Register(Input::Keyboard::Keycodes::Down);
            repeater.Register(Input::Keyboard::Keycodes::PageUp);
            repeater.Register(Input::Keyboard::Keycodes::PageDown);
            
            
            repeater.Repeat.Register([this](Input::Key key) {
                namespace Keycodes = Input::Keyboard::Keycodes;
                using Input::Keyboard::Modifier;
                
                long org = this->GetFocusIndex();
                long ind = org;
                
                switch(key) {
                case Keycodes::Down:
                    ind++;
                    break;
                    
                case Keycodes::Up:
                    ind--;
                    break;
                    
                case Keycodes::PageDown:
                    ind += long(floor(maxdisplay));
                    break;
                    
                case Keycodes::PageUp:
                    ind -= long(floor(maxdisplay));
                    break;
                }
                
                if(ind < 0)
                    ind = 0;
                if(ind >= this->GetCount())
                    ind = this->GetCount() - 1;
                
                if(ind != org) {
                    this->SetFocusIndex(ind);
                    EnsureVisible(ind);
                }

            });
        }
        
        bool fitheight(int maxpixels) {
            int curh = GetInteriorSize().Height;
            int overhead = GetCurrentHeight() - curh;
            int defh = stack.GetTemplate().GetUnitSize();

            auto li = stack.GetTemplate(UI::ComponentTemplate::ItemTag);
            if(li)
                defh = li->GetSize(GetCurrentSize()).Height;

            int expected = int( std::ceil( (this->GetCount()+overscroll) * (maxdisplay == 0 ? defh : curh/maxdisplay) ) ) + overhead;
            bool smalldone = false;

            //will possibly fit
            if(expected < maxpixels) {

                int tried = 0;

                do {
                    SetHeight(Pixels(expected));
                    stack.Update(true);
                    Refresh();

                    auto diff = maxdisplay - (this->GetCount()+overscroll);

                    //it fits everything, might not be tightest but that's ok. We can also eat up a bit of overscroll
                    if((diff < 1 && diff >= -overscroll/2) || maxdisplay == 0)
                        return true;

                    curh = GetInteriorSize().Height;
                    overhead = GetCurrentHeight() - curh;
                    int curexpected = int( std::ceil( (this->GetCount()+overscroll) * (curh/maxdisplay) ) ) + overhead;

                    if(abs(expected - curexpected) == 0) { //no progress, exit
                        break;
                    }
                    else if(abs(expected - curexpected) < 0.1 && smalldone) { //we will allow one small progress, no more
                        break;
                    }
                    else if(abs(expected - curexpected) < 0.1) {
                        smalldone = true;
                    }
                    expected = curexpected;
                    tried++;
                } while(expected < maxpixels && tried < 5);

            }

            SetHeight(Pixels(maxpixels));
            return false;
        }

        ~ListboxWidgetBase() {
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
                this->sel_prepare(w);
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
            vscroller->Maximum      = float(this->STR_::getsize());
            vscroller->Range        = 1;
            *vscroller              = scrolloffset;
            vscroller->SmallChange  = float(scrolldist);
            vscroller->SetSmoothChangeSpeed(scrollspeed);
            vscroller->ValueChanged.Register(*this, &ListboxWidgetBase::ScrollTo);
            
            return vscroller;
        }
        
        void updatescroll() {
            if(!isscrolling)
                Utils::ASSERT_FALSE("This should not happen");
            
            auto cur = scrolloffset;
            auto curspeed = scrollspeed;
            
            if(1000*abs(target-cur)/scrollspeed > maxscrolltime) {
                //due to integer division, this value would be scrollspeed at some points, which will reset smooth speed
                //if not, when scrolling is finished it will be reset
                curspeed = 1000*abs(target-cur) / maxscrolltime;
            
                auto scroller = dynamic_cast<VScroller<float>*>(stack.GetWidget(UI::ComponentTemplate::VScrollTag));
                if(scroller)
                    scroller->SetSmoothChangeSpeed(curspeed);
            }
            
            float dist = (float)curspeed/1000 * Time::DeltaTime();
            
            bool done = false;
            
            if(target < cur) {
                if(cur - dist <= target) {
                    cur = target;
                }
                else {
                    cur -= dist;
                }
            }
            else if(target > cur) {
                if(cur + dist >= target) {
                    cur = target;
                }
                else {
                    cur += dist;
                }
            }
            else {
                done = true;
            }                
            
            scrolloffset = cur;
            
            Refresh();
            
            if(done) {
                isscrolling = false;
                
                auto scroller = dynamic_cast<VScroller<float>*>(stack.GetWidget(UI::ComponentTemplate::VScrollTag));
                if(scroller)
                    scroller->SetSmoothChangeSpeed(scrollspeed); //just in case
                    
                stack.RemoveFrameEvent();
            }
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
        
        virtual void boundschanged() override {
            Refresh();
        }

        std::map<UI::ComponentTemplate::Tag, Containers::Collection<W_>> widgets;
        std::map<const W_*, long> indexes;
        std::map<long, W_*> widgetlist;
        UI::LayerAdapter contents;
        float scrolloffset = 0.f;
        float target = 0.f;
        float overscroll = 1;
        int scrollspeed = 20;
        int maxscrolltime = 500;
        int scrolldist  = 5;
        bool isscrolling = false;
        float maxdisplay = 0; //maximum elements that can be displayed
        
        Input::KeyRepeater repeater;
    };

    /**
     * This is a simple listbox. It does not store any additional information
     * about each item, therefore, can store large amounts of items. Items are
     * stored in an std::vector. Only one element can be selected at a time.
     */
    template<class T_, void (*TW_)(const T_ &, ListItem &) = internal::SetTextUsingFrom<T_, ListItem>>
    class SimpleListbox : 
        public ListboxWidgetBase<T_, ListItem, 
            internal::LBTRF_ListItem<T_, ListItem, SimpleListbox<T_, TW_>>,
            internal::LBSTR_STLVector<T_, ListItem, std::vector<T_>, SimpleListbox<T_, TW_>>,
            internal::LBSELTR_Single<T_, ListItem, SimpleListbox<T_, TW_>>,
            false, TW_
        >
    {
        using Base = ListboxWidgetBase<T_, ListItem, 
            internal::LBTRF_ListItem<T_, ListItem, SimpleListbox<T_, TW_>>,
            internal::LBSTR_STLVector<T_, ListItem, std::vector<T_>, SimpleListbox<T_, TW_>>,
            internal::LBSELTR_Single<T_, ListItem, SimpleListbox<T_, TW_>>,
            false, TW_
        >;
        
        friend internal::LBTRF_ListItem<T_, ListItem, SimpleListbox<T_, TW_>>;
        friend internal::LBSTR_STLVector<T_, ListItem, std::vector<T_>, SimpleListbox<T_, TW_>>;
        friend internal::LBSELTR_Single<T_, ListItem, SimpleListbox<T_, TW_>>;
        
    public:
        explicit SimpleListbox(Registry::TemplateType temp = Registry::Listbox_Regular) : Base(Registry::Active()[temp]) {
        }
        
        explicit SimpleListbox(const UI::Template &temp) : Base(temp) {
        }
        
        virtual bool Activate() {
            return false;
        }
        
        SimpleListbox &operator =(const T_ value) {
            this->SetSelection(value);
            
            return *this;
        }
        
        using Base::ComponentStackWidget::Widget::Remove;
        using internal::LBSTR_STLVector<T_, ListItem, std::vector<T_>, SimpleListbox<T_>>::Remove;
    };
    
    /**
     * This is a simple multi select listbox. It does not store any additional 
     * information about each item, therefore, can store large amounts of items.
     * However, default event strategy is triggering the event for every selected
     * and unselected item. This might cause problems if the list is too long. 
     * When the event method is set to single, only one event will be called for
     * single action. Items are stored in an std::vector. You may use Selection
     * and SelectedIndices to iterate selected elements. In both members, selection
     * is always sorted by the position in the listbox.
     */
    template<class T_, void (*TW_)(const T_ &, ListItem &) = internal::SetTextUsingFrom<T_, ListItem>>
    class MultiListbox : 
        public ListboxWidgetBase<T_, ListItem, 
            internal::LBTRF_ListItem<T_, ListItem, MultiListbox<T_, TW_>>,
            internal::LBSTR_STLVector<T_, ListItem, std::vector<T_>, MultiListbox<T_, TW_>>,
            internal::LBSELTR_Multi<T_, ListItem, MultiListbox<T_, TW_>>,
            false, TW_
        >
    {
        using Base = ListboxWidgetBase<T_, ListItem, 
            internal::LBTRF_ListItem<T_, ListItem, MultiListbox<T_, TW_>>,
            internal::LBSTR_STLVector<T_, ListItem, std::vector<T_>, MultiListbox<T_, TW_>>,
            internal::LBSELTR_Multi<T_, ListItem, MultiListbox<T_, TW_>>,
            false, TW_
        >;
        
        friend internal::LBTRF_ListItem<T_, ListItem, MultiListbox<T_, TW_>>;
        friend internal::LBSTR_STLVector<T_, ListItem, std::vector<T_>, MultiListbox<T_, TW_>>;
        friend internal::LBSELTR_Multi<T_, ListItem, MultiListbox<T_, TW_>>;
        
    public:
        explicit MultiListbox(Registry::TemplateType temp = Registry::Listbox_Regular) : Base(Registry::Active()[temp]) {
        }
        
        explicit MultiListbox(const UI::Template &temp) : Base(temp) {
        }
        
        virtual bool Activate() {
            return false;
        }
        
        MultiListbox &operator =(const T_ value) {
            this->SetSelection(value);
            
            return *this;
        }
        
        using Base::ComponentStackWidget::Widget::Remove;
        using internal::LBSTR_STLVector<T_, ListItem, std::vector<T_>, MultiListbox<T_>>::Remove;
    };
    
    /**
     * This is a simple collectionbox. It does not store any additional information
     * about each item, therefore, can store large amounts of items. Items are
     * stored in a Containers::Collection. Only one element can be selected at a time.
     */
    template<class T_, void (*TW_)(const T_ &, ListItem &) = internal::SetTextUsingFrom<T_, ListItem>>
    class SimpleCollectionbox : 
        public ListboxWidgetBase<T_, ListItem, 
            internal::LBTRF_ListItem<T_, ListItem, SimpleCollectionbox<T_, TW_>>,
            internal::LBSTR_Collection<T_, ListItem, SimpleCollectionbox<T_, TW_>>,
            internal::LBSELTR_Single<T_, ListItem, SimpleCollectionbox<T_, TW_>>,
            false, TW_
        >
    {
        using Base = ListboxWidgetBase<T_, ListItem, 
            internal::LBTRF_ListItem<T_, ListItem, SimpleCollectionbox<T_, TW_>>,
            internal::LBSTR_Collection<T_, ListItem, SimpleCollectionbox<T_, TW_>>,
            internal::LBSELTR_Single<T_, ListItem, SimpleCollectionbox<T_, TW_>>,
            false, TW_
        >;
        
        friend internal::LBTRF_ListItem<T_, ListItem, SimpleCollectionbox<T_, TW_>>;
        friend internal::LBSTR_Collection<T_, ListItem, SimpleCollectionbox<T_, TW_>>;
        friend internal::LBSELTR_Single<T_, ListItem, SimpleCollectionbox<T_, TW_>>;
        
    public:
        explicit SimpleCollectionbox(Registry::TemplateType temp = Registry::Listbox_Regular) : Base(Registry::Active()[temp]) {
        }
        
        explicit SimpleCollectionbox(const UI::Template &temp) : Base(temp) {
        }
        
        virtual bool Activate() {
            return false;
        }
        
        SimpleCollectionbox &operator =(const T_ value) {
            this->SetSelection(value);
            
            return *this;
        }
        
        using Base::ComponentStackWidget::Widget::Remove;
        using internal::LBSTR_Collection<T_, ListItem, SimpleCollectionbox<T_>>::Remove;
    };
    
    /**
     * This is a simple multi select collectionbox. It does not store any additional 
     * information about each item, therefore, can store large amounts of items.
     * However, default event strategy is triggering the event for every selected
     * and unselected item. This might cause problems if the list is too long. 
     * When the event method is set to single, only one event will be called for
     * single action. Items are stored in Containers::Collection. You may use Selection
     * and SelectedIndices to iterate selected elements. In both members, selection
     * is always sorted by the position in the collectionbox.
     */
    template<class T_, void (*TW_)(const T_ &, ListItem &) = internal::SetTextUsingFrom<T_, ListItem>>
    class MultiCollectionbox : 
        public ListboxWidgetBase<T_, ListItem, 
            internal::LBTRF_ListItem<T_, ListItem, MultiCollectionbox<T_, TW_>>,
            internal::LBSTR_Collection<T_, ListItem, MultiCollectionbox<T_, TW_>>,
            internal::LBSELTR_Multi<T_, ListItem, MultiCollectionbox<T_, TW_>>,
            false, TW_
        >
    {
        using Base = ListboxWidgetBase<T_, ListItem, 
            internal::LBTRF_ListItem<T_, ListItem, MultiCollectionbox<T_, TW_>>,
            internal::LBSTR_Collection<T_, ListItem, MultiCollectionbox<T_, TW_>>,
            internal::LBSELTR_Multi<T_, ListItem, MultiCollectionbox<T_, TW_>>,
            false, TW_
        >;
        
        friend internal::LBTRF_ListItem<T_, ListItem, MultiCollectionbox<T_, TW_>>;
        friend internal::LBSTR_Collection<T_, ListItem, MultiCollectionbox<T_, TW_>>;
        friend internal::LBSELTR_Multi<T_, ListItem, MultiCollectionbox<T_, TW_>>;
        
    public:
        explicit MultiCollectionbox(Registry::TemplateType temp = Registry::Listbox_Regular) : Base(Registry::Active()[temp]) {
        }
        
        explicit MultiCollectionbox(const UI::Template &temp) : Base(temp) {
        }
        
        virtual bool Activate() {
            return false;
        }
        
        MultiCollectionbox &operator =(const T_ value) {
            this->SetSelection(value);
            
            return *this;
        }
        
        using Base::ComponentStackWidget::Widget::Remove;
        using internal::LBSTR_Collection<T_, ListItem, MultiCollectionbox<T_>>::Remove;
    };
    
    template<class T_, class W_, class F_>
    std::ostream &operator <<(std::ostream &out, const internal::LBSELTR_Single<T_, W_, F_> &list) {
        out << static_cast<T_>(list);
        
        return out;
    }
    
} }

