#pragma once

#include <string>

namespace Gorgon { namespace UI {
    
    /// Initializes the UI system. Creates a simple widget template
    /// generator based on the primary monitor resolution. Density
    /// controls the size of the widgets. Increased density leads
    /// to smaller widgets. 120 leads to 12pt/16px on a FullHD monitor.
    /// This is a very relaxed and easy to use and read size.
    void Initialize(std::string fontname = "", int density = 120, int min = 10);
    
} }
