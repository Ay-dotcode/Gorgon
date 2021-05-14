#pragma once

#include <string>
#include <functional>

namespace Gorgon { 

namespace Widgets {
    class Registry;
}
    
namespace UI {
    
    /// Initializes the UI system. Creates a simple widget template
    /// generator based on the primary monitor resolution. Density
    /// controls the size of the widgets. Increased density leads
    /// to smaller widgets. 7.5 leads to 12pt/16px on a FullHD monitor.
    /// This is a very relaxed and easy to use and read. 10 is
    /// more or less standard density.
    void Initialize(float density = 7.5, int min = 9);
    
    /// Initializes the UI system with the supplied registry. The
    /// registry will be activated. Ensure registry is initialized
    /// before supplied to this function.
    void Initialize(Widgets::Registry &reg);
    
    /// Calculates based on the primary monitor resolution. Density
    /// controls the size of the font. Increased density leads
    /// to smaller fonts. 7.5 leads to 12pt/16px on a FullHD monitor.
    /// This is a very relaxed and easy to use and read. 10 is
    /// more or less standard density.
    int FontHeight(float density = 7.5, int min = 9);
    
    extern std::function<void(std::string)> InPageHandler;
    
    /// Sets a function that will be called if an in page link starting with # is encountered in
    /// a widget that supports links.
   inline void RegisterInPageLinkHandler(std::function<void(std::string)> handler) {
        InPageHandler = handler;
    }
    
} }
