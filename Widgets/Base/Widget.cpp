#include "Widget.h"

#include "Container.h"


namespace gge { namespace widgets {


	bool WidgetBase::call_container_setfocus() {
		if(!Container)
			return false;

		return Container->setfocus(this);
	}

	bool WidgetBase::located(ContainerBase *container, int Order) {
		Container=container;
		BaseLayer=&container->CreateWidgetLayer(Order);

		return true;
	}


}}
