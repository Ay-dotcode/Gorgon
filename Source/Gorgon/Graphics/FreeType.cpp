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
    }
    
    bool FreeType::LoadFile(const std::string &filename) {
        lib->destroyface();
        
        auto error = FT_New_Face(lib->library, filename.c_str(), 0, &lib->face );
        
        return error == FT_Err_Ok && lib->face;
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
        
        auto error = FT_Set_Pixel_Sizes(lib->face, 0, size);
        
        if(error != FT_Err_Ok)
            return false;
        
        
        //transform and load metrics
        
        
        return true;
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
    
    
    bool FreeType::IsFileLoaded() const {
        return lib->face != nullptr;
    }
    
    bool FreeType::IsReady() const {
        return height != 0 && (lib->face != nullptr || !glyphmap.empty());
    }
} }
