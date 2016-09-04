#include "Font.h"
#include <Gorgon/Utils/Assert.h>

namespace Gorgon { namespace Graphics {


    void BasicFont::print(TextureTarget& target, const std::string& text, Geometry::Pointf location, RGBAf color) const { 
        auto cur = location;
        for(auto it=text.begin(); it!=text.end(); ++it) {
            Byte b = *it;
            Glyph g;
            
            if(b<127) { //ASCII
                g = b;
            }
            else {
                Utils::NotImplemented("Unicode support is not yet implemented.");
            }
            
            if(g == '\t') {
                auto stops = renderer->MaxWidth() * 8;
                cur.X += stops;
                cur.X /= stops;
                cur.X = std::floor(cur.X);
                cur.X *= stops;
            }
            else if(g == '\n') {
                cur.X = location.X;
                cur.Y += renderer->LineHeight();
            }
            else if(g > 32) {
                renderer->Render(g, target, cur, color);
                cur.X += renderer->GetSize(g).Width;
            }
        }
    }

    void BasicFont::print(TextureTarget& target, const std::string& text, Geometry::Rectanglef location, TextAlignment align, RGBAf color) const { }


    Geometry::Size BasicFont::GetSize(const std::string& text) const {
        Geometry::Pointf cur = {0, 0};
        int maxx = 0;
        
        auto lh = renderer->LineHeight();
        
        for(auto it=text.begin(); it!=text.end(); ++it) {
            Byte b = *it;
            Glyph g;
            
            if(b<127) { //ASCII
                g = b;
            }
            else {
                Utils::NotImplemented("Unicode support is not yet implemented.");
            }
            
            if(g == '\t') {
                auto stops = renderer->MaxWidth() * 8;
                cur.X += stops;
                cur.X /= stops;
                cur.X = std::floor(cur.X);
                cur.X *= stops;
            }
            else if(g == '\n') {
                if(maxx < cur.X) maxx = (int)std::round(cur.X);
                
                cur.X = 0;
                cur.Y += lh;
            }
            else if(g > 32) {
                cur.X += renderer->GetSize(g).Width;
            }
        }
        
        if(maxx < cur.X) maxx = (int)std::round(cur.X);
        
        return {maxx, (int)std::round(cur.Y + lh)};
    }

} }
