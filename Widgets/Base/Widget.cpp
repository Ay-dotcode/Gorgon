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

	bool WidgetBase::IsFocused() {
		return Container->GetFocused()==this;
	}

	void WidgetBase::locateto(ContainerBase* container, int Order, utils::SortedCollection<WidgetBase>::Wrapper * w) {
		Container=container;
		utils::CheckAndDelete(BaseLayer);
		BaseLayer=&container->CreateWidgetLayer(Order);
		wrapper=w;
		BaseLayer->MouseCallback.Set(*this, &WidgetBase::MouseEvent);
		if(BaseLayer)
			BaseLayer->isVisible=isvisible;
		BaseLayer->Move(location);
		BaseLayer->Resize(size);
	}

	void WidgetBase::call_container_removefocus() {
		if(Container && IsFocused())
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
		if(IsFocused() && Container)
			Container->FocusNext();
	}

	bool WidgetBase::IsEnabled() {
		if(!Container)
			return isenabled; 
		else
			return isenabled && Container->IsEnabled();
	}

	void WidgetBase::Resize(utils::Size Size) {
		size=Size;
		if(BaseLayer)
			BaseLayer->Resize(Size);

		if(Container)
			Container->WidgetBoundsChanged();

		Draw();
	}

	void WidgetBase::Move(utils::Point Location) {
		location=Location;
		if(BaseLayer)
			BaseLayer->Move(Location);

		if(Container)
			Container->WidgetBoundsChanged();
	}

	void WidgetBase::playsound(resource::SoundResource *snd) {
		int x=location.x+size.Width/2;
		int y=location.y+size.Height/2;
		if(Container) {
			x+=Container->AbsoluteLeft();
			y+=Container->AbsoluteTop();
		}
		snd->Create3DWave((Main.getWidth()+Main.getHeight())/8.0f)->Play().Set3DPosition((float)x, (float)y, 0);
	}


}}
