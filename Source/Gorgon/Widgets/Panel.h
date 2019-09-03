#pragma once

#include "../UI/ComponentStackWidget.h"
#include "../UI/WidgetContainer.h"
#include "../Property.h"

namespace Gorgon { namespace Widgets {
    
    class Panel : public UI::ComponentStackWidget, public UI::WidgetContainer {
    public:
        
        Panel(const Panel &) = delete;
        
        Panel(Panel &&) = default;
        
        explicit Panel(const UI::Template &temp);
        

		virtual bool Activate() override;


		virtual bool KeyEvent(Input::Key, float) override;


		virtual bool CharacterEvent(Char) override;


		virtual bool IsVisible() const override;


		virtual Geometry::Size GetInteriorSize() const override;


		virtual bool ResizeInterior(Geometry::Size size) override;

	protected:
		virtual void focused() override;


		virtual void focuslost() override;


		virtual Layer &getlayer() override;

	};
    
} }
