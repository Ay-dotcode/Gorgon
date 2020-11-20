#include <Gorgon/Main.h>
#include <Gorgon/Window.h>
#include <Gorgon/WindowManager.h>
#include <Gorgon/Graphics/Layer.h>
#include <Gorgon/Graphics/FreeType.h>
#include <Gorgon/Graphics/BlankImage.h>
#include <Gorgon/Input/Mouse.h>
#include <Gorgon/Input/Layer.h> 


Gorgon::Graphics::BlankImage 
    application_bg  (0.22f),
    ui_bg           (0.32f);


#ifdef WIN32
#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>
int CALLBACK WinMain(
  _In_ HINSTANCE hInstance,
  _In_ HINSTANCE hPrevInstance,
  _In_ LPSTR     lpCmdLine,
  _In_ int       nCmdShow
) {
#else
int main() {
#endif
    
    //Initialize everything with the system name of ResourceManagementUI
    Gorgon::Initialize("ResourceManagementUI");
    
    //Create our window, this will immediately show it. For size we
    //used the size in the design.
    Gorgon::Window window({500, 700}, "Resource Management");
    
    
    //Load Icon.png, this step might fail in debugger due to using
	//a different path for debugging. For instance, in Visual Studio
	//default debugging path is the build directory. You can change
	//this setting or you might simply copy the following file into
	//your build directory. The file will be copied to bin directory
	//automatically. See CMake file for how this is done.
    Gorgon::Graphics::Bitmap icon;
    icon.Import("Icon.png");

    //Show the icon as window icon
    Gorgon::WindowManager::Icon ico(icon.GetData());
    window.SetIcon(ico);
    
    //Load a font to display some text
    Gorgon::Graphics::FreeType font;
    
    //This blocky font works better without anti-aliasing
    font.DisableAntiAliasing();
    
    //Load font and set height to 12px
    font.LoadFile("DejaVuSans.ttf", 12);
    
    //Terminate the application when the window is closed
    window.DestroyedEvent.Register([&]() {
        exit(0);
    });
    
    //Create a layer just like the design in GIMP.
    Gorgon::Graphics::Layer background_layer, ui_layer;
    
    //default layer is the size of its container and start from 0, 0
    window.Add(background_layer);
    
    //Layers are ordered in the order they are added to their parent
    //last being on top. This can be changed.
    ui_layer.Move(2, 2);
    ui_layer.Resize(300, 38);
    window.Add(ui_layer);
    
    //area requires mouse input
    Gorgon::Input::Layer input_layer;

    //Draw the image in the layer, with no parameters image will fill the layer.
    //If a layer is not cleared, anything that is drawn on it will remain.
    //This means we do not need to update the background continuously. It is best
    //to read this data externally.
    application_bg.DrawIn(background_layer);
    ui_bg.DrawIn(background_layer, 50, 50, 400, 40);
    
    //until we call quit
    while(true) {

        //Display ui
        ui_layer.Clear();
        
        font.Print(ui_layer, Gorgon::String::Concat("Resource Management"), 11, 11, 400 - 22, 0.0f);
        
        font.Print(ui_layer, Gorgon::String::Concat("By: Luca Tonini"), 11, 11, 400 - 22, Gorgon::Graphics::TextAlignment::Right, 0.0f);
        
        //do what needs to run the system
        Gorgon::NextFrame();
    }
    
    return 0;
}
