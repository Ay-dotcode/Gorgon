#pragma once

#include "HTMLRenderer.h"

namespace Gorgon { namespace Graphics {

void HTMLRenderer::print(TextureTarget &target, const std::string &str, int x, int y) {
    unsigned int offset = 0;
    int tagcnt = 0; // !!! debug
    bool intag = false;
    bool tagtaken = false;
    bool remove = false;
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
                remove = false;
                removestyle(tag);
                if(drawunderlined) {
                    drawunderlined = false;
                    // @TODO handle properties of underlined tag
                    target.Draw((float)underlinedstart, (float)(y + renderer.GetGlyphRenderer()->GetUnderlineOffset()),
                                (float)(x  - underlinedstart),
                                (float)renderer.GetGlyphRenderer()->GetLineThickness());
                }
                if(drawstriked) {
                    drawstriked = false;
                    // @TODO: handle properties of strike tag
                    target.Draw((float)strikedstart, (float)(y + renderer.GetStrikePosition()),
                                (float)(x  - strikedstart),
                                (float)renderer.GetGlyphRenderer()->GetLineThickness());
                }
            }
            else {
                underlinedstart = x;
                strikedstart = x;
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
            int width = renderer.GetGlyphRenderer()->GetSize(internal::decode(start, str.end())).Width;
            int spacing = renderer.GetGlyphRenderer()->GetGlyphSpacing();
            offset += width;
            offset += spacing;
            text += current;
        }
    }
    
    if(!text.empty()) {
        renderer.Print(target, text, x, y);
    }
}
        
} // end of namespace Graphics
} // end of namespace Gorgon
