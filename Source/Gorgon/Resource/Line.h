#pragma once

#include "AnimationStorage.h"
#include "../Graphics/Line.h"


namespace Gorgon { namespace Resource {
	class File;
	class Reader;

	class Line : public Graphics::ILineProvider, public SizelessAnimationStorage {
	public:
		explicit Line(Graphics::BitmapLineProvider &prov);

		explicit Line(Graphics::AnimatedBitmapLineProvider &prov);

		Line() : ILineProvider(Graphics::Orientation::Horizontal) { }

		GID::Type GetGID() const override {
			return GID::Line;
		}

		void SetProvider(Graphics::BitmapLineProvider &value) {
			prov = &value;
		}

		void SetProvider(Graphics::AnimatedBitmapLineProvider &value) {
			prov = &value;
		}

		virtual Graphics::RectangularAnimation &CreateStart() const override {
			if(!prov)
				throw std::runtime_error("Provider is not set.");

			return prov->CreateStart();
		}

		virtual Graphics::RectangularAnimation &CreateMiddle() const override {
			if(!prov)
				throw std::runtime_error("Provider is not set.");

			return prov->CreateMiddle();
		}

		virtual Graphics::RectangularAnimation &CreateEnd() const override {
			if(!prov)
				throw std::runtime_error("Provider is not set.");

			return prov->CreateEnd();
		}

		virtual void SetOrientation(Graphics::Orientation value) override {
			if(!prov)
				throw std::runtime_error("Provider is not set.");

			prov->SetOrientation(value);
		}

		virtual Graphics::Orientation GetOrientation() const override {
			if(!prov)
				throw std::runtime_error("Provider is not set.");

			return prov->GetOrientation();
		}

		virtual void SetTiling(bool value) override {
			if(!prov)
				throw std::runtime_error("Provider is not set.");

			prov->SetTiling(value);
		}

		virtual bool GetTiling() const override {
			if(!prov)
				throw std::runtime_error("Provider is not set.");

			return prov->GetTiling();
		}

		virtual Gorgon::Graphics::Line &CreateAnimation(Gorgon::Animation::ControllerBase &timer) const override {
			if(!prov)
				throw std::runtime_error("Provider is not set.");

			return dynamic_cast<Gorgon::Graphics::Line &>(prov->CreateAnimation(timer));
		}
		
		virtual Gorgon::Graphics::Line &CreateAnimation(bool create=true) const override {
			if(!prov)
				throw std::runtime_error("Provider is not set.");

			return dynamic_cast<Gorgon::Graphics::Line &>(prov->CreateAnimation(create));
		}

		/// This function loads a line resource from the file
		static Line *LoadResource(std::weak_ptr<File> file, std::shared_ptr<Reader> reader, unsigned long size);

		/// Saves a provider directly
		static void SaveThis(Writer &writer, const Graphics::BitmapLineProvider &line);

		/// Saves a provider directly
		static void SaveThis(Writer &writer, const Graphics::AnimatedBitmapLineProvider &line);

	protected:
		void save(Writer &writer) const override;

		virtual Graphics::SizelessAnimationStorage animmoveout() override { Utils::NotImplemented(); }

	private:
		virtual ~Line() { 
			if(own) 
				delete prov;
		}

		ILineProvider *prov = nullptr;
		
		bool own = false;
	};

} }
