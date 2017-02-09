#include "Rectangle.h"
#include "Animation.h"
#include "Image.h"
#include "File.h"
#include "Reader.h"

namespace Gorgon { namespace Resource {

	
	Rectangle::Rectangle(Graphics::BitmapRectangleProvider &prov) : prov(&prov)
	{ }

	Rectangle::Rectangle(Graphics::AnimatedBitmapRectangleProvider &prov) : prov(&prov)
	{ }

	Rectangle *Rectangle::LoadResource(std::weak_ptr<File> f, std::shared_ptr<Reader> reader, unsigned long totalsize) {
		auto target = reader->Target(totalsize);

		auto file = f.lock();

		auto rectangle = new Rectangle();
		bool tile;
		enum {
			unknown, img, anim
		} type = unknown;
		int c = 0;

		while(!target) {
			auto gid = reader->ReadGID();
			auto size= reader->ReadChunkSize();

			if(gid == GID::Rectangle_Props) {
				tile = reader->ReadBool();
                reader->ReadBool(); //unused
                reader->ReadBool();
                reader->ReadBool();
                reader->ReadBool();
                reader->ReadBool();
			}
			else if(gid == GID::Rectangle_PropsII) {
				tile = reader->ReadBool();
            }
			else {
				auto resource = file->LoadChunk(*rectangle, gid, size, false);

				if(resource) {
					if(resource->GetGID() == GID::Image) {
						if(type == unknown)
							type = img;
						else if(type == anim)
							throw std::runtime_error("Animations and images cannot be mixed in a rectangle.");

					}
					else if(resource->GetGID() == GID::Animation) {
						if(type == unknown)
							type = anim;
						else if(type == img)
							throw std::runtime_error("Animations and images cannot be mixed in a rectangle.");
					}
					else {
						throw std::runtime_error("Rectangle can only contain images or animations");
					}

					if(++c > 9) {
						throw std::runtime_error("Rectangle cannot have more than 9 parts");
					}

					rectangle->children.Add(resource);
				}
			}
		}

		if(type == anim) {
			Animation *tl = nullptr, *tm = nullptr, *tr = nullptr;
			Animation *ml = nullptr, *mm = nullptr, *mr = nullptr;
			Animation *bl = nullptr, *bm = nullptr, *br = nullptr;
            
			if(rectangle->children.GetCount() == 1) {
				mm = dynamic_cast<Animation*>(rectangle->children.First().CurrentPtr());
			}
			else if(rectangle->children.GetCount() == 4 || rectangle->children.GetCount() == 5) {
                int n=0;
				tm = dynamic_cast<Animation*>(&rectangle->children[n++]);
				ml = dynamic_cast<Animation*>(&rectangle->children[n++]);
                if(rectangle->children.GetCount() == 5)
                    mm = dynamic_cast<Animation*>(&rectangle->children[n++]);
                
				mr = dynamic_cast<Animation*>(&rectangle->children[n++]);
				bm = dynamic_cast<Animation*>(&rectangle->children[n++]);
			}
			else if(rectangle->children.GetCount() >= 8) {
                int n=0;
				tl = dynamic_cast<Animation*>(&rectangle->children[n++]);
				tm = dynamic_cast<Animation*>(&rectangle->children[n++]);
				tr = dynamic_cast<Animation*>(&rectangle->children[n++]);
				ml = dynamic_cast<Animation*>(&rectangle->children[n++]);
                if(rectangle->children.GetCount() == 9)
                    mm = dynamic_cast<Animation*>(&rectangle->children[n++]);
				mr = dynamic_cast<Animation*>(&rectangle->children[n++]);
				bl = dynamic_cast<Animation*>(&rectangle->children[n++]);
				bm = dynamic_cast<Animation*>(&rectangle->children[n++]);
				br = dynamic_cast<Animation*>(&rectangle->children[n++]);
			}
			rectangle->SetProvider(
				*new Graphics::AnimatedBitmapRectangleProvider(tl, tm, tr, ml, mm, mr, bl, bm, br)
			);
			rectangle->SetTiling(tile);
		}
		else if(type == img) {
			Image *tl = nullptr, *tm = nullptr, *tr = nullptr;
			Image *ml = nullptr, *mm = nullptr, *mr = nullptr;
			Image *bl = nullptr, *bm = nullptr, *br = nullptr;
            
			if(rectangle->children.GetCount() == 1) {
				mm = dynamic_cast<Image*>(rectangle->children.First().CurrentPtr());
			}
			else if(rectangle->children.GetCount() == 4 || rectangle->children.GetCount() == 5) {
                int n=0;
				tm = dynamic_cast<Image*>(&rectangle->children[n++]);
				ml = dynamic_cast<Image*>(&rectangle->children[n++]);
                if(rectangle->children.GetCount() == 5)
                    mm = dynamic_cast<Image*>(&rectangle->children[n++]);
                
				mr = dynamic_cast<Image*>(&rectangle->children[n++]);
				bm = dynamic_cast<Image*>(&rectangle->children[n++]);
			}
			else if(rectangle->children.GetCount() >= 8) {
                int n=0;
				tl = dynamic_cast<Image*>(&rectangle->children[n++]);
				tm = dynamic_cast<Image*>(&rectangle->children[n++]);
				tr = dynamic_cast<Image*>(&rectangle->children[n++]);
				ml = dynamic_cast<Image*>(&rectangle->children[n++]);
                if(rectangle->children.GetCount() == 9)
                    mm = dynamic_cast<Image*>(&rectangle->children[n++]);
				mr = dynamic_cast<Image*>(&rectangle->children[n++]);
				bl = dynamic_cast<Image*>(&rectangle->children[n++]);
				bm = dynamic_cast<Image*>(&rectangle->children[n++]);
				br = dynamic_cast<Image*>(&rectangle->children[n++]);
			}
			rectangle->SetProvider(
				*new Graphics::BitmapRectangleProvider(tl, tm, tr, ml, mm, mr, bl, bm, br)
			);
			rectangle->SetTiling(tile);
		}
		//else empty rectangle

		return rectangle;
	}

	void Rectangle::save(Writer &writer) const {
		auto start = writer.WriteObjectStart(this);

		if(prov) {
			writer.WriteChunkHeader(GID::Rectangle_PropsII, 4);
			writer.WriteBool(GetTiling());

			if(dynamic_cast<Graphics::BitmapRectangleProvider*>(prov)) {
				auto provider = dynamic_cast<Graphics::BitmapRectangleProvider*>(prov);

                auto tl = provider->GetTL();
                auto tm = provider->GetTM();
                auto tr = provider->GetTR();
                auto ml = provider->GetML();
                auto mm = provider->GetMM();
                auto mr = provider->GetMR();
                auto bl = provider->GetBL();
                auto bm = provider->GetBM();
                auto br = provider->GetBR();

				if(tm && ml && mr && bm && !tl && !tr && !bl && !br) {
					Image::SaveThis(writer, *tm);
					Image::SaveThis(writer, *ml);
					if(mm)
						Image::SaveThis(writer, *mm);
					Image::SaveThis(writer, *mr);
                    Image::SaveThis(writer, *bm);
				}
				else if(tm && ml && mr && bm && tl && tr && bl && br) {
					Image::SaveThis(writer, *tl);
					Image::SaveThis(writer, *tm);
					Image::SaveThis(writer, *tr);
					Image::SaveThis(writer, *ml);
					if(mm)
						Image::SaveThis(writer, *mm);
					Image::SaveThis(writer, *mr);
					Image::SaveThis(writer, *bl);
					Image::SaveThis(writer, *bm);
					Image::SaveThis(writer, *br);
				}
				else if(!tr && !tm && !tr && !ml && !mr && !bl && !bm && !br) {
                    if(mm)
                        Image::SaveThis(writer, *mm);
				}
				else {
					//non standard

					if(tl)
						Image::SaveThis(writer, *tl);
					else
						Image::SaveThis(writer, Graphics::Bitmap(0,0,Graphics::ColorMode::RGBA));

					if(tm)
						Image::SaveThis(writer, *tm);
					else
						Image::SaveThis(writer, Graphics::Bitmap(0,0,Graphics::ColorMode::RGBA));

					if(tr)
						Image::SaveThis(writer, *tr);
					else
						Image::SaveThis(writer, Graphics::Bitmap(0,0,Graphics::ColorMode::RGBA));

					if(ml)
						Image::SaveThis(writer, *ml);
					else
						Image::SaveThis(writer, Graphics::Bitmap(0,0,Graphics::ColorMode::RGBA));

					if(mm)
						Image::SaveThis(writer, *mm);
					else
						Image::SaveThis(writer, Graphics::Bitmap(0,0,Graphics::ColorMode::RGBA));

					if(mr)
						Image::SaveThis(writer, *mr);
					else
						Image::SaveThis(writer, Graphics::Bitmap(0,0,Graphics::ColorMode::RGBA));

					if(bl)
						Image::SaveThis(writer, *bl);
					else
						Image::SaveThis(writer, Graphics::Bitmap(0,0,Graphics::ColorMode::RGBA));

					if(bm)
						Image::SaveThis(writer, *bm);
					else
						Image::SaveThis(writer, Graphics::Bitmap(0,0,Graphics::ColorMode::RGBA));

					if(br)
						Image::SaveThis(writer, *br);
					else
						Image::SaveThis(writer, Graphics::Bitmap(0,0,Graphics::ColorMode::RGBA));
                }
			}
			else if(dynamic_cast<Graphics::AnimatedBitmapRectangleProvider*>(prov)) {
				auto provider = dynamic_cast<Graphics::AnimatedBitmapRectangleProvider*>(prov);

                auto tl = provider->GetTL();
                auto tm = provider->GetTM();
                auto tr = provider->GetTR();
                auto ml = provider->GetML();
                auto mm = provider->GetMM();
                auto mr = provider->GetMR();
                auto bl = provider->GetBL();
                auto bm = provider->GetBM();
                auto br = provider->GetBR();

				if(tm && ml && mr && bm && !tl && !tr && !bl && !br) {
					Animation::SaveThis(writer, *tm);
					Animation::SaveThis(writer, *ml);
					if(mm)
						Animation::SaveThis(writer, *mm);
					Animation::SaveThis(writer, *mr);
                    Animation::SaveThis(writer, *bm);
				}
				else if(tm && ml && mr && bm && tl && tr && bl && br) {
					Animation::SaveThis(writer, *tl);
					Animation::SaveThis(writer, *tm);
					Animation::SaveThis(writer, *tr);
					Animation::SaveThis(writer, *ml);
					if(mm)
						Animation::SaveThis(writer, *mm);
					Animation::SaveThis(writer, *mr);
					Animation::SaveThis(writer, *bl);
					Animation::SaveThis(writer, *bm);
					Animation::SaveThis(writer, *br);
				}
				else if(!tr && !tm && !tr && !ml && !mr && !bl && !bm && !br) {
                    if(mm)
                        Animation::SaveThis(writer, *mm);
				}
				else {
					//non standard

					if(tl)
						Animation::SaveThis(writer, *tl);
					else
						Animation::SaveThis(writer, Graphics::BitmapAnimationProvider());

					if(tm)
						Animation::SaveThis(writer, *tm);
					else
						Animation::SaveThis(writer, Graphics::BitmapAnimationProvider());

					if(tr)
						Animation::SaveThis(writer, *tr);
					else
						Animation::SaveThis(writer, Graphics::BitmapAnimationProvider());

					if(ml)
						Animation::SaveThis(writer, *ml);
					else
						Animation::SaveThis(writer, Graphics::BitmapAnimationProvider());

					if(mm)
						Animation::SaveThis(writer, *mm);
					else
						Animation::SaveThis(writer, Graphics::BitmapAnimationProvider());

					if(mr)
						Animation::SaveThis(writer, *mr);
					else
						Animation::SaveThis(writer, Graphics::BitmapAnimationProvider());

					if(bl)
						Animation::SaveThis(writer, *bl);
					else
						Animation::SaveThis(writer, Graphics::BitmapAnimationProvider());

					if(bm)
						Animation::SaveThis(writer, *bm);
					else
						Animation::SaveThis(writer, Graphics::BitmapAnimationProvider());

					if(br)
						Animation::SaveThis(writer, *br);
					else
						Animation::SaveThis(writer, Graphics::BitmapAnimationProvider());
                }
            }
		}

		writer.WriteEnd(start);
	}

}
}
