#include "App.h"
#include <Gorgon/UI/Organizers/Flow.h>

namespace UI{

    //Initialization of constructor class 
    UI::App::App ( Gorgon::Geometry::Size size, int fontSize, std::string title ):window(size,title), btnImport("Import"),btnFileFrom("Path From"), btnFileTo("Path To"), pnlSettings(Gorgon::Widgets::Registry::Panel_Left){
        
        
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
        
        
        
        //Initialization of settings  panel
        pnlSettings.SetWidth(510);
        pnlSettings.Move(20, 20);
        pnlSettings.SetHeight(310);
        pnlSettings.EnableScroll(false, false);
        window.Add(pnlSettings);
        
        
        //Organizers to display elements to the panel.
        auto &org = pnlSettings.CreateOrganizer<Gorgon::UI::Organizers::Flow>();
        
        org << std::endl << 2 << "Label" << btnFileFrom << btnFileTo
         << org.Break << std::endl << 2 << "Label" << btnImport ;
        
        //Allows the program to be terminated if the window is closed.
        window.DestroyedEvent.Register([&]() {
            exit(0);
        });
            
    }

    
    
    

}
