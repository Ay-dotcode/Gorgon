#pragma once
#include "Base/Widget.h"
#include "../Engine/GraphicLayers.h"


namespace gge { namespace widgets {

	class LayerWidget : public WidgetBase {
	public:

		LayerWidget() {
			Resize(100,100);
			layer.EnableClipping=true;
		}

		virtual bool MouseEvent(input::mouse::Event::Type event, utils::Point location, int amount)  {
			return true;
		}

		using WidgetBase::SetBlueprint;
		virtual void SetBlueprint(const Blueprint &bp) {

		}

		using WidgetBase::Resize;
		virtual void Resize(utils::Size Size)  {
			WidgetBase::Resize(Size);

			layer.Resize(Size);
		}

		operator LayerBase() {
			return layer;
		}

		LayerBase &GetLayer() {
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

		graphics::Basic2DLayer layer;
	};


}}
