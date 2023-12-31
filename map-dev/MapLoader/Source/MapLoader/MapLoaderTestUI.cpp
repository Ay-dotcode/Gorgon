#include "MapLoaderTestUI.h"
#include "MapLoader.h"
#include <Gorgon/UI/Window.h>
#include <Gorgon/UI/Dimension.h>


// Create Window it self
TestUI::TestUI(UI::Window &window) : 
    window(window)
{
    window.Add(sidePanel);
    sidePanel.SetInteriorWidth(8_u); 
    sidePanel.SetHeight(100_perc); 
    sidePanel.AttachOrganizer(sideOrganizer); 

    window.AddNextTo(layerbox); 
    layerbox.SetWidth(window.GetInteriorSize().Width - layerbox.GetCurrentLocation().X); 
    layerbox.SetHeight(100_perc); 

    window.ResizedEvent.Register([this] -> void {
        layerbox.SetWidth(this->window.GetInteriorSize().Width - layerbox.GetCurrentLocation().X);
    });

    layerbox.GetLayer().Add(targetlayer); 

    load.Text = "Load";
    clear.Text = "Clear";

    load.SetWidth(4);
    load.ClickEvent.Register(*this, &TestUI::Run); 

    clear.ClickEvent.Register([this] -> void {
        bitmap.Clear(); 
    });
}

void TestUI::Run() {
    MapLoader loader; 
}