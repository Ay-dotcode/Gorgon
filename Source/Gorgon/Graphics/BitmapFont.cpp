#include "BitmapFont.h"
#include "../Utils/Assert.h"
#include "../Filesystem/Iterator.h"
#include "Bitmap.h"
#include "../Filesystem.h"
#include "../Containers/Hashmap.h"

namespace Gorgon { namespace Graphics {


    void BitmapFont::AddGlyph(Glyph glyph, const RectangularDrawable& bitmap, float baseline) {
		auto size = bitmap.GetSize();

        if(maxwidth == 0) {
            maxwidth = size.Width;
            lineheight = size.Height;
        }
        else {
            if(size.Width != maxwidth)
                isfixedw = false;
            
            if(maxwidth < size.Width)
                maxwidth = size.Width;
            
            if(lineheight < size.Height)
                lineheight = size.Height;
        }
        
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
            glyph.image->Draw(target, location + Geometry::Pointf(0, glyph.offset), color);
        }
		else if(glyphmap.count(0)) {
			auto glyph = glyphmap.at(0);
			glyph.image->Draw(target, location + Geometry::Pointf(0, glyph.offset), color);
		}
    }
    

	void BitmapFont::DetermineDimensions() {
		for(auto &g : glyphmap) {
			auto size = g.second.image->GetSize();

			if(maxwidth == 0) {
				maxwidth = size.Width;
				lineheight = size.Height;
			}
			else {
				if(size.Width != maxwidth)
					isfixedw = false;

				if(maxwidth < size.Width)
					maxwidth = size.Width;

				if(lineheight < size.Height)
					lineheight = size.Height;
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

		for(auto p : files) {
			if(prepare)
				p.second.Prepare();

			std::string name = p.first;

			if(prefixlen)
				name = name.substr(prefixlen);

			if(naming == Alpha) {
				AddGlyph(name[0], p.second, (float)baseline);
			}
			else if(naming == Decimal) {
				AddGlyph((Glyph)String::To<long long>(name) + start, p.second, (float)baseline);
			}
			else {
				AddGlyph((Glyph)String::HexTo<long long>(name) + start, p.second, (float)baseline);
			}
		}

		if(baseline == -1) {
			baseline = int(std::round(lineheight * 0.7));
		}

        return files.GetSize();
    }

} }
