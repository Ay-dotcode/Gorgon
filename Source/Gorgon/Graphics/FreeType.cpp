#include "FreeType.h"

#include <set>

#include "Bitmap.h"
#include "../Filesystem.h"

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
            
            delete[] data;
            
            face = nullptr;
            data = nullptr;
            
            if(!vecdata.empty()) {
                std::vector<Byte> n;
                std::swap(vecdata, n);
            }
        }
        
        ~ftlib() {
            FT_Done_FreeType(library);
            library = nullptr;
            
            destroyface();
        }
        
        FT_Library library;
        FT_Face    face = nullptr;
        std::vector<Byte> vecdata; //if we own our data
        const Byte *data = nullptr;
    };
    
    
    FreeType::FreeType() : BasicFont(dynamic_cast<GlyphRenderer &>(*this)) {
        lib = new ftlib;
    }
    
    FreeType::~FreeType() {
        delete lib;
        
        destroylist.Destroy();
    }
    
    bool FreeType::finalizeload() {
        isfixedw = (lib->face->face_flags & FT_FACE_FLAG_FIXED_WIDTH) != 0;
        
        //no unicode charmap table
        if(lib->face->charmap == nullptr) {
            auto error = FT_Select_Charmap(lib->face, FT_ENCODING_APPLE_ROMAN);
            
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
        
        filename = "";
        vecdata = nullptr;
        data = nullptr;
        
        return true;
    }

    
    
    bool FreeType::LoadFile(const std::string &filename) {
        lib->destroyface();
        
        auto error = FT_New_Face(lib->library, filename.c_str(), 0, &lib->face);
        
        if(error != FT_Err_Ok || lib->face == nullptr)
            return false;
        
        if(!finalizeload())
            return false;
        
        this->filename = Filesystem::Canonical(filename);
        
        return true;
    }
    
    
    bool FreeType::LoadFile(const std::string &filename, int size, bool loadascii) {
        if(!LoadFile(filename))
            return false;
        
        if(!LoadMetrics(size))
            return false;
        
        if(loadascii)
            return LoadGlyphs({0, {0x20, 0x7f}});
        else
            return true;
    }
    
    
    bool FreeType::Load(const std::vector<Byte> &data) {
        lib->destroyface();
        
        auto error = FT_New_Memory_Face(lib->library, &data[0], (long)data.size(), 0, &lib->face);
        
        if(error != FT_Err_Ok || lib->face == nullptr)
            return false;
        
        if(!finalizeload())
            return false;
        
        vecdata = &data;
        
        return true;
    }
    
        
    bool FreeType::Load(const Byte *data, long datasize) {
        lib->destroyface();
        
        auto error = FT_New_Memory_Face(lib->library, data, (long)datasize, 0, &lib->face);
        
        if(error != FT_Err_Ok || lib->face == nullptr)
            return false;
        
        if(!finalizeload())
            return false;
        
        this->data = data;
        this->datasize = datasize;
        
        return true;
    }
    
    
    bool FreeType::Assume(std::vector<Byte> &data) {
        lib->destroyface();
        
        std::swap(lib->vecdata, data);
        
        auto error = FT_New_Memory_Face(lib->library, &lib->vecdata[0], (long)lib->vecdata.size(), 0, &lib->face);
        
        if(error != FT_Err_Ok || lib->face == nullptr)
            return false;
        
        if(!finalizeload())
            return false;
        
        this->vecdata = &lib->vecdata;
        
        return true;
    }
    
    
    bool FreeType::Assume(const Byte *data, long datasize) {
        lib->destroyface();
        
        lib->data = data;
        
        auto error = FT_New_Memory_Face(lib->library, data, datasize, 0, &lib->face);
        
        if(error != FT_Err_Ok || lib->face == nullptr)
            return false;
        
        if(!finalizeload())
            return false;
        
        this->data = data;
        this->datasize = datasize;
        
        return true;
    }
    

	bool FreeType::savedata(std::ostream &stream) {
		if(!vecdata && !data && filename == "")
			return false;

		if(vecdata)
			IO::WriteVector(stream, *vecdata);
		else if(data)
			IO::WriteArray(stream, data, datasize);
		else {
			std::ifstream file(filename, std::ios::binary);

			if(!file.is_open())
				return false;

			char buffer[1024];
			while(!file.read(buffer, 1024).bad()) {
				if(file.gcount() <= 0)
					break;

				IO::WriteArray(stream, buffer, (unsigned long)file.gcount());
			}
		}

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
            
            baseline = std::round(lib->face->size->metrics.ascender/64.f);
            
            linegap = std::round(lib->face->size->metrics.height/64.f);
           
            underlinepos  = (int)std::round(baseline - FT_MulFix((lib->face->underline_position),yscale)/64.f);
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
            
            baseline = std::round(lib->face->size->metrics.ascender/64.f);
        
            maxwidth = s.width;
            height   = s.height;
            
            underlinepos = (int)std::round((baseline + height) / 2.f);
            
            linethickness = height / 10;
            
            linegap = std::round(lib->face->size->metrics.height/64.f);
            
            if(linethickness < 1) linethickness = 1;
        }
        
		this->size = (float)size;
        return true;
    }
    
    
    bool FreeType::loadglyphs(GlyphRange range, bool prepare) const {
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
            
            if(ftbmp.pitch < 0) {
                for(unsigned y=0; y<ftbmp.rows; y++) {
                    for(unsigned x=0; x<ftbmp.width; x++) {
                        bmp(x, ftbmp.rows - y - 1, 0) = ftbmp.buffer[x + y*ftbmp.pitch];
                    }
                }
            }
            else {
                for(unsigned y=0; y<ftbmp.rows; y++) {
                    for(unsigned x=0; x<ftbmp.width; x++) {
                        bmp(x, y, 0) = ftbmp.buffer[x + y*ftbmp.pitch];
                    }
                }
            }
            
            destroylist.Add(bmp);
            
            glyphmap[g] = {bmp, std::round(slot->advance.x/64.f), {(int)slot->bitmap_left, (int)-slot->bitmap_top}, (unsigned int)index};
            ft_to_map[index] = g;
            
            if(g < 127 && isdigit(g) && digw < bmp.GetWidth())
                digw = bmp.GetWidth();
            
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

    float FreeType::GetCursorAdvance(Glyph chr) const  {
		if(glyphmap.count(chr))
			return glyphmap.at(chr).advance;
        else if(chr == '\t')
            return (float)height;
        else if(internal::isspace(chr))
            return float(height / 4);
		else if(glyphmap.count(0))
			return glyphmap.at(0).advance;
		else
			return 0;
	}
	
	bool FreeType::Exists(Glyph g) const {
        return glyphmap.count(g);
    }
    
    bool FreeType::Available(Glyph g) const {
        if(glyphmap.count(g))
			return true;
        
        if(!lib->face)
            return false;
        
        return FT_Get_Char_Index(lib->face, g) != 0;
    }
    
    void FreeType::Render(Glyph chr, TextureTarget &target, Geometry::Pointf location, RGBAf color) const {
        if(glyphmap.count(chr)) {
            auto glyph = glyphmap.at(chr);
            glyph.image->Draw(target, location + glyph.offset + Geometry::Pointf(0.f, (float)baseline), color);
        }
		else if(glyphmap.count(0) && !internal::isspace(chr) && !internal::isnewline(chr) && chr != '\t') {
			auto glyph = glyphmap.at(0);
			glyph.image->Draw(target, location + glyph.offset + Geometry::Pointf(0.f, (float)baseline), color);
		}
    }
    
    Geometry::Pointf FreeType::KerningDistance(Glyph chr1, Glyph chr2) const {
        if(!lib->face || !haskerning || !glyphmap.count(chr1) || !glyphmap.count(chr2))
            return {0.f, 0.f};
        
        FT_Vector p;
        FT_Get_Kerning(lib->face, glyphmap.at(chr1).ftindex, glyphmap.at(chr2).ftindex, FT_KERNING_DEFAULT, &p);
        
        if(p.x != 0)
            p.x = p.x;

        return {std::round(p.x / 64.f), std::round(p.y / 64.f)};
    }

    void FreeType::Prepare(const std::string& text) const { 
        std::set<Glyph> list;
        
        for(auto it=text.begin(); it!=text.end(); it++) {
            auto g = internal::decode(it, text.end());
            if(!Exists(g))
                list.insert(g);
        }
        
        auto it = list.begin();
        auto prev = list.begin();
        
        std::vector<GlyphRange> ranges;
        
        while(it != list.end()) {
            it = std::adjacent_find(it, list.end(), [](int l, int r) { return l+1<r; });
            
            if(it == list.end()) {
                ranges.push_back({*prev, *list.rbegin()});
                break;
            }
            else {
                ranges.push_back({*prev, *(it)});
            }
            
            it++;
            prev = it;
        }
        
        /*for(auto r : ranges) {
            if(r.Start > 127 || r.Start <= 32) {
                if(r.Start == r.End)
                    std::cout<<"U"<<std::hex<<(r.Start)<<std::endl;
                else
                    std::cout<<"U"<<std::hex<<(r.Start)<<" - "<<"U"<<(r.End)<<std::endl;
            }
            else if(r.Start == r.End)
                std::cout<<((char)r.Start)<<std::endl;
            else
                std::cout<<((char)r.Start)<<" - "<<((char)r.End)<<std::endl;
        }*/
        
        for(auto &range : ranges) 
            loadglyphs(range, true);
    }
    
    
    BitmapFont FreeType::CopyToBitmap(bool prepare) const {
        BitmapFont font;
        
        font.SetBaseline(baseline);
        font.SetLineThickness(linethickness);
        font.SetUnderlineOffset(underlinepos);
        font.SetLineGap(linegap);
        
        //copy kerning table
        if(haskerning) {
            for(auto &l : glyphmap) {
                for(auto &r : glyphmap) {
                    auto p = KerningDistance(l.first, r.first);
                    
                    if(p.X != 0 || p.Y != 0) {
                        font.SetKerning(l.first, r.first, p);
                    }
                }
            }
        }
        
        std::map<const RectangularDrawable*, Bitmap*> newmapping;
        
        for(auto &g : glyphmap) {
            auto img = dynamic_cast<const Bitmap*>(g.second.image);
            if(!img)
                continue;
            
            if(newmapping.count(img) == 0) {
                newmapping[img] = new Bitmap(img->Duplicate());
                
                if(prepare)
                    newmapping[img]->Prepare();
            }
            
            font.AssumeGlyph(g.first, *newmapping[img], g.second.offset + Geometry::Pointf(0, baseline), g.second.advance);
        }
        
        return font;
    }
    
    
    BitmapFont FreeType::MoveOutBitmap() {
        BitmapFont font;
        
        font.SetBaseline(baseline);
        font.SetLineThickness(linethickness);
        font.SetUnderlineOffset(underlinepos);
        font.SetLineGap(linegap);
        
        //copy kerning table
        if(haskerning) {
            for(auto &l : glyphmap) {
                for(auto &r : glyphmap) {
                    auto p = KerningDistance(l.first, r.first);
                    
                    if(p.X != 0 || p.Y != 0) {
                        font.SetKerning(l.first, r.first, p);
                    }
                }
            }
        }

        //add glpyhs
        for(auto &g : glyphmap) {
            font.AddGlyph(g.first, *g.second.image, g.second.offset + Geometry::Pointf(0, baseline), g.second.advance);
        }
        
        //transfer ownership
        for(const auto &i : destroylist) {
            font.Adopt(i);
        }
        
        //clear to ensure they wont be destroyed
        destroylist.Clear(); 
        
        Clear();
        
        return font;
    }
    

    void FreeType::Clear(){
        glyphmap.clear();
        destroylist.DeleteAll();
        
		digw = 0;
    }
} }
