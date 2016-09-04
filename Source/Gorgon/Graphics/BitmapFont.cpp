#include "BitmapFont.h"
#include <Gorgon/Utils/Assert.h>

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
    

    bool BitmapFont::Import(const std::string& path, ImportNamingTemplate, const std::string& prefix, int baseline, bool trim) { 
        
        return false;
    }

} }
