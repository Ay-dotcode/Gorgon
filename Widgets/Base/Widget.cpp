#include "Widget.h"

#include "Container.h"


namespace gge { namespace widgets {


	bool WidgetBase::call_container_setfocus() {
		if(!Container)
			return false;

		return Container->setfocus(this);
	}
	
	bool WidgetBase::locating(ContainerBase *container, int Order) {
		Detach();

		return true;
	}

	void WidgetBase::call_container_widget_visibility_change(bool state) {
		if(Container)
			Container->widget_visibility_change(this, state);
	}

	void WidgetBase::call_container_forceremovefocus() {
		if(Container)
			Container->ForceRemoveFocus();
	}

	void WidgetBase::located(ContainerBase* container, utils::SortedCollection<WidgetBase>::Wrapper *w, int Order) {
		locateto(container, Order, w);

		Draw();
	}

	bool WidgetBase::IsFocussed() {
		return Container->GetFocussed()==this;
	}

	void WidgetBase::locateto(ContainerBase* container, int Order, utils::SortedCollection<WidgetBase>::Wrapper * w) {
		Container=container;
		BaseLayer=&container->CreateWidgetLayer(Order);
		wrapper=w;
		BaseLayer->MouseCallback.Set(*this, &WidgetBase::MouseEvent);
		BaseLayer->Move(location);
		BaseLayer->Resize(size);
	}

	void WidgetBase::call_container_removefocus() {
		if(Container && IsFocussed())
			Container->RemoveFocus();
	}

	void WidgetBase::SetContainer(ContainerBase &container) {
		container.AddWidget(this);
	}

	void WidgetBase::Detach() {
		if(!Container)
			return;

		Container->RemoveWidget(this);
	}

	void WidgetBase::Disable() {
		isenabled=false; 
		if(IsFocussed() && Container)
			Container->FocusNext();
	}

	bool WidgetBase::IsEnabled() {
		if(!Container)
			return isenabled; 
		else
			return isenabled && Container->IsEnabled();
	}


}}
