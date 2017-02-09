#pragma once

#include "AnimationStorage.h"
#include "../Graphics/Rectangle.h"


namespace Gorgon { namespace Resource {
	class File;
	class Reader;

	class Rectangle : public Graphics::IRectangleProvider, public SizelessAnimationStorage {
	public:
		explicit Rectangle(Graphics::BitmapRectangleProvider &prov);

		explicit Rectangle(Graphics::AnimatedBitmapRectangleProvider &prov);

		Rectangle() = default;

		GID::Type GetGID() const override {
			return GID::Rectangle;
		}

		void SetProvider(Graphics::BitmapRectangleProvider &value) {
			prov = &value;
		}

		void SetProvider(Graphics::AnimatedBitmapRectangleProvider &value) {
			prov = &value;
		}

		virtual Graphics::RectangularAnimation &CreateTL() const override {
			if(!prov)
				throw std::runtime_error("Provider is not set.");

			return prov->CreateTL();
		}

		virtual Graphics::RectangularAnimation &CreateTM() const override {
			if(!prov)
				throw std::runtime_error("Provider is not set.");

			return prov->CreateTM();
		}

		virtual Graphics::RectangularAnimation &CreateTR() const override {
			if(!prov)
				throw std::runtime_error("Provider is not set.");

			return prov->CreateTR();
		}

		virtual Graphics::RectangularAnimation &CreateML() const override {
			if(!prov)
				throw std::runtime_error("Provider is not set.");

			return prov->CreateML();
		}

		virtual Graphics::RectangularAnimation &CreateMM() const override {
			if(!prov)
				throw std::runtime_error("Provider is not set.");

			return prov->CreateMM();
		}

		virtual Graphics::RectangularAnimation &CreateMR() const override {
			if(!prov)
				throw std::runtime_error("Provider is not set.");

			return prov->CreateMR();
		}

		virtual Graphics::RectangularAnimation &CreateBL() const override {
			if(!prov)
				throw std::runtime_error("Provider is not set.");

			return prov->CreateBL();
		}

		virtual Graphics::RectangularAnimation &CreateBM() const override {
			if(!prov)
				throw std::runtime_error("Provider is not set.");

			return prov->CreateBM();
		}

		virtual Graphics::RectangularAnimation &CreateBR() const override {
			if(!prov)
				throw std::runtime_error("Provider is not set.");

			return prov->CreateBR();
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

		virtual Gorgon::Graphics::Rectangle &CreateAnimation(Gorgon::Animation::ControllerBase &timer) const override {
			if(!prov)
				throw std::runtime_error("Provider is not set.");

			return dynamic_cast<Gorgon::Graphics::Rectangle &>(prov->CreateAnimation(timer));
		}
		
		virtual Gorgon::Graphics::Rectangle &CreateAnimation(bool create=true) const override {
			if(!prov)
				throw std::runtime_error("Provider is not set.");

			return dynamic_cast<Gorgon::Graphics::Rectangle &>(prov->CreateAnimation(create));
		}

		/// This function loads a rectangle resource from the file
		static Rectangle *LoadResource(std::weak_ptr<File> file, std::shared_ptr<Reader> reader, unsigned long size);

		/// Saves a provider directly
		static void SaveThis(Writer &writer, const Graphics::BitmapRectangleProvider &rectangle);

		/// Saves a provider directly
		static void SaveThis(Writer &writer, const Graphics::AnimatedBitmapRectangleProvider &rectangle);

	protected:
		void save(Writer &writer) const override;

		virtual Graphics::SizelessAnimationStorage animmoveout() override { Utils::NotImplemented(); }

	private:
		virtual ~Rectangle() { 
			if(own) 
				delete prov;
		}

		IRectangleProvider *prov = nullptr;
		
		bool own = false;
	};

} }
