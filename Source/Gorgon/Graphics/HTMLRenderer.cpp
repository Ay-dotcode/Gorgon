#include "HTMLRenderer.h"

#include <utility>
#include <vector>
#include <string>

namespace Gorgon { namespace Graphics {

namespace HTMLRendererInternal {
Utils::Logger Logger;
}

    
void HTMLRenderer::parseandprint(TextureTarget &target, const std::string &str, int x, int y) {
    
    // clear previous state
    drawunderlined = false;
    drawstriked = false;
    underlinedstart = 0;
    strikedstart = 0;
    baselineoffset = 0;
    xx = x;
    yy = y;
    
    unsigned int offset = 0;
    int tagcnt = 0; // !!! debug
    bool intag = false;
    bool remove = false;
    bool inquote = false;
    bool parseattrbts = false;
    char current;
    std::string text, tag, attribute, attval;
    std::vector<std::pair<std::string, std::string>> attributes;
    
    for(std::size_t i = 0; i < str.length(); i++) {
        current = str[i];
        if(current == '<') {
            if(!intag && !text.empty()) {
                print(target, text, xx, yy);
                xx += offset;
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
                    // !!! do we need baselineoffset here? it's always zero (or is it?)
                    // @TODO handle properties of underlined tag
                    target.Draw((float)underlinedstart, (float)(yy + renderer.GetGlyphRenderer()->GetUnderlineOffset() /*+ baselineoffset*/),
                                (float)(xx  - underlinedstart),
                                (float)renderer.GetGlyphRenderer()->GetLineThickness());
                }
                if(drawstriked) {
                    drawstriked = false;
                    // !!! do we need baselineoffset here? it's always zero (or is it?)
                    // @TODO: handle properties of strike tag
                    target.Draw((float)strikedstart, (float)(yy + renderer.GetStrikePosition()  /*+ baselineoffset*/),
                                (float)(xx  - strikedstart),
                                (float)renderer.GetGlyphRenderer()->GetLineThickness());
                }
            }
            else {
                parseattrbts = false;
                if(!attribute.empty()) {
                    attributes.emplace_back(std::pair<std::string, std::string>(attribute, attval));
                    attribute.clear();
                    attval.clear();
                }
                
                attributes.clear();
                applystyle(tag);
            }
            tag.clear();
            intag = false;
            tagcnt++; // !!! debug
            continue;
        }
        else if(current == '/') {
            remove = true;
            continue;
        }
        else if(current == '\"' || current == '\'') {
            if(inquote) {
                inquote = false;
            }
            else {
                inquote = true;
            }
            continue;
        }
        
        if(intag) {
            // start parsing attributes once we encounter a space while parsing tag
            if(internal::isspace(current) && !parseattrbts) {
                parseattrbts = true;
                continue;
            }
            if(parseattrbts) {
                // ignore assignment character
                if(current == '=') {
                    continue;
                }
                // keep filling attribute vector as we encounter spaces while parsing attributes
                else if(internal::isspace(current)) {
                    attributes.emplace_back(std::pair<std::string, std::string>(attribute, attval));
                    attribute.clear();
                    attval.clear();
                }
                // if we are inside quote, accumulate into attribute value string
                // otherwise, use attirubte string
                else {
                    if(inquote) {
                        attval += current;
                    }
                    else {
                        attribute += current;
                    }
                }
            }
            else {
                tag += current;
            }
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
        print(target, text, xx, yy);
    }
}
        
} // end of namespace Graphics
} // end of namespace Gorgon
