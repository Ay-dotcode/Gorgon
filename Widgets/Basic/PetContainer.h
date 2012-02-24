#pragma once


#include "../Base/Container.h"
#include "..\..\Engine\Input.h"


namespace gge { namespace widgets {
	
	template<class T_>
	class PetContainer : public ContainerBase {
	public:

		typedef bool (T_::*ControlFn)();

		PetContainer(T_ &parent) : parent(parent),
			ContainerBase(), BaseLayer(), isactive(false)
		{ }

		virtual void Resize(utils::Size Size) {
			ContainerBase::Resize(Size);
			
			
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
			if(!parent.IsEnabled())
				return false;

			return ContainerBase::IsEnabled();
		}

		virtual bool IsVisible()  {
			if(parent.IsVisible())
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
			if(newwidget) {
				isactive=true;
				parent.Focus();
			}
		}

		operator LayerBase &() {
			return BaseLayer;
		}

		void InformEnabledChange(bool state) {
			for(auto it=Widgets.First();it.isValid();it.Next()) {
				call_widget_containerenabledchanged(*it, false);
			}
		}

		std::map<input::keyboard::Key, WidgetBase*> &GetAccessKeys() {
			return AccessKeys;
		}


		bool KeyboardEvent(input::keyboard::Event event) { 
			if(Focused) {
				if(Focused->KeyboardEvent(event.event, event.keycode))
					return true;
			}

			return false;
		}

		LayerBase BaseLayer;

	protected:

		bool isactive;
		T_ &parent;


	};

	template<class T_>
	class ExtendedPetContiner : public PetContainer<T_> {
	public:
		LayerBase ExtenderLayer;

		ExtendedPetContiner(T_ &parent) : PetContainer(parent), extenderbase(NULL) {

		}

		void AttachTo(LayerBase *layer, LayerBase *extender, int order=0) {
			if(layer && extender) {
				utils::CheckAndDelete(extenderbase);
				extenderbase=extender;
				layer->Add(BaseLayer, order);
				extender->Add(ExtenderLayer, order);
			}
			else {
				BaseLayer.parent=NULL;
				ExtenderLayer.parent=NULL;
				utils::CheckAndDelete(extenderbase);
			}
		}
		void AttachTo(LayerBase &layer, LayerBase &extender, int order=0) {
			AttachTo(&layer, &extender, order);
		}

		virtual widgets::WidgetLayer &CreateExtenderLayer()  {
			widgets::WidgetLayer *layer=new widgets::WidgetLayer;
			ExtenderLayer.Add(layer, BaseLayer.SubLayers.LowestOrder()-1);

			return *layer;
		}

	protected:
		LayerBase *extenderbase;

	};

}}
