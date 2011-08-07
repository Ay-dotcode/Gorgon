#pragma once


#include "Container.h"
#include "..\..\Engine\Input.h"


namespace gge { namespace widgets {

	class VirtualPanel : public ContainerBase {
	public:

		VirtualPanel() : 
			ContainerBase(), BaseLayer(NULL), 
			BackgroundLayer(), WidgetLayer(), 
			ExtenderLayer(), isactive(false)
		{
			KeyboardToken=input::keyboard::Events.Register(this, &VirtualPanel::KeyboardEvent);
			input::keyboard::Events.Disable(KeyboardToken);
		}


		LayerBase *GetBaseLayer() {
			return BaseLayer;
		}

		bool HasBaseLayer() {
			return BaseLayer!=NULL;
		}

		void LandOn(LayerBase &layer) {
			BaseLayer=&layer;
			Resize(BaseLayer->BoundingBox.GetSize());
			BaseLayer->Add(BackgroundLayer, 1);
			BaseLayer->Add(WidgetLayer, 0);
			BaseLayer->Add(ExtenderLayer, -1);
		}

		virtual void Resize(utils::Size Size) {
			ContainerBase::Resize(Size);
			if(BaseLayer)
				BaseLayer->Resize(Size);

			BackgroundLayer.Resize(Size);
			WidgetLayer.Resize(Size);
			ExtenderLayer.Resize(Size);
		}

		void Detach() {
			if(BaseLayer) {
				BaseLayer->Remove(BackgroundLayer);
				BaseLayer->Remove(WidgetLayer);
				BaseLayer->Remove(ExtenderLayer);
				BaseLayer=NULL;
			}
		}

		virtual bool IsActive()  {
			return isactive;
		}

		virtual void Deactivate()  {
			input::keyboard::Events.Disable(KeyboardToken);
			isactive=false;
		}

		virtual bool IsVisible()  {
			if(!BaseLayer)
				return false;

			return ContainerBase::IsVisible();
		}

		virtual void Show(bool setfocus=false)  {
			if(BaseLayer)
				BaseLayer->isVisible=true;

			ContainerBase::Show(setfocus);
		}

		virtual void Hide()  {
			if(BaseLayer)
				BaseLayer->isVisible=false;

			ContainerBase::Hide();
		}

		virtual LayerBase &CreateBackgroundLayer()  {
			LayerBase *layer=new LayerBase;
			BackgroundLayer.Add(layer, BackgroundLayer.SubLayers.HighestOrder()+1);

			return *layer;
		}

		virtual widgets::WidgetLayer &CreateWidgetLayer(int Order=0)  {
			widgets::WidgetLayer *layer=new widgets::WidgetLayer;
			WidgetLayer.Add(layer, Order);

			return *layer;
		}

		virtual widgets::WidgetLayer &CreateExtenderLayer()  {
			widgets::WidgetLayer *layer=new widgets::WidgetLayer;
			ExtenderLayer.Add(layer, ExtenderLayer.SubLayers.LowestOrder()-1);

			return *layer;
		}

		virtual void focus_changed(WidgetBase *newwidget)  {
			if(newwidget) {
				input::keyboard::Events.Enable(KeyboardToken);
				isactive=true;
			}
			else {
				Deactivate();
			}
		}


		bool KeyboardEvent(input::keyboard::Event event) { return ContainerBase::DistributeKeyboardEvent(event.event, event.keycode); }

		virtual ~VirtualPanel() {
			input::keyboard::Events.Unregister(KeyboardToken);
		}


	protected:
		LayerBase *BaseLayer;
		LayerBase BackgroundLayer;
		LayerBase WidgetLayer;
		LayerBase ExtenderLayer;


		bool isactive;

		utils::ConsumableEvent<>::Token KeyboardToken;


	};

}}
