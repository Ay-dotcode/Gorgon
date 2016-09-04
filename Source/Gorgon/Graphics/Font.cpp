#include "Font.h"

namespace Gorgon { namespace Graphics {

    void BitmapFont::AddGlyph(Glyph glyph, const RectangularDrawable& bitmap, float baseline) {
        if(maxwidth == 0) {
            auto size = bitmap.GetSize();
            maxwidth = size.Width;
            lineheight = size.Height;
        }
        else {
            auto size = bitmap.GetSize();
            
            if(size.Width != maxwidth)
                isfixedw = false;
            
            if(maxwidth < size.Width)
                maxwidth = size.Width;
            
            if(lineheight < size.Height)
                lineheight = size.Height;
        }
        
        glyphmap[glyph] = GlyphDescriptor(bitmap, this->baseline - baseline);
    }


    void BitmapFont::Render(Glyph chr, TextureTarget& target, Geometry::Pointf location, RGBAf color) const {
        if(glyphmap.count(chr)) {
            auto glyph = glyphmap.at(chr);
            glyph.image->Draw(target, location + Geometry::Pointf(0, glyph.offset), color);
        }
    }


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

} }
