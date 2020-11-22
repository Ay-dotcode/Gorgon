#include "App.h"

namespace UI{

    //Initialization of constructor class 
    UI::App::App ( Gorgon::Geometry::Size size, int fontSize, std::string fontname, std::string title ):window(size,title), initFont(fontname,fontSize){
        
        //Set up default window settings
        Gorgon::Graphics::Initialize();
        
        //Import application icon and apply to window
        //This may fail if the debug path is not set correctly
        ico.Import("Icon.png");
        icon = Gorgon::WindowManager::Icon{ico.GetData()};
        window.SetIcon(icon);
        
        
        //Creates the background for the application and adds it to the window.
        window.Add(backgroundLayer);
        backgroundLayer.Draw(0xff567f9e);
        applicationBG.SetColor(0.22f);
        applicationBG.DrawIn(backgroundLayer);
        
        //Allows the program to be terminated of the window is closed.
        window.DestroyedEvent.Register([&]() {
            exit(0);
        });
            
    }

    
    //construction of UI font, color of text, hover effect, 
    //background color of texts,ect
    App::initUIfont::initUIfont ( std::string font, int fs ){
        Gorgon::Graphics::Initialize();
        
        uiGen.Init(fs, font);
        uiGen.Activate();
    }

}
