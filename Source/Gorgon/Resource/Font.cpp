#include "Font.h"
#include "File.h"


namespace Gorgon { namespace Resource {


    Font::Font(Graphics::GlyphRenderer& renderer) {
        SetRenderer(renderer);
    }
    
    void Font::SetRenderer(Graphics::GlyphRenderer& renderer) {
        if(isowner)
            delete data;
        
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
        isowner = false;
    }
    
    void Gorgon::Resource::Font::save(Gorgon::Resource::Writer& writer) const {
        auto start = writer.WriteObjectStart(this);
        
        auto bf = dynamic_cast<Graphics::BitmapFont*>(data);
        
        if(bf) {            
            auto propstart = writer.WriteChunkStart(GID::Font_BitmapProps);

            writer.WriteInt32(2); //h-sep, not used anymore
            writer.WriteInt32(bf->GetGlyphSpacing());
            writer.WriteInt32(bf->GetBaseLine());
            writer.WriteInt32(bf->GetHeight());
            writer.WriteInt32(bf->GetLineThickness());
            writer.WriteInt32(bf->GetUnderlineOffset());
            writer.WriteInt32(bf->GetMaxWidth());
            
            writer.WriteEnd(propstart);
            
            Containers::Collection<const Graphics::RectangularDrawable> bmps;
            
            for(auto &p : *bf) {
                bmps.Add(p.second.image);
            }
            
            auto mapstart = writer.WriteChunkStart(GID::Font_Charmap_II);

            for(auto &p : *bf) {
                writer.WriteInt32 (p.first);
                writer.WriteUInt32(bmps.FindLocation(p.second.image));
                writer.WritePointf(p.second.offset);
                writer.WriteFloat(p.second.advance);
            }
            
            writer.WriteEnd(mapstart);
            
            for(auto &i : bmps) {
                auto bmp = dynamic_cast<const Graphics::Bitmap *>(&i);
                
                if(!bmp || !bmp->HasData())
                    throw std::runtime_error("Saving bitmap resource requires existing data buffers.");
                
                Image::SaveThis(writer, *bmp);
            }
        }
        else if(data) {
            Utils::ASSERT_FALSE("Unrecognized font type to save");
        }
        
        writer.WriteEnd(start);
    }

    Font *Font::LoadResource(std::weak_ptr<Gorgon::Resource::File> file, std::shared_ptr<Gorgon::Resource::Reader> reader, long unsigned int totalsize) { 
		auto target = reader->Target(totalsize);
        
        auto font = new Font;
        bool recalc = false;
        int bl = 0;
        
        Graphics::BitmapFont *bf = nullptr;
        Containers::Collection<Graphics::Bitmap> glyphs;
        std::map<Graphics::Glyph, Graphics::BitmapFont::GlyphDescriptor> descs;
        
        while(!target) {
			auto gid = reader->ReadGID();
			auto size= reader->ReadChunkSize();

			if(gid == GID::Font_Props) {
                bf = new Graphics::BitmapFont;
                
                font->AssumeRenderer(*bf);
                
                reader->ReadInt32();
                bf->SetGlyphSpacing(reader->ReadInt32());
                bf->SetBaseline(reader->ReadInt32());
                
                recalc = true;
            }
            else if(gid == GID::Font_BitmapProps) {
                bf = new Graphics::BitmapFont;
                
                font->AssumeRenderer(*bf);
                
                reader->ReadInt32();
                bf->SetGlyphSpacing(reader->ReadInt32());
                bl = reader->ReadInt32();
                bf->SetBaseline(bl);
                bf->SetHeight(reader->ReadInt32());
                bf->SetLineThickness(reader->ReadInt32());
                bf->SetUnderlineOffset(reader->ReadInt32());
                bf->SetMaxWidth(reader->ReadInt32());
                
                bf->SetLineGap(std::round(bf->GetHeight() * 1.2f));//temporary solution
            }
            else if(gid == GID::Font_Charmap) {
               if(!bf)
                    throw std::runtime_error("Unexpected image, either font type is wrong or is not set.");

               if(size != 4*256)
                    throw std::runtime_error("Invalid font charmap");
                
                for(int i=0; i<256; i++) {
                    descs.insert(std::make_pair(i, Graphics::BitmapFont::GlyphDescriptor(reader->ReadInt32(), {0, 0}, 0)));
                }
            }
            else if(gid == GID::Font_Charmap_II) {
               if(!bf)
                    throw std::runtime_error("Unexpected image, either font type is wrong or is not set.");

               if((size/20)*20 != size)
                    throw std::runtime_error("Invalid font charmap");
                
                for(unsigned i=0; i<size/20; i++) {
                    auto g = reader->ReadInt32();
                    auto ind = reader->ReadUInt32();
                    auto off = reader->ReadPointf();
                    auto adv = reader->ReadFloat();
                    descs.insert(std::make_pair(
                        g, Graphics::BitmapFont::GlyphDescriptor(
                        ind, off, adv
                    )));
                }
            }
            else if(gid == GID::Font_Image || gid == GID::Image) {
                if(!bf)
                    throw std::runtime_error("Unexpected image, either font type is wrong or is not set.");
                
                auto img = Image::LoadResource(file, reader, size);
                glyphs.Push(img);
				font->tobeprepared.Push(img);
            }
            else {
				if(!reader->ReadCommonChunk(*font, gid, size)) {
					Utils::ASSERT_FALSE("Unknown chunk: "+String::From(gid));
					reader->EatChunk(size);
				}
            }
        }
        
        for(auto &p : descs) {
            if(p.second.index<0 || p.second.index>=glyphs.GetSize())
                throw std::runtime_error("Invalid glyph index");
            
            bf->AssumeGlyph(p.first, glyphs[p.second.index], p.second.offset, p.second.advance);
        }

        return font;
    }

    void Font::Prepare() {        
		Base::Prepare();

		for(auto &img : tobeprepared)
			img.Prepare();
    }

    void Font::Discard() {        
        auto bf = dynamic_cast<Graphics::BitmapFont*>(data);
        
        if(bf) {
            bf->Pack(false);
        }

		tobeprepared.Clear();
    }
} }
