#include "App.h"


namespace UI{

    //Initialization of constructor class 
    UI::App::App (Gorgon::Geometry::Size size, int fontSize, std::string title, std::vector<std::string> &tcommands, std::string filePath):
    
    window(size,title),
    initstyl(Gorgon::String::Concat(Gorgon::Filesystem::ExeDirectory(), "/Goldman-Regular.ttf"),14), 
    cli(tcommands),
    btnImport("IMPORT"),
    btnClearFileFrom("Clear"), 
    btnClearFileTo("Clear"), 
    btnExit("EXIT"), 
    btnClassInfo("i"),
    btnScaleInfo("i"),
    metadata("  Add metadata file for each resource."),
    fileList(begin(Gorgon::Enumerate<FileTypes>()),end(Gorgon::Enumerate<FileTypes>())), 
    scaleList(begin(Gorgon::Enumerate<Scales>()),end(Gorgon::Enumerate<Scales>())),
    pathTo(filePath), 
    pathFrom(filePath),
    lblMetadata("Type each metadata element in the textbox below followed by a coma.")
    
    {
        
        //Import application icon and apply to window
        //This may fail if the debug path is not set correctly
        ico.Import(Gorgon::String::Concat(Gorgon::Filesystem::ExeDirectory(),"/GRM-Logo-72x72.png"));
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
        
        metadata1.SetEnabled(false);
        lblMetadata.SetEnabled(false);
        
        fileList.List.SetSelectedIndex(0);
        scaleList.List.SetSelectedIndex(0);
        
        //Organizers to display elements to the panel.
        auto &org = pnlSettings.CreateOrganizer<Gorgon::UI::Organizers::Flow>();
        
        
        org 
        << org.Break 
        << 1 << "" << 4 << "Import From:" << 19 << pathFrom << btnClearFileFrom
        << org.Break << org.Break
        << 1 << "" << 4 << "Import To:" << 19 << pathTo << btnClearFileTo 
        << org.Break << org.Break
        << 1 << "" << 4 << "File Type:" << 6 << fileList << 2 << "" << 4 << "Resource Class:" << 5 << resourceClass<< 1 <<btnClassInfo
        << org.Break << org.Break 
        << 1 << "" << 4 << "Resource Scale:" << 2 << scaleList  << 1 << btnScaleInfo
        << org.Break << org.Break 
        << 1 << "" << 10 << metadata  << "" << org.Break
        << 1 << "" << 17 << lblMetadata << "" << org.Break
        << 1 << "" << 25 << metadata1 << ""
        << org.Break << org.Break
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
        
        //Info button
        btnClassInfo.PressEvent.Register([&]{
            Gorgon::UI::ShowMessage("Resource Class Info","This is the type of resource that you are importing for you prohject. It can be character models, tiles, items, sounds or UI elements and so on. \n\n This is what your .gor file will be saved as. With the scale size included.");
        });
        
        //Info Button
        btnScaleInfo.PressEvent.Register([&]{
            Gorgon::UI::ShowMessage("Resource Scale Info","This is the scale at which you want to import your resource. \n\n The scale sze will be made present on the saved .gor file.");
        });
        
        //Checkbox button selected
        metadata.ChangedEvent.Register([&] {            
            org.Reorganize();
            metadata1.SetEnabled(bool(metadata));
            lblMetadata.SetEnabled(bool(metadata));
        });
        
        //Clear File Path From button action
        btnClearFileFrom.PressEvent.Register([&](){
            pathFrom.clear();
        });
        
        //Clear File Path To button action
        btnClearFileTo.PressEvent.Register([&](){
            pathTo.clear();
        });
    }
    
    
    void App::Import(){
        std::string fromPath = pathFrom.GetText();
        std::string toPath = pathTo.GetText();
        
        if(resourceClass.GetText() == ""){
                Gorgon::UI::ShowMessage("Please Enter a Resource class name before Importing.");
        }
        else{
        
            if(fileList == Audio){
                audio.DoImport(pathFrom, pathTo, resourceClass.GetText());
            
            }else if(fileList == Image){
                
                image.Set(scaleList, pathFrom, pathTo, resourceClass.GetText());
                image.DoImport();
            } 
        }
        
        //std::cout << "Importing Resources From \"" << fromPath << "\" to \"" << toPath << "\"" << std::endl;
        
        
    }
    
    
    
    

    //UI Styling
    App::initStyle::initStyle(std::string fontname, int fh) {
        Gorgon::Graphics::Initialize();

        uiStyle.Init ( fh, fontname );
        uiStyle.Background.Regular = { Gorgon::Graphics::Color::White, 0.2f };
        uiStyle.Background.Hover = { Gorgon::Graphics::Color::Black, 0.5f };
        uiStyle.Background.Edit = { Gorgon::Graphics::Color::LightTan, 0.2f };
        uiStyle.Forecolor.Regular = Gorgon::Graphics::Color::White;
        uiStyle.Forecolor.Hover = Gorgon::Graphics::Color::Amber;
        uiStyle.Forecolor.Down = { Gorgon::Graphics::Color::Black, 0.3f };
        uiStyle.Background.Disabled = Gorgon::Graphics::Color::Black;
        //uiStyle.Border.Color = Gorgon::Graphics::Color::Grey;
        uiStyle.Background.Panel = {Gorgon::Graphics::RGBA(54,54,54,1),0.99};
        uiStyle.Focus.Color = Gorgon::Graphics::Color::Yellow;
        uiStyle.Activate();
        
    }
    

}
