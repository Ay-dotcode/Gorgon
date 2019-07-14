#pragma once

#include <set>
#include <vector>

#include "Template.h"
#include "Component.h"
#include "../Containers/Hashmap.h"
#include "../Input/Layer.h"
#include "../Geometry/Point3D.h"

namespace Gorgon { namespace UI {

	class ComponentStack : public Layer, public Updatable {
	public:
		/// should handle instantiation as well
		explicit ComponentStack(const Template &temp, Geometry::Size size);
		
		~ComponentStack() {
			for(auto &p : storage) {
				delete p.second;
			}

			substacks.Destroy();
		}

        /// Adds the given component to the top of the stack. This function will be called
        /// to add all components in the given template
		void AddToStack(const ComponentTemplate &temp, bool reversed);
        
        /// Adds a condition and its associated components to the stack
		void AddCondition(ComponentCondition condition, bool transition = true) { ReplaceCondition(ComponentCondition::Always, condition, transition); }
        
        /// Replaces a condition with another one
		void ReplaceCondition(ComponentCondition from, ComponentCondition to, bool transition = true);
        
        /// Removes a condition and its associated components
        void RemoveCondition(ComponentCondition condition, bool transition = true)  { ReplaceCondition(condition, ComponentCondition::Always, transition); }

        /// Sets the data for a specific data effect. This value will be cached by
        /// the stack for condition changes. This variant supports string based data.
        void SetData(ComponentTemplate::DataEffect effect, const std::string &text);
        
        /// Sets the data for a specific data effect. This value will be cached by
        /// the stack for condition changes. This variant supports image based data.
        /// Ownership of the image stays with the caller.
		void SetData(ComponentTemplate::DataEffect effect, const Graphics::Drawable &image);

		/// Sets the value for this stack. Value of the stack can affect various
		/// properties of components. This will set the individual channels separately.
		/// Values should be between 0 and 1.
		void SetValue(float first) { SetValue(first, value[1], value[2], value[3]); }

		/// Sets the value for this stack. Value of the stack can affect various
		/// properties of components. This will set the individual channels separately.
		/// Values should be between 0 and 1.
		void SetValue(float first, float second) { SetValue(first, second, value[2], value[3]); }

		/// Sets the value for this stack. Value of the stack can affect various
		/// properties of components. This will set the individual channels separately.
		/// Values should be between 0 and 1.
		void SetValue(float first, float second, float third) { SetValue(first, second, third, value[3]); }

		/// Sets the value for this stack. Value of the stack can affect various
		/// properties of components. This will set the individual channels separately.
		/// Values should be between 0 and 1.
		void SetValue(float first, float second, float third, float fourth);

		/// Sets the value for the stack using a point in coordinate system
		void SetValue(Geometry::Pointf pos) { SetValue(pos.X, pos.Y); }

		/// Sets the value for the stack using a point in coordinate system
		void SetValue(Geometry::Point3D pos) { SetValue(pos.X, pos.Y, pos.Z); }

		/// Sets the value for the stack using a color
		void SetValue(Graphics::RGBAf color) { SetValue(color.R, color.G, color.B, color.A); }

		/// Sets the value for the stack using a color
		void SetValue(Graphics::RGBA color) { SetValue((Graphics::RGBAf)color); }

		/// Changes the value transition speed. A speed of 0 will disable smooth transition.
		void SetValueTransitionSpeed(std::array<float, 4> val) {
			valuespeed = val;
		}

		/// GetValue returns the current transitional value, this will also enable value event
		/// to be called every time transitional value is updated
		void ReturnTransitionalValue() {
			returntarget = false;
		}

		/// GetValue returns the target value. This is the default mode.
		void ReturnTargetValue() {
			returntarget = true;
		}

		/// Sets the function that will be called whenever the value is changed
		void SetValueEvent(std::function<void()> handler) {
			value_fn = handler;
		}

		/// Returns the value of the stack
		std::array<float, 4> GetValue() const { return returntarget ? targetvalue : value; }

		/// Sets the function that will be used to convert a value to a string. The handler will receive the value channel, data effect
		/// that is causing the translation and the value that needs to be transformed.
		void SetValueToText(std::function<std::string(int, ComponentTemplate::DataEffect, const std::array<float, 4> &)> handler) {
			valuetotext = handler;
		}

		/**
		 * @name Repeating components
		 * It is possible to repeat components automatically. For this, template should
		 * have components marked with the respective RepeatMode and you need to set the
		 * repeat points. Repeat points uses the same system as value. Repeating components
		 * should have at least Always condition to work properly. It is possible to set
		 * the condition of each repeat individually. This system uses indexes of repeat
		 * points instead of values to speed up rendering.
		 * 
		 * @{
		 */

		/// Sets the repeat with the given mode to the given vector. Use std::move(data) for
		/// efficient transfer
		void SetRepeat(ComponentTemplate::RepeatMode mode, std::vector<std::array<float, 4>> data) {
            repeats[mode] = std::move(data);
			checkrepeatupdate(mode);
		}
		
        /// Adds a new repeating point to the given mode. Empty values will be set as 0.
		void AddRepeat(ComponentTemplate::RepeatMode mode, float first) {
			AddRepeat(mode, first, 0, 0, 0);
        }
		
        /// Adds a new repeating point to the given mode. Empty values will be set as 0.
		void AddRepeat(ComponentTemplate::RepeatMode mode, float first, float second) {
			AddRepeat(mode, first, second, 0, 0);
        }
		
        /// Adds a new repeating point to the given mode. Empty values will be set as 0.
		void AddRepeat(ComponentTemplate::RepeatMode mode, float first, float second, float third) {
			AddRepeat(mode, first, second, third, 0);
        }
		
        /// Adds a new repeating point to the given mode.
		void AddRepeat(ComponentTemplate::RepeatMode mode, float first, float second, float third, float fourth) {
            repeats[mode].push_back({{first, second, third, fourth}});
			checkrepeatupdate(mode);
        }
		
        /// Adds a new repeating point to the given mode. Empty values will be set as 0.
		void AddRepeat(ComponentTemplate::RepeatMode mode, Geometry::Pointf pos) {
			AddRepeat(mode, pos.X, pos.Y, 0, 0);
        }
		
        /// Adds a new repeating point to the given mode. Empty values will be set as 0.
		void AddRepeat(ComponentTemplate::RepeatMode mode, Geometry::Point3D pos) {
			AddRepeat(mode, pos.X, pos.Y, pos.Z, 0);
        }
		
        /// Adds a new repeating point to the given mode.
		void AddRepeat(ComponentTemplate::RepeatMode mode, Graphics::RGBAf color) {
			AddRepeat(mode, color.R, color.G, color.B, color.A);
        }
		
        /// Adds a new repeating point to the given mode.
		void AddRepeat(ComponentTemplate::RepeatMode mode, Graphics::RGBA color) {
            AddRepeat(mode, (Graphics::RGBAf)color);
        }

        /// Removes all repeat points from the given mode. Call RemoveAllConditions along with
		/// this function if you are using conditions for repeats.
		void RemoveRepeats(ComponentTemplate::RepeatMode mode) {
            repeats.erase(mode);
			checkrepeatupdate(mode);
		}

		/// Sets the condition of a specific repeat index. Nothing will happen if index is
		/// out of bounds or condition does not exist. Setting condition to always will simply
		/// remove the condition.
		void SetConditionOf(ComponentTemplate::RepeatMode mode, int index, ComponentCondition condition) {
			if(repeatconditions[mode][index] != condition) {
				repeatconditions[mode][index] = condition;
				checkrepeatupdate(mode);
			}
		}

		/// Removes all conditions for a repeat mode.
		void RemoveAllConditionsOf(ComponentTemplate::RepeatMode mode) {
			repeatconditions[mode].clear();
			checkrepeatupdate(mode);
		}

		/// @}

        using Layer::Resize;
        
		/// Notifies the stack about a size change
		virtual void Resize(const Geometry::Size &value) override {
            Layer::Resize(value);
            mouse.Resize(value);
            
			if(size != value) {
				size = value;
				Update();
			}
		}
		
		/// Returns the template used by this stack
		const Template &GetTemplate() const {
			return temp;
		}		

		/// Updates the layout of the component stack
		virtual void Update() override;

		void Render() override;

		void ResetAnimation() {
			controller.Reset();
		}
		
		/// Returns if this component stack is disabled. Both disabling and enabling animations are counted as disabled.
		bool IsDisabled() const {
            return conditions.count(ComponentCondition::Disabled) || 
                   transitions.count({ComponentCondition::Always, ComponentCondition::Disabled}) || 
                   transitions.count({ComponentCondition::Disabled, ComponentCondition::Always});
        }
		
		/// Changes the default emsize of 10. This value can be overridden.
		void SetEMSize(int value) {
            emsize = value;
        }
		
		/// This function instructs stack to handle mouse to automatically change hover/down
		/// states, unless disabled state is active.
		void HandleMouse(Input::Mouse::Button accepted = Input::Mouse::Button::All);
        
        /// Returns whether the component marked with the tag has a substack. If multiple components
        /// are marked to have substack, only the first one is considered.
        bool TagHasSubStack(ComponentTemplate::Tag tag) const {
            auto comp = gettag(tag);
            
            if(!comp)
                return false;
            else
                return substacks.Exists(&comp->GetTemplate());
        }
        
        /// Translates the given coordinates back to values using value scaling and channel mapping.
        /// Only works if the value affects the component location or size.
        std::array<float, 4> CoordinateToValue(ComponentTemplate::Tag tag, Geometry::Point location);
        
        /// Translates the given coordinates to component space in pixels.
        Geometry::Point TranslateCoordinates(ComponentTemplate::Tag tag, Geometry::Point location);
        
        /// Translates the given coordinates to component space in pixels.
        Geometry::Point TranslateCoordinates(int ind, Geometry::Point location);
        
        /// Translates the given coordinates to component space from 0 to 1.
        Geometry::Pointf TransformCoordinates(ComponentTemplate::Tag tag, Geometry::Point location);
        
        /// Translates the given coordinates to component space from 0 to 1.
        Geometry::Pointf TransformCoordinates(int ind, Geometry::Point location);
        
        /// Returns the boundaries of the component marked with the given tag. This function may cause
        /// update thus may take time to execute.
        Geometry::Bounds TagBounds(ComponentTemplate::Tag tag);
        
        /// Returns the boundaries of the component with the given index.
        Geometry::Bounds BoundsOf(int ind);

		/// Returns the index of the component at the given location.
		int ComponentAt(Geometry::Point location) {
			Geometry::Bounds b;

			return ComponentAt(location, b);
		}

		/// Returns the index of the component at the given location while returning the bounds
		/// of the component. 
		int ComponentAt(Geometry::Point location, Geometry::Bounds &bounds);

		/// Returns if a component at ind exists. If ind is negative or out of range, this function simply
		/// returns false.
		bool ComponentExists(int ind) const {
			return Between(ind, 0, indices) && stacksizes[ind];
		}

		/// Returns the template at the given index. If the index does not exists, this function may crash
		/// use ComponentExists function to check if it is safe to use the index.
		const ComponentTemplate &GetTemplate(int ind) const {
			return get(ind).GetTemplate();
		}
        
        /** @name Mouse Events
         * These function will allow handling mouse events. If the mouse event is originating from a
         * substack, it will have a tag other than NoTag. If the tag for the substack is set NoTag,
         * handler will receive UnknownTag. It is possible to obtain the tag and index under the
         * mouse using ComponentAt functions. Along with TranslateCoordinates or 
         * TransformCoordinates functions, it is possible to get the location of a mouse event on a
         * specific component.
         * 
         * @{
         */
        
        /// Sets the mouse down event. If HandleMouse function is called, this function will first
        /// perform mouse event transition, then it will call this handler.
        void SetMouseDownEvent(std::function<void(ComponentTemplate::Tag, Geometry::Point, Input::Mouse::Button)> handler) {
            down_fn = handler;
            
            for(auto stack : substacks) {
                stack.second.SetMouseDownEvent([stack, this](ComponentTemplate::Tag tag, Geometry::Point point, Input::Mouse::Button btn) {
                    down_fn(stack.first->GetTag() == ComponentTemplate::NoTag ? ComponentTemplate::UnknownTag : stack.first->GetTag(), point, btn);
                });
            }
        }
        
        /// Sets the mouse up event. If HandleMouse function is called, this function will first
        /// perform mouse event transition, then it will call this handler. This event will be
        /// called even if mouse down is not handled.
        void SetMouseUpEvent(std::function<void(ComponentTemplate::Tag, Geometry::Point, Input::Mouse::Button)> handler) {
            up_fn = handler;
             
            for(auto stack : substacks) {
                stack.second.SetMouseUpEvent([stack, this](ComponentTemplate::Tag tag, Geometry::Point point, Input::Mouse::Button btn) {
                    up_fn(stack.first->GetTag() == ComponentTemplate::NoTag ? ComponentTemplate::UnknownTag : stack.first->GetTag(), point, btn);
                });
            }
       }
        
        /// Sets the mouse down event. If HandleMouse function is called, this function will first
        /// perform mouse event transition, then it will call this handler.
        void SetClickEvent(std::function<void(ComponentTemplate::Tag, Geometry::Point, Input::Mouse::Button)> handler) {
            click_fn = handler;
            
            for(auto stack : substacks) {
                stack.second.SetClickEvent([stack, this](ComponentTemplate::Tag tag, Geometry::Point point, Input::Mouse::Button btn) {
                    click_fn(stack.first->GetTag() == ComponentTemplate::NoTag ? ComponentTemplate::UnknownTag : stack.first->GetTag(), point, btn);
                });
            }
        }
        
        /// Sets the mouse mvoe event. If HandleMouse function is called, this function will first
        /// perform mouse event transition, then it will call this handler. If this event is not handled
        /// mouse move event of the layer will not be handled too.
        void SetMouseMoveEvent(std::function<void(ComponentTemplate::Tag, Geometry::Point)> handler) {
            move_fn = handler;
            
            mouse.SetMove([this] (Geometry::Point location){
                if(move_fn)
                    move_fn(ComponentTemplate::NoTag, location);
            });
            
            for(auto stack : substacks) {
                stack.second.SetMouseMoveEvent([stack, this](ComponentTemplate::Tag tag, Geometry::Point point) {
                    move_fn(stack.first->GetTag() == ComponentTemplate::NoTag ? ComponentTemplate::UnknownTag : stack.first->GetTag(), point);
                });
            }
        }
        
        /// Sets the mouse over event that is fired when the mouse moves over the component stack or a substack.
        /// it will not be fired for mouse moving over a specific component. If HandleMouse function is called, 
        /// this function will first perform mouse event transition, then it will call this handler.
        void SetMouseOverEvent(std::function<void(ComponentTemplate::Tag)> handler) {
            over_fn = handler;
            
            for(auto stack : substacks) {
                stack.second.SetMouseOverEvent([stack, this](ComponentTemplate::Tag tag) {
                    over_fn(stack.first->GetTag() == ComponentTemplate::NoTag ? ComponentTemplate::UnknownTag : stack.first->GetTag());
                });
            }
        }
        
        /// Sets the mouse out event that is fired when the mouse moves over the component stack or a substack.
        /// it will not be fired for mouse moving over a specific component. If HandleMouse function is called, 
        /// this function will first perform mouse event transition, then it will call this handler. This event 
        /// will be called even if mouse over is not handled.
        void SetMouseOutEvent(std::function<void(ComponentTemplate::Tag)> handler) {
            out_fn = handler;
            
            for(auto stack : substacks) {
                stack.second.SetMouseOutEvent([stack, this](ComponentTemplate::Tag tag) {
                    out_fn(stack.first->GetTag() == ComponentTemplate::NoTag ? ComponentTemplate::UnknownTag : stack.first->GetTag());
                });
            }
        }
        
        /// Sets the handler for scroll (HScroll or VScroll), zoom and rotate events. All these events depend on 
        /// specific hardware and may not be available.
        void SetOtherMouseEvent(std::function<void(ComponentTemplate::Tag, Input::Mouse::EventType, Geometry::Point, float)> handler);

        
        /// @}

		Event<ComponentStack> ConditionChanged;
        
        
	private:
		Component &get(int ind, int stack = -1) const {
			ASSERT(stacksizes[ind], String::Concat("Stack for index ", ind, " is empty"));

			if(stack == -1) {
				stack = stacksizes[ind]-1;
			}

			return data[ind + stack * indices];
		}

		//returns top of stack if condition does not exist
		Component &get(int ind, ComponentCondition condition) const;

		void update();

		void update(Component &parent);

		void render(Component &component, Graphics::Layer &parentlayer, Geometry::Point offset, Graphics::RGBAf color = 1.f, int ind = -1);

        void grow();
        
		int getemsize(const Component &comp);

		float calculatevalue(int channel, const Component &comp) const { return calculatevalue(value, channel, comp); }

		float calculatevalue(const std::array<float, 4> &data, int channel, const Component &comp) const;

		void checkrepeatupdate(ComponentTemplate::RepeatMode mode);
        
        //to should contain the final condition even if there is no transition
        bool addcondition(ComponentCondition from, ComponentCondition to, ComponentCondition hint = ComponentCondition::None);

        //to should contain the final condition to be removed
        //caller should erase transition
        bool removecondition(ComponentCondition from, ComponentCondition to);
        
        Component *gettag(ComponentTemplate::Tag tag) const {
            for(int i=0; i<indices; i++) {
                if(stacksizes[i] > 0) {
                    auto &comp = get(i);
                    
                    if(comp.GetTemplate().GetTag() == tag)
                        return &comp;
                }
            }
            
            return nullptr;
        }

        int emsize = 10;
        
		Component *data = nullptr;
        std::vector<int> stacksizes;
        
        std::set<ComponentCondition> disabled;
        std::set<ComponentCondition> conditions;
        std::map<ComponentCondition, ComponentCondition> future_transitions;
        
		std::map<ComponentTemplate::DataEffect, std::string> stringdata;
		Containers::Hashmap<ComponentTemplate::DataEffect, const Graphics::Drawable> imagedata;
        std::map<ComponentTemplate::RepeatMode, std::vector<std::array<float, 4>>> repeats;
		std::map<ComponentTemplate::RepeatMode, std::map<int, ComponentCondition>> repeatconditions; 
		std::array<float, 4> value ={{0.f, 0.f, 0.f, 0.f}};

		//for animation
		std::array<float, 4> targetvalue = {{0.f, 0.f, 0.f, 0.f}};
		//value speed = 0 disables animation
		std::array<float, 4> valuespeed = {{0.f, 0.f, 0.f, 0.f}};
		bool returntarget = false;


        std::map<std::pair<ComponentCondition, ComponentCondition>, unsigned long> transitions;
        
        int stackcapacity = 3;
        
        int indices = 0;

		bool updaterequired = false;
        
        bool handlingmouse = false;

		Geometry::Size size;
        
        const Template &temp;

		Containers::Hashmap<const ComponentTemplate *, ComponentStack> substacks;

		std::map<const ComponentTemplate*, ComponentStorage*> storage;
		std::map<const ComponentTemplate*, std::vector<Component>> repeated;

		Animation::Timer controller;
        
        Graphics::Layer base;
        Input::Layer mouse;
        Input::Mouse::Button mousebuttonaccepted;
        
        std::function<void(ComponentTemplate::Tag, Geometry::Point, Input::Mouse::Button)> down_fn;
        std::function<void(ComponentTemplate::Tag, Geometry::Point, Input::Mouse::Button)> click_fn;
        std::function<void(ComponentTemplate::Tag, Geometry::Point, Input::Mouse::Button)> up_fn;
        std::function<void(ComponentTemplate::Tag, Geometry::Point)> move_fn;
        std::function<void(ComponentTemplate::Tag)> over_fn;
        std::function<void(ComponentTemplate::Tag)> out_fn;
        std::function<void(ComponentTemplate::Tag, Input::Mouse::EventType, Geometry::Point, float)> other_fn; //scroll, zoom, rotate
		std::function<void()> value_fn;
        
        std::function<std::string(int ind, ComponentTemplate::DataEffect, const std::array<float, 4> &value)> valuetotext;
	};

}}
