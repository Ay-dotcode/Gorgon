#include "Widget.h"
#include "WidgetContainer.h"
#include "../Widgets/Registry.h"

namespace Gorgon { namespace UI {

    bool Widget::Remove() {
        if(!parent)
            return true;

        return parent->Remove(*this);
    }

    bool Widget::Focus() {
        if(!parent)
            return false;

        if(!allowfocus() || !visible || !enabled)
            return false;

        return parent->SetFocusTo(*this);
    }

    bool Widget::Defocus() {
        if(!IsFocused() || !parent)
            return true;

        return parent->RemoveFocus();
    }

    WidgetContainer &Widget::GetParent() const {
        if(parent == nullptr)
            throw std::runtime_error("Widget has no parent");

        return *parent;
    }


    void Widget::SetVisible(bool value) {
        if(visible != value) {
            visible = value;
            
            if(value)
                show();
            else {
                if(IsFocused() && HasParent())
                    GetParent().FocusPrevious();
                
                Defocus();
                hide();
            }
            
            if(parent)
                parent->childboundschanged(this);
        }
    }
    
    
    bool Widget::EnsureVisible() const {
        if(!parent)
            return false;
        
        return parent->EnsureVisible(*this);
    }

    void Widget::boundschanged(){
        if(parent)
            parent->childboundschanged(this);
        
        BoundsChangedEvent();
    }


    /// Called when this widget added to the given container
    void Widget::addedto(WidgetContainer &container) {
        if(parent == &container)
            return;

        parent = &container;

        if(IsVisible())
            boundschanged();

        parentenabledchanged(parent->IsEnabled());
    }

    void Widget::removed(){
        if(!parent)
            return;
        
        parent->childboundschanged(this);
        
        if(parent && parent->GetHoveredWidget() == this)
            parent->SetHoveredWidget(nullptr);

        parent = nullptr;
        
        if(IsVisible())
            boundschanged(); 
    }
    

    void Widget::focuslost() {
        focus = false;
        FocusEvent();
    }


    void Widget::focused() {
        focus = true;
        FocusEvent();
    }


    Widget::~Widget() {
        DestroyedEvent();
        
        if(HasParent()) {
            GetParent().deleted(this);
        }
    }


    void Widget::mouseenter() {
        if(parent)
            parent->SetHoveredWidget(this);

        MouseEnterEvent();
    }


    void Widget::mouseleave() {
        if(parent && parent->GetHoveredWidget() == this)
            parent->SetHoveredWidget(nullptr);

        MouseLeaveEvent();
    }

    void Widget::calculatebounds() {
        int unitsize = 0, spacing = 0;
        Geometry::Size sz;
        if(HasParent()) {
            unitsize = GetParent().GetUnitSize();
            spacing = GetParent().GetSpacing();
            sz = GetParent().GetInteriorSize() + Geometry::Size(spacing, spacing);
        }
        else {
            unitsize = Widgets::Registry::Active().GetUnitSize();
            spacing  = Widgets::Registry::Active().GetSpacing();
            sz = GetCurrentSize();
        }

        auto l= UI::Convert(
            location, sz,
            unitsize, spacing,
            Widgets::Registry::Active().GetEmSize()
        );

        if(llocation != l) {
            move(l);
            llocation = l;
        }

        auto s = UI::Convert(
            size, sz,
            unitsize, spacing,
            Widgets::Registry::Active().GetEmSize()
        );

        if(size.Width.IsRelative()) {
            s.Width -= spacing;
        }

        if(size.Height.IsRelative()) {
            s.Height -= spacing;
        }

        if(lsize != s) {
            resize(s);
            lsize = s;
        }
    }

    void Widget::Move(const UnitPoint &value) {
        if(location == value)
            return;

        location = value;

        calculatebounds();
    }

    void Widget::Resize(const UnitSize& value) {
        if(size == value)
            return;

        size = value;

        calculatebounds();
    }

    int Widget::Convert(const UnitDimension &val, bool vertical, bool size) const {
        int unitsize = 0, spacing = 0;
        Geometry::Size sz;
        if(HasParent()) {
            unitsize = GetParent().GetUnitSize();
            spacing = GetParent().GetSpacing();
            sz = GetParent().GetInteriorSize() + Geometry::Size(spacing, spacing);
        }
        else {
            unitsize = Widgets::Registry::Active().GetUnitSize();
            spacing  = Widgets::Registry::Active().GetSpacing();
            sz = GetCurrentSize();
        }

        return val(vertical ? sz.Height : sz.Width, unitsize, spacing, size, Widgets::Registry::Active().GetEmSize());
    }

    Geometry::Size Widget::Convert(const UnitSize &val) const {
        int unitsize = 0, spacing = 0;
        Geometry::Size sz;
        if(HasParent()) {
            unitsize = GetParent().GetUnitSize();
            spacing = GetParent().GetSpacing();
            sz = GetParent().GetInteriorSize() + Geometry::Size(spacing, spacing);
        }
        else {
            unitsize = Widgets::Registry::Active().GetUnitSize();
            spacing  = Widgets::Registry::Active().GetSpacing();
            sz = GetCurrentSize();
        }

        return UI::Convert(val, sz, unitsize, spacing, Widgets::Registry::Active().GetEmSize());
    }

    Geometry::Point Widget::Convert(const UnitPoint &val) const {
        int unitsize = 0, spacing = 0;
        Geometry::Size sz;
        if(HasParent()) {
            unitsize = GetParent().GetUnitSize();
            spacing = GetParent().GetSpacing();
            sz = GetParent().GetInteriorSize() + Geometry::Size(spacing, spacing);
        }
        else {
            unitsize = Widgets::Registry::Active().GetUnitSize();
            spacing  = Widgets::Registry::Active().GetSpacing();
            sz = GetCurrentSize();
        }

        return UI::Convert(val, sz, unitsize, spacing, Widgets::Registry::Active().GetEmSize());
    }

    UnitDimension Widget::Convert(Dimension::Unit target, const UnitDimension &val, bool vertical, bool size) const {
        int unitsize = 0, spacing = 0;
        Geometry::Size sz;
        if(HasParent()) {
            unitsize = GetParent().GetUnitSize();
            spacing = GetParent().GetSpacing();
            sz = GetParent().GetInteriorSize() + Geometry::Size(spacing, spacing);
        }
        else {
            unitsize = Widgets::Registry::Active().GetUnitSize();
            spacing  = Widgets::Registry::Active().GetSpacing();
            sz = GetCurrentSize();
        }

        int em = Widgets::Registry::Active().GetEmSize();

        int px = val(vertical ? sz.Height : sz.Width, unitsize, spacing, size, em);

        switch(target) {
        case Dimension::Pixel:
        default:
            return Pixels(px);
        case Dimension::MilliPixel:
            return {(float)px, Dimension::MilliPixel};
        case Dimension::Percent:
        case Dimension::BasisPoint:
            return {(float)px/(vertical ? sz.Height : sz.Width), target};
        case Dimension::UnitSize:
        case Dimension::MilliUnitSize:
            if(size)
                return {(float)(px+spacing)/(unitsize+spacing), target};
            else
                return {(float)px/(unitsize+spacing), target};
        case Dimension::EM:
            return {(float)px/em, Dimension::EM};
        }
    }

    UnitSize Widget::Convert(Dimension::Unit target, const UnitSize &val) const {
        int unitsize = 0, spacing = 0;
        Geometry::Size sz;
        if(HasParent()) {
            unitsize = GetParent().GetUnitSize();
            spacing = GetParent().GetSpacing();
            sz = GetParent().GetInteriorSize() + Geometry::Size(spacing, spacing);
        }
        else {
            unitsize = Widgets::Registry::Active().GetUnitSize();
            spacing  = Widgets::Registry::Active().GetSpacing();
            sz = GetCurrentSize();
        }

        int em = Widgets::Registry::Active().GetEmSize();

        auto px = UI::Convert(val, sz, unitsize, spacing, em);

        switch(target) {
        case Dimension::Pixel:
        default:
            return Pixels(px);
        case Dimension::MilliPixel:
            return {{(float)px.Width, target}, {(float)px.Height, target}};;
        case Dimension::Percent:
        case Dimension::BasisPoint:
            return {{(float)px.Width/sz.Width, target}, {(float)px.Height/sz.Height, target}};;
        case Dimension::UnitSize:
        case Dimension::MilliUnitSize:
            return {{(float)(px.Width+spacing)/(unitsize+spacing), target}, {(float)(px.Height+spacing)/(unitsize+spacing), target}};
        case Dimension::EM:
            return {{(float)px.Width/em, Dimension::EM}, {(float)px.Height/em, Dimension::EM}};;
        }
    }

    UnitPoint Widget::Convert(Dimension::Unit target, const UnitPoint &val) const {
        int unitsize = 0, spacing = 0;
        Geometry::Size sz;
        if(HasParent()) {
            unitsize = GetParent().GetUnitSize();
            spacing = GetParent().GetSpacing();
            sz = GetParent().GetInteriorSize() + Geometry::Size(spacing, spacing);
        }
        else {
            unitsize = Widgets::Registry::Active().GetUnitSize();
            spacing  = Widgets::Registry::Active().GetSpacing();
            sz = GetCurrentSize();
        }

        int em = Widgets::Registry::Active().GetEmSize();


        auto px = UI::Convert(val, sz, unitsize, spacing, em);

        switch(target) {
        case Dimension::Pixel:
        default:
            return Pixels(px);
        case Dimension::MilliPixel:
            return {{(float)px.X, target}, {(float)px.Y, target}};;
        case Dimension::Percent:
        case Dimension::BasisPoint:
            return {{(float)px.X/sz.Width, target}, {(float)px.Y/sz.Height, target}};;
        case Dimension::UnitSize:
        case Dimension::MilliUnitSize:
            return {{(float)px.X/(unitsize+spacing), target}, {(float)px.Y/(unitsize+spacing), target}};
        case Dimension::EM:
            return {{(float)px.X/em, Dimension::EM}, {(float)px.Y/em, Dimension::EM}};;
        }
    }

    void Widget::parentboundschanged() {
        calculatebounds();
    }

} }
