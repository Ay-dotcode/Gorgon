#pragma once


#include <string>
#include <functional>

#include "../Base/Widget.h"
#include "../../Utils/Property.h"
#include "../../Utils/EventChain.h"
#include "../Base/Container.h"



namespace gge { namespace widgets {


	class IListItem {
	public:

		IListItem(int index, std::function<void(IListItem&, int)> trigger) : Index(index), trigger(trigger) { }

		virtual WidgetBase &GetWidget() =0;

		virtual ~IListItem() { }

		void Trigger() {
			trigger(*this, Index);
		}
		int Index;

		std::function<void(IListItem&, int, gge::utils::Point)> DragNotify;

	protected:

		std::function<void(IListItem&, int)> trigger;
	};

}}

