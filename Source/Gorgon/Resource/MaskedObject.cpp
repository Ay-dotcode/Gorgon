#include "MaskedObject.h"
#include "Animation.h"
#include "Image.h"
#include "File.h"
#include "Reader.h"
#include "Null.h"

namespace Gorgon {
	namespace Resource {


		MaskedObject::MaskedObject(Graphics::MaskedBitmapProvider &prov) : prov(&prov), IMaskedObjectProvider() 
		{ }

		MaskedObject::MaskedObject(Graphics::MaskedBitmapAnimationProvider &prov) : prov(&prov), IMaskedObjectProvider() 
		{ }

		MaskedObject::MaskedObject(Graphics::MaskedObjectProvider &prov) : prov(&prov), IMaskedObjectProvider()
		{ }

		template<class T_>
		static Graphics::basic_MaskedObjectProvider<T_> &fillfrom(Containers::Collection<Base> &children) {
			T_ *b = nullptr, *m = nullptr;

			if(children.GetCount() == 1) {
				b = dynamic_cast<T_*>(&children[0]);
			}
			else if(children.GetCount() == 2) {
				b = dynamic_cast<T_*>(&children[0]);
				m = dynamic_cast<T_*>(&children[1]);
			}

			return *new Graphics::basic_MaskedObjectProvider<T_>(b, m);
		}

		MaskedObject *MaskedObject::LoadResource(std::weak_ptr<File> f, std::shared_ptr<Reader> reader, unsigned long totalsize) {
			auto target = reader->Target(totalsize);

			auto file = f.lock();

			auto masked = new MaskedObject();

			enum {
				unknown, img, anim, mixed
			} type = unknown;
			int c = 0;

			while(!target) {
				auto gid = reader->ReadGID();
				auto size= reader->ReadChunkSize();

				auto resource = file->LoadChunk(*masked, gid, size, false);

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
						throw std::runtime_error("MaskedObject can only contain images or animations");
					}

					if(++c > 3) {
						throw std::runtime_error("MaskedObject cannot have more than 3 parts");
					}

					masked->children.Add(resource);
				}
			}

			if(type == anim) {
				masked->SetProvider(
					fillfrom<Graphics::BitmapAnimationProvider>(masked->children)
				);
			}
			else if(type == img) {
				masked->SetProvider(
					fillfrom<Graphics::Bitmap>(masked->children)
				);
			}
			else if(type == mixed) {
				masked->SetProvider(
					fillfrom<Graphics::RectangularAnimationProvider>(masked->children)
				);
			}
			//else empty masked object

			masked->own = true;
			return masked;
		}

		template <class T_>
		static void savethis(Writer &writer, const Graphics::basic_MaskedObjectProvider<T_> &provider) {
			auto b = provider.GetBase();
			auto m = provider.GetMask();

			if(!m) {
				SaveAnimation(writer, m);
			}
			else {
				SaveAnimation(writer, b);
				SaveAnimation(writer, m);
			}
		}

		void MaskedObject::save(Writer &writer) const {
			auto start = writer.WriteObjectStart(this);

			if(dynamic_cast<Graphics::MaskedBitmapProvider*>(prov)) {
				savethis(writer, *dynamic_cast<Graphics::MaskedBitmapProvider*>(prov));
			}
			else if(dynamic_cast<Graphics::MaskedBitmapAnimationProvider*>(prov)) {
				savethis(writer, *dynamic_cast<Graphics::MaskedBitmapAnimationProvider*>(prov));
			}
			else if(dynamic_cast<Graphics::MaskedObjectProvider*>(prov)) {
				savethis(writer, *dynamic_cast<Graphics::MaskedObjectProvider*>(prov));
			}
			else if(prov != nullptr) {
				throw std::runtime_error("Unknown masked provider");
			}

			writer.WriteEnd(start);
		}

		void MaskedObject::SaveThis(Writer &writer, const Graphics::IMaskedObjectProvider &provider) {
			auto start = writer.WriteChunkStart(GID::MaskedObject);
			auto prov = &provider;

			if(dynamic_cast<const Graphics::MaskedBitmapProvider*>(prov)) {
				savethis(writer, *dynamic_cast<const Graphics::MaskedBitmapProvider*>(prov));
			}
			else if(dynamic_cast<const Graphics::MaskedBitmapAnimationProvider*>(prov)) {
				savethis(writer, *dynamic_cast<const Graphics::MaskedBitmapAnimationProvider*>(prov));
			}
			else if(dynamic_cast<const Graphics::MaskedObjectProvider*>(prov)) {
				savethis(writer, *dynamic_cast<const Graphics::MaskedObjectProvider*>(prov));
			}
			else {
				throw std::runtime_error("Unknown masked provider");
			}

			writer.WriteEnd(start);
		}

		template<class T_, class F_>
		static void setthis(F_ f, Graphics::basic_MaskedObjectProvider<T_> *provider, T_ *o) {
			Utils::ASSERT_FALSE("Should not run");
		}

		template<class F_>
		static void setthis(F_ f, Graphics::MaskedBitmapProvider *provider, Graphics::Bitmap *o) {
			if(!o) return; // do nothing

			if(dynamic_cast<Image*>(o))
				std::bind(f, provider, new Graphics::Bitmap(dynamic_cast<Image*>(o)->MoveOutAsBitmap()))();
			else
				std::bind(f, provider, new Graphics::Bitmap(std::move(*o)))();
		}

		template<class F_>
		static void setthis(F_ f, Graphics::MaskedBitmapAnimationProvider *provider, Graphics::BitmapAnimationProvider *o) {
			if(!o) return; // do nothing

			if(dynamic_cast<Image*>(o))
				std::bind(f, provider, new Graphics::BitmapAnimationProvider(dynamic_cast<Animation*>(o)->MoveOutAsBitmap()))();
			else
				std::bind(f, provider, new Graphics::BitmapAnimationProvider(std::move(*o)))();
		}

		template<class F_>
		static void setthis(F_ f, Graphics::MaskedBitmapAnimationProvider *provider, Graphics::RectangularAnimationProvider *o) {
			if(!o) return; // do nothing

			if(dynamic_cast<Image*>(o))
				std::bind(f, provider, new Graphics::Bitmap(dynamic_cast<Image*>(o)->MoveOutAsBitmap()))();
			if(dynamic_cast<Animation*>(o))
				std::bind(f, provider, new Graphics::BitmapAnimationProvider(dynamic_cast<Animation*>(o)->MoveOutAsBitmap()))();
			else
				std::bind(f, provider, o->MoveOutProvider())();
		}

		template<class T_>
		static void moveout(Graphics::basic_MaskedObjectProvider<T_> *provider, Graphics::IMaskedObjectProvider *&p) {
			auto bp = new Graphics::basic_MaskedObjectProvider<T_>();
			p = bp;

			auto b = provider->GetBase();
			auto m = provider->GetMask();

			setthis(&Graphics::basic_MaskedObjectProvider<T_>::SetBase, bp, b);
			setthis(&Graphics::basic_MaskedObjectProvider<T_>::SetMask, bp, m);

			bp->OwnProviders();
		}

		Graphics::RectangularAnimationStorage MaskedObject::animmoveout() {
			if(!prov)
				throw std::runtime_error("Provider is not set");

			IMaskedObjectProvider *p = nullptr;


			if(dynamic_cast<Graphics::MaskedBitmapProvider*>(prov)) {
				auto provider = dynamic_cast<Graphics::MaskedBitmapProvider*>(prov);
				moveout(provider, p);
			}
			else if(dynamic_cast<Graphics::MaskedBitmapAnimationProvider*>(prov)) {
				auto provider = dynamic_cast<Graphics::MaskedBitmapAnimationProvider*>(prov);
				moveout(provider, p);
			}
			else if(dynamic_cast<Graphics::MaskedObjectProvider*>(prov)) {
				auto provider = dynamic_cast<Graphics::MaskedObjectProvider*>(prov);
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

	}
}
