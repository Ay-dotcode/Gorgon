#pragma once


#include <string>
#include "..\Base\Widget.h"
#include "..\..\Utils\Property.h"
#include "..\..\Utils\EventChain.h"
#include "..\Base\Container.h"



namespace gge { namespace widgets {


	namespace listbox { 
		template<class T_, void(*CF_)(const T_ &, std::string &)>
		class Base; 
	}

	template<class T_, void(*CF_)(const T_ &, std::string &)>
	class IListItem {
	public:

		enum SelectionTypes {
			SingleSelect,
			ToggleSelect, //click toggles
			MultiSelect, //control click adds to selected, support for shift multi select and drag multi select
		};

		typedef utils::EventChain<listbox::Base<T_, CF_>, IListItem*> ToggleNotifyFunction;

		IListItem(T_ value) : INIT_PROPERTY(IListItem, Value)
		{ }

		IListItem &operator =(const T_ &s) {
			Text=s;

			return *this;
		}
		
		operator T_() {
			return getValue();
		}

		virtual WidgetBase &GetWidget() =0;

		utils::Property<IListItem, T_> Value;

	protected:

		virtual void setValue(const T_ &value) = 0;
		virtual T_ getValue() const = 0;

	};

}}
