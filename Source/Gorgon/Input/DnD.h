#pragma once

#include <string>

#include "./Layer.h"
#include "../Resource/GID.h"


namespace Gorgon { 

    
    /**
     * Base object for data to be exchanged. This object should be type casted to derived type to access
     * stored data. Name could be used to show the data type.
     */
    class ExchangeData { // this class will be moved elsewhere
    public:
        virtual ~ExchangeData() { }
        
        /// Should return the name of the data type
        virtual std::string         Name() const = 0;
        
        /// Should return the type id of the data
        virtual Resource::GID::Type Type() const = 0;
    };
    
    /**
     * Stores text data for data exchange
     */
    class TextData : public ExchangeData {
    public:
        explicit TextData(std::string text = "") : text(text) { }
        
        /// Changes the text in this data
        void SetText(std::string value) {
            text = value;
        }
        
        /// Returns the text in this data
        std::string GetText() const {
            return text;
        }
        
        virtual std::string Name() const override {
            return "Text";
        }
        
        virtual Resource::GID::Type Type() const override {
            return Resource::GID::Text;
        }
        
    private:
        std::string text;
    };
    
namespace Input {
    
    /**
     * @page "Drag & Drop"
     * Drag and drop facilities in Gorgon Library has two starting points: within the system,
     * and from the operating system. Both systems can be handled the same way. To receive
     * drop and drag related events, a DropTarget should be created. This is a layer that works
     * very similarly to a regular input layer. However, it will not invoke any actions unless
     * an object is being dragged. 
     * While drag continues, source event will keep receiving move event. Targeted mouse event
     * will receive over event if hit test succeeds. If it accepts over event, target layer 
	 * will receive move and drop events. If mouse exits the current target area, out event will
     * fire. This will be fired even if over event returns false, allowing the destination
	 * to change and restore pointer shape. If target event handler accepts drop event, source 
	 * will receive accept event. If it is not accepted or there is no target that accepts the 
	 * data, source will receive cancel event. Any object is allowed to cancel drag operation.
	 * During move event, destination can return false to denote that the dragged object can
	 * no longer be dropped over it and out event of the source is called. If destination returns 
	 * false to drop event, drag operation will be canceled. If an operation is canceled and if
	 * the object is over a layer, out event is called before cancel event.
     */

   
    class DragInfo;
    
    /**
     * This layer acts as a drop target.
     */
    class DropTarget : public Gorgon::Layer {
    public:
        using Gorgon::Layer::Layer;

        //BEGIN Event handling
        
        /// @name Event handling
        /// @{

        /// Sets hit check function. When set, events only occur if hit check returns
        /// true. Events follow hit check in a sequential manner, thus, if a handler
        /// is called, this means hit check has already succeeded in the current
        /// layout.
        void SetHitCheck(std::function<bool(DropTarget &, Geometry::Point)> fn) {
            hitcheck = fn;
        }
        
        /// Sets hit check function. When set, events only occur if hit check returns
        /// true. Events follow hit check in a sequential manner, thus, if a handler
        /// is called, this means hit check has already succeeded in the current
        /// layout.
        void SetHitCheck(std::function<bool(Geometry::Point)> fn) {
            hitcheck = [fn](DropTarget &, Geometry::Point point) { return fn(point); };
        }

        /// Sets hit check function. When set, events only occur if hit check returns
        /// true. Events follow hit check in a sequential manner, thus, if a handler
        /// is called, this means hit check has already succeeded in the current
        /// layout. This variant accepts class member function.
        template<class C_>
        void SetHitCheck(C_ &c, std::function<bool(C_ &, DropTarget &, Geometry::Point)> fn) {
            C_ *my = &c;
            hitcheck = [fn, my](DropTarget &layer, Geometry::Point point) { return my->fn(layer, point); };
        }
        
        /// Sets hit check function. When set, events only occur if hit check returns
        /// true. Events follow hit check in a sequential manner, thus, if a handler
        /// is called, this means hit check has already succeeded in the current
        /// layout. This variant accepts class member function.
        template<class C_>
        void SetHitCheck(C_ &c, std::function<bool(C_ &, Geometry::Point)> fn) {
            C_ *my = &c;
            hitcheck = [fn, my](DropTarget &, Geometry::Point point) { return my->fn(point); };
        }

        /// Sets hit check function. When set, events only occur if hit check returns
        /// true. Events follow hit check in a sequential manner, thus, if a handler
        /// is called, this means hit check has already succeeded in the current
        /// layout. This variant accepts class member function.
        template<class C_>
        void SetHitCheck(C_ *my, std::function<bool(C_ &, DropTarget &, Geometry::Point)> fn) {
            hitcheck = [fn, my](DropTarget &layer, Geometry::Point point) { return my->fn(layer, point); };
        }
        
        /// Sets hit check function. When set, events only occur if hit check returns
        /// true. Events follow hit check in a sequential manner, thus, if a handler
        /// is called, this means hit check has already succeeded in the current
        /// layout. This variant accepts class member function.
        template<class C_>
        void SetHitCheck(C_ *my, std::function<bool(C_ &, Geometry::Point)> fn) {
            hitcheck = [fn, my](DropTarget &, Geometry::Point point) { return my->fn(point); };
        }
        
        /// Removes hit check handler, default action for hit check is to return true.
        void ResetHitCheck() {
            hitcheck = {};
        }



		/// Sets over function. If set, called whenever an object is dragged over this layer.
		/// If event handler returns true, source will receive over event as well.
		void SetOver(std::function<bool(DropTarget &, DragInfo &)> fn) {
			over = fn;
		}

		/// Sets over function. If set, called whenever an object is dragged over this layer.
		/// If event handler returns true, source will receive over event as well.
		void SetOver(std::function<bool(DragInfo &)> fn) {
			over = [fn](DropTarget &, DragInfo &data) { return fn(data); };
		}

		/// Sets over function. If set, called whenever an object is dragged over this layer.
		/// If event handler returns true, source will receive over event as well.
		/// This variant accepts class member function.
		template<class C_>
		void SetOver(C_ &c, std::function<bool(C_ &, DropTarget &, DragInfo &)> fn) {
			C_ *my = &c;
			over = [fn, my](DropTarget &layer, DragInfo &data) { return my->fn(layer, data); };
		}

		/// Sets over function. If set, called whenever an object is dragged over this layer.
		/// If event handler returns true, source will receive over event as well.
		/// This variant accepts class member function.
		template<class C_>
		void SetOver(C_ &c, std::function<bool(C_ &, DragInfo &)> fn) {
			C_ *my = &c;
			over = [fn, my](DropTarget &, DragInfo &data) { return my->fn(data); };
		}

		/// Sets over function. If set, called whenever an object is dragged over this layer.
		/// If event handler returns true, source will receive over event as well.
		/// This variant accepts class member function.
		template<class C_>
		void SetOver(C_ *my, std::function<bool(C_ &, DropTarget &, DragInfo &)> fn) {
			over = [fn, my](DropTarget &layer, DragInfo &data) { return my->fn(layer, data); };
		}

		/// Sets over function. If set, called whenever an object is dragged over this layer.
		/// If event handler returns true, source will receive over event as well.
		/// This variant accepts class member function.
		template<class C_>
		void SetOver(C_ *my, std::function<bool(C_ &, DragInfo &)> fn) {
			over = [fn, my](DropTarget &, DragInfo &data) { return my->fn(data); };
		}

		/// Removes over handler, default action for over is to return true.
		void ResetOver() {
			over = {};
		}



		/// Sets out function. If set, called whenever an object is dragged out of this layer.
		/// This event is called even if over returns false.
		void SetOut(std::function<void(DropTarget &, DragInfo &)> fn) {
			out = fn;
		}

		/// Sets out function. If set, called whenever an object is dragged out of this layer.
		/// This event is called even if over returns false.
		void SetOut(std::function<void(DragInfo &)> fn) {
			out = [fn](DropTarget &, DragInfo &data) { return fn(data); };
		}

		/// Sets out function. If set, called whenever an object is dragged out of this layer.
		/// This event is called even if over returns false.
		/// This variant accepts class member function.
		template<class C_>
		void SetOut(C_ &c, std::function<void(C_ &, DropTarget &, DragInfo &)> fn) {
			C_ *my = &c;
			out = [fn, my](DropTarget &layer, DragInfo &data) { return my->fn(layer, data); };
		}

		/// Sets out function. If set, called whenever an object is dragged out of this layer.
		/// This event is called even if over returns false.
		/// This variant accepts class member function.
		template<class C_>
		void SetOut(C_ &c, std::function<void(C_ &, DragInfo &)> fn) {
			C_ *my = &c;
			out = [fn, my](DropTarget &, DragInfo &data) { return my->fn(data); };
		}

		/// Sets out function. If set, called whenever an object is dragged out of this layer.
		/// This event is called even if over returns false.
		/// This variant accepts class member function.
		template<class C_>
		void SetOut(C_ *my, std::function<void(C_ &, DropTarget &, DragInfo &)> fn) {
			out = [fn, my](DropTarget &layer, DragInfo &data) { return my->fn(layer, data); };
		}

		/// Sets out function. If set, called whenever an object is dragged out of this layer.
		/// This event is called even if over returns false.
		/// This variant accepts class member function.
		template<class C_>
		void SetOut(C_ *my, std::function<void(C_ &, DragInfo &)> fn) {
			out = [fn, my](DropTarget &, DragInfo &data) { return my->fn(data); };
		}

		/// Removes out handler.
		void ResetOut() {
			out = {};
		}



		/// Sets move function. If set, called whenever an object is dragged move of this layer.
		/// Returning false from this event's handler will cause drag operation to move out of
		/// this layer.
		void SetMove(std::function<bool(DropTarget &, DragInfo &, Geometry::Point)> fn) {
			move = fn;
		}

		/// Sets move function. If set, called whenever an object is dragged move of this layer.
		/// Returning false from this event's handler will cause drag operation to move out of
		/// this layer.
		void SetMove(std::function<bool(DragInfo &, Geometry::Point)> fn) {
			move = [fn](DropTarget &, DragInfo &data, Geometry::Point p) { return fn(data, p); };
		}

		/// Sets move function. If set, called whenever an object is dragged move of this layer.
		/// Returning false from this event's handler will cause drag operation to move out of
		/// this layer.
		/// This variant accepts class member function.
		template<class C_>
		void SetMove(C_ &c, std::function<bool(C_ &, DropTarget &, DragInfo &, Geometry::Point)> fn) {
			C_ *my = &c;
			move = [fn, my](DropTarget &layer, DragInfo &data, Geometry::Point p) { return my->fn(layer, data, p); };
		}

		/// Sets move function. If set, called whenever an object is dragged move of this layer.
		/// Returning false from this event's handler will cause drag operation to move out of
		/// this layer.
		/// This variant accepts class member function.
		template<class C_>
		void SetMove(C_ &c, std::function<bool(C_ &, DragInfo &, Geometry::Point)> fn) {
			C_ *my = &c;
			move = [fn, my](DropTarget &, DragInfo &data, Geometry::Point p) { return my->fn(data, p); };
		}

		/// Sets move function. If set, called whenever an object is dragged move of this layer.
		/// Returning false from this event's handler will cause drag operation to move out of
		/// this layer.
		/// This variant accepts class member function.
		template<class C_>
		void SetMove(C_ *my, std::function<bool(C_ &, DropTarget &, DragInfo &, Geometry::Point)> fn) {
			move = [fn, my](DropTarget &layer, DragInfo &data, Geometry::Point p) { return my->fn(layer, data, p); };
		}

		/// Sets move function. If set, called whenever an object is dragged move of this layer.
		/// Returning false from this event's handler will cause drag operation to move out of
		/// this layer.
		/// This variant accepts class member function.
		template<class C_>
		void SetMove(C_ *my, std::function<bool(C_ &, DragInfo &, Geometry::Point)> fn) {
			move = [fn, my](DropTarget &, DragInfo &data, Geometry::Point p) { return my->fn(data, p); };
		}

		/// Sets move function. If set, called whenever an object is dragged move of this layer.
		/// Returning false from this event's handler will cause drag operation to move out of
		/// this layer.
		void SetMove(std::function<bool(DropTarget &, DragInfo &)> fn) {
			move = [fn](DropTarget &layer, DragInfo &data, Geometry::Point p) { return fn(layer, data); };
		}

		/// Sets move function. If set, called whenever an object is dragged move of this layer.
		/// Returning false from this event's handler will cause drag operation to move out of
		/// this layer.
		void SetMove(std::function<bool(DragInfo &)> fn) {
			move = [fn](DropTarget &, DragInfo &data, Geometry::Point p) { return fn(data); };
		}

		/// Sets move function. If set, called whenever an object is dragged move of this layer.
		/// Returning false from this event's handler will cause drag operation to move out of
		/// this layer.
		/// This variant accepts class member function.
		template<class C_>
		void SetMove(C_ &c, std::function<bool(C_ &, DropTarget &, DragInfo &)> fn) {
			C_ *my = &c;
			move = [fn, my](DropTarget &layer, DragInfo &data, Geometry::Point p) { return my->fn(layer, data); };
		}

		/// Sets move function. If set, called whenever an object is dragged move of this layer.
		/// Returning false from this event's handler will cause drag operation to move out of
		/// this layer.
		/// This variant accepts class member function.
		template<class C_>
		void SetMove(C_ &c, std::function<bool(C_ &, DragInfo &)> fn) {
			C_ *my = &c;
			move = [fn, my](DropTarget &, DragInfo &data, Geometry::Point p) { return my->fn(data); };
		}

		/// Sets move function. If set, called whenever an object is dragged move of this layer.
		/// Returning false from this event's handler will cause drag operation to move out of
		/// this layer.
		/// This variant accepts class member function.
		template<class C_>
		void SetMove(C_ *my, std::function<bool(C_ &, DropTarget &, DragInfo &)> fn) {
			move = [fn, my](DropTarget &layer, DragInfo &data, Geometry::Point p) { return my->fn(layer, data); };
		}

		/// Sets move function. If set, called whenever an object is dragged move of this layer.
		/// Returning false from this event's handler will cause drag operation to move out of
		/// this layer.
		/// This variant accepts class member function.
		template<class C_>
		void SetMove(C_ *my, std::function<bool(C_ &, DragInfo &)> fn) {
			move = [fn, my](DropTarget &, DragInfo &data, Geometry::Point p) { return my->fn(data); };
		}

		/// Removes move handler, default is to continue the drag operation.
		void ResetMove() {
			move ={};
		}



		/// Sets drop function. If set, called whenever an object is dropped to this layer.
		/// Returning false from this event's handler will cause drag operation to be canceled.
		void SetDrop(std::function<bool(DropTarget &, DragInfo &, Geometry::Point)> fn) {
			drop = fn;
		}

		/// Sets drop function. If set, called whenever an object is dropped to this layer.
		/// Returning false from this event's handler will cause drag operation to be canceled.
		void SetDrop(std::function<bool(DragInfo &, Geometry::Point)> fn) {
			drop = [fn](DropTarget &, DragInfo &data, Geometry::Point p) { return fn(data, p); };
		}

		/// Sets drop function. If set, called whenever an object is dropped to this layer.
		/// Returning false from this event's handler will cause drag operation to be canceled.
		/// This variant accepts class member function.
		template<class C_>
		void SetDrop(C_ &c, std::function<bool(C_ &, DropTarget &, DragInfo &, Geometry::Point)> fn) {
			C_ *my = &c;
			drop = [fn, my](DropTarget &layer, DragInfo &data, Geometry::Point p) { return my->fn(layer, data, p); };
		}

		/// Sets drop function. If set, called whenever an object is dropped to this layer.
		/// Returning false from this event's handler will cause drag operation to be canceled.
		/// This variant accepts class member function.
		template<class C_>
		void SetDrop(C_ &c, std::function<bool(C_ &, DragInfo &, Geometry::Point)> fn) {
			C_ *my = &c;
			drop = [fn, my](DropTarget &, DragInfo &data, Geometry::Point p) { return my->fn(data, p); };
		}

		/// Sets drop function. If set, called whenever an object is dropped to this layer.
		/// Returning false from this event's handler will cause drag operation to be canceled.
		/// This variant accepts class member function.
		template<class C_>
		void SetDrop(C_ *my, std::function<bool(C_ &, DropTarget &, DragInfo &, Geometry::Point)> fn) {
			drop = [fn, my](DropTarget &layer, DragInfo &data, Geometry::Point p) { return my->fn(layer, data, p); };
		}

		/// Sets drop function. If set, called whenever an object is dropped to this layer.
		/// Returning false from this event's handler will cause drag operation to be canceled.
		/// This variant accepts class member function.
		template<class C_>
		void SetDrop(C_ *my, std::function<bool(C_ &, DragInfo &, Geometry::Point)> fn) {
			drop = [fn, my](DropTarget &, DragInfo &data, Geometry::Point p) { return my->fn(data, p); };
		}

		/// Sets drop function. If set, called whenever an object is dropped to this layer.
		/// Returning false from this event's handler will cause drag operation to be canceled.
		void SetDrop(std::function<bool(DropTarget &, DragInfo &)> fn) {
			drop = [fn](DropTarget &layer, DragInfo &data, Geometry::Point p) { return fn(layer, data); };
		}

		/// Sets drop function. If set, called whenever an object is dropped to this layer.
		/// Returning false from this event's handler will cause drag operation to be canceled.
		void SetDrop(std::function<bool(DragInfo &)> fn) {
			drop = [fn](DropTarget &, DragInfo &data, Geometry::Point) { return fn(data); };
		}

		/// Sets drop function. If set, called whenever an object is dropped to this layer.
		/// Returning false from this event's handler will cause drag operation to be canceled.
		/// This variant accepts class member function.
		template<class C_>
		void SetDrop(C_ &c, std::function<bool(C_ &, DropTarget &, DragInfo &)> fn) {
			C_ *my = &c;
			drop = [fn, my](DropTarget &layer, DragInfo &data, Geometry::Point) { return my->fn(layer, data); };
		}

		/// Sets drop function. If set, called whenever an object is dropped to this layer.
		/// Returning false from this event's handler will cause drag operation to be canceled.
		/// This variant accepts class member function.
		template<class C_>
		void SetDrop(C_ &c, std::function<bool(C_ &, DragInfo &)> fn) {
			C_ *my = &c;
			drop = [fn, my](DropTarget &, DragInfo &data, Geometry::Point) { return my->fn(data); };
		}

		/// Sets drop function. If set, called whenever an object is dropped to this layer.
		/// Returning false from this event's handler will cause drag operation to be canceled.
		/// This variant accepts class member function.
		template<class C_>
		void SetDrop(C_ *my, std::function<bool(C_ &, DropTarget &, DragInfo &)> fn) {
			drop = [fn, my](DropTarget &layer, DragInfo &data, Geometry::Point) { return my->fn(layer, data); };
		}

		/// Sets drop function. If set, called whenever an object is dropped to this layer.
		/// Returning false from this event's handler will cause drag operation to be canceled.
		/// This variant accepts class member function.
		template<class C_>
		void SetDrop(C_ *my, std::function<bool(C_ &, DragInfo &)> fn) {
			drop = [fn, my](DropTarget &, DragInfo &data, Geometry::Point) { return my->fn(data); };
		}

		/// Removes drop handler, default is to cancel the drag operation.
		void ResetDrop() {
			drop = {};
		}



		/// Sets cancel function. If set, called whenever the DnD event that is accepted to be over
		/// this layer is canceled.
		void SetCancel(std::function<void(DropTarget &, DragInfo &)> fn) {
			cancel = fn;
		}

		/// Sets cancel function. If set, called whenever the DnD event that is accepted to be over
		/// this layer is canceled.
		void SetCancel(std::function<void(DragInfo &)> fn) {
			cancel = [fn](DropTarget &, DragInfo &data) { return fn(data); };
		}

		/// Sets cancel function. If set, called whenever the DnD event that is accepted to be over
		/// this layer is canceled.
		/// This variant accepts class member function.
		template<class C_>
		void SetCancel(C_ &c, std::function<void(C_ &, DropTarget &, DragInfo &)> fn) {
			C_ *my = &c;
			cancel = [fn, my](DropTarget &layer, DragInfo &data) { return my->fn(layer, data); };
		}

		/// Sets cancel function. If set, called whenever the DnD event that is accepted to be over
		/// this layer is canceled.
		/// This variant accepts class member function.
		template<class C_>
		void SetCancel(C_ &c, std::function<void(C_ &, DragInfo &)> fn) {
			C_ *my = &c;
			cancel = [fn, my](DropTarget &, DragInfo &data) { return my->fn(data); };
		}

		/// Sets cancel function. If set, called whenever the DnD event that is accepted to be over
		/// this layer is canceled.
		/// This variant accepts class member function.
		template<class C_>
		void SetCancel(C_ *my, std::function<void(C_ &, DropTarget &, DragInfo &)> fn) {
			cancel = [fn, my](DropTarget &layer, DragInfo &data) { return my->fn(layer, data); };
		}

		/// Sets cancel function. If set, called whenever the DnD event that is accepted to be over
		/// this layer is canceled.
		/// This variant accepts class member function.
		template<class C_>
		void SetCancel(C_ *my, std::function<void(C_ &, DragInfo &)> fn) {
			cancel = [fn, my](DropTarget &, DragInfo &data) { return my->fn(data); };
		}

		/// Removes cancel handler.
		void ResetCancel() {
			cancel = {};
		}
        ///@}
        
        //END

    protected:
		std::function<bool(DropTarget &, Geometry::Point)>              hitcheck;
		std::function<bool(DropTarget &, DragInfo &)>					over;
		std::function<void(DropTarget &, DragInfo &)>					out;
		std::function<bool(DropTarget &, DragInfo &, Geometry::Point)>  move;
		std::function<bool(DropTarget &, DragInfo &, Geometry::Point)>  drop;
		std::function<void(DropTarget &, DragInfo &)>					cancel;
	};
    
	class DragSource {
    public:
        
    private:
		std::function<bool(DragSource &, DragInfo &)>					over;
		std::function<void(DragSource &, DragInfo &)>					out;
		std::function<bool(DragSource &, DragInfo &, Geometry::Point)>  move;
		std::function<bool(DragSource &, DragInfo &)>                   accept;
		std::function<void(DragSource &, DragInfo &)>					cancel;
	};
    
    /**
     * Contains information about a drag operation. Drag operations can contain multiple data types.
     */
    class DragInfo {
    public:
        /// Constructor, requires the source for drag operation
        DragInfo(DragSource &source) : source(&source) { }
        
        /// Adds text data to this info object
        void AddTextData(const std::string &text);
        
        /// Adds file data to this info object
        void AddFileData(const std::string &text);
        
        /// Check whether this drag info has the given data
        bool HasData(Resource::GID::Type type) const;
        
        /// Returns the data associated with the given type, throws runtime_error
        /// if data does not exists.
        ExchangeData &GetData(Resource::GID::Type type) const;
        
        /// Returns the data at the given index
        ExchangeData &operator [](int ind) const;
        
        /// Returns the number of data stored in this object
        int GetSize() const;
        
        /// If this drag operation has a target. The target should accept drag over
        /// event for it to be registered
        bool HasTarget() const;
        
        /// Returns the target of the drag operation. The target should accept drag 
        /// over event for it to be registered. Throws runtime_error if target does
        /// not exists
        DropTarget &GetTarget() const;
        
        /// Sets the target of the drag operation. This function is automatically
        /// called. Manually calling this function might have unintended consequences.
        void SetTarget(DropTarget &value);
        
        /// Removes the target of the drag operation. This function is automatically
        /// called. Manually calling this function might have unintended consequences.
        void RemoveTarget();
        
        /// Destructor
        ~DragInfo() {
            destroylist.Destroy();
        }
    private:
        Containers::Collection<ExchangeData> data;
        Containers::Collection<ExchangeData> destroylist;
        
        DragSource *source = nullptr;
        DropTarget *active = nullptr;
    };

} }
