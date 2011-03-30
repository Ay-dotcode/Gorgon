#pragma once

#include "../Utils/Collection.h"
#include "../Utils/LinkedList.h"

#include "Input.h"

namespace gge {
	////This class is the base class for all layer types.
	/// All common methods are implemented with a common way.
	/// This class always implements basic processing functions.
	/// Derived classes can override them. Unless overridden these
	/// basic functions only propagate the call.
	class LayerBase {
	public:
		////Whether this layer is visible, invisible layers will not
		/// be drawn or receive any events
		bool isVisible;
		////Sub-layers that this layer holds, all the sub-layers are
		/// considered to be above current layer
		utils::LinkedList<LayerBase> SubLayers;

		////Parent layer
		LayerBase *parent;
		////Main class for GGE
		//GGEMain *main;

		LayerBase() : parent(NULL) { }

		virtual LayerBase *Add(LayerBase *layer, int Order=0) { if(layer->parent) layer->parent->Remove(layer); layer->parent=this; SubLayers.AddItem(layer, (float)Order); return layer; }
		virtual LayerBase &Add(LayerBase &layer, int Order=0) { if(layer.parent) layer.parent->Remove(layer); layer.parent=this; SubLayers.AddItem(&layer, (float)Order); return layer; }
		virtual void Remove(LayerBase *layer) { SubLayers.Remove(layer); }
		virtual void Remove(LayerBase &layer) { SubLayers.Remove(&layer); }
		////Size of layer
		int W;
		////Size of layer
		int H;
		////Position of layer
		int X;
		////Position of layer
		int Y;
		////Renders the current layer, default handling is to pass
		/// the request to the sub-layers
		virtual void Render();

		void setOrder(int Order) {
			if(parent)
				parent->SubLayers.FindListItem(this)->setOrder((float)Order);
		}

		void OrderToTop() {
			if(parent)
				parent->SubLayers.FindListItem(this)->setOrder(parent->SubLayers.LowestOrder()-1);
		}

		int getOrder() { 
			if(parent)
				return (int)parent->SubLayers.FindListItem(this)->getOrder(); 
			else
				return 0;
		}

		void Move(int X,int Y) {
			this->X=X;
			this->Y=Y;
		}

		void Resize(int W, int H) {
			this->W=W;
			this->H=H;
		}

		void SetRectangle(Rectangle rect) {
			X=rect.Left;
			Y=rect.Top;
			W=rect.Width;
			H=rect.Height;
		}

		void SetRectangle(int Left, int Top, int Width, int Height) {
			X=Left;
			Y=Top;
			W=Width;
			H=Height;
		}

		virtual ~LayerBase() {
			if(parent)
				parent->Remove(this);
		}

	protected:
		////Processes the mouse event for the current layer, default
		/// handling is to pass the request to the sub-layers
		virtual bool PropagateMouseEvent(input::MouseEventType event, int x, int y, void *data);
		virtual bool PropagateMouseScrollEvent(int amount, input::MouseEventType event, int x, int y, void *data);
	};

	class InputLayer : public LayerBase, public input::BasicPointerTarget {
	public:
		InputLayer(int X, int Y, int W, int H) : LayerBase() {
			this->X=X;
			this->Y=Y;
			this->W=W;
			this->H=H;
		}
		virtual bool PropagateMouseEvent(input::MouseEventType event, int x, int y, void *data);
		virtual bool PropagateMouseScrollEvent(int amount, input::MouseEventType event, int x, int y, void *data);
	};
}
