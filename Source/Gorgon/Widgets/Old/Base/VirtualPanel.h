#pragma once


#include "Container.h"
#include "../../Engine/Input.h"


namespace gge { namespace widgets {

	class VirtualPanel;

	void deactivatetoplevels(VirtualPanel *except);
	extern utils::Collection<VirtualPanel> toplevels;
	extern VirtualPanel *activevp;

	class VirtualPanel : public ContainerBase {
		friend void Initialize(GGEMain &Main,int);
	public:

		VirtualPanel() : 
			ContainerBase(), BaseLayer(NULL), 
			BackgroundLayer(), WidgetLayer(), 
			ExtenderLayer(), isactive(false)
		{
			KeyboardToken=input::keyboard::Events.Register(this, &VirtualPanel::KeyboardEvent);
			input::keyboard::Events.Disable(KeyboardToken);
			toplevels.Add(this);
		}

		VirtualPanel(int) : 
		ContainerBase(), BaseLayer(NULL), 
			BackgroundLayer(), WidgetLayer(), 
			ExtenderLayer(), isactive(false)
		{
		}

		void init() {
			KeyboardToken=input::keyboard::Events.Register(this, &VirtualPanel::KeyboardEvent);
			input::keyboard::Events.Disable(KeyboardToken);

			Main.BeforeTerminateEvent.Register(this, &VirtualPanel::terminate);
			toplevels.Add(this);
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

		virtual bool IsActive() const  {
			return isactive;
		}

		virtual void Deactivate()  {
			if(!isactive) return;
			if(Focused)
				RemoveFocus();

			if(KeyboardToken!=input::keyboard::Events.NullToken)
				input::keyboard::Events.Disable(KeyboardToken);
			isactive=false;
			activevp=NULL;
		}

		virtual bool IsVisible() const  {
			if(!BaseLayer)
				return false;

			return ContainerBase::IsVisible();
		}

		virtual void Show(bool setfocus=false)  {
			if(BaseLayer)
				BaseLayer->IsVisible=true;

			ContainerBase::Show(setfocus);
		}

		virtual void Hide()  {
			Deactivate();

			if(BaseLayer)
				BaseLayer->IsVisible=false;

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

		virtual widgets::ExtenderLayer &CreateExtenderLayer()  {
			widgets::ExtenderLayer *layer=new widgets::ExtenderLayer;
			ExtenderLayer.Add(layer, ExtenderLayer.SubLayers.LowestOrder()-1);

			return *layer;
		}

		virtual widgets::WidgetLayer &CreateExtenderWidgetLayer()  {
			widgets::WidgetLayer *layer=new widgets::WidgetLayer;
			ExtenderLayer.Add(layer, ExtenderLayer.SubLayers.LowestOrder()-1);

			return *layer;
		}

		virtual void focus_changed(WidgetBase *newwidget)  {
			if(KeyboardToken==input::keyboard::Events.NullToken)
				return;

			if(newwidget) {
				deactivatetoplevels(this);
				input::keyboard::Events.Enable(KeyboardToken);
				isactive=true;
				activevp=this;
			}
			else {
				Deactivate();
			}
		}


		bool KeyboardEvent(input::keyboard::Event event) { return ContainerBase::DistributeKeyboardEvent(event.event, event.keycode); }

		virtual ~VirtualPanel() {
			if(KeyboardToken!=input::keyboard::Events.NullToken) {
				Deactivate();
				input::keyboard::Events.Unregister(KeyboardToken);
				toplevels.Remove(this);
			}
		}

		virtual utils::Point AbsoluteLocation()  {
			if(HasBaseLayer()) {
				return BaseLayer->BoundingBox.TopLeft();
			}
			else {
				return utils::Point(0,0);
			}
		}

		void SetOrder(int order) {
			if(BaseLayer)
				BaseLayer->SetOrder(order);
		}


	protected:
		LayerBase *BaseLayer;
		LayerBase BackgroundLayer;
		LayerBase WidgetLayer;
		LayerBase ExtenderLayer;


		bool isactive;

		utils::ConsumableEvent<>::Token KeyboardToken;

		void terminate() {
			Deactivate();
			input::keyboard::Events.Unregister(KeyboardToken);
			KeyboardToken=input::keyboard::Events.NullToken;
			toplevels.Remove(this);
			while(auto it=Widgets.First()) {
				if(it->BoundToContainer)
					it.Delete();
				else
					it->Detach();
			}
		}


	};

}}
