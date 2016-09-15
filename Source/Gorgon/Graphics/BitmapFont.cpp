#include "BitmapFont.h"
#include "../Utils/Assert.h"
#include "../Filesystem/Iterator.h"
#include "Bitmap.h"
#include "../Filesystem.h"
#include "../Containers/Hashmap.h"

namespace Gorgon { namespace Graphics {


    void BitmapFont::AddGlyph(Glyph glyph, const RectangularDrawable& bitmap, int baseline) {
		auto size = bitmap.GetSize();

		int lh = size.Height;

        if(maxwidth == 0) {
            maxwidth = size.Width;
            height = lh;
        }
        else {
            if(size.Width != maxwidth)
                isfixedw = false;
            
            if(maxwidth < size.Width)
                maxwidth = size.Width;
            
            if(height < lh)
                height = lh;
        }

		if(isdigit(glyph) && digw < size.Width) {
			digw = size.Width;
		}

		if(glyph > 127)
			isascii = false;
        
        glyphmap[glyph] = GlyphDescriptor(bitmap, this->baseline - baseline);
    }


	Geometry::Size BitmapFont::GetSize(Glyph chr) const {
		if(glyphmap.count(chr))
			return glyphmap.at(chr).image->GetSize();
		else if(glyphmap.count(0))
			return glyphmap.at(0).image->GetSize();
		else
			return{0, 0};
	}

	void BitmapFont::Render(Glyph chr, TextureTarget& target, Geometry::Pointf location, RGBAf color) const {
        if(glyphmap.count(chr)) {
            auto glyph = glyphmap.at(chr);
            glyph.image->Draw(target, location + Geometry::Pointf(0, (Float)glyph.offset), color);
        }
		else if(glyphmap.count(0)) {
			auto glyph = glyphmap.at(0);
			glyph.image->Draw(target, location + Geometry::Pointf(0, (Float)glyph.offset), color);
		}
    }
    

	void BitmapFont::DetermineDimensions() {
		for(auto &g : glyphmap) {
			auto size = g.second.image->GetSize();

			int lh = size.Height;

			if(maxwidth == 0) {
				maxwidth = size.Width;
				height = lh;
			}
			else {
				if(size.Width != maxwidth)
					isfixedw = false;

				if(maxwidth < size.Width)
					maxwidth = size.Width;

				if(height < lh)
					height = lh;
			}
		}
	}

	int BitmapFont::ImportFolder(const std::string& path, ImportNamingTemplate naming, int start, std::string prefix, int baseline, bool trim, bool toalpha, bool prepare) {
		Containers::Hashmap<std::string, Bitmap> files; // map of file labels to bitmaps

		Filesystem::Iterator dir;
		try {
			dir = Filesystem::Iterator(path);
		}
		catch(...) {
			return false;
		}

		for(; dir.IsValid(); dir.Next()) {
			auto name = *dir;

			if(name.length() > 1 && (name[0]=='.'||name[0]=='_'))
				continue;

			auto basename = Filesystem::GetBasename(name);

			//prefix check
			if(prefix != "") {
				if(basename.substr(0, prefix.length()) != prefix)
					continue;
			}

			//skip directories
			if(!Filesystem::IsFile(Filesystem::Join(path, name))) continue;

			auto bmp = new Bitmap;

			//if load is successful, add it to the list
			try {
				if(bmp->Import(Filesystem::Join(path, name))) {
					files.Add(basename, bmp);
					destroylist.Push(bmp);
				}
				else {
					delete bmp;
				}
			}
			catch(...) {
				delete bmp;
			}
		}

		if(files.GetSize() == 0)
			return 0;

		if(naming == Automatic) {
			bool digitonly = true;
			bool hexonly   = true;
			bool multichar = false;

			int minval = 0;

			bool searchprefix = true;
			bool first = true;

			//prefix search
			if(files.GetSize() != 1 && prefix == "") {
				prefix = files.Last().Current().first;

				for(auto p : files) {
					std::string name = p.first;

					int i;
					for(i=0; i<(int)std::min(name.length(), prefix.length()); i++) {
						if(name[i] != prefix[i]) {
							break;
						}
					}

					if(i == 0) {
						prefix = "";
						break;
					}

					prefix = prefix.substr(0, i);
				}
			}

			int prefixlen = prefix.length();

			for(auto p : files) {
				std::string name = p.first;
				
				if(prefixlen)
					name = name.substr(prefixlen);

				if(name.length() > 1)
					multichar = true;

				for(auto c : name) {
					if(!isdigit(c)) {
						if(digitonly) {
							//minval was in hex until now, we never noticed.
							minval = String::HexTo<int>(String::From(minval));
						}

						digitonly = false;

						if((c < 'a' || c > 'f') && (c < 'A' || c > 'F')) {
							hexonly = false;
						}
					}
				}

				if(digitonly) {
					int num = String::To<int>(name);

					if(num < minval)
						minval = num;
				}
				else if(hexonly) {
					int num = String::HexTo<int>(name);

					if(num < minval)
						minval = num;
				}
			}

			if(!multichar) {
				naming = Alpha;
			}
			else if(digitonly) {
				naming = Decimal;
			}
			else if(hexonly) {
				naming = Hexadecimal;
			}
			else {
				for(auto p : files) {
					destroylist.Remove(p.second);
				}

				files.Destroy();

				return 0;
			}

			if(start == 0 && minval == 0) {
				start = 32;
			}
		}

		int prefixlen = prefix.length();

		int maxh = 0;
        
        //width of the space, might need adjusting if trimming
        int spw = 0;
        Bitmap *spim = nullptr;

		for(auto p : files) {
			auto bl = baseline;

			auto h = p.second.GetHeight();
            
            Glyph g;

			std::string name = p.first;

			if(prefixlen)
				name = name.substr(prefixlen);
            
			if(naming == Alpha) {
				g = name[0];
			}
			else if(naming == Decimal) {
				g = (Glyph)String::To<long long>(name) + start;
			}
			else {
				g = (Glyph)String::HexTo<long long>(name) + start;
			}
			
			if(g == ' ') {
                spw = p.second.GetWidth();
                spim = &p.second;
            }

			if(trim) {
				auto res = p.second.Trim();
				if(res.Top != p.second.GetHeight())
					bl -= res.Top;
			}

			if(HasAlpha(p.second.GetMode()) && toalpha) {
				p.second.StripRGB();
			}

			if(prepare)
				p.second.Prepare();

            AddGlyph(g, p.second, bl);

			if(maxh < h)
				maxh = h;
		}

		if(baseline == -1) {
			baseline = int(std::round(height * 0.7));
		}

		if(trim && spacing==0) spacing = 1;

		height = maxh;
        
        if(trim && spim && maxwidth == spw) {
            //check if glyph is empty, if so we can resize it.
            int alphaloc = AlphaIndex(spim->GetMode());
            
            bool isempty = true;
            
            if(alphaloc != -1) {
                isempty = spim->ForPixels(std::bind(std::equal_to<Byte>(), 0, std::placeholders::_1), alphaloc);
            }
            else 
                isempty = false;
            
            if(isempty) {
                spim->Resize({(int)std::ceil(height/3.f), 1}, spim->GetMode());
                spim->Clean();
                
                if(prepare) spim->Prepare();
            }
        }

		if(start > 0 && glyphmap.size()) {
			if(glyphmap.count(127)) {
				glyphmap[0] = glyphmap[127];
			}
			else if(glyphmap.count('?')) {
				glyphmap[0] = glyphmap['?'];
			}
			else {
				glyphmap[0] = glyphmap.begin()->second;
			}
		}

        return files.GetSize();
    }


    void BitmapFont::Pack(bool tight, bool delold) { 
        Containers::Collection<const Bitmap> bitmaps;
        std::vector<std::pair<Glyph, int>> packing;
        
        for(auto &p : glyphmap) {
            auto bmp = dynamic_cast<const Bitmap*>(p.second.image);
            
            if(bmp) {
                bitmaps.Add(bmp);
                // if an image is repeated twice, collection push won't add it second time
                packing.push_back(std::make_pair(p.first, bitmaps.FindLocation(bmp)));
            }
        }
        
        auto &bmp = *new Bitmap;
        destroylist.Push(bmp);
        auto list = bmp.CreateLinearAtlas(std::move(bitmaps), tight ? Bitmap::None : Bitmap::Zero);
        bmp.Prepare();
        auto newimgs = bmp.CreateAtlasImages(std::move(list));
        
        for(auto g : packing) {
            auto im = glyphmap[g.first].image;
            auto it = destroylist.Find(im);
            
            if(it.IsValid())
                it.Delete();
            else if(delold)
                delete im;
            
            auto tex = new TextureImage(std::move(newimgs[g.second]));
            destroylist.Add(tex);
            glyphmap[g.first].image = tex;
        }
    }

} }
