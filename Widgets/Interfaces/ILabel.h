#pragma once


#include <string>
#include "../Base/Widget.h"
#include "../../Utils/Property.h"
#include "../../Utils/EventChain.h"
#include "../Base/Container.h"



namespace gge { namespace widgets {

	class ILabel {
	public:

		ILabel() : INIT_PROPERTY(ILabel, Text),settingfocus(false)
		{ }

		utils::TextualProperty<ILabel> Text;

		operator std::string() {
			return Text;
		}

		ILabel &operator =(const std::string &s) {
			Text=s;

			return *this;
		}

		virtual WidgetBase *GetWidget() {
			return NULL;
		}

	protected:
		//REQUIRED
		virtual void setText(const std::string &text) = 0;
		virtual std::string getText() const = 0;


		//SUPPLIED
		//Note that before calling this, its a good idea to call WidgetBase::MouseEvent first
		//return value should always depend on interface helpers.
		bool default_mousehandler(input::mouse::Event::Type event, utils::Point location, int amount) {
			return false;
		}
		template<class T_>
		bool default_focusreceived(T_ *Container) {

			//if(Container && !settingfocus) {
			//	settingfocus=true;
			//	return Container->FocusNext();
			//}

			//settingfocus=false;
			return false;
		}

		bool settingfocus;

	};

}}
