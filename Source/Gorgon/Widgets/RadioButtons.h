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
			this->elements.Destroy();
		}
		
		using WidgetBase::Move;

		using WidgetBase::Resize;

		virtual void Move(Geometry::Point value) override {
			location = value;

			if(HasParent())
				this->PlaceIn(GetParent(), location, spacing);
		}


		virtual Geometry::Point GetLocation() const override {
			return location;
		}

		
		virtual void Resize(Geometry::Size size) override {
			size.Height -= spacing * (this->elements.GetSize() - 1);

			if(size.Height < 0)
				size.Height = 0;

			size.Height /= this->elements.GetSize();

			for(auto p : this->elements) {
				p.second.Resize(size);
			}

			this->PlaceIn(GetParent(), location, spacing);
		}


		virtual Geometry::Size GetSize() const override {
			int h = 0;
			int maxw = 0;
			for(auto p : this->elements) {
				if(h != 0)
					h += spacing;

				auto s = p.second.GetSize();

				h += s.Height;

				if(s.Width > maxw)
					maxw = s.Width;
			}

			return {maxw, h};
		}


		virtual bool Activate() override {
			return false;
		}

		void SetSpacing(int value) {
			if(spacing == value)
				return;

			spacing = value;

			if(HasParent())
				this->PlaceIn(GetParent(), location, spacing);
		}

		void Add(const T_ value) {
            auto &c = *new W_(temp, String::From(value));
			UI::RadioControl<T_, W_>::Add(value, c);
            
            if(!IsVisible())
                c.Hide();
		}

		void Add(const T_ value, std::string text) {
            auto &c = *new W_(temp, text);
            UI::RadioControl<T_, W_>::Add(value, c);
            
            if(!IsVisible())
                c.Hide();
        }

		using UI::RadioControl<T_, W_>::ChangedEvent;

        using UI::RadioControl<T_, W_>::Exists;

        using UI::RadioControl<T_, W_>::Get;

        using UI::RadioControl<T_, W_>::Set;

	protected:
		virtual void addto(Layer &layer) override { }


		virtual void removefrom(Layer &layer) override { }


		virtual void setlayerorder(Layer &layer, int order) override {
			if(!HasParent()) 
				return;

			for(auto p : this->elements) {
				GetParent().ChangeZorder(p.second, order);
                order++;
			}
		}


		virtual bool allowfocus() const override {
			return false;
		}


		virtual bool addingto(UI::WidgetContainer &container) override {
			this->PlaceIn(container, location, spacing);

			return true;
		}


		virtual bool removingfrom() override {
			for(auto p : this->elements) {
				p.second.Remove();
			}

			return true;
		}

		Geometry::Point location = {0, 0};
		int spacing = 4;
		const UI::Template &temp;
        
    private:
        virtual void show() override {
            for(auto p : this->elements) {
                p.second.Show();
            }
        }
        
        virtual void hide() override {
            for(auto p : this->elements) {
                p.second.Hide();
            }
        }
	};
    
} }
