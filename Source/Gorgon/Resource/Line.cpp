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

		return line;
	}

	void Line::save(Writer &writer) const {
		auto start = writer.WriteObjectStart(this);

		if(prov) {
			writer.WriteChunkHeader(GID::Line_Props, 2 * 4);
			writer.WriteBool(GetTiling());
			writer.WriteEnum32(GetOrientation());

			if(dynamic_cast<Graphics::BitmapLineProvider*>(prov)) {
				auto provider = dynamic_cast<Graphics::BitmapLineProvider*>(prov);

				auto s = provider->GetStart();
				auto m = provider->GetMiddle();
				auto e = provider->GetEnd();

				if(s && e) {
					Image::SaveThis(writer, *s);
					if(m)
						Image::SaveThis(writer, *m);
					Image::SaveThis(writer, *e);
				}
				else if(m && !s && !e) {
					Image::SaveThis(writer, *m);
				}
				else if(!m && !s && !e) {
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
			else if(dynamic_cast<Graphics::AnimatedBitmapLineProvider*>(prov)) {
				auto provider = dynamic_cast<Graphics::AnimatedBitmapLineProvider*>(prov);

				auto s = provider->GetStart();
				auto m = provider->GetMiddle();
				auto e = provider->GetEnd();

				if(s && e) {
					Animation::SaveThis(writer, *s);
					if(m)
						Animation::SaveThis(writer, *m);
					Animation::SaveThis(writer, *e);
				}
				else if(m && !s && !e) {
					Animation::SaveThis(writer, *m);
				}
				else if(!m && !s && !e) {
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
		}

		writer.WriteEnd(start);
	}

}
}
