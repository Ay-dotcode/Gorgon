#pragma once

#include <string>

#include "Color.h"
#include "Font.h"
#include "TextureTargets.h"

namespace Gorgon { namespace Graphics {

class HTMLRenderer {
        
        enum class Tag: unsigned int {
            Underlined,
            Bold,
            End
        };
        
    public:
        HTMLRenderer(GlyphRenderer &glyphrend, RGBAf color = 1.f, TextShadow shadow = {});
        
        void Print(TextureTarget &target, const std::string &text, int x, int y);
        
    private:
        void print(TextureTarget &target, const std::string &str, int x, int y);
        
        Tag string2tag(const std::string &tag);
        
        void applystyle(const std::string &str);
        
        void removestyle(const std::string &str);
        
        GlyphRenderer &glyphrend;
        StyledRenderer renderer;
    };
    
}}
