#include <Gorgon/Main.h>
#include <Gorgon/Window.h>
#include <Gorgon/WindowManager.h>
#include <Gorgon/Graphics/Layer.h>
#include <Gorgon/Graphics/FreeType.h>
#include <Gorgon/Graphics/BlankImage.h>
#include <Gorgon/Input/Mouse.h>
#include <Gorgon/Input/Layer.h>
    
    //WARNING:
    //This sample game is designed to show how a very simple game can be coded
    //using Gorgon Game Engine. The way to handle game events and to structure
    //game logic shown in this example is not recommended, even for small games.
    //In this example we aim for minimum 

    
/*#ifdef WIN32
int CALLBACK WinMain(
  _In_ HINSTANCE hInstance,
  _In_ HINSTANCE hPrevInstance,
  _In_ LPSTR     lpCmdLine,
  _In_ int       nCmdShow
) {
#else*/
int main() {
//#endif
    
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
    font.DisableAntiAliasing();
    font.LoadFile("Boxy-Bold.ttf", 20);
    
    //Terminate the application when the window is closed
    window.DestroyedEvent.Register([&]() {
        exit(0);
    });
    
    //Create 3 layers just like the design in GIMP.
    Gorgon::Graphics::Layer background_layer, ui_layer, game_layer;
    
    //default layer is the size of its container and start from 0, 0
    window.Add(background_layer);
    
    //Layers are ordered in the order they are added to their parent
    //last being on top. This can be changed.
    ui_layer.Move(2, 2);
    ui_layer.Resize(400, 38);
    window.Add(ui_layer);
    
    //Clipping will prevent any graphics from overflowing and is not
    //set by default
    game_layer.EnableClipping();
    game_layer.Move(2, 42);
    game_layer.Resize(400, 400);
    window.Add(game_layer);
    
    //Game area requires mouse input
    Gorgon::Input::Layer input_layer;

    //Mouse events do not need to be clipped. By adding the layer into
    //game layer, we do not need to adjust it
    game_layer.Add(input_layer);
    
    
    //Create bg image, BlankImage is a solid block that can be drawn
    //its size is not important when used to fill a region. Single
    //parameter supplied to it is color, when a float is given it is
    //the lightness between 0 and 1.
    Gorgon::Graphics::BlankImage 
        application_bg  (0.22f),
        game_bg         (0.85f),
        ui_bg           (0.32f);
    
    //Draw the image in the layer, with no parameters image will fill the layer.
    //If a layer is not cleared, anything that is drawn on it will remain.
    //This means we do not need to update the background continuously. It is best
    //to read this data externally.
    application_bg.DrawIn(background_layer);
    game_bg.DrawIn(background_layer, 2, 42, 400, 400);
    ui_bg.DrawIn(background_layer, 2, 2, 400, 38);
    
    
    //Game variables. These should be in a separate place. However, for
    //simplicity we will keep them here in this example.
    
    //number of greens hit - wrong hits
    int score = 0;
    
    //wrong hits, or not hitting a green will cause you to loose a life
    int lives = 3;
    
    //until we call quit
    while(true) {
        
        //display ui
        ui_layer.Clear();
        //11, 11 is position and 400 - 22 is the width of the area
        font.Print(ui_layer, Gorgon::String::Concat("Score: ", score), 11, 11, 400 - 22, 0.0f);
        
        font.Print(ui_layer, Gorgon::String::Concat("Lives: ", lives), 11, 11, 400 - 22, Gorgon::Graphics::TextAlignment::Right, 0.0f);
        
        //do what needs to run the system
        Gorgon::NextFrame();
    }
    
    return 0;
}
