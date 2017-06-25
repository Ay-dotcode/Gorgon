#pragma once

#include "HTMLRenderer.h"

namespace Gorgon { namespace Graphics {


HTMLRenderer::HTMLRenderer(GlyphRenderer &glyphrend, RGBAf color, TextShadow shadow):
    glyphrend(glyphrend),
    renderer(glyphrend, color, shadow)
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
            offset += glyphrend.GetSize(internal::decode(start, str.end())).Width;
            offset += glyphrend.GetGlyphSpacing();
            text += current;
        }
    }
    
    if(!text.empty()) {
        renderer.Print(target, text, x, y);
    }
}

HTMLRenderer::Tag HTMLRenderer::string2tag(const std::string &tag) {
    if(tag == "u") { return Tag::Underlined; }
    else if(tag == "i") { return Tag::Bold; }
    else { assert(false); }
}

void HTMLRenderer::applystyle(const std::string &str) {
    Tag tag = string2tag(str);
    switch(tag) {
        case Tag::Underlined:
            renderer.SetUnderline(true);
            break;
        case Tag::Bold:
            //!!! italic implementation comes here
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
            //!!! italic implementation comes here
            break;
        default:
            assert(false);
            break;
    }
}
        
} // end of namespace Graphics
} // end of namespace Gorgon
