#include <Gorgon/Main.h>
#include <Gorgon/Window.h>
#include <Gorgon/WindowManager.h>
#include <Gorgon/Graphics/Layer.h>
#include <Gorgon/Graphics/FreeType.h>
#include <Gorgon/Graphics/BlankImage.h>
#include <Gorgon/Input/Mouse.h>

int main() {
    //Initialize everything with the system name of GreenShooter
    Gorgon::Initialize("GreenShooter");
    
    //Create our window, this will immediately show it. For size we
    //used the size in the design.
    Gorgon::Window window({404, 444}, "Green Shooter");
    
    
    //Load Icon.png, this step might fail in debugger due to using
	//a different path for debugging. For instance, in Visual Studio
	//default debugging path is the build directory. You can change
	//this setting or you might simply copy the following file into
	//your build directory. The file will be copied to bin directory
	//automatically.
    Gorgon::Graphics::Bitmap icon;
    icon.Import("Icon.png");

    //Show the icon as window icon
    Gorgon::WindowManager::Icon ico(icon.GetData());
    window.SetIcon(ico);
    
    //Load a font to display some text
    Gorgon::Graphics::FreeType font;    
    font.LoadFile("Boxy-Bold.ttf", 20);
    
    
    //Terminate the application when the window is closed
    window.DestroyedEvent.Register([&]() {
        exit(0);
    });
    
    //until we call quit
    while(true) {
        //do what needs to run the system
        Gorgon::NextFrame();
    }
    
    return 0;
}
