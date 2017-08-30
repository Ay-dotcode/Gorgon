#pragma once

#include <string>
#include <cassert>
#include <map>
#include <utility>

#include "Color.h"
#include "Font.h"
#include "TextureTargets.h"

#include "../Utils/Assert.h"

namespace Gorgon { namespace Graphics {

// !!! find a better place?
class FontFamily {
    
public:
    
    enum class Style: unsigned int{
        Normal = 0,
        Italic,
        Bold,
        Custom,
        End
    };
    
    FontFamily(const std::map<Style, GlyphRenderer *> fonts): fonts(fonts)
    {}
    
    GlyphRenderer* GetGlyphRenderer(Style style) {
        
        if(fonts.count(style)) {
            return fonts[style];
        }
        
        const unsigned int start = static_cast<unsigned int>(Style::Normal);
        const unsigned int end = static_cast<unsigned int>(Style::End);
        for(unsigned int i = start; i < end; i++) {
            if(fonts.count(static_cast<Style>(i))) {
                return fonts[static_cast<Style>(i)];
            }
        }
        
        // !!! we do not want to return a nullptr, let it crash
        ASSERT(false, "empty font family map");
        
        // !!! silence compiler warnings
        return nullptr;
    }
    
    void AddFont(Style style, GlyphRenderer *renderer) {
        fonts[style] = renderer;
    }
    
    void RemoveFont(Style style) {
        fonts.erase(style);
    }
    
    bool HasFont(Style style) const {
        return static_cast<bool>(fonts.count(style));
    }

private:
    std::map<Style, GlyphRenderer*> fonts;
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
            renderer(*fontfamily.GetGlyphRenderer(FontFamily::Style::Normal)),
            drawunderlined(false),
            drawstriked(false),
            underlinedstart(0),
            strikedstart(0),
            baselineoffset(0)
        {}

    void Print(TextureTarget &target, const std::string &text, int x, int y) {
        parseandprint(target, text, x, y);
    }
        
    private:
        void parseandprint(TextureTarget &target, const std::string &str, int x, int y);
        
        Tag string2tag(const std::string &tag) {
            if(tag == "u")           { return Tag::Underlined; }
            else if(tag == "strike") { return Tag::Striked; }
            else if(tag == "b")      { return Tag::Bold; }
            else                     { ASSERT(false, "unsupported tag: " + tag); return Tag::End; }
        }

        void applystyle(const std::string &str) {
            Tag tag = string2tag(str);
            switch(tag) {
                case Tag::Underlined:
                    underlinedstart = xx;
                    break;
                case Tag::Striked:
                    strikedstart = xx;
                    break;
                case Tag::Bold:
                    changeglyphrenderer(FontFamily::Style::Bold);
                    break;
                default:
                    ASSERT(false, "unsupported tag: " + str);
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
                    changeglyphrenderer(FontFamily::Style::Normal);
                    break;
                default:
                    ASSERT(false, "unsupported tag: " + str);
                    break;
            }
        }
        
        // !!! further cases can ben covered in a switch-case/if-else block
        void changeglyphrenderer(FontFamily::Style newstyle) {
            // store previous baseline
            int prevbaselineoffset = renderer.GetGlyphRenderer()->GetBaseLine();
            
            renderer.SetGlyphRenderer(fontfamily.GetGlyphRenderer(newstyle));
            
            // calculate the baseline offset if there is a difference in baselines
            if((prevbaselineoffset - renderer.GetGlyphRenderer()->GetBaseLine()) < 0) {
                baselineoffset =  prevbaselineoffset - renderer.GetGlyphRenderer()->GetBaseLine();
            }
            else {
                baselineoffset = 0;
            }
        }
        
        void print(TextureTarget &target, const std::string &text, int x, int y) {
            renderer.Print(target, text, x, y + baselineoffset);
        }
        
        FontFamily &fontfamily;
        StyledRenderer renderer;
        bool drawunderlined;
        bool drawstriked;
        unsigned int underlinedstart;
        unsigned int strikedstart;
        int baselineoffset;
        int xx;
        int yy;
    };
    
}}
