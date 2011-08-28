#pragma once


#include "../Base/Container.h"
#include "..\..\Engine\Input.h"


namespace gge { namespace widgets {
	
	template<class T_>
	class PetContainer : public ContainerBase {
	public:

		typedef bool (T_::*ControlFn)();

		PetContainer(T_ &parent, ControlFn IsEnabledFn=NULL, ControlFn IsVisibleFn=NULL) : parent(parent),
			IsEnabledFn(IsEnabledFn), IsVisibleFn(IsVisibleFn),
			ContainerBase(), BaseLayer(), isactive(false)
		{ }

		virtual void Resize(utils::Size Size) {
			BaseLayer.Resize(Size);
		}

		void Detach() {
			/*Do nothing*/
		}

		virtual bool IsActive()  {
			return isactive;
		}

		virtual void Deactivate()  {
			isactive=false;
		}

		virtual bool IsEnabled() {
			if(IsEnabledFn)
				if(!(parent.*IsEnabledFn)())
					return false;

			return ContainerBase::IsEnabled();
		}

		virtual bool IsVisible()  {
			if(IsVisibleFn)
				if(!(parent.*IsVisibleFn)())
					return false;

			return BaseLayer.isVisible;
		}

		virtual void Show(bool setfocus=false)  {
			ContainerBase::Show(setfocus);

			BaseLayer.isVisible=true;
		}

		virtual void Hide()  {
			BaseLayer.isVisible=false;
		}

		virtual LayerBase &CreateBackgroundLayer()  {
			LayerBase *layer=new LayerBase;
			BaseLayer.Add(layer, BaseLayer.SubLayers.HighestOrder()+1);

			return *layer;
		}

		virtual widgets::WidgetLayer &CreateWidgetLayer(int Order=0)  {
			widgets::WidgetLayer *layer=new widgets::WidgetLayer;
			BaseLayer.Add(layer, Order);

			return *layer;
		}

		virtual widgets::WidgetLayer &CreateExtenderLayer()  {
			widgets::WidgetLayer *layer=new widgets::WidgetLayer;
			BaseLayer.Add(layer, BaseLayer.SubLayers.LowestOrder()-1);

			return *layer;
		}

		virtual void focus_changed(WidgetBase *newwidget)  {
			if(newwidget)
				isactive=true;
		}

		operator LayerBase &() {
			return BaseLayer;
		}


		bool KeyboardEvent(input::keyboard::Event event) { 
			if(Focussed) {
				if(Focussed->KeyboardEvent(event.event, event.keycode))
					return true;
			}

			return false;
		}

		LayerBase BaseLayer;

		ControlFn IsVisibleFn;
		ControlFn IsEnabledFn;

	protected:

		bool isactive;
		T_ &parent;


	};

}}
