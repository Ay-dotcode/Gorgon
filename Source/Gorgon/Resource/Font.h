#pragma once

#include "Base.h"
#include "Image.h"
#include "../Graphics/Font.h"
#include "../Graphics/BitmapFont.h"
#include "../Utils/Assert.h"

namespace Gorgon { namespace Resource {
	class File;
	class Reader;

    /**
     * Font resource. Can hold a bitmap font for now. Raster fonts over free
     * type will be implmented later.
     */
    class Font : public Base {
    public:
        Font() { }
        
        /// Filling constructor, for now can only hold Graphics::BitmapFont. All images
        /// in the renderer should be bitmaps or their derivatives with data buffers
        /// still attached.
        Font(Graphics::GlyphRenderer &renderer);
        
        Font(const Font &) = delete;
        
        void Swap(Font &other);
        
        //todo duplicate

		virtual GID::Type GetGID() const override { return GID::Font; }
        
        /// Bitmap font requires its glyphs to be added in bitmap form. This allows them to be saved. In this
        /// variant, given image will be wrapped with Resource::Image
        void AddGlyph(Graphics::Bitmap &&img);
        
        void AddGlyph(const Image &img);        
        
		virtual void Prepare() override { }
        
		virtual void Discard() override { }
        
        /// Returns true if the resource has renderer.
        bool HasRenderer() const {
            return data != nullptr;
        }
        
        /// Returns the renderer stored in this resource.
        Graphics::GlyphRenderer &GetRenderer() const {
            ASSERT(data, "Renderer is not set");
            
            return *data;
        }
        
        /// Removes the renderer from this resource.
        void RemoveRenderer() {
            data = nullptr;
        }
        
		/// This function loads a bitmap font resource from the given file
		static Font *LoadResource(std::weak_ptr<File> file, std::shared_ptr<Reader> reader, unsigned long size);
        
    protected:
		void save(Writer &writer) const override;

        Graphics::GlyphRenderer *data = nullptr;
    };

} }


