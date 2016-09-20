#include "Font.h"
#include "File.h"


namespace Gorgon { namespace Resource {
    
/*	BitmapFont *LoadBitmapFontResource(File &File, std::istream &Data, int Size) {
		BitmapFont *font=new BitmapFont;
		int chmap[256]={};
		int cpos=0,i;

		font->Seperator=1;

		int target=Data.tellg()+Size;
		while(Data.tellg()<target) {
			int gid,size;
			ReadFrom(Data, gid);
			ReadFrom(Data, size);

			if(gid==GID::Font_Charmap) {
				Data.read((char*)chmap, 4*256);
			}
			else if(gid==GID::Guid) {
				font->guid.LoadLong(Data);
			}
			else if(gid==GID::SGuid) {
				font->guid.Load(Data);
			}
			else if(gid==GID::Font_Image || gid==GID::Image) {
				Image *img=LoadImageResource(File,Data,size);

				for(i=0;i<256;i++)
					if(chmap[i]==cpos)
						font->Characters[i]=img;

				font->Subitems.Add(img);
				cpos++;
			}
			else if(gid==GID::Font_Props) {
				ReadFrom(Data,font->Seperator);
				ReadFrom(Data,font->VerticalSpacing);
				ReadFrom(Data,font->Baseline);
			}
			else {
				EatChunk(Data,size);
			}
		}

		return font;
	}*/


    Font::Font(Graphics::GlyphRenderer& renderer) {
        ASSERT(dynamic_cast<Graphics::BitmapFont*>(&renderer), "Font resource can only be a bitmapfont (for now)");
        
#ifndef NDEBUG
        auto br = dynamic_cast<Graphics::BitmapFont*>(&renderer);
        
        if(br) {
            for(auto &p : *br) {
                if(dynamic_cast<const Graphics::Bitmap*>(p.second.image)) {
                    ASSERT(dynamic_cast<const Graphics::Bitmap*>(p.second.image)->HasData(), "You shouldn't discard bitmap data for Font resource to work.");
                }
                else
                    Utils::ASSERT_FALSE("Bitmap resource images should be bitmaps.");
            }
        }
#endif
        
        data = &renderer;
    }
    
    void Gorgon::Resource::Font::save(Gorgon::Resource::Writer& writer) const {
        auto start = writer.WriteObjectStart(this);
		
		auto propstart = writer.WriteChunkStart(GID::Font_Props);

        writer.WriteInt32(2); //h-sep, not used anymore
        writer.WriteInt32(data->GetXSpacing());
        writer.WriteInt32(data->GetBaseLine());
        writer.WriteInt32(data->GetHeight());
        writer.WriteInt32(data->GetLineThickness());
        writer.WriteInt32(data->GetUnderlineOffset());
        
        writer.WriteEnd(propstart);
        
        
        auto mapstart = writer.WriteChunkStart(GID::Font_Charmap_II);

        
        
        writer.WriteEnd(mapstart);
        
    }

} }
