#pragma once

#include "Containers/Collection.h"
#include "Geometry/Point.h"
#include "Geometry/Bounds.h"

namespace Gorgon {
	/// This class is the base class for all layer types.
	/// All common methods are implemented with a common way.
	/// This class always implements basic processing functions.
	/// Derived classes can override them. Unless overridden these
	/// basic functions only propagate the call. Copying a layer is
	/// dangerous and therefore disabled. @nosubgrouping
	class Layer {
	public:
		
		/// Initializing constructor
		Layer(const Geometry::Bounds &bounds) :
			parent(nullptr), 
			bounds(bounds),
			isvisible(true),
			Children(children)
		{ }

		/// Constructor that sets the layer to cover entire parent, no matter how big it is. The
		/// location of the layer is set to be the origin
		Layer() : Layer(EntireRegion) { }

		/// Constructor that places the layer to the given location
		Layer(const Geometry::Point &location) :
		Layer({location, Geometry::Size::Max()})
		{ }
		
		/// Copy constructor is disabled.
		Layer(const Layer&) = delete;
		
		/// Move constructor
		Layer(Layer &&other) : Children(children) {
			Swap(other);
		}
		
		/// Copy assignment is deleted
		Layer &operator =(const Layer &)=delete;
		
		/// Move assignment
		Layer &operator =(Layer &&other) {
			if(parent)
				parent->Remove(this);
			
			children.Clear();
			
			Swap(other);
		}

		/// Destructor
		virtual ~Layer() {
			if(parent)
				parent->Remove(this);
		}
		
		/// Swaps two layers, mostly used for move semantics
		void Swap(Layer &other) {
			using std::swap;
			
			swap(bounds, other.bounds);
			swap(isvisible, other.isvisible);
			swap(children, other.children);
			
			if(parent==other.parent) return;
			
			if(parent) {
				parent->Remove(this);
			}
			if(other.parent) {
				other.parent->Remove(other);
				other.parent->Add(this);
				if(parent)
					parent->Add(other);
			}
		}

		/// @name Children related functions
		/// @{
		/// These functions deals with child layers.
		
		/// Adds the given layer as a child. Notice that the newly added
		/// layer is drawn on top of others.
		void Add(Layer &layer) {
			if(layer.parent) 
				layer.parent->Remove(layer);

			layer.parent=this;
			children.Add(layer);
			layer.located(this);
			
			added(layer);
		}

		/// Adds the given layer as a child. Notice that the newly added
		/// layer is drawn on top of others.
		void Add(Layer *layer) {
			if(!layer) return;
			
			Add(*layer);
		}
		
		/// Inserts the given layer before the given index. The newly inserted
		/// layer will be drawn *under* the layer in the given index
		void Insert(Layer &layer, int before) {
			if(layer.parent) 
				layer.parent->Remove(layer);

			layer.parent=this;
			children.Insert(layer, before);
			layer.located(this);
			
			added(layer);
		}
		
		/// Inserts the given layer before the given index. The newly inserted
		/// layer will be drawn *under* the layer in the given index
		void Insert(Layer *layer, int before) {
			if(!layer) return;
			
			Insert(*layer, before);
		}

		/// Removes the given layer
		void Remove(Layer &layer) {
			children.Remove(layer);
			
			removed(layer);
		}
		
		/// Removes the given layer
		void Remove(Layer *layer) {
			if(!layer) return;
			
			Remove(*layer);
			layer.located(nullptr);
		}
		
		/// Returns whether this layer has a parent
		bool HasParent() const {
			return parent!=nullptr;
		}
		
		/// Returns the parent of this layer. Throws if no parent is set. Use HasParent
		/// to make sure this layer has a parent
		/// @throw std::runtime_error if no parent is set
		virtual Layer &GetParent() const {
			if(!parent) throw std::runtime_error("No parent is set");
			
			return *parent;
		}
		/// @}
		
		/// @name Iterator related functions
		/// @{
		/// These functions return an iterator that allows children to be iterated.
		
		/// An iterator pointing to the start of the children
		Containers::Collection<Layer>::ConstIterator begin() const {
			return children.begin();
		}
		
		/// An iterator pointing to the end of the children
		Containers::Collection<Layer>::ConstIterator end() const {
			return children.end();
		}
		
		/// An iterator pointing to the start of the children
		Containers::Collection<Layer>::ConstIterator First() const {
			return children.First();
		}
		
		/// An iterator pointing to the last item of the children
		Containers::Collection<Layer>::ConstIterator Last() const {
			return children.Last();
		}
		/// @}

		
		/// @name Ordering functions
		/// @{
		/// This layer should be a in another layer for these methods to work.

		/// Places this layer before the given index. This layer will be drawn
		/// above the given index.
		void PlaceBefore(int before) {
			if(parent) {
				if(before==parent->children.GetCount()) before --;
				
				parent->children.MoveBefore(this, before++);
			}
		}

		/// Places this layer to the top of the layer stack its in. This makes
		/// sure that its not being covered by other layers apart from its own
		/// children
		void PlaceToTop() {
			if(parent)
				parent->children.MoveBefore(this, parent->children.GetCount());
		}

		/// Places this layer to the bottom of the layer stack.
		void PlaceToBottom() {
			if(parent)
				parent->children.MoveBefore(this, (unsigned)0);
		}

		/// Gets the current order of the stack. 0 means the layer is the bottommost
		/// layer in the stack
		int GetOrder() const { 
			if(parent)
				return parent->children.FindLocation(this);
			else
				return -1;
		}
		/// @}

		/// @name Geometry related functions
		/// @{
		/// These functions modify or return the boundaries of this layer
		
		/// Moves this layer to the given location
		void Move(const Geometry::Point &location) {
			bounds.Move(location);
		}

		/// Resizes the layer to the given size
		void Resize(const Geometry::Size &size) {
			bounds.Resize(size);
		}

		/// Sets the boundaries of this layer.
		void SetBounds(const Geometry::Bounds &bounds) {
			this->bounds=bounds;
		}
		
		/// Returns the size of the layer
		Geometry::Size GetSize() const {
			return bounds.GetSize();
		}
		
		/// Returns the current location of the layer
		Geometry::Point GetLocation() const {
			return bounds.TopLeft();
		}
		
		/// Retuns the boundaries of the layer
		Geometry::Bounds GetBounds() const {
			return bounds;
		}
		/// @}

		/// Displays this window, may generate Activated event
		virtual void Show() { isvisible=true; }

		/// Hides this window, may generate Deactivated event
		virtual void Hide() { isvisible=false; }
		
		bool IsVisible() { 
			if(parent && !parent->isvisible) return false;
			
			return isvisible;
		}
		
		////Renders the current layer, default handling is to pass
		/// the request to the sub-layers
		virtual void Render();
		
		/// Sub-layers that this layer holds, all the sub-layers are
		/// considered to be above current layer
		const Containers::Collection<Layer> &Children;
		
		/// When used as bounds, represents the entire region its placed in.
		static const Geometry::Bounds EntireRegion;

	protected:
		/// Will be called when a layer is added. This function will even be called
		/// when the given layer was already in the children.
		virtual void added(Layer &layer) { }
		
		/// Will be called when a layer is removed. This function will be called even
		/// if the given layer is not a child of this layer.
		virtual void removed(Layer &layer) { }
		
		/// Will be called when this layer is added to another.
		virtual void located(Layer *parent) { }
		
		/// Child layers that this layer holds, all child layers are
		/// considered to be above current layer
		Containers::Collection<Layer> children;
		
		/// Parent layer, could be nullptr
		Layer *parent;

		/// Bounds of this layer.
		Geometry::Bounds bounds;

		/// Whether this layer is visible, invisible layers will not
		/// be drawn or receive any events
		bool isvisible;
	};

}
