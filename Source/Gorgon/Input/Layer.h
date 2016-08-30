#pragma once

#include "../Layer.h"
#include "../Geometry/Bounds.h"

namespace Gorgon { namespace Input {

        /**
         * Input layer allows mouse events to be handled. Location of mouse events are
         * normalized to the layer. If hit check is not set, entire region of the layer
         * is set to receive events. This means, if click is handled, any layer beneath
         * this layer will not receive mouse down event prior to click event. All event
         * functions may contain original layer, that parameter can also be skipped.
         * Handlers accept many different function signatures. For instance, if you
         * want to handle left click event with member function of class Button you can
         * register handler using the following line:
         * @code
         * layer.SetClick(this, 
         */
		class Layer : public Gorgon::Layer {
		public:
			using Gorgon::Layer::Layer;

            /// @name Mouse event handling
            /// @{

            /// Sets hit check function. When set, events only occur if hit check returns
            /// true. Events follow hit check in a sequential manner, thus, if a handler
            /// is called, this means hit check has already succeeded in the current
            /// layout.
            void SetHitCheck(std::function<bool(Layer &, Geometry::Point)> fn) {
                hitcheck = fn;
            }
            
            /// Sets hit check function. When set, events only occur if hit check returns
            /// true. Events follow hit check in a sequential manner, thus, if a handler
            /// is called, this means hit check has already succeeded in the current
            /// layout.
            void SetHitCheck(std::function<bool(Geometry::Point)> fn) {
                hitcheck = [fn](Layer &, Geometry::Point point) { return fn(point); };
            }

            /// Sets hit check function. When set, events only occur if hit check returns
            /// true. Events follow hit check in a sequential manner, thus, if a handler
            /// is called, this means hit check has already succeeded in the current
            /// layout. This variant accepts class member function.
            template<class C_>
            void SetHitCheck(C_ &c, std::function<bool(C_ &, Layer &, Geometry::Point)> fn) {
                C_ *my = &c;
                hitcheck = [fn, my](Layer &layer, Geometry::Point point) { return my->fn(layer, point); };
            }
            
            /// Sets hit check function. When set, events only occur if hit check returns
            /// true. Events follow hit check in a sequential manner, thus, if a handler
            /// is called, this means hit check has already succeeded in the current
            /// layout. This variant accepts class member function.
            template<class C_>
            void SetHitCheck(C_ &c, std::function<bool(C_ &, Geometry::Point)> fn) {
                C_ *my = &c;
                hitcheck = [fn, my](Layer &, Geometry::Point point) { return my->fn(point); };
            }

            /// Sets hit check function. When set, events only occur if hit check returns
            /// true. Events follow hit check in a sequential manner, thus, if a handler
            /// is called, this means hit check has already succeeded in the current
            /// layout. This variant accepts class member function.
            template<class C_>
            void SetHitCheck(C_ *my, std::function<bool(C_ &, Layer &, Geometry::Point)> fn) {
                hitcheck = [fn, my](Layer &layer, Geometry::Point point) { return my->fn(layer, point); };
            }
            
            /// Sets hit check function. When set, events only occur if hit check returns
            /// true. Events follow hit check in a sequential manner, thus, if a handler
            /// is called, this means hit check has already succeeded in the current
            /// layout. This variant accepts class member function.
            template<class C_>
            void SetHitCheck(C_ *my, std::function<bool(C_ &, Geometry::Point)> fn) {
                hitcheck = [fn, my](Layer &, Geometry::Point point) { return my->fn(point); };
            }
            
            /// Removes hit check handler, default action for hit check is to return true.
            void ResetHitCheck() {
                hitcheck = {};
            }

            
            
            /// Sets click handler. If hit check function is set, this event is only called
            /// if hit check returns true.
            void SetClick(std::function<void(Layer &, Geometry::Point, Input::Mouse::Button)> fn) {
                click = fn;
            }
            
            /// Sets click handler. If hit check function is set, this event is only called
            /// if hit check returns true.
            void SetClick(std::function<void(Geometry::Point, Input::Mouse::Button)> fn) {
                click = [fn](Layer &, Geometry::Point point, Input::Mouse::Button btn) { fn(point, btn); };
            }

            /// Sets click handler. If hit check function is set, this event is only called
            /// if hit check returns true. This variant accepts class member function.
            template<class C_>
            void SetClick(C_ &c, std::function<void(C_ &, Layer &, Geometry::Point, Input::Mouse::Button)> fn) {
                C_ *my = &c;
                click = [fn, my](Layer &layer, Geometry::Point point, Input::Mouse::Button btn) { my->fn(layer, point, btn); };
            }
            
            /// Sets click handler. If hit check function is set, this event is only called
            /// if hit check returns true. This variant accepts class member function.
            template<class C_>
            void SetClick(C_ &c, std::function<void(C_ &, Geometry::Point, Input::Mouse::Button)> fn) {
                C_ *my = &c;
                click = [fn, my](Layer &, Geometry::Point point, Input::Mouse::Button btn) { my->fn(point, btn); };
            }

            /// Sets click handler. If hit check function is set, this event is only called
            /// if hit check returns true. This variant accepts class member function.
            template<class C_>
            void SetClick(C_ *my, std::function<void(C_ &, Layer &, Geometry::Point, Input::Mouse::Button)> fn) {
                click = [fn, my](Layer &layer, Geometry::Point point, Input::Mouse::Button btn) { my->fn(layer, point, btn); };
            }
            
            /// Sets click handler. If hit check function is set, this event is only called
            /// if hit check returns true. This variant accepts class member function.
            template<class C_>
            void SetClick(C_ *my, std::function<void(C_ &, Geometry::Point, Input::Mouse::Button)> fn) {
                click = [fn, my](Layer &, Geometry::Point point, Input::Mouse::Button btn) { my->fn(point, btn); };
            }
            
            /// Sets click handler. If hit check function is set, this event is only called
            /// if hit check returns true.
            void SetClick(std::function<void(Layer &, Input::Mouse::Button)> fn) {
                click = [fn](Layer &layer, Geometry::Point, Input::Mouse::Button btn) { fn(layer, btn); };
            }
            
            /// Sets click handler. If hit check function is set, this event is only called
            /// if hit check returns true.
            void SetClick(std::function<void(Input::Mouse::Button)> fn) {
                click = [fn](Layer &, Geometry::Point, Input::Mouse::Button btn) { fn(btn); };
            }

            /// Sets click handler. If hit check function is set, this event is only called
            /// if hit check returns true. This variant accepts class member function.
            template<class C_>
            void SetClick(C_ &c, std::function<void(C_ &, Layer &, Input::Mouse::Button)> fn) {
                C_ *my = &c;
                click = [fn, my](Layer &layer, Input::Mouse::Button btn) { my->fn(layer, btn); };
            }
            
            /// Sets click handler. If hit check function is set, this event is only called
            /// if hit check returns true. This variant accepts class member function.
            template<class C_>
            void SetClick(C_ &c, std::function<void(C_ &, Input::Mouse::Button)> fn) {
                C_ *my = &c;
                click = [fn, my](Layer &, Geometry::Point, Input::Mouse::Button btn) { my->fn(btn); };
            }

            /// Sets click handler. If hit check function is set, this event is only called
            /// if hit check returns true. This variant accepts class member function.
            template<class C_>
            void SetClick(C_ *my, std::function<void(C_ &, Layer &, Input::Mouse::Button)> fn) {
                click = [fn, my](Layer &layer, Input::Mouse::Button btn) { my->fn(layer, btn); };
            }
            
            /// Sets click handler. If hit check function is set, this event is only called
            /// if hit check returns true. This variant accepts class member function.
            template<class C_>
            void SetClick(C_ *my, std::function<void(C_ &, Input::Mouse::Button)> fn) {
                click = [fn, my](Layer &, Geometry::Point, Input::Mouse::Button btn) { my->fn(btn); };
            }
            
            /// Sets click handler. If hit check function is set, this event is only called
            /// if hit check returns true.
            void SetClick(std::function<void(Layer &, Geometry::Point)> fn, Input::Mouse::Button btn = Input::Mouse::Button::Left) {
                click = [fn, btn](Layer &layer, Geometry::Point point, Input::Mouse::Button b) { if(b&&btn) fn(layer, point); };
            }
            
            /// Sets click handler. If hit check function is set, this event is only called
            /// if hit check returns true.
            void SetClick(std::function<void(Geometry::Point)> fn, Input::Mouse::Button btn = Input::Mouse::Button::Left) {
                click = [fn, btn](Layer &, Geometry::Point point, Input::Mouse::Button b) { if(b&&btn) fn(point); };
            }

            /// Sets click handler. If hit check function is set, this event is only called
            /// if hit check returns true. This variant accepts class member function.
            template<class C_>
            void SetClick(C_ &c, std::function<void(C_ &, Layer &, Geometry::Point)> fn, Input::Mouse::Button btn = Input::Mouse::Button::Left) {
                C_ *my = &c;
                click = [fn, my, btn](Layer &layer, Geometry::Point point, Input::Mouse::Button b) { if(b&&btn) my->fn(layer, point); };
            }
            
            /// Sets click handler. If hit check function is set, this event is only called
            /// if hit check returns true. This variant accepts class member function.
            template<class C_>
            void SetClick(C_ &c, std::function<void(C_ &, Geometry::Point)> fn, Input::Mouse::Button btn = Input::Mouse::Button::Left) {
                C_ *my = &c;
                click = [fn, my, btn](Layer &, Geometry::Point point, Input::Mouse::Button b) { if(b&&btn) my->fn(point); };
            }
 
            /// Sets click handler. If hit check function is set, this event is only called
            /// if hit check returns true. This variant accepts class member function.
            template<class C_>
            void SetClick(C_ *my, std::function<void(C_ &, Layer &, Geometry::Point)> fn, Input::Mouse::Button btn = Input::Mouse::Button::Left) {
                click = [fn, my, btn](Layer &layer, Geometry::Point point, Input::Mouse::Button b) { if(b&&btn) my->fn(layer, point); };
            }
            
            /// Sets click handler. If hit check function is set, this event is only called
            /// if hit check returns true. This variant accepts class member function.
            template<class C_>
            void SetClick(C_ *my, std::function<void(C_ &, Geometry::Point)> fn, Input::Mouse::Button btn = Input::Mouse::Button::Left) {
                click = [fn, my, btn](Layer &, Geometry::Point point, Input::Mouse::Button b) { if(b&&btn) my->fn(point); };
            }
            
            /// Sets click handler. If hit check function is set, this event is only called
            /// if hit check returns true.
            void SetClick(std::function<void(Layer &)> fn, Input::Mouse::Button btn = Input::Mouse::Button::Left) {
                click = [fn, btn](Layer &layer, Geometry::Point, Input::Mouse::Button b) { if(b&&btn) fn(layer); };
            }
            
            /// Sets click handler. If hit check function is set, this event is only called
            /// if hit check returns true.
            void SetClick(std::function<void()> fn, Input::Mouse::Button btn = Input::Mouse::Button::Left) {
                click = [fn, btn](Layer &, Geometry::Point, Input::Mouse::Button b) { if(b&&btn) fn(); };
            }

            /// Sets click handler. If hit check function is set, this event is only called
            /// if hit check returns true. This variant accepts class member function.
            template<class C_>
            void SetClick(C_ &c, std::function<void(C_ &, Layer &)> fn, Input::Mouse::Button btn = Input::Mouse::Button::Left) {
                C_ *my = &c;
                click = [fn, my, btn](Layer &layer, Input::Mouse::Button b) { if(b&&btn) my->fn(layer); };
            }
            
            /// Sets click handler. If hit check function is set, this event is only called
            /// if hit check returns true. This variant accepts class member function.
            template<class C_>
            void SetClick(C_ &c, std::function<void(C_ &)> fn, Input::Mouse::Button btn = Input::Mouse::Button::Left) {
                C_ *my = &c;
                click = [fn, my, btn](Layer &, Geometry::Point, Input::Mouse::Button b) { if(b&&btn) my->fn(); };
            }

            /// Sets click handler. If hit check function is set, this event is only called
            /// if hit check returns true. This variant accepts class member function.
            template<class C_>
            void SetClick(C_ *my, std::function<void(C_ &, Layer &)> fn, Input::Mouse::Button btn = Input::Mouse::Button::Left) {
                click = [fn, my, btn](Layer &layer, Input::Mouse::Button b) { if(b&&btn) my->fn(layer); };
            }
            
            /// Sets click handler. If hit check function is set, this event is only called
            /// if hit check returns true. This variant accepts class member function.
            template<class C_>
            void SetClick(C_ *my, std::function<void(C_ &)> fn, Input::Mouse::Button btn = Input::Mouse::Button::Left) {
                click = [fn, my, btn](Layer &, Geometry::Point, Input::Mouse::Button b) { if(b&&btn) my->fn(); };
            }
            
            /// Removes click handler
            void ResetClick() {
                click = {};
            }



			/// Sets scroll function.
			void SetScroll(std::function<void(Layer &, Geometry::Point, int)> fn) {
				vscroll = fn;
			}

			/// Sets hit check function. When set, events only occur if hit check returns
			/// true. Events follow hit check in a sequential manner, thus, if a handler
			/// is called, this means hit check has already succeeded in the current
			/// layout.
			void SetScroll(std::function<void(Geometry::Point, int)> fn) {
				vscroll = [fn](Layer &, Geometry::Point point, int amount) { fn(point, amount); };
			}

			/// Sets hit check function. When set, events only occur if hit check returns
			/// true. Events follow hit check in a sequential manner, thus, if a handler
			/// is called, this means hit check has already succeeded in the current
			/// layout. This variant accepts class member function.
			template<class C_>
			void SetScroll(C_ &c, std::function<void(C_ &, Layer &, Geometry::Point, int)> fn) {
				C_ *my = &c;
				vscroll = [fn, my](Layer &layer, Geometry::Point point, int amount) { my->fn(layer, point, amount); };
			}

			/// Sets hit check function. When set, events only occur if hit check returns
			/// true. Events follow hit check in a sequential manner, thus, if a handler
			/// is called, this means hit check has already succeeded in the current
			/// layout. This variant accepts class member function.
			template<class C_>
			void SetScroll(C_ &c, std::function<void(C_ &, Geometry::Point, int)> fn) {
				C_ *my = &c;
				vscroll = [fn, my](Layer &, Geometry::Point point, int amount) { my->fn(point, amount); };
			}

			/// Sets hit check function. When set, events only occur if hit check returns
			/// true. Events follow hit check in a sequential manner, thus, if a handler
			/// is called, this means hit check has already succeeded in the current
			/// layout. This variant accepts class member function.
			template<class C_>
			void SetScroll(C_ *my, std::function<void(C_ &, Layer &, Geometry::Point, int)> fn) {
				vscroll = [fn, my](Layer &layer, Geometry::Point point, int amount) { my->fn(layer, point, amount); };
			}

			/// Sets hit check function. When set, events only occur if hit check returns
			/// true. Events follow hit check in a sequential manner, thus, if a handler
			/// is called, this means hit check has already succeeded in the current
			/// layout. This variant accepts class member function.
			template<class C_>
			void SetScroll(C_ *my, std::function<void(C_ &, Geometry::Point, int)> fn) {
				vscroll = [fn, my](Layer &, Geometry::Point point, int amount) { my->fn(point, amount); };
			}

			/// Sets scroll function.
			void SetScroll(std::function<void(Layer &, int)> fn) {
				vscroll = [fn](Layer &layer, Geometry::Point, int amount) { fn(layer, amount); };
			}

			/// Sets hit check function. When set, events only occur if hit check returns
			/// true. Events follow hit check in a sequential manner, thus, if a handler
			/// is called, this means hit check has already succeeded in the current
			/// layout.
			void SetScroll(std::function<void(int)> fn) {
				vscroll = [fn](Layer &, Geometry::Point, int amount) { fn(amount); };
			}

			/// Sets hit check function. When set, events only occur if hit check returns
			/// true. Events follow hit check in a sequential manner, thus, if a handler
			/// is called, this means hit check has already succeeded in the current
			/// layout. This variant accepts class member function.
			template<class C_>
			void SetScroll(C_ &c, std::function<void(C_ &, Layer &, int)> fn) {
				C_ *my = &c;
				vscroll = [fn, my](Layer &layer, Geometry::Point, int amount) { my->fn(layer, amount); };
			}

			/// Sets hit check function. When set, events only occur if hit check returns
			/// true. Events follow hit check in a sequential manner, thus, if a handler
			/// is called, this means hit check has already succeeded in the current
			/// layout. This variant accepts class member function.
			template<class C_>
			void SetScroll(C_ &c, std::function<void(C_ &, int)> fn) {
				C_ *my = &c;
				vscroll = [fn, my](Layer &, Geometry::Point, int amount) { my->fn(amount); };
			}

			/// Sets hit check function. When set, events only occur if hit check returns
			/// true. Events follow hit check in a sequential manner, thus, if a handler
			/// is called, this means hit check has already succeeded in the current
			/// layout. This variant accepts class member function.
			template<class C_>
			void SetScroll(C_ *my, std::function<void(C_ &, Layer &, int)> fn) {
				vscroll = [fn, my](Layer &layer, Geometry::Point, int amount) { my->fn(layer, amount); };
			}

			/// Sets hit check function. When set, events only occur if hit check returns
			/// true. Events follow hit check in a sequential manner, thus, if a handler
			/// is called, this means hit check has already succeeded in the current
			/// layout. This variant accepts class member function.
			template<class C_>
			void SetScroll(C_ *my, std::function<void(C_ &, int)> fn) {
				vscroll = [fn, my](Layer &, Geometry::Point, int amount) { my->fn(amount); };
			}

			/// Removes hit check handler, default action for hit check is to return true.
			void ResetScroll() {
				vscroll ={};
			}

            
            ///@}
		protected:
            std::function<bool(Layer &, Geometry::Point)> hitcheck;
            std::function<void(Layer &, Geometry::Point, Input::Mouse::Button)> click;
            std::function<void(Layer &, Geometry::Point, Input::Mouse::Button)> down;
            std::function<void(Layer &, Geometry::Point, Input::Mouse::Button)> up;
            std::function<bool(Layer &, Geometry::Point)> move;
            std::function<void(Layer &, Geometry::Point)> over;
			std::function<void(Layer &)> out;
			std::function<void(Layer &, Geometry::Point, int amount)> vscroll;
			std::function<void(Layer &, Geometry::Point, int amount)> hscroll;
            

			/// Propagates a mouse event. Some events will be called directly.
			virtual MouseHandler propagate_mouseevent(Input::Mouse::EventType event, Geometry::Point location, Input::Mouse::Button button, int amount) override;

			/// Propagates a scroll event.
			//virtual MouseHandler propagate_scrollevent(Input::Mouse::ScrollType direction, Geometry::Point location, int amount) override;

		};

} }
