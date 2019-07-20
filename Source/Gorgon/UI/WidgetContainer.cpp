#include "WidgetContainer.h"

namespace Gorgon { namespace UI {
    
	bool WidgetContainer::Add(WidgetBase &widget) {
		if(!addingwidget(widget))
			return false;

		if(!widget.addingto(*this))
			return false;

		layer().Add(widget.GetLayer());
		widgets.Add(widget);

		widgetadded(widget);
		widget.addedto(*this);

		return true;
	}

	bool WidgetContainer::Insert(WidgetBase &widget, int index) {
		if(!addingwidget(widget))
			return false;

		if(!widget.addingto(*this))
			return false;

		if(index <= focusindex)
			focusindex++;

		layer().Insert(widget.GetLayer(), index);
		widgets.Insert(widget, index);

		widgetadded(widget);
		widget.addedto(*this);

		return true;
	}

	bool WidgetContainer::Remove(WidgetBase &widget) {
		auto pos = widgets.Find(widget);

		if(pos == widgets.end())
			return true;

		if(!removingwidget(widget))
			return false;

		if(!widget.removingfrom())
			return false;

		if(focusindex == pos - widgets.begin())
			FocusNext();
		
		if(focusindex > pos - widgets.begin())
			focusindex--;

		layer().Remove(widget.GetLayer());
		widgets.Remove(pos);

		widgetremoved(widget);
		widget.removed();

		return true;
	}

	void WidgetContainer::ForceRemove(WidgetBase &widget) {
		auto pos = widgets.Find(widget);

		if(pos == widgets.end())
			return;

		if(focusindex == pos - widgets.begin())
			FocusNext();

		if(focusindex > pos - widgets.begin())
			focusindex--;

		layer().Remove(widget.GetLayer());
		widgets.Remove(pos);

		widgetremoved(widget);
		widget.removed();
	}

	void WidgetContainer::ChangeFocusOrder(WidgetBase &widget, int order) {
		auto pos = widgets.Find(widget);

		if(pos == widgets.end())
			return;

		if(order <= focusindex && (pos-widgets.begin()) > focusindex) {
			focusindex++;
		}
		else if(order >= focusindex && (pos-widgets.begin()) < focusindex) {
			focusindex--;
		}
		else if((pos-widgets.begin()) == focusindex) {
			focusindex = order;
		}

		widgets.MoveBefore(widget, order);
	}

	void WidgetContainer::ChangeZorder(WidgetBase &widget, int order) {
		auto &l = layer();
		auto pos = l.Children.Find(widget.GetLayer());

		if(pos == l.Children.end())
			return;

		widget.GetLayer().PlaceBefore(order);
	}

	bool WidgetContainer::FocusFirst() {
		return false;
	}

	bool WidgetContainer::FocusNext() {
		return false;
	}

} }
