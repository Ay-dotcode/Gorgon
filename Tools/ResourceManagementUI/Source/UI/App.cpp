#include "App.h"


namespace UI{

    //Initialization of constructor class 
    UI::App::App (Gorgon::Geometry::Size size, int fontSize, std::string title, std::vector<std::string> &tcommands, std::string filePath):window(size,title),initstyl("/home/luca/Gorgon/Tools/ResourceManagementUI/Bin/Goldman-Regular.ttf",14), cli(tcommands),btnImport("IMPORT"),btnFileFrom("Path From"), btnFileTo("Path To"), btnExit("EXIT"), metadata("  Add metadata file for each resource."), fileList(begin(Gorgon::Enumerate<FileTypes>()),end(Gorgon::Enumerate<FileTypes>())), pathTo(filePath), pathFrom(filePath){
        
        
        //Import application icon and apply to window
        //This may fail if the debug path is not set correctly
        ico.Import("/home/luca/Gorgon/Tools/ResourceManagementUI/Bin/GRM-Logo-72x72.png");
        icon = Gorgon::WindowManager::Icon{ico.GetData()};
        window.SetIcon(icon);
        
        
        //Creates the background for the application and adds it to the window.
        window.Add(backgroundLayer);
        backgroundLayer.Draw(0xff567f9e);
        applicationBG.SetColor(0.22f);
        applicationBG.DrawIn(backgroundLayer);
        
        
        
        //Initialization of settings  panel
        pnlSettings.SetWidth(size.Width);
        pnlSettings.Move(0, 0);
        pnlSettings.SetHeight(size.Height);
        pnlSettings.EnableScroll(false, false);
        window.Add(pnlSettings);
        
        fileList.List.SetSelectedIndex(0);
        
        //Organizers to display elements to the panel.
        auto &org = pnlSettings.CreateOrganizer<Gorgon::UI::Organizers::Flow>();
        
        org 
        << org.Break 
        << 1 << "" << 4 << "Import From:" << 19 << pathFrom << btnFileFrom
        << org.Break << org.Break
        << 1 << "" << 4 << "Import To:" << 19 << pathTo << btnFileTo 
        << org.Break << org.Break
        << 1 << "" << 4 << "File Type:" << 5 << fileList << 2 << "" << 8 << metadata 
        << org.Break << org.Break << org.Break << org.Break
        << org.Break 
        << btnExit << 22 << "" << btnImport ;
        
        btnImport.PressEvent.Register([&]{
            Import();
        });
        
        //Allows the program to be terminated if the window is closed.
        window.DestroyedEvent.Register([&]() {
            window.Quit();
        });
        
        //Program terminates if Exit button is clicked and confirmedv.
        btnExit.PressEvent.Register([&]{
            pnlSettings.Disable();
            Gorgon::UI::AskYesNo("Exit","Any currently running imports will be lost!\n\n\nAre you sure you want to leave?", [&]{exit(0);}, [&]{pnlSettings.Enable();});
        });
    }
    
    void App::Import(){
        std::string fromPath = pathFrom.GetText();
        std::string toPath = pathTo.GetText();
        std::cout << "Importing Resources From \"" << fromPath << "\" to \"" << toPath << "\"" << std::endl;
    }
    
    

    //UI 
    App::initStyle::initStyle(std::string fontname, int fh) {
        Gorgon::Graphics::Initialize();

        uiStyle.Init ( fh, fontname );
        uiStyle.Background.Regular = { Gorgon::Graphics::Color::White, 0.2f };
        uiStyle.Background.Hover = { Gorgon::Graphics::Color::Black, 0.2f };
        uiStyle.Background.Edit = { Gorgon::Graphics::Color::LightTan, 0.2f };
        uiStyle.Forecolor.Regular = Gorgon::Graphics::Color::White;
        uiStyle.Forecolor.Hover = Gorgon::Graphics::Color::Amber;
        uiStyle.Forecolor.Down = { Gorgon::Graphics::Color::Black, 0.3f };
        //uiStyle.Border.Color = Gorgon::Graphics::Color::Grey;
        uiStyle.Background.Panel = {Gorgon::Graphics::RGBA(54,54,54,1),1};
        uiStyle.Focus.Color = Gorgon::Graphics::Color::Yellow;
        uiStyle.Activate();
        
    }
    

}
