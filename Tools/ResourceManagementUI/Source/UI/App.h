#pragma once

#include <string>

#include <Gorgon/Graphics/Layer.h>
#include <Gorgon/UI/Window.h>
#include <Gorgon/Geometry/Size.h>
#include <Gorgon/Widgets/Panel.h>
#include <Gorgon/UI.h>
#include <Gorgon/Layer.h>
#include <Gorgon/Graphics/BlankImage.h>
#include <Gorgon/UI/ComponentStack.h>
#include <Gorgon/Widgets/Generator.h>

#include <Gorgon/Widgets/Button.h>
#include <Gorgon/Widgets/Checkbox.h>
#include <Gorgon/Widgets/RadioButtons.h>
//#include <Gorgon/Widgets/Label.h>

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
#include <Gorgon/Graphics/Color.h>
#include <Gorgon/Graphics/FreeType.h>

#include <Gorgon/Graphics/Color.h>
#include <Gorgon/Filesystem.h>

#include <Gorgon/UI/Organizers/Flow.h>
#include <Gorgon/Widgets/DialogWindow.h>
#include <Gorgon/UI/Dialog.h>

#include "Cli.h"


namespace UI{
    
    class InitUI {
    public:
        InitUI() {
            Gorgon::Graphics::Initialize();
            Gorgon::UI::Initialize();            
        };
    };
    
    class App{
    public:    
        //Declaration function for UI to be called in Main.cpp
        App (Gorgon::Geometry::Size size, int fontSize, std::string title, std::vector<std::string> &tcommands, std::string filePath );
        
        
    private:
        
        void Import();
        //Gorgon Main UI Window
        Gorgon::UI::Window window;
        
        //This will initialize UI after the window is created.
        InitUI uninit;
        
        //Structure to handle styling of UI
        struct initStyle {
            initStyle(std::string fontname, int fh);
            
            Gorgon::Widgets::SimpleGenerator uiStyle;
        };
        
        //Initialize styling
        initStyle initstyl;
        
        UI::Cli cli;
        
        //Gorgon Icon objects for app window
        Gorgon::Graphics::Bitmap ico;
        Gorgon::WindowManager::Icon icon;
        
        //Gorgon Layer objects
        Gorgon::Graphics::Layer backgroundLayer;
        
        //background of default window
        Gorgon::Graphics::BlankImage applicationBG;
        
        //Gorgon Button Objects
        Gorgon::Widgets::Button btnImport, btnFileFrom, btnFileTo, btnExit, btnCheckExit;
        
        //Gorgon Checkbox opjects
        //Gorgon::Widgets::Checkbox metadata;
        
        Gorgon::Widgets::Textbox pathTo, pathFrom;
        
        //Gorgon panel objects
        Gorgon::Widgets::Panel pnlSettings;
        
    };
}
