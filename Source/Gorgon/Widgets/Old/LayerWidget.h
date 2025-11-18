#pragma once
#include "Base/Widget.h"
#include "../Engine/GraphicLayers.h"


namespace gge { namespace widgets {

	class LayerWidget : public WidgetBase {
	public:

		LayerWidget() {
			Resize(100,100);
			layer.ClippingEnabled=true;
		}

		virtual bool MouseHandler(input::mouse::Event::Type event, utils::Point location, int amount)  {
			return !input::mouse::Event::isScroll(event);
		}


		virtual bool Focus() {
			return false;
		}

		using WidgetBase::SetBlueprint;
		virtual void SetBlueprint(const Blueprint &bp) {

		}

		using WidgetBase::Resize;
		virtual void Resize(utils::Size Size)  {
			WidgetBase::Resize(Size);

			layer.Resize(Size);
		}

		operator WidgetLayer() {
			return layer;
		}

		WidgetLayer &GetLayer() {
			return layer;
		}


	protected:

		virtual bool detach(ContainerBase *container)  {
			layer.parent=NULL;

			return WidgetBase::detach(container);
		}

		virtual void draw()  {
			
		}

		virtual void located(ContainerBase* container, utils::SortedCollection<WidgetBase>::Wrapper *w, int Order) {
			WidgetBase::located(container, w, Order);

			if(BaseLayer)
				BaseLayer->Add(layer);
			else
				layer.parent=NULL;
		}

		WidgetLayer layer;
	};


}}
