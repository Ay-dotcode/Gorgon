#pragma once
#include <Gorgon/Graphics/Bitmap.h>
#include <Gorgon/Layer.h>
#include <Gorgon/UI/Organizers/Flow.h>
#include <Gorgon/UI/Window.h>
#include <Gorgon/Widgets/Button.h>
#include <Gorgon/Widgets/Layerbox.h>
#include <Gorgon/Widgets/Window.h>
#include <Gorgon/Widgets/Panel.h>

namespace UI = Gorgon::UI; 
namespace Widgets = Gorgon::Widgets; 

using namespace Gorgon::UI::literals; 

class TestUI {

private: 
    Widgets::Panel sidePanel; 
    UI::Organizers::Flow sideOrganizer; 
    UI::Window &window; 
    Gorgon::Widgets::Layerbox layerbox; 
    Gorgon::Graphics::Bitmap bitmap; 
    Gorgon::Layer targetlayer; 

    Gorgon::Widgets::Button load, clear; 

public: 
    TestUI(UI::Window&); 
    void Run();

};
