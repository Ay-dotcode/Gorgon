#pragma once

#include <string>

namespace Gorgon { namespace UI {
    
    /// Initializes the UI system. Creates a simple widget template
    /// generator based on the primary monitor resolution. Density
    /// controls the size of the widgets. Increased density leads
    /// to smaller widgets. 7.5 leads to 12pt/16px on a FullHD monitor.
    /// This is a very relaxed and easy to use and read. 10 is
    /// more or less standard density.
    void Initialize(std::string fontname = "", std::string boldfontname = "", float density = 7.5, int min = 9);
    
    /// Initializes the UI system. Creates a simple widget template
    /// generator based on the primary monitor resolution. Density
    /// controls the size of the widgets. Increased density leads
    /// to smaller widgets. 7.5 leads to 12pt/16px on a FullHD monitor.
    /// This is a very relaxed and easy to use and read. 10 is
    /// more or less standard density.
    inline void Initialize(float density, int min = 9) {
        Initialize("", "", density, min);
    }
    
    /// Calculates based on the primary monitor resolution. Density
    /// controls the size of the font. Increased density leads
    /// to smaller fonts. 7.5 leads to 12pt/16px on a FullHD monitor.
    /// This is a very relaxed and easy to use and read. 10 is
    /// more or less standard density.
    int FontHeight(float density = 7.5, int min = 9);
    
    
    
} }
