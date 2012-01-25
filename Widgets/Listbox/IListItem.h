#pragma once


#include <string>
#include "..\Base\Widget.h"
#include "..\..\Utils\Property.h"
#include "..\..\Utils\EventChain.h"
#include "..\Base\Container.h"



namespace gge { namespace widgets {


	template<class T_, void(*CF_)(const T_ &, std::string &)>
	class IListItem;

	namespace prvt { 
		template <class T_, void(*CF_)(const T_ &, std::string &)>
		struct ilistnotifier {
			virtual void Fire(IListItem<T_,CF_> *source,bool raise) = 0;
		};

		template <class T_, void(*CF_)(const T_ &, std::string &), class R_>
		struct listnotifyholder : ilistnotifier<T_, CF_> {
			listnotifyholder(R_ *target, void (R_::*handler)(IListItem<T_,CF_> *,bool)) : 
				target(target), handler(handler)
			{
			}

			void Fire(IListItem<T_,CF_> *source,bool raise) {
				(target->*handler)(source,raise);
			}

			R_ *target;
			void (R_::*handler)(IListItem<T_,CF_> *,bool);
		};
	}

	template<class T_, void(*CF_)(const T_ &, std::string &)>
	class IListItem {
	public:

		IListItem(T_ value) : INIT_PROPERTY(IListItem, Value), notifier(NULL)
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

		virtual ~IListItem() { utils::CheckAndDelete(notifier); }


	protected:

		virtual void setValue(const T_ &value) = 0;
		virtual T_ getValue() const = 0;

		prvt::ilistnotifier<T_,CF_> *notifier;
	};

}}

