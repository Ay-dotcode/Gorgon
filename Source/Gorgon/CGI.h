#pragma once

#include "Graphics/Color.h"

namespace Gorgon { namespace CGI {
    
    /**
     * Fills a drawing with a solid color
     */
    template<class Color_ = Graphics::RGBA>
    class SolidFill {
    public:
        typedef Color_ ColorType;
        
        /// Implicit typecast from a solid color
        SolidFill(Color_ color) : color(color) { }
        
        /// Sets the color
        void SetColor(Color_ value) {
            color = value;
        }
        
        /// Returns the color
        Color_ GetColor() const {
            return color;
        }
        
        Color_ operator()(float/* relx*/, float/* rely*/, float/* absx*/, float/* absy*/, Color_/* underlying*/) {
            return color;
        }
        
    private:
        Color_ color;
    };
    
} }
