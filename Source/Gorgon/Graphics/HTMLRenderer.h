#pragma once

#include <string>
#include <cassert>
#include <map>
#include <utility>

#include "Color.h"
#include "Font.h"
#include "TextureTargets.h"

#include "../Utils/Assert.h"
#include "../Utils/Logging.h"

#define HR_REQ_SEMICOLON(statement) do { statement; } while(false)
#define HR_LOG(string, state)       HR_REQ_SEMICOLON(HTMLRendererInternal::Logger.Log(string, state))
#define HR_LOG_ERROR(string)        HR_LOG(string, Utils::Logger::State::Error)
#define HR_LOG_NOTICE(string)       HR_LOG(string, Utils::Logger::State::Notice)
#define HR_LOG_MESSAGE(string)      HR_LOG(string, Utils::Logger::State::Message)

#define HR_BTWSOR_TAG_ATTR_PAIR(tag, attribute) \
    (static_cast<unsigned int>(tag) | static_cast<unsigned int>(attribute))

namespace Gorgon { namespace Graphics {

namespace HTMLRendererInternal {
extern Utils::Logger Logger;
}

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
            HR_LOG_NOTICE("font style is found");
            return fonts[style];
        }
        
        HR_LOG_NOTICE("could not find font style, will use the next available font");
        
        // given style does not exist, return the immediate next font style
        // !!! is it the best course of action?
        const unsigned int start = static_cast<unsigned int>(Style::Normal);
        const unsigned int end = static_cast<unsigned int>(Style::End);
        for(unsigned int i = start; i < end; i++) {
            if(fonts.count(static_cast<Style>(i))) {
                return fonts[static_cast<Style>(i)];
            }
        }
        
        HR_LOG_ERROR("couldn't find any font, about to crash!!!");
        
        // couldn't find any font
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

}; // end of class FontFamily

class HTMLRenderer {
public:
    HTMLRenderer(FontFamily &fontfamily, RGBAf color = 1.f, TextShadow shadow = {}):
        fontfamily(fontfamily),
        renderer(*fontfamily.GetGlyphRenderer(FontFamily::Style::Normal)),
        drawunderlined(false),
        drawstriked(false),
        underlinedstart(0),
        strikedstart(0),
        baselineoffset(0),
        ucolor(1.f),
        scolor(1.f)
    {}

    void Print(TextureTarget &target, const std::string &text, int x, int y) {
        parseandprint(target, text, x, y);
    }
    
private:
    enum class Tag: unsigned int {
        Underlined = 0x00000000,
        Striked    = 0x00000001,
        Bold       = 0x00000002,
        End
    };
    
    enum class Attribute: unsigned int {
        Color = 0x00010000,
        End
    };
    
    FontFamily &fontfamily;
    StyledRenderer renderer;
    bool drawunderlined;
    bool drawstriked;
    unsigned int underlinedstart;
    unsigned int strikedstart;
    int baselineoffset;
    int xx;
    int yy;
    
    RGBAf ucolor; // underlined
    RGBAf scolor; // strike
    
    
    
    // private methods
    void parseandprint(TextureTarget &target, const std::string &str, int x, int y);
    
    Tag string2tag(const std::string &tag) {
        if(tag == "u")           { return Tag::Underlined; }
        else if(tag == "strike") { return Tag::Striked; }
        else if(tag == "b")      { return Tag::Bold; }
        else                     { ASSERT(false, "unsupported tag: " + tag); return Tag::End; }
    }
    
    Attribute string2attribute(const std::string &attribute) {
        if(attribute == "color") { return Attribute::Color; }
        else                     { ASSERT(false, "unsupported attribute: " + attribute); return Attribute::End; }
    }

    void applytag(Tag tag) {
        // !!! TODO tag2string function
        HR_LOG_NOTICE("applying tag: " + std::to_string(static_cast<unsigned int>(tag)));
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
                // !!! TODO tag2string function
                ASSERT(false, "unsupported tag: " + std::to_string(static_cast<unsigned int>(tag)));
                break;
        }
    }

    // !!! TODO find a way to merge this function with removetag
    // !!! the problem is underline and strike application is done
    // !!! after removetag is called
    void clearattributes(Tag tag) {
        switch(tag) {
            case Tag::Underlined:
                HR_LOG_NOTICE("clearing underline attributes");
                ucolor = RGBAf(1.f);
                break;
            case Tag::Striked:
                HR_LOG_NOTICE("clearing strike attributes");
                scolor = RGBAf(1.f);
                break;
            default:
                return;
        }
    }
    
    
    void removetag(Tag tag) {
        // !!! TODO tag2string function
        HR_LOG_NOTICE("removing tag: " + std::to_string(static_cast<unsigned int>(tag)));
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
                // !!! TODO tag2string function
                ASSERT(false, "unsupported tag: " + std::to_string(static_cast<unsigned int>(tag)));
                break;
        }
    }
    
    RGBAf extractcolor(const std::string color) {
        if(color == "white")      { return RGBAf(1.f); }
        else if(color == "black") { return RGBAf(0.f); }
        else if(color == "green") { return RGBAf(0.f, 1.f, 0.f, 1.f); }
        else {
            HR_LOG_NOTICE("could not extract given color, using white");
            return RGBAf(1.f); 
        }
    }
    
    void applyattributes(Tag tag, const std::vector<std::pair<std::string, std::string>> &attributes) {
        Attribute attribute;
        for(const auto &attstr: attributes) {
            attribute = string2attribute(attstr.first);
            unsigned int mappedval = (static_cast<unsigned int>(attribute) & static_cast<unsigned int>(tag));
            if(/*issupported(tag, attribute)*/true) {
                switch(HR_BTWSOR_TAG_ATTR_PAIR(tag, attribute)) {
                    case HR_BTWSOR_TAG_ATTR_PAIR(Tag::Underlined, Attribute::Color):
                        HR_LOG_NOTICE("changing underline color");
                        ucolor = extractcolor(attstr.second);
                        break;
                    case HR_BTWSOR_TAG_ATTR_PAIR(Tag::Striked, Attribute::Color):
                        HR_LOG_NOTICE("changing strike color");
                        scolor = extractcolor(attstr.second);
                        break;
                    default:
                        ASSERT(false, "unsupported attribute: " + attstr.first);
                        break;
                }
            }
            else {
                // !!! TODO tag2string function
                HR_LOG_NOTICE("attribute is not part of tag (" + std::to_string(static_cast<unsigned int>(tag)) + ")");
            }
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
    
}; // end of class HTMLRenderer
    
}} // end of namespace Gorgon::Graphics
