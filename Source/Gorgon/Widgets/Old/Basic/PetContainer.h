#pragma once


#include "../Base/Container.h"
#include "../../Engine/Input.h"
#include "../ExtenderLayer.h"


namespace gge { namespace widgets {
	
	template<class T_>
	class PetContainer : public ContainerBase {
	public:

		typedef bool (T_::*ControlFn)();

		PetContainer(T_ &parent) : parent(parent),
			ContainerBase(), BaseLayer(), isactive(false), alwaysenabled(false)
		{ }

		virtual void Resize(utils::Size Size) {
			ContainerBase::Resize(Size);
			
			
			BaseLayer.Resize(Size);
		}

		void Detach() {
			/*Do nothing*/
		}

		virtual bool IsActive() const  {
			return isactive;
		}

		virtual void Deactivate()  {
			isactive=false;
		}

		virtual bool IsEnabled() const {
			if(alwaysenabled)
				return true;

			if(!parent.IsEnabled())
				return false;

			return ContainerBase::IsEnabled();
		}

		virtual bool IsVisible() const  {
			if(parent.IsVisible())
				return false;

			return BaseLayer.IsVisible;
		}

		virtual void Show(bool setfocus=false)  {
			ContainerBase::Show(setfocus);

			BaseLayer.IsVisible=true;
		}

		virtual void Hide()  {
			BaseLayer.IsVisible=false;
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

		virtual widgets::ExtenderLayer &CreateExtenderLayer()  {
			widgets::ExtenderLayer *layer=new widgets::ExtenderLayer;
			BaseLayer.Add(layer, BaseLayer.SubLayers.LowestOrder()-1);

			return *layer;
		}

		virtual widgets::WidgetLayer &CreateExtenderWidgetLayer()  {
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

		void AttachTo(LayerBase *layer, int order=0) {
			if(layer) {
				layer->Add(this->BaseLayer, order);
			}
			else {
				this->BaseLayer.parent=NULL;
			}
		}
		void AttachTo(LayerBase &layer, int order=0) {
			AttachTo(&layer, order);
		}

		operator LayerBase &() {
			return BaseLayer;
		}

		void InformEnabledChange(bool state) {
			for(auto it=Widgets.First();it.IsValid();it.Next()) {
				call_widget_containerenabledchanged(*it, state);
			}
		}

		std::map<input::keyboard::Key, WidgetBase*> &GetAccessKeys() {
			return AccessKeys;
		}


		bool KeyboardEvent(input::keyboard::Event event) { 
			if(Focused) {
				if(Focused->KeyboardHandler(event.event, event.keycode))
					return true;
			}

			return false;
		}

		LayerBase BaseLayer;

		bool alwaysenabled;

	protected:

		bool isactive;
		T_ &parent;

	};

	template<class T_>
	class ExtendedPetContiner : public PetContainer<T_> {
	public:
		LayerBase Extender;

		ExtendedPetContiner(T_ &parent) : PetContainer<T_>(parent), extenderbase(NULL) {

		}

		void AttachTo(LayerBase *layer, ExtenderLayer *extender, int order=0) {
			if(layer && extender) {
				utils::CheckAndDelete(extenderbase);
				extenderbase=extender;
				layer->Add(this->BaseLayer, order);
				extender->Add(this->Extender, order);
			}
			else {
				this->BaseLayer.parent=NULL;
				Extender.parent=NULL;
				utils::CheckAndDelete(extenderbase);
			}
		}
		void AttachTo(LayerBase &layer, LayerBase &extender, int order=0) {
			AttachTo(&layer, &extender, order);
		}

		virtual widgets::ExtenderLayer &CreateExtenderLayer()  {
			widgets::ExtenderLayer *layer=new widgets::ExtenderLayer;
			Extender.Add(layer, Extender.SubLayers.LowestOrder()-1);

			return *layer;
		}

		virtual widgets::WidgetLayer &CreateExtenderWidgetLayer()  {
			widgets::WidgetLayer *layer=new widgets::WidgetLayer;
			Extender.Add(layer, Extender.SubLayers.LowestOrder()-1);

			return *layer;
		}

		ExtenderLayer *extenderbase;

	};

}}
