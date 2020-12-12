#include "App.h"
#include <Gorgon/UI/Organizers/Flow.h>


namespace UI{

    //Initialization of constructor class 
    UI::App::App (Gorgon::Geometry::Size size, int fontSize, std::string title ):window(size,title),initstyl("Goldman-Regular.ttf",14), btnImport("IMPORT"),btnFileFrom("Path From"), btnFileTo("Path To"), btnExit("EXIT"){
        
        
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
        pnlSettings.SetWidth(550);
        pnlSettings.Move(0, 0);
        pnlSettings.SetHeight(350);
        pnlSettings.EnableScroll(false, false);
        window.Add(pnlSettings);
        
        //Organizers to display elements to the panel.
        auto &org = pnlSettings.CreateOrganizer<Gorgon::UI::Organizers::Flow>();
        
        org 
        << org.Break 
        << 1 << "" << 4 << "Import From:" << 9 << pathFrom << btnFileFrom
        << org.Break << org.Break
        << 1 << "" << 4 << "Import To:" << 9 << pathTo << btnFileTo 
        << org.Break 
        << org.Break << org.Break << org.Break << org.Break << org.Break
        << org.Break 
        << btnExit << 12 << "" << btnImport ;
        
        btnImport.PressEvent.Register([&]{
            Import();
        });
        
        //Allows the program to be terminated if the window is closed.
        window.DestroyedEvent.Register([&]() {
            exit(0);
        });
        
        //Program terminates if Exit button is clicked.
        btnExit.PressEvent.Register([&]{
            exit(0);
        });
            
    }
    
    void App::Import(){
        std::string fromPath = pathFrom.GetText();
        std::string toPath = pathTo.GetText();
        std::cout << "Importing Resources From \"" << fromPath << "\" to \"" << toPath << "\"" << std::endl;
    }

    
    App::initStyle::initStyle(std::string fontname, int fh) {
        Gorgon::Graphics::Initialize();

        uiStyle.Init ( fh, fontname );
        uiStyle.Background.Regular = { Gorgon::Graphics::Color::White, 0.2f };
        uiStyle.Background.Edit = { Gorgon::Graphics::Color::LightYellow, 0.2f };
        uiStyle.Forecolor.Regular = Gorgon::Graphics::Color::White;
        uiStyle.Forecolor.Hover = Gorgon::Graphics::Color::Black;
        uiStyle.Forecolor.Down = { Gorgon::Graphics::Color::Black, 0.3f };
        //uiStyle.Border.Color = Gorgon::Graphics::Color::Grey;
        uiStyle.Background.Panel = {Gorgon::Graphics::RGBA(54,54,54,1),1};
        uiStyle.Focus.Color = Gorgon::Graphics::Color::Yellow;
        uiStyle.Activate();
        
    }
    

}
