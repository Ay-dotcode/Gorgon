#pragma once


#include <string>
#include "..\Base\Widget.h"
#include "..\..\Utils\Property.h"
#include "..\..\Utils\EventChain.h"
#include "..\Base\Container.h"
#include "..\..\Engine\Graphic2D.h"



namespace gge { namespace widgets {

	class IButton {
	public:

		IButton() : 
		  INIT_PROPERTY(IButton,Text),
		  INIT_PROPERTY(IButton,Icon),
		  clickevent("ClickEvent", this)
		{ }

		utils::TextualProperty<IButton> Text;
		utils::ReferenceProperty<IButton, graphics::RectangularGraphic2D> Icon;


		operator std::string() {
			return Text;
		}

		IButton &operator =(const std::string &s) {
			Text=s;

			return *this;
		}

		virtual void Fire() = 0;

		utils::EventChain<IButton> &ClickEvent() { return clickevent; }

		virtual WidgetBase *GetWidget() {
			return NULL;
		}

	protected:
		//REQUIRED
		virtual void setText(const std::string &text) = 0;
		virtual std::string getText() const = 0;
		virtual void setIcon(graphics::RectangularGraphic2D* text) = 0;
		virtual graphics::RectangularGraphic2D *getIcon() const = 0;

		utils::EventChain<IButton> clickevent;
	};


} }
