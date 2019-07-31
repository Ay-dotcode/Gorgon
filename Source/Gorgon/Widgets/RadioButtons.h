#pragma once

#pragma once

#include "../UI/ComponentStackWidget.h"
#include "../UI/RadioControl.h"
#include "../Property.h"
#include "Checkbox.h"

namespace Gorgon { namespace Widgets {
    
    template<class T_, class W_ = Checkbox>
    class RadioButtons : public UI::WidgetBase, protected UI::RadioControl<T_, W_> {
    public:
		explicit RadioButtons(const UI::Template &temp) : temp(temp) { }

        ~RadioButtons() {
			elements.Destroy();
		}
		
		using WidgetBase::Move;

		using WidgetBase::Resize;

		virtual void Move(Geometry::Point value) override {
			location = value;

			if(HasParent())
				PlaceIn(GetParent(), location, spacing);
		}


		virtual Geometry::Point GetLocation() const override {
			return location;
		}


		virtual void Resize(Geometry::Size size) override {
			//no resize
		}


		virtual Geometry::Size GetSize() const override {
			//needs calculation

			return {0, 0};
		}


		virtual bool Activate() override {
			return false;
		}

		void SetSpacing(int value) {
			if(spacing == value)
				return;

			spacing = value;

			if(HasParent())
				PlaceIn(GetParent(), location, spacing);
		}

		void Add(const T_ value) {
			RadioControl<T_, W_>::Add(value, *new W_(temp, String::From(value)));
		}

		void Add(const T_ value, std::string text) {
			RadioControl<T_, W_>::Add(value, *new W_(temp, text));
		}

		using RadioControl<T_, W_>::ChangedEvent;

		using RadioControl<T_, W_>::Exists;

		using RadioControl<T_, W_>::Get;

		using RadioControl<T_, W_>::Set;

	protected:
		virtual void addto(Layer &layer) override { }


		virtual void removefrom(Layer &layer) override { }


		virtual void setlayerorder(Layer &layer, int order) override {
			for(auto it=elements.Last(); it.IsValid(); it.Previous()) {
				//todo
			}
		}


		virtual bool allowfocus() const override {
			return false;
		}


		virtual bool addingto(UI::WidgetContainer &container) override {
			PlaceIn(container, location, spacing);

			return true;
		}


		virtual bool removingfrom() override {
			for(auto p : elements) {
				p.second.Remove();
			}

			return true;
		}

		Geometry::Point location = {0, 0};
		int spacing = 4;
		const UI::Template &temp;
	};
    
} }
