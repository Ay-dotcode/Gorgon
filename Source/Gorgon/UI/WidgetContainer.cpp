#include "WidgetContainer.h"

namespace Gorgon { namespace UI {
    
    bool WidgetContainer::Add(Widget &widget) {
        if(widgets.Find(widget) != widgets.end())
            return true;

        if(!addingwidget(widget))
            return false;

        if(!widget.addingto(*this))
            return false;

        if(widget.HasParent())
            if(!widget.GetParent().Remove(widget))
                return false;

        widget.addto(getlayer());
        widgets.Add(widget);

        widgetadded(widget);
        widget.addedto(*this);

        return true;
    }

    bool WidgetContainer::Insert(Widget &widget, int index) {
        if(widgets.Find(widget) != widgets.end()) {
            ChangeFocusOrder(widget, index);
            ChangeZorder(widget, index);

            return true;
        }

        if(!addingwidget(widget))
            return false;

        if(!widget.addingto(*this))
            return false;

        //we are inserting before the focused widget
        if(index <= focusindex)
            focusindex++;

        widget.addto(getlayer());
        widget.setlayerorder(getlayer(), index);

        widgets.Insert(widget, index);

        widgetadded(widget);
        widget.addedto(*this);

        return true;
    }

    bool WidgetContainer::Remove(Widget &widget) {
        auto pos = widgets.Find(widget);

        //not our widget
        if(pos == widgets.end())
            return true;

        if(!removingwidget(widget))
            return false;

        if(!widget.removingfrom())
            return false;

        ForceRemove(widget);

        return true;
    }

    void WidgetContainer::ForceRemove(Widget &widget) {
        auto pos = widgets.Find(widget);

        //not our widget
        if(pos == widgets.end())
            return;

        if(focusindex == pos - widgets.begin())
            FocusNext();

        if(focusindex > pos - widgets.begin())
            focusindex--;

        widget.removefrom(getlayer());
        widgets.Remove(pos);

        widgetremoved(widget);
        widget.removed();
    }

    void WidgetContainer::ChangeFocusOrder(Widget &widget, int order) {
        auto pos = widgets.Find(widget);

        //not our widget
        if(pos == widgets.end())
            return;

        //widget is moving across currently focused widget
        if(order <= focusindex && (pos-widgets.begin()) > focusindex) {
            focusindex++;
        }
        //widget is moving across currently focused widget
        else if(order >= focusindex && (pos-widgets.begin()) < focusindex) {
            focusindex--;
        }
        //this is the focused widget
        else if((pos-widgets.begin()) == focusindex) {
            focusindex = order;
        }

        widgets.MoveBefore(widget, order);
    }

    int WidgetContainer::GetFocusOrder(const Widget& widget) const {
        auto pos = widgets.Find(widget);

        if(pos == widgets.end())
            return -1;

        return pos - widgets.begin();
    }

    void WidgetContainer::ChangeZorder(Widget &widget, int order) {
        auto &l = getlayer();
        auto pos = widgets.Find(widget);

        if(pos == widgets.end())
            return;

        widget.setlayerorder(l, order);
    }

    bool WidgetContainer::FocusFirst() {
       return FocusNext(-1);
    }

    bool WidgetContainer::FocusNext() {
        return FocusNext(focusindex);
    }


    bool WidgetContainer::FocusNext(int after) {
        if(focused && !focused->canloosefocus() && CurrentFocusStrategy() == Deny)
            return false;

        for(int i=after+1; i<widgets.GetSize(); i++) {
            if(widgets[i].allowfocus() && widgets[i].IsVisible() && widgets[i].IsEnabled()) {
                auto prevfoc = focused;

                focused = &widgets[i];
                focusindex = i;

                if(prevfoc)
                    prevfoc->focuslost();

                widgets[i].focused();

                focuschanged();
        
                EnsureVisible(widgets[i]);
                
                return true;
            }
        }

        //last widget is focused, try bubbling the operation up to the parent
        //if this container is top level focus will rollover

        // if this container is a widget
        if(dynamic_cast<Widget*>(this)) {
            auto w = dynamic_cast<Widget*>(this);

            // that has a parent
            if(w->HasParent()) {
                //try to focus previous widget
                if(w->GetParent().FocusNext()) {
                    focusindex = -1;

                    return true;
                }

                //if not possible, rollover
            }
        }

        //rollover
        for(int i=0; i<after; i++) {
            if(widgets[i].allowfocus() && widgets[i].IsVisible() && widgets[i].IsEnabled()) {
                auto prevfoc = focused;

                focused = &widgets[i];
                focusindex = i;

                if(prevfoc)
                    prevfoc->focuslost();

                widgets[i].focused();

                focuschanged();
        
                EnsureVisible(widgets[i]);
                return true;
            }
        }

        //nothing is found
        return false;
    }

    bool WidgetContainer::FocusPrevious(int before) {
        if(focused && !focused->canloosefocus() && CurrentFocusStrategy() == Deny)
            return false;

        for(int i=before-1; i>=0; i--) {
            if(widgets[i].allowfocus() && widgets[i].IsVisible() && widgets[i].IsEnabled()) {
                auto prevfoc = focused;

                focused = &widgets[i];
                focusindex = i;

                if(prevfoc)
                    prevfoc->focuslost();

                widgets[i].focused();
                if(dynamic_cast<WidgetContainer*>(&widgets[i]) && widgets[i].IsFocused()) {
                    //when focusing previous, if a container is encountered it should focus its last widget
                    //instead of its first.
                    dynamic_cast<WidgetContainer*>(&widgets[i])->FocusLast();
                }

                focuschanged();
        
                EnsureVisible(widgets[i]);
                return true;
            }
        }
        
        //first widget is focused, try bubbling the operation up to the parent
        //if this container is top level focus will rollover
        
        // if this container is a widget
        if(dynamic_cast<Widget*>(this)) {
            auto w = dynamic_cast<Widget*>(this);
            
            // that has a parent
            if(w->HasParent()) {
                //try to focus previous widget
                if(w->GetParent().FocusPrevious()) {
                    focusindex = -1;

                    if(dynamic_cast<WidgetContainer*>(&w->GetParent()) && w->IsFocused()) {
                        //when focusing previous, if a container is encountered it should focus its last widget
                        //instead of its first.
                        dynamic_cast<WidgetContainer*>(&w->GetParent())->FocusLast();
                    }

                    return true;
                }
                
                //if not possible, rollover
            }
        }

        //rollover
        for(int i=widgets.GetSize()-1; i>before; i--) {
            if(widgets[i].allowfocus() && widgets[i].IsVisible() && widgets[i].IsEnabled()) {
                auto prevfoc = focused;

                focused = &widgets[i];
                focusindex = i;

                if(prevfoc)
                    prevfoc->focuslost();

                widgets[i].focused();
                if(dynamic_cast<WidgetContainer*>(&widgets[i]) && widgets[i].IsFocused()) {
                    //when focusing previous, if a container is encountered it should focus its last widget
                    //instead of its first.
                    dynamic_cast<WidgetContainer*>(&widgets[i])->FocusLast();
                }

                focuschanged();
        
                EnsureVisible(widgets[i]);
                return true;
            }
        }

        //nothing is found
        return false;
    }

    bool WidgetContainer::SetFocusTo(Widget &widget) {
        if(CurrentFocusStrategy() == Deny)
            return false;
        
        auto pos = widgets.Find(widget);

        //not our widget
        if(pos == widgets.end())
            return false;

        //already focused, no need to check
        if(&widget == focused)
            return true;

        if(!widget.allowfocus() || !widget.IsVisible() || !widget.IsEnabled())
            return false;

        if(focused && !focused->canloosefocus())
            return false;

        auto prevfoc = focused;

        focusindex = pos - widgets.begin();
        focused = &widget;

        if(prevfoc)
            prevfoc->focuslost();

        widget.focused();
        
        focuschanged();
        
        EnsureVisible(widget);

        return true;
    }

    
    bool WidgetContainer::RemoveFocus() {
        if(!focused)
            return true;
        
        if(!focused->canloosefocus())
            return false;
        
        ForceRemoveFocus();
        
        return true;
    }
    
    void WidgetContainer::ForceRemoveFocus() {
        if(!focused)
            return;
        
        auto prevfoc = focused;
        
        focused = nullptr;
        focusindex = -1;
        
        prevfoc->focuslost();
        
        focuschanged();
    }


    Gorgon::UI::Widget & WidgetContainer::GetFocus() const {
        if(!focused)
            throw std::runtime_error("No widget is focused");

        return *focused;
    }

    bool WidgetContainer::handlestandardkey(Input::Key key) {
        namespace Keycodes = Input::Keyboard::Keycodes;

        if((key == Keycodes::Enter || key == Keycodes::Numpad_Enter) && 
        (Input::Keyboard::CurrentModifier == Input::Keyboard::Modifier::Ctrl || Input::Keyboard::CurrentModifier == Input::Keyboard::Modifier::None)) {
            if(def)
                return def->Activate();
            
            return false;
        }
        else if(key == Keycodes::Escape && Input::Keyboard::CurrentModifier == Input::Keyboard::Modifier::None) {
            if(cancel)
                return cancel->Activate();

            return false;
        }
        else if(key == Keycodes::Tab) {
            if(Input::Keyboard::CurrentModifier == Input::Keyboard::Modifier::Shift) {
                FocusPrevious();
                return true;
            }

            if(Input::Keyboard::CurrentModifier == Input::Keyboard::Modifier::None) {
                FocusNext();
                return true;
            }
        }

        return false;
    }

    bool WidgetContainer::distributekeyevent(Input::Key key, float state, bool handlestandard) {
        if(CurrentFocusStrategy() == Deny)
            return false;
        
        if(focused && focused->KeyEvent(key, state))
            return true;

        if(CurrentFocusStrategy() == Strict)
            return false;
        
        if(handlestandard && state)
            return handlestandardkey(key);

        return false;
    }

    bool WidgetContainer::distributecharevent(Char c) {
        if(CurrentFocusStrategy() == Deny)
            return false;
        
        if(focused)
            return focused->CharacterEvent(c);

        return false;
    }

    void WidgetContainer::distributeparentenabled(bool state) {
        for(auto &w : widgets)
            w.parentenabledchanged(state);
    }

    void WidgetContainer::childboundschanged(Widget *) {
        if(organizer)
            organizer->Reorganize();
    }

    void WidgetContainer::RemoveOrganizer() {
        if(organizer == nullptr)
            return;
        
        auto org = organizer;
        
        organizer = nullptr;
        
        org->RemoveFrom();
        
        if(ownorganizer)
            delete org;
        
        ownorganizer = false;
    }
    
    void WidgetContainer::AttachOrganizer(Organizers::Base &organizer) {
        if(this->organizer == &organizer)
            return;
        
        this->organizer = &organizer;
        
        organizer.AttachTo(*this);
        
        ownorganizer = false;
        
        organizer.Reorganize();
        
    }

    WidgetContainer::~WidgetContainer() { 
        if(ownorganizer)
            delete organizer;
    }

    void WidgetContainer::Displaced() {
        for(auto &w : widgets) {
            w.displaced();
        }
    }

} }
