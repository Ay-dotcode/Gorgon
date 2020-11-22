#pragma once

#include <string>

#include <Gorgon/Graphics/Layer.h>
#include <Gorgon/UI/Window.h>
#include <Gorgon/Geometry/Size.h>
#include <Gorgon/Widgets/Panel.h>

#include <Gorgon/Layer.h>
#include <Gorgon/Graphics/BlankImage.h>
#include <Gorgon/UI/ComponentStack.h>
#include <Gorgon/Widgets/Generator.h>
#include <Gorgon/UI/Window.h>
#include <Gorgon/Widgets/Button.h>
#include <Gorgon/Widgets/Checkbox.h>
#include <Gorgon/Widgets/RadioButtons.h>
#include <Gorgon/Widgets/Label.h>
#include <Gorgon/Widgets/Panel.h>
#include <Gorgon/UI/RadioControl.h>
#include <Gorgon/WindowManager.h>
#include <Gorgon/CGI/Line.h>
#include <Gorgon/UI/Organizers/List.h>
#include <Gorgon/Widgets/Textbox.h>
#include <Gorgon/Widgets/Numberbox.h>
#include <Gorgon/Widgets/GeometryBoxes.h>
#include <Gorgon/Widgets/Composer.h>
#include <typeinfo>
#include <Gorgon/UI/Template.h>
#include <Gorgon/CGI/Line.h>
#include <Gorgon/Widgets/Composer.h>

namespace UI{
    class App{
    public:    
        //Declaration function for UI to be called in Main.cpp
        App (Gorgon::Geometry::Size size, int fontSize, std::string fontname, std::string title );
        
        
    private:
        //Gorgon Main UI Window
        Gorgon::UI::Window window;
        
        
        //Gorgon Icon objects for app window
        Gorgon::Graphics::Bitmap ico;
        Gorgon::WindowManager::Icon icon;
        
        //Gorgon Layer objects
        Gorgon::Graphics::Layer backgroundLayer;
        
        Gorgon::Graphics::BlankImage applicationBG;
        
        //Gorgon Button Objects
        //Gorgon::Widgets::Button btnImport;
        
        //Gorgon Checkbox opjects
        //Gorgon::Widgets::Checkbox metadata;
        
        //Gorgon panel objects
        //Gorgon::Widgets::Panel pnl;
        
        //Structure for font of UI
        struct initUIfont{
            initUIfont(std::string font, int fs);
            
            Gorgon::Widgets::SimpleGenerator uiGen;
        };
        
        //Initialization of font Structure
        initUIfont initFont;
    };
}
