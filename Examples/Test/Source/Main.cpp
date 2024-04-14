#include <Gorgon/Main.h>
#include <Gorgon/UI.h>
#include <Gorgon/UI/Window.h>
#include <Gorgon/Game/Map/Tiled.h>
#include "Application.h"


int main() {
    Gorgon::Initialize("CG");

    //Change this line to change window size or window title
    Gorgon::UI::Window window({1200, 900}, "CG Template");

    Gorgon::UI::Initialize();

    Application app(window);
    window.AllowResize();

    window.ClosingEvent.Register([&app](bool &allow) {
        allow = app.Quit();
    });

    window.Run();


    return 0;
}
