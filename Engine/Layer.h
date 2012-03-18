#pragma once

#include "../Utils/Collection.h"
#include "../Utils/Point2D.h"
#include "../Utils/SortedCollection.h"

#include "Input.h"
#include "../Utils/Size2D.h"
#include "../Utils/Rectangle2D.h"
#include "../Utils/Bounds2D.h"

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
		utils::SortedCollection<LayerBase> SubLayers;

		////Parent layer
		LayerBase *parent;
		utils::SortedCollection<LayerBase>::Wrapper   *wrapper;
		utils::Bounds BoundingBox;

		LayerBase();

		LayerBase(const utils::Bounds &b) : 
			parent(NULL), 
			wrapper(NULL), 
			BoundingBox(b),
			isVisible(true)
		{ }

		LayerBase(int L, int T, int R, int B) : 
			parent(NULL), 
			wrapper(NULL), 
			BoundingBox(L,T,R,B),
			isVisible(true)
		{ }

		LayerBase(int X,int Y);

		LayerBase(const utils::Point &p);

		virtual LayerBase *Add(LayerBase *layer, int Order=0) {
			if(layer->parent) 
				layer->parent->Remove(layer);

			layer->parent=this;
			layer->wrapper=&SubLayers.Add(layer, Order); 

			return layer; 
		}
		virtual LayerBase &Add(LayerBase &layer, int Order=0) { 
			return *Add(&layer,Order);
		}
		virtual void Remove(LayerBase *layer) { 
			SubLayers.Remove(layer);
			layer->parent=NULL;
			layer->wrapper=NULL;
		}
		virtual void Remove(LayerBase &layer) { 
			SubLayers.Remove(&layer); 
			layer.parent=NULL;
			layer.wrapper=NULL;
		}
		
		////Renders the current layer, default handling is to pass
		/// the request to the sub-layers
		virtual void Render();

		void SetOrder(int Order) {
			if(parent && wrapper)
				wrapper->Reorder(Order);
		}

		void OrderToTop() {
			if(parent && wrapper)
				wrapper->Reorder(parent->SubLayers.LowestOrder()-1);
		}

		void OrderToBottom() {
			if(parent && wrapper)
				wrapper->Reorder(parent->SubLayers.HighestOrder()+1);
		}

		int GetOrder() const { 
			if(parent && wrapper)
				return wrapper->GetKey(); 
			else
				return 0;
		}

		void Move(int X,int Y) {
			BoundingBox.MoveTo(X,Y);
		}

		void Resize(int W, int H) {
			BoundingBox.SetSize(W, H);
		}

		void Move(const utils::Point &p) {
			BoundingBox.MoveTo(p);
		}

		void Resize(const utils::Size &s) {
			BoundingBox.SetSize(s);
		}

		void SetRectangle(utils::Rectangle rect) {
			BoundingBox=rect;
		}

		void SetRectangle(int Left, int Top, int Width, int Height) {
			BoundingBox=utils::Rectangle(Left, Top, Width, Height);
		}

		void SetBounds(int Left, int Top, int Right, int Bottom) {
			BoundingBox=utils::Bounds(Left, Top, Right, Bottom);
		}

		virtual ~LayerBase();

		////Processes the mouse event for the current layer, default
		/// handling is to pass the request to the sub-layers
		virtual bool PropagateMouseEvent(input::mouse::Event::Type event, utils::Point location, int amount);

	protected:
	};

	class InputLayer : public LayerBase, public input::mouse::EventProvider {
	public:
 		InputLayer() : LayerBase() { }

		InputLayer(const utils::Bounds &b) : LayerBase(b) { }

		InputLayer(int L, int T, int R, int B) : LayerBase(L,T,R,B) { }

		InputLayer(int X,int Y) : LayerBase(X,Y) { }

		InputLayer(const utils::Point &p) : LayerBase(p) { }

		virtual bool PropagateMouseEvent(input::mouse::Event::Type event, utils::Point location, int amount);
	};
}
