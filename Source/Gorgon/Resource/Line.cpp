#include "Line.h"
#include "Animation.h"
#include "Image.h"
#include "File.h"
#include "Reader.h"

namespace Gorgon { namespace Resource {

	
	Line::Line(Graphics::BitmapLineProvider &prov) : prov(&prov), ILineProvider(Graphics::Orientation::Horizontal) 
	{ }

	Line::Line(Graphics::AnimatedBitmapLineProvider &prov) : prov(&prov), ILineProvider(Graphics::Orientation::Horizontal)
	{ }

	Line *Line::LoadResource(std::weak_ptr<File> f, std::shared_ptr<Reader> reader, unsigned long totalsize) {
		auto target = reader->Target(totalsize);

		auto file = f.lock();

		auto line = new Line();
		bool tile;
		Graphics::Orientation orient;
		enum {
			unknown, img, anim
		} type = unknown;
		int c = 0;

		while(!target) {
			auto gid = reader->ReadGID();
			auto size= reader->ReadChunkSize();

			if(gid == GID::Line_Props) {
				tile = reader->ReadBool();
				orient = reader->ReadEnum32<Graphics::Orientation>();
			}
			else {
				auto resource = file->LoadChunk(*line, gid, size, false);

				if(resource) {
					if(resource->GetGID() == GID::Image) {
						if(type == unknown)
							type = img;
						else if(type == anim)
							throw std::runtime_error("Animations and images cannot be mixed in a line.");

					}
					else if(resource->GetGID() == GID::Animation) {
						if(type == unknown)
							type = anim;
						else if(type == img)
							throw std::runtime_error("Animations and images cannot be mixed in a line.");
					}
					else {
						throw std::runtime_error("Line can only contain images or animations");
					}

					if(++c > 3) {
						throw std::runtime_error("Line cannot have more than 3 parts");
					}

					line->children.Add(resource);
				}
			}
		}

		if(type == anim) {
			Animation *s = nullptr, *m = nullptr, *e = nullptr;
			if(line->children.GetCount() == 1) {
				m = dynamic_cast<Animation*>(line->children.First().CurrentPtr());
			}
			else if(line->children.GetCount() == 2) {
				s = dynamic_cast<Animation*>(line->children.First().CurrentPtr());
				e = dynamic_cast<Animation*>(line->children.Last().CurrentPtr());
			}
			else {
				s = dynamic_cast<Animation*>(line->children.First().CurrentPtr());
				m = dynamic_cast<Animation*>(&line->children[1]);
				e = dynamic_cast<Animation*>(line->children.Last().CurrentPtr());
			}
			line->SetProvider(
				*new Graphics::AnimatedBitmapLineProvider(orient, s, m, e)
			);
			line->SetTiling(tile);
		}
		else if(type == img) {
			Image *s = nullptr, *m = nullptr, *e = nullptr;
			if(line->children.GetCount() == 1) {
				m = dynamic_cast<Image*>(line->children.First().CurrentPtr());
			}
			else if(line->children.GetCount() == 2) {
				s = dynamic_cast<Image*>(line->children.First().CurrentPtr());
				e = dynamic_cast<Image*>(line->children.Last().CurrentPtr());
			}
			else {
				s = dynamic_cast<Image*>(line->children.First().CurrentPtr());
				m = dynamic_cast<Image*>(&line->children[1]);
				e = dynamic_cast<Image*>(line->children.Last().CurrentPtr());
			}
			line->SetProvider(
				*new Graphics::BitmapLineProvider(orient, s, m, e)
			);
			line->SetTiling(tile);
		}
		//else empty line

		line->owned = true;
		return line;
	}

	void Line::save(Writer &writer) const {
		auto start = writer.WriteObjectStart(this);

		if(prov) {
			if(dynamic_cast<Graphics::BitmapLineProvider*>(prov)) {
                savethis(writer, *dynamic_cast<Graphics::BitmapLineProvider*>(prov));
			}
			else if(dynamic_cast<Graphics::AnimatedBitmapLineProvider*>(prov)) {
                savethis(writer, *dynamic_cast<Graphics::AnimatedBitmapLineProvider*>(prov));
			}
		}

		writer.WriteEnd(start);
	}
	
	void Line::SaveThis(Writer &writer, const Graphics::BitmapLineProvider &provider) {
        auto start = writer.WriteChunkStart(GID::Line);
        
        savethis(writer, provider);

		writer.WriteEnd(start);
    }
	
	void Line::savethis(Writer &writer, const Graphics::BitmapLineProvider &provider) {
        writer.WriteChunkHeader(GID::Line_Props, 2 * 4);
        writer.WriteBool(provider.GetTiling());
        writer.WriteEnum32(provider.GetOrientation());

        auto s = provider.GetStart();
        auto m = provider.GetMiddle();
        auto e = provider.GetEnd();

        if(s && e) {
            Image::SaveThis(writer, *s);
            if(m)
                Image::SaveThis(writer, *m);
            Image::SaveThis(writer, *e);
        }
        else if(!s && !e) {
            if(m)
                Image::SaveThis(writer, *m);
        }
        else {
            //non standard

            if(s)
                Image::SaveThis(writer, *s);
            else
                Image::SaveThis(writer, Graphics::Bitmap(0,0,Graphics::ColorMode::RGBA));

            if(m)
                Image::SaveThis(writer, *m);
            else
                Image::SaveThis(writer, Graphics::Bitmap(0, 0, Graphics::ColorMode::RGBA));

            if(e)
                Image::SaveThis(writer, *e);
            else
                Image::SaveThis(writer, Graphics::Bitmap(0, 0, Graphics::ColorMode::RGBA));
        }
    }
	
	void Line::SaveThis(Writer &writer, const Graphics::AnimatedBitmapLineProvider &provider) {
        auto start = writer.WriteChunkStart(GID::Line);
        
        savethis(writer, provider);

		writer.WriteEnd(start);
    }
	
	void Line::savethis(Writer &writer, const Graphics::AnimatedBitmapLineProvider &provider) {
        writer.WriteChunkHeader(GID::Line_Props, 2 * 4);
        writer.WriteBool(provider.GetTiling());
        writer.WriteEnum32(provider.GetOrientation());

        auto s = provider.GetStart();
        auto m = provider.GetMiddle();
        auto e = provider.GetEnd();

        if(s && e) {
            Animation::SaveThis(writer, *s);
            if(m)
                Animation::SaveThis(writer, *m);
            Animation::SaveThis(writer, *e);
        }
        else if(!s && !e) {
            if(m)
                Animation::SaveThis(writer, *m);
        }
        else {
            //non standard

            if(s)
                Animation::SaveThis(writer, *s);
            else
                Animation::SaveThis(writer, Graphics::BitmapAnimationProvider());

            if(m)
                Animation::SaveThis(writer, *m);
            else
                Animation::SaveThis(writer, Graphics::BitmapAnimationProvider());

            if(e)
                Animation::SaveThis(writer, *e);
            else
                Animation::SaveThis(writer, Graphics::BitmapAnimationProvider());
        }
    }
    
    Graphics::SizelessAnimationStorage Line::animmoveout() {        
        if(!prov)
            throw std::runtime_error("Provider is not set");
        
        ILineProvider *p = nullptr;
        
        if(dynamic_cast<Graphics::BitmapLineProvider*>(prov)) {
            auto provider = dynamic_cast<Graphics::BitmapLineProvider*>(prov);
            auto bp = new Graphics::BitmapLineProvider(prov->GetOrientation());
            p = bp;
            bp->SetTiling(prov->GetTiling());
            
            if(provider->GetStart()) {
                if(dynamic_cast<Image*>(provider->GetStart()))
                    bp->SetStart(new Graphics::Bitmap(dynamic_cast<Image*>(provider->GetStart())->MoveOut()));
                else
                    bp->SetStart(new Graphics::Bitmap(std::move(*provider->GetStart())));
            }
            
            if(provider->GetMiddle()) {
                if(dynamic_cast<Image*>(provider->GetMiddle()))
                    bp->SetMiddle(new Graphics::Bitmap(dynamic_cast<Image*>(provider->GetMiddle())->MoveOut()));
                else
                    bp->SetMiddle(new Graphics::Bitmap(std::move(*provider->GetMiddle())));
            }
            
            if(provider->GetEnd()) {
                if(dynamic_cast<Image*>(provider->GetEnd()))
                    bp->SetEnd(new Graphics::Bitmap(dynamic_cast<Image*>(provider->GetEnd())->MoveOut()));
                else
                    bp->SetEnd(new Graphics::Bitmap(std::move(*provider->GetEnd())));
            }
            
            bp->OwnProviders();
        }
        else if(dynamic_cast<Graphics::AnimatedBitmapLineProvider*>(prov)) {
            auto provider = dynamic_cast<Graphics::AnimatedBitmapLineProvider*>(prov);
            auto bp = new Graphics::AnimatedBitmapLineProvider(prov->GetOrientation());
            p = bp;
            bp->SetTiling(prov->GetTiling());
            
            if(provider->GetStart()) {
                if(dynamic_cast<Animation*>(provider->GetStart()))
                    bp->SetStart(new Graphics::BitmapAnimationProvider(dynamic_cast<Animation*>(provider->GetStart())->MoveOut()));
                else
                    bp->SetStart(new Graphics::BitmapAnimationProvider(std::move(*provider->GetStart())));
            }
            
            if(provider->GetMiddle()) {
                if(dynamic_cast<Animation*>(provider->GetMiddle()))
                    bp->SetMiddle(new Graphics::BitmapAnimationProvider(dynamic_cast<Animation*>(provider->GetMiddle())->MoveOut()));
                else
                    bp->SetMiddle(new Graphics::BitmapAnimationProvider(std::move(*provider->GetMiddle())));
            }
            
            if(provider->GetEnd()) {
                if(dynamic_cast<Animation*>(provider->GetEnd()))
                    bp->SetEnd(new Graphics::BitmapAnimationProvider(dynamic_cast<Animation*>(provider->GetEnd())->MoveOut()));
                else
                    bp->SetEnd(new Graphics::BitmapAnimationProvider(std::move(*provider->GetEnd())));
            }
            
            bp->OwnProviders();
        }

        if(!p)
            throw std::runtime_error("Provider is not set");
        
		for(auto &child : children) {
            child.DeleteResource();
        }

        children.Clear();
        
        if(!p)
            throw std::runtime_error("Provider is not set");
        
        return Graphics::SizelessAnimationStorage(*p, true);
    }

} }
