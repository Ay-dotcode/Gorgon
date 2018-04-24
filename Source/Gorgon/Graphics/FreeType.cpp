#include "Bitmap.h"
#include "FreeType.h"

#include <ft2build.h>
#include FT_FREETYPE_H


namespace Gorgon { namespace Graphics {
    
    struct ftlib {
        ftlib() {
            FT_Init_FreeType(&library);
        }
        
        void destroyface() {
            if(face)
                FT_Done_Face(face);
            
            face = nullptr;
        }
        
        ~ftlib() {
            FT_Done_FreeType(library);
            library = nullptr;
            
            destroyface();
        }
        
        FT_Library library;
        FT_Face    face = nullptr;
    };
    
    
    FreeType::FreeType() {
        lib = new ftlib;
    }
    
    FreeType::~FreeType() {
        delete lib;
        
        destroylist.Destroy();
    }
    
    bool FreeType::LoadFile(const std::string &filename) {
        lib->destroyface();
        
        auto error = FT_New_Face(lib->library, filename.c_str(), 0, &lib->face );
        
        if(error != FT_Err_Ok || lib->face == nullptr)
            return false;
        
        isfixedw = (lib->face->face_flags & FT_FACE_FLAG_FIXED_WIDTH) != 0;
        
        //no unicode charmap table
        if(lib->face->charmap == nullptr) {
            error = FT_Select_Charmap(lib->face, FT_ENCODING_APPLE_ROMAN);
            
            if(error != FT_Err_Ok)
                error = FT_Select_Charmap(lib->face, FT_ENCODING_ADOBE_LATIN_1);
            
            if(error != FT_Err_Ok)
                error = FT_Select_Charmap(lib->face, FT_ENCODING_ADOBE_STANDARD);
            
            if(error != FT_Err_Ok)
                error = FT_Select_Charmap(lib->face, FT_ENCODING_ADOBE_EXPERT);
            
            if(error != FT_Err_Ok)
                error = FT_Select_Charmap(lib->face, FT_ENCODING_ADOBE_CUSTOM);
            
            if(error == FT_Err_Ok) {
                isascii = true;
            }
            else {
                error = FT_Select_Charmap(lib->face, FT_ENCODING_MS_SYMBOL);
                
                if(error == FT_Err_Ok) {
                    issymbol = true;
                }
                else {
                    lib->destroyface();
                    
                    return false;
                }
            }
        }
        
        haskerning = FT_HAS_KERNING(lib->face);
        
        return true;
    }
    
    bool FreeType::LoadFile(const std::string &filename, int size, bool loadascii) {
        if(!LoadFile(filename))
            return false;
        
        if(!LoadMetrics(size))
            return false;
        
        if(loadascii)
            return LoadGlyphs(0x20, 0x7f);
        else
            return true;
    }
    
    
    bool FreeType::LoadMetrics(int size) {
        if(!lib->face)
            return false;
        
        if(IsScalable()) {            
            auto error = FT_Set_Pixel_Sizes(lib->face, 0, size);
            
            if(error != FT_Err_Ok)
                return false;
        
            auto xscale = lib->face->size->metrics.x_scale;
            auto yscale = lib->face->size->metrics.y_scale;
            
            maxwidth = (int)std::round(FT_MulFix((lib->face->bbox.xMax-lib->face->bbox.xMin), xscale)/64.f);
            height   = (int)std::round(FT_MulFix((lib->face->bbox.yMax-lib->face->bbox.yMin), yscale)/64.f);
            
            baseline = (int)std::round(lib->face->size->metrics.ascender/64.f);
           
            underlinepos  = baseline - (int)std::round(FT_MulFix((lib->face->underline_position),yscale)/64.f);
            linethickness = (int)std::round(FT_MulFix((lib->face->underline_thickness),xscale)/64.f);
         }
        else {
            //search bitmap size table and find values that is closest to the given one.
            
            int mindiff = INT_MAX;
            int minind  = -1;
            
            for(int i=0; i<lib->face->num_fixed_sizes; i++) {
                auto s = lib->face->available_sizes[i];
                auto diff = abs(s.height - size);
                
                if(diff < mindiff) {
                    mindiff = diff;
                    minind  = i;
                }
            }
            
            if(minind == -1)
                return false;
            
            auto s = lib->face->available_sizes[minind];

            auto error = FT_Set_Pixel_Sizes(lib->face, s.width, s.height);
            
            if(error != FT_Err_Ok)
                return false;
            
            baseline = (int)std::round(lib->face->size->metrics.ascender/64.f);
        
            maxwidth = s.width;
            height   = s.height;
            
            underlinepos = (int)std::round((baseline + height) / 2.f);
            
            linethickness = height / 10;
            
            if(linethickness < 1) linethickness = 1;
        }
        
        return true;
    }
    
    
    bool FreeType::LoadGlyphs(GlyphRange range, bool prepare) {
        if(!lib->face)
            return false;
        
        int done = 0;
                    
        auto slot = lib->face->glyph;

        for(Glyph g = range.Start; g <= range.End; g++) {
            done++; //already loaded glyphs are also counted as done
            
            //we already have this glyph
            if(glyphmap.count(g)) continue;

            auto index = FT_Get_Char_Index(lib->face, g);
            
            //check if glyph is already loaded. if so use the same
            if(ft_to_map.count(index) && glyphmap.count(ft_to_map[index])) {
                glyphmap[g] = glyphmap[ft_to_map[index]];
                continue;
            }
            
            auto error = FT_Load_Glyph(lib->face, index, FT_LOAD_RENDER);
            
            if(error != FT_Err_Ok) {
                if(range.Count() > 1) {
                    //error, this one is not done
                    done--;
                    continue;
                }
                else
                    return false;
            }
            
            auto &ftbmp = slot->bitmap;
            
            auto &bmp = *new Bitmap(ftbmp.width, ftbmp.rows, ColorMode::Alpha);

            //FT and Gorgon ColorMode::Alpha has same binary representation.
            bmp.Assign(ftbmp.buffer);
            
            destroylist.Add(bmp);
            
            glyphmap[g] = {bmp, (int)std::round(slot->advance.x/64.f), {(int)slot->bitmap_left, (int)-slot->bitmap_top}};
            ft_to_map[index] = g;
            
            if(prepare)
                bmp.Prepare();
        }
        
        return done > 0;
    }

    
    bool FreeType::IsScalable() const {
        if(!lib->face)
            return false;
        
        return (lib->face->face_flags & FT_FACE_FLAG_SCALABLE) != 0;
    }
     
     
    std::vector<int> FreeType::GetPresetSizes() const {
        if(!lib->face)
            return {};
        
        std::vector<int> ret;
        ret.reserve(lib->face->num_fixed_sizes);
     
        for(int i=0; i<lib->face->num_fixed_sizes; i++) {
            ret.push_back(lib->face->available_sizes[i].height);
        }
        
        return ret;
    }
    
    
    std::string FreeType::GetFamilyName() const {
        if(!lib->face)
            return {};
        
        return lib->face->family_name;
    }
    
    
    std::string FreeType::GetStyleName() const {
        if(!lib->face)
            return {};
        
        return lib->face->style_name;
    }
    
    
    bool FreeType::IsFileLoaded() const {
        return lib->face != nullptr;
    }
    
    
    bool FreeType::IsReady() const {
        return height != 0 && (lib->face != nullptr || !glyphmap.empty());
    }

    Geometry::Size FreeType::GetSize(Glyph chr) const {
		if(glyphmap.count(chr))
			return glyphmap.at(chr).image->GetSize();
		else if(glyphmap.count(0) && !internal::isspace(chr) && !internal::isnewline(chr) && chr != '\t')
			return glyphmap.at(0).image->GetSize();
		else
			return{0, 0};
	}

    int FreeType::GetCursorAdvance(Glyph chr) const  {
		if(glyphmap.count(chr))
			return glyphmap.at(chr).advance;
        else if(chr == '\t')
            return height;
        else if(internal::isspace(chr))
            return height / 4;
		else if(glyphmap.count(0))
			return glyphmap.at(0).advance;
		else
			return 0;
	}
	
	bool FreeType::Exists(Glyph g) const {
        if(glyphmap.count(g))
			return true;
        
        //if(!lib->face)
            return false;
        
        //return FT_Get_Char_Index(lib->face, g) != 0;
    }
    
    void FreeType::Render(Glyph chr, TextureTarget &target, Geometry::Pointf location, RGBAf color) const {
        //todo load additional glyphs when necessary
        if(glyphmap.count(chr)) {
            auto glyph = glyphmap.at(chr);
            glyph.image->Draw(target, location + glyph.offset + Geometry::Pointf(0.f, (float)baseline), color);
        }
		else if(glyphmap.count(0) && !internal::isspace(chr) && !internal::isnewline(chr) && chr != '\t') {
			auto glyph = glyphmap.at(0);
			glyph.image->Draw(target, location + glyph.offset + Geometry::Pointf(0.f, (float)baseline), color);
		}
    }
    
} }
