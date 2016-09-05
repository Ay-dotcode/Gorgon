#pragma once

#include <stdint.h>
#include <map>

#include "Font.h"
#include "Drawables.h"
#include "../Containers/Collection.h"


namespace Gorgon { namespace Graphics {


    /**
     * Bitmap fonts provide an easy way to render text on the screen. It is a GlyphRenderer
     * but also acts as TextRenderer for convenience. If control over TextRenderer is required,
     * it is best to use BitmapFont to construct a new TextRenderer and use the TextRenderer
     * to render the text instead of using BitmapFont itself for the task. 
     */
    class BitmapFont : public GlyphRenderer, public TextRenderer {
    public:
        class GlyphDescriptor {
        public:
            GlyphDescriptor() { }
            
            GlyphDescriptor(const RectangularDrawable &image, float offset) : image(&image), offset(offset) { }
            
            const RectangularDrawable *image = nullptr;
            float offset = 0;
        };
        
        enum ImportNamingTemplate {
            /// Filenames will be examined to determine the template. When this
            /// is set and prefix is empty, prefix is also tried to be determined
            Automatic,
            /// Characters are the filenames, not recommended as some symbols will
            /// not be accepted as filename
            Alpha,
            /// Decimal code of the character is used as filename
            Decimal,
            /// Hexadecimal code of the character is used as filename
            Hexadecimal
        };
        
        explicit BitmapFont(int baseline = 0) : baseline(baseline) { }
        
        BitmapFont(const BitmapFont &) = delete;
        
        //duplicate and move constructor
        
        BitmapFont &operator =(const BitmapFont &) = delete;
        
        ~BitmapFont() {
            delete renderer;
			destroylist.Destroy();
        }
        
        /// Changes the renderer to the requested class. You may also use bitmap
        /// font to create a new TextRender to have more control over it.
        template<class R_, class ...P_>
        void ChangeRenderer(P_ &&...params) {
            delete renderer;
            renderer = new R_(std::forward<P_>(params)...);
        }
        
        /// Adds a new glyph bitmap to the list. If a previous one exists, it will be replaced.
        /// Ownership of bitmap is not transferred.
        void AddGlyph(Glyph glyph, const RectangularDrawable &bitmap, float baseline = 0);
        
        /// Converts invidual glyphs to a single atlas
        void Pack();
        
        virtual Geometry::Size GetSize(const std::string & text) const override { return renderer->GetSize(text); }
        
        virtual Geometry::Size GetSize(const std::string & text, float width) const override { return renderer->GetSize(text,width); }
        
        virtual int GetCharacterIndex(const std::string &text, Geometry::Pointf location) const override { return renderer->GetCharacterIndex(text, location); }
        
        virtual Geometry::Pointf GetPosition(const std::string &text, int index) const override { return renderer->GetPosition(text, index); }

        virtual Geometry::Pointf GetPosition(const std::string &text, float w, int index) const override { return renderer->GetPosition(text, w, index); }
        
        virtual int GetCharacterIndex(const std::string &text, float w, Geometry::Pointf location) const override { return renderer->GetCharacterIndex(text, w, location); } 
        
        virtual bool IsASCII() const override {
            return isascii;
        }
        
        virtual Geometry::Size GetSize(Glyph chr) const override;
        
        virtual void Render(Glyph chr, TextureTarget &target, Geometry::Pointf location, RGBAf color) const override;

		virtual bool IsFixedWidth() const override { return isfixedw; }
		
		/// todo
		virtual float KerningDistance(Glyph chr1, Glyph chr2) const override { return spacing; }
         
        virtual int MaxWidth() const override { return maxwidth; }
        
        virtual int LineHeight() const override { return lineheight; }
        
		virtual int BaseLine() const override { return baseline; }

		/// Changes the line height of the font.
		void SetLineHeight(int value) { lineheight = value; }

		/// Changes the maximum width for a character
		void SetMaxWidth(int value) { maxwidth = value; }

		/// Searches through the currently registered glyphs to determine dimensions
		void DetermineDimensions();
        
        /// Changes the spacing between glyphs
        void SetGlyphSpacing(float spacing) { this->spacing = spacing; }
        
        /// Imports bitmap font images from a folder with the specified file naming template.
        /// Automatic detection will only work if there is a single bitmap font set in the
        /// folder. If baseline is set to a negative value, it would be calculated as 70%
        /// of the font height. Only png files are considered for import. If converttoalpha
		/// is set, then the images read will be converted to alpha only images. Import will
		/// import separate images, you may use Pack function to pack them to an image atlas.
		/// If prepare is set, the imported images will be prepared and font will be ready
		/// to be used. As of now, this function cannot deal with animated fonts. start 
		/// parameter can be used for adjusting numeric offset. If template naming is 
		/// automatic and the value is left as 0, it will be determined automatically so that
		/// the imported images will be matched with printable characters. Any files that
		/// start with . and _ is ignored, unless it is the name of the file. This function 
		/// will return the number of images imported. Imported images will be destroyed by 
		/// this object. Automatic conversion can cause issues with suffixes, however, if
		/// naming is set, any additional text after the number or character is ignored.
		/// If prepare is set to false, maxwidth and lineheight will not be set properly.
        int ImportFolder(const std::string &path, ImportNamingTemplate naming = Automatic, int start = 0, 
						 std::string prefix = "", int baseline = -1, bool trim = false, bool converttoalpha = false, 
						 bool prepare = true);
       
    protected:
        virtual void print(TextureTarget &target, const std::string &text, Geometry::Pointf location, RGBAf color) const override {
            renderer->Print(target, text, location, color);
        }
        
        virtual void print(TextureTarget &target, const std::string &text, 
                           Geometry::Rectanglef location, TextAlignment align, RGBAf color) const override {
            renderer->Print(target, text, location, align, color);
        }
                           
                           
        std::map<Glyph, GlyphDescriptor> glyphmap;
		Containers::Collection<RectangularDrawable> destroylist;
        
        bool isascii = true;
        
        int isfixedw = true;
        
        int maxwidth = 0;
        
        int lineheight = 0;
        
        int baseline;
        
        float spacing = 0;
        
    private:
        TextRenderer *renderer = new BasicFont(*this);
    };
    
} }
