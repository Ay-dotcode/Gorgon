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
    class BitmapFont : public GlyphRenderer, public BasicFont {
    public:
        class GlyphDescriptor {
        public:
            GlyphDescriptor() { }
            
            GlyphDescriptor(const RectangularDrawable &image, int offset) : image(&image), offset(offset) { }
            
            const RectangularDrawable *image = nullptr;
            int offset = 0;
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

		enum DeleteConstants {
			None,
			Owned,
			All
		};
        
        explicit BitmapFont(int baseline = 0) : BasicFont(dynamic_cast<GlyphRenderer &>(*this)), baseline(baseline) { }
        
        BitmapFont(const BitmapFont &) = delete;
        
        //duplicate and move constructor
        
        BitmapFont &operator =(const BitmapFont &) = delete;
        
        ~BitmapFont() {
			destroylist.Destroy();
        }
                
        /// Adds a new glyph bitmap to the list. If a previous one exists, it will be replaced.
        /// Ownership of bitmap is not transferred.
        void AddGlyph(Glyph glyph, const RectangularDrawable &bitmap, int baseline = 0);

		virtual bool IsASCII() const override {
			return isascii;
		}

        /// Converts individual glyphs to a single atlas. Only the glyphs that are registered as bitmaps can be packed.
        /// This function will automatically detect types and act accordingly. If the ownership of the packed images
        /// belong to the font and del parameter is set to owned, owned images that are created either by import or a
		/// previous pack will be destroyed. If it is set to all, all images that took part in packing will be destroyed
		/// If tight packing is set, glyphs will be placed next to each other, saving space. However, if resized, they 
		/// will have artifacts.
        void Pack(bool tight = false, DeleteConstants del = Owned);
        
        virtual Geometry::Size GetSize(Glyph chr) const override;
        
        virtual void Render(Glyph chr, TextureTarget &target, Geometry::Pointf location, RGBAf color) const override;

		virtual bool IsFixedWidth() const override { return isfixedw; }

		virtual bool Exists(Glyph g) const override { return glyphmap.count(g) != 0; }
		
		/// todo
		virtual int KerningDistance(Glyph chr1, Glyph chr2) const override { return 0; }
         
        virtual int GetMaxWidth() const override { return maxwidth; }
        
        virtual int GetHeight() const override { return height; }
        
		virtual int GetBaseLine() const override { return baseline; }

		virtual int GetXSpacing() const override { return spacing; }

		virtual int GetDigitWidth() const override { return digw; }

		virtual int GetLineThickness() const override { return linethickness; }

		virtual int GetUnderlineOffset() const override { return underlinepos; }


		/// Changes the spacing between consecutive characters
		void SetSpacing(int value) {
			xspace = value;
		}

		/// Changes the line height of the font. Adding glyphs may override this value.
		void SetHeight(int value) { height = value; }

		/// Changes the maximum width for a character. Adding glyphs may override this value.
		void SetMaxWidth(int value) { maxwidth = value; }

		/// Searches through the currently registered glyphs to determine dimensions. This 
		/// function will calculate following values: height, max width, underline offset.
		/// Baseline is set to 0.7 * height if it is 0.
		void DetermineDimensions();
        
        /// Changes the spacing between glyphs
        void SetGlyphSpacing(int spacing) { this->spacing = spacing; }

		/// Changes the line thickness to the specified value.
		void SetLineThickness(int value) { linethickness = value; }

		/// Changes the underline position to the specified value.
		void SetUnderlineOffset(int value) { underlinepos = value; }
        
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
		/// This function calculates the line thickness using trimmed height of the underscore.
		/// If trimming is not set, this functionality will not work. Additionally, this 
		/// function will set underline position to halfway between baseline and bottom.
        int ImportFolder(const std::string &path, ImportNamingTemplate naming = Automatic, int start = 0, 
						 std::string prefix = "", int baseline = -1, bool trim = true, bool converttoalpha = true, 
						 bool prepare = true);
       
    protected:
                           
        std::map<Glyph, GlyphDescriptor> glyphmap;
		Containers::Collection<RectangularDrawable> destroylist;
        
        int isfixedw = true;
        
        int maxwidth = 0;
        
        int height = 0;
        
        int baseline;

		int digw = 0;

		bool isascii = true;

		int xspace = 1;
        
        int spacing = 0;

		int linethickness = 1;

		int underlinepos = 0;
    };
    
} }
