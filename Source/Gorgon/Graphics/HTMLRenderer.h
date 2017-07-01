#pragma once

#include <string>
#include <initializer_list>
#include <cassert>

#include "Color.h"
#include "Font.h"
#include "TextureTargets.h"

namespace Gorgon { namespace Graphics {

// !!! find a better place?
class FontFamily {
public:
    
    enum class Style: unsigned int {
        Normal = 0,
        Italic,
        Bold,
        End
    };
    
    FontFamily(std::initializer_list<std::pair<GlyphRenderer *, Style>> list);
    
    GlyphRenderer& GetGlyphRenderer(Style style);
    

private:
    std::vector<std::pair<GlyphRenderer *, Style>> fonts;
};
    
class HTMLRenderer {
        
        enum class Tag: unsigned int {
            Underlined = 0,
            Striked,
            Bold,
            End
        };
        
    public:
        HTMLRenderer(FontFamily &fontfamily, RGBAf color = 1.f, TextShadow shadow = {});
        
        void Print(TextureTarget &target, const std::string &text, int x, int y);
        
    private:
        void print(TextureTarget &target, const std::string &str, int x, int y);
        
        Tag string2tag(const std::string &tag);
        
        void applystyle(const std::string &str);
        
        void removestyle(const std::string &str);
        
        FontFamily &fontfamily;
        StyledRenderer renderer;
        bool drawunderlined;
        bool drawstriked;
        unsigned int underlinedstart;
        unsigned int strikedstart;
    };
    
}}
