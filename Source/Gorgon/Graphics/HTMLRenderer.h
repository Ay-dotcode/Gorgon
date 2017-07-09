#pragma once

#include <string>
#include <cassert>
#include <map>
#include <utility>

#include "Color.h"
#include "Font.h"
#include "TextureTargets.h"

namespace Gorgon { namespace Graphics {

// !!! find a better place?
class FontFamily {
    
public:
    
    enum class Style: unsigned int{
        Normal = 0,
        Italic,
        Bold,
        End
    };
    
    FontFamily(const std::map<Style, GlyphRenderer *> fonts): fonts(fonts)
    {}
    
    GlyphRenderer& GetGlyphRenderer(Style style) {
        if(fonts.count(style)) {
            return *(fonts[style]);
        }
        
        const unsigned int start = static_cast<unsigned int>(Style::Normal);
        const unsigned int end = static_cast<unsigned int>(Style::End);
        for(unsigned int i = start; i < end; i++) {
            if(fonts.count(static_cast<Style>(i))) {
                return *(fonts[static_cast<Style>(i)]);
            }
        }
        
        // !!! empty map
        assert(false);
    }
    
    void AddFont(Style style, GlyphRenderer * renderer) {
        fonts[style] = renderer;
    }
    
    void RemoveFont(Style style) {
        fonts.erase(style);
    }
    
    bool HasFont(Style style) const {
        return fonts.count(style);
    }

private:
    std::map<Style, GlyphRenderer *> fonts;
};
    
class HTMLRenderer {
        
        enum class Tag: unsigned int {
            Underlined = 0,
            Striked,
            Bold,
            End
        };
        
    public:
        HTMLRenderer(FontFamily &fontfamily, RGBAf color = 1.f, TextShadow shadow = {}):
            fontfamily(fontfamily),
            renderer(fontfamily.GetGlyphRenderer(FontFamily::Style::Normal)),
            drawunderlined(false),
            drawstriked(false)
        {}

    void Print(TextureTarget &target, const std::string &text, int x, int y) {
        print(target, text, x, y);
    }
        
    private:
        void print(TextureTarget &target, const std::string &str, int x, int y);
        
        Tag string2tag(const std::string &tag) {
            if(tag == "u") { return Tag::Underlined; }
            else if(tag == "strike") { return Tag::Striked; }
            else if(tag == "b") { return Tag::Bold; }
            else { assert(false); }
        }

        void applystyle(const std::string &str) {
            Tag tag = string2tag(str);
            switch(tag) {
                case Tag::Underlined:
                    break;
                case Tag::Striked:
                    break;
                case Tag::Bold:
                    renderer.SetGlyphRenderer(&fontfamily.GetGlyphRenderer(FontFamily::Style::Bold));
                    break;
                default:
                    assert(false);
                    break;
            }
        }

        void removestyle(const std::string &str) {
            Tag tag = string2tag(str);
            switch(tag) {
                case Tag::Underlined:
                    drawunderlined = true;
                    break;
                case Tag::Striked:
                    drawstriked = true;
                    break;
                case Tag::Bold:
                    renderer.SetGlyphRenderer(&fontfamily.GetGlyphRenderer(FontFamily::Style::Normal));
                    break;
                default:
                    assert(false);
                    break;
            }
        }
        
        FontFamily &fontfamily;
        StyledRenderer renderer;
        bool drawunderlined;
        bool drawstriked;
        unsigned int underlinedstart;
        unsigned int strikedstart;
    };
    
}}
