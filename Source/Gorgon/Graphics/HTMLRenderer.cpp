#pragma once

#include "HTMLRenderer.h"

namespace Gorgon { namespace Graphics {

FontFamily::FontFamily(std::initializer_list<std::pair<GlyphRenderer *, Style>> list)
    : fonts(list)
{}

GlyphRenderer& FontFamily::GetGlyphRenderer(Style style) {
    for(const auto &font: fonts) {
        if(font.second == style) {
            return *(font.first);
        }
    }
    
    // !!! better error handling is needed
    assert(false);
}

HTMLRenderer::HTMLRenderer(FontFamily &fontfamily, RGBAf color, TextShadow shadow)
    : fontfamily(fontfamily),
      renderer(fontfamily.GetGlyphRenderer(FontFamily::Style::Normal))
{}

void HTMLRenderer::Print(TextureTarget &target, const std::string &text, int x, int y) {
    print(target, text, x, y);
}

void HTMLRenderer::print(TextureTarget &target, const std::string &str, int x, int y) {
    bool intag = false;
    bool tagtaken = false;
    bool remove = false;
    int tagcnt = 0; // !!! debug
    unsigned int offset = 0;
    char current;
    std::string text, tag;
    for(std::size_t i = 0; i < str.length(); i++) {
        current = str[i];
        if(current == '<') {
            if(!intag && !text.empty()) {
                renderer.Print(target, text, x, y);
                x += offset;
                offset = 0;
                text.clear();
            }
            intag = true;
            continue;
        }
        else if(current == '>') {
            if(remove) {
                removestyle(tag);
            }
            else {
                applystyle(tag);
            }
            tag.clear();
            tagtaken = false;
            intag = false;
            tagcnt++; // !!! debug
            continue;
        }
        else if(current == '/') {
            remove = true;
            continue;
        }

        if(intag) {
            if(!tagtaken) {
                if(internal::isspace(current)) {
                    tagtaken = true;
                    continue;
                }
            }
            tag += current; 
        }
        else {
            auto start = str.begin() + i;
            offset += renderer.GetGlyphRenderer()->GetSize(internal::decode(start, str.end())).Width;
            offset += renderer.GetGlyphRenderer()->GetGlyphSpacing();
            text += current;
        }
    }
    
    if(!text.empty()) {
        renderer.Print(target, text, x, y);
    }
}

HTMLRenderer::Tag HTMLRenderer::string2tag(const std::string &tag) {
    if(tag == "u") { return Tag::Underlined; }
    else if(tag == "b") { return Tag::Bold; }
    else { assert(false); }
}

void HTMLRenderer::applystyle(const std::string &str) {
    Tag tag = string2tag(str);
    switch(tag) {
        case Tag::Underlined:
            renderer.SetUnderline(true);
            break;
        case Tag::Bold:
            renderer.SetGlyphRenderer(&fontfamily.GetGlyphRenderer(FontFamily::Style::Bold));
            break;
        default:
            assert(false);
            break;
    }
}

void HTMLRenderer::removestyle(const std::string &str) {
    Tag tag = string2tag(str);
    switch(tag) {
        case Tag::Underlined:
            renderer.SetUnderline(false);
            break;
        case Tag::Bold:
            renderer.SetGlyphRenderer(&fontfamily.GetGlyphRenderer(FontFamily::Style::Normal));
            break;
        default:
            assert(false);
            break;
    }
}
        
} // end of namespace Graphics
} // end of namespace Gorgon
