#include "Line.h"
#include "Animation.h"
#include "Image.h"
#include "File.h"
#include "Reader.h"
#include "Null.h"

namespace Gorgon { namespace Resource {

	
	Line::Line(Graphics::BitmapLineProvider &prov) : prov(&prov), ILineProvider(Graphics::Orientation::Horizontal) 
	{ }
	
	Line::Line(Graphics::AnimatedBitmapLineProvider &prov) : prov(&prov), ILineProvider(Graphics::Orientation::Horizontal)
	{ }

	Line::Line(Graphics::LineProvider &prov) : prov(&prov), ILineProvider(Graphics::Orientation::Horizontal)
	{ }

	template<class T_>
	static Graphics::basic_LineProvider<T_> &fillfrom(Containers::Collection<Base> &children, Graphics::Orientation orient) {
		T_ *s = nullptr, *m = nullptr, *e = nullptr;

		if(children.GetCount() == 1) {
			m = dynamic_cast<T_*>(&children[0]);
		}
		else if(children.GetCount() == 2) {
			s = dynamic_cast<T_*>(&children[0]);
			e = dynamic_cast<T_*>(&children[1]);
		}
		else {
			if(children[0].GetGID() != GID::Null)
				s = dynamic_cast<T_*>(&children[0]);
			if(children[1].GetGID() != GID::Null)
				m = dynamic_cast<T_*>(&children[1]);
			if(children[2].GetGID() != GID::Null)
				e = dynamic_cast<T_*>(&children[2]);
		}

		return *new Graphics::basic_LineProvider<T_>(orient, s, m, e);
	}

	Line *Line::LoadResource(std::weak_ptr<File> f, std::shared_ptr<Reader> reader, unsigned long totalsize) {
		auto target = reader->Target(totalsize);

		auto file = f.lock();

		auto line = new Line();
		bool tile;
		Graphics::Orientation orient;
		enum {
			unknown, img, anim, mixed
		} type = unknown;
		int c = 0;

		while(!target) {
			auto gid = reader->ReadGID();
			auto size= reader->ReadChunkSize();

			if(gid == GID::Line_Props) {
				reader->ReadGuid(); //mask is ignored
				tile = reader->ReadBool();
				orient = reader->ReadEnum32<Graphics::Orientation>();
			}
			else if(gid == GID::Line_Props_II) {
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
							type = mixed;

					}
					else if(resource->GetGID() == GID::Animation) {
						if(type == unknown)
							type = anim;
						else if(type == img)
							type = mixed;
					}
					else if(resource->GetGID() == GID::Null) {
						//null is allowed
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
			line->SetProvider(
				fillfrom<Graphics::BitmapAnimationProvider>(line->children, orient)
			);
		}
		else if(type == img) {
			line->SetProvider(
				fillfrom<Graphics::Bitmap>(line->children, orient)
			);
		}
		else if(type == mixed) {
			line->SetProvider(
				fillfrom<Graphics::RectangularAnimationProvider>(line->children, orient)
			);
		}
		//else empty line

		if(type != unknown)
			line->SetTiling(tile);

		line->own = true;
		return line;
	}
	
	template <class T_>
	static void savethis(Writer &writer, const Graphics::basic_LineProvider<T_> &provider) {
        writer.WriteChunkHeader(GID::Line_Props_II, 2 * 4);
        writer.WriteBool(provider.GetTiling());
        writer.WriteEnum32(provider.GetOrientation());

        auto s = provider.GetStart();
        auto m = provider.GetMiddle();
        auto e = provider.GetEnd();

        if(s && e) {
			SaveAnimation(writer, s);
            if(m)
				SaveAnimation(writer, m);
			SaveAnimation(writer, e);
        }
        else if(!s && !e) {
			SaveAnimation(writer, m);
        }
        else {
			SaveAnimation(writer, s);
			SaveAnimation(writer, m);
			SaveAnimation(writer, e);
        }
    }

	void Line::save(Writer &writer) const {
		auto start = writer.WriteObjectStart(this);

        if(dynamic_cast<Graphics::BitmapLineProvider*>(prov)) {
            savethis(writer, *dynamic_cast<Graphics::BitmapLineProvider*>(prov));
        }
        else if(dynamic_cast<Graphics::AnimatedBitmapLineProvider*>(prov)) {
            savethis(writer, *dynamic_cast<Graphics::AnimatedBitmapLineProvider*>(prov));
        }
        else if(dynamic_cast<Graphics::LineProvider*>(prov)) {
            savethis(writer, *dynamic_cast<Graphics::LineProvider*>(prov));
        }
		else if(prov != nullptr) {
			throw std::runtime_error("Unknown line provider");
		}

		writer.WriteEnd(start);
	}

	void Line::SaveThis(Writer &writer, const Graphics::ILineProvider &provider) {
		auto start = writer.WriteChunkStart(GID::Line);
		auto prov = &provider;

		if(dynamic_cast<const Graphics::BitmapLineProvider*>(prov)) {
			savethis(writer, *dynamic_cast<const Graphics::BitmapLineProvider*>(prov));
		}
		else if(dynamic_cast<const Graphics::AnimatedBitmapLineProvider*>(prov)) {
			savethis(writer, *dynamic_cast<const Graphics::AnimatedBitmapLineProvider*>(prov));
		}
		else if(dynamic_cast<const Graphics::LineProvider*>(prov)) {
			savethis(writer, *dynamic_cast<const Graphics::LineProvider*>(prov));
		}
		else {
			throw std::runtime_error("Unknown line provider");
		}

		writer.WriteEnd(start);
	}

	template<class T_, class F_>
	static void setthis(F_ f, Graphics::basic_LineProvider<T_> *provider, T_ *o) {
		Utils::ASSERT_FALSE("Should not run");
	}

	template<class F_>
	static void setthis(F_ f, Graphics::BitmapLineProvider *provider, Graphics::Bitmap *o) {
		if(!o) return; // do nothing

		if(dynamic_cast<Image*>(o))
			std::bind(f, provider, new Graphics::Bitmap(dynamic_cast<Image*>(o)->MoveOutAsBitmap()))();
		else
			std::bind(f, provider, new Graphics::Bitmap(std::move(*o)))();
	}

	template<class F_>
	static void setthis(F_ f, Graphics::AnimatedBitmapLineProvider *provider, Graphics::BitmapAnimationProvider *o) {
		if(!o) return; // do nothing

		if(dynamic_cast<Image*>(o))
			std::bind(f, provider, new Graphics::BitmapAnimationProvider(dynamic_cast<Animation*>(o)->MoveOutAsBitmap()))();
		else
			std::bind(f, provider, new Graphics::BitmapAnimationProvider(std::move(*o)))();
	}

	template<class F_>
	static void setthis(F_ f, Graphics::AnimatedBitmapLineProvider *provider, Graphics::RectangularAnimationProvider *o) {
		if(!o) return; // do nothing

		if(dynamic_cast<Image*>(o))
			std::bind(f, provider, new Graphics::Bitmap(dynamic_cast<Image*>(o)->MoveOutAsBitmap()))();
		if(dynamic_cast<Animation*>(o))
			std::bind(f, provider, new Graphics::BitmapAnimationProvider(dynamic_cast<Animation*>(o)->MoveOutAsBitmap()))();
		else
			std::bind(f, provider, o->MoveOutProvider())();
	}

	template<class T_>
	static void moveout(Graphics::basic_LineProvider<T_> *provider, Graphics::ILineProvider *&p) {
		auto bp = new Graphics::basic_LineProvider<T_>(provider->GetOrientation());
		p = bp;
		bp->SetTiling(provider->GetTiling());

		auto s = provider->GetStart();
		auto m = provider->GetMiddle();
		auto e = provider->GetEnd();

		setthis(&Graphics::basic_LineProvider<T_>::SetStart, bp, s);
		setthis(&Graphics::basic_LineProvider<T_>::SetMiddle, bp, m);
		setthis(&Graphics::basic_LineProvider<T_>::SetEnd, bp, e);

		bp->OwnProviders();
	}
	   
    Graphics::RectangularAnimationStorage Line::animmoveout() {
        if(!prov)
            throw std::runtime_error("Provider is not set");
        
        ILineProvider *p = nullptr;
        
        
        if(dynamic_cast<Graphics::BitmapLineProvider*>(prov)) {
            auto provider = dynamic_cast<Graphics::BitmapLineProvider*>(prov);
			moveout(provider, p);
        }
		else if(dynamic_cast<Graphics::AnimatedBitmapLineProvider*>(prov)) {
			auto provider = dynamic_cast<Graphics::AnimatedBitmapLineProvider*>(prov);
			moveout(provider, p);
		}
		else if(dynamic_cast<Graphics::LineProvider*>(prov)) {
			auto provider = dynamic_cast<Graphics::LineProvider*>(prov);
			moveout(provider, p);
		}

        if(!p)
            throw std::runtime_error("Provider is not set");
        
		for(auto &child : children) {
            child.DeleteResource();
        }

        children.Clear();
        
        if(own)
            delete prov;
        
        prov = nullptr;
        
        
        if(!p)
            throw std::runtime_error("Provider is not set");
        
        return Graphics::RectangularAnimationStorage(*p, true);
    }

} }
