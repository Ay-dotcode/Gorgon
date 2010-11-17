#pragma once

#include "IWidgetContainer.h"

namespace gorgonwidgets {
	struct DialogAction {
		DialogAction() : Cancel(false) { }

		bool Cancel;
	};

	class IDialog {
	public:
		virtual void Show() = 0;
		virtual void Show(IWidgetContainer &container) = 0;
		virtual void Hide() = 0;
		virtual void Move(int X, int Y)		= 0;
		virtual void Resize(int W, int H)	= 0;
		virtual void SetParent(IWidgetContainer &container) = 0;

		virtual bool isVisible() = 0;
	};
}