#pragma once

#include <stdint.h>
#include <Gorgon/Utils/Assert.h>
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
        /// to be used internally.
        class GlyphDescriptor {
        public:
            GlyphDescriptor() { }
            
            GlyphDescriptor(const RectangularDrawable &image, int offset) : image(&image), offset(offset) { }
            
            GlyphDescriptor(int index, int offset) : index(index), offset(offset) { }
            
            union {
                const RectangularDrawable *image = nullptr;
                int index;
            };
            
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
        
        BitmapFont Duplicate() {
            Utils::NotImplemented();
        }
        
        BitmapFont(BitmapFont &&other) : BasicFont(dynamic_cast<GlyphRenderer &>(*this)) {
            using std::swap;
            
            swap(glyphmap, other.glyphmap);
            
            swap(destroylist, other.destroylist);
            
            isfixedw = other.isfixedw;
            
            maxwidth = other.maxwidth;
            
            height = other.height;
            
            baseline = other.baseline;

            digw = other.digw;

            isascii = other.isascii;
            
            spacing = other.spacing;

            linethickness = other.linethickness;

            underlinepos = other.underlinepos;
        }
      
        BitmapFont &operator =(const BitmapFont &) = delete;
        
        /// Moves another bitmap font into this one. This font will be destroyed
        /// in this process
        BitmapFont &operator =(BitmapFont &&other) {
            using std::swap;
            
            destroylist.Destroy();
            glyphmap.clear();
            
            swap(glyphmap, other.glyphmap);
            
            swap(destroylist, other.destroylist);
            
            isfixedw = other.isfixedw;
            
            maxwidth = other.maxwidth;
            
            height = other.height;
            
            baseline = other.baseline;

            digw = other.digw;

            isascii = other.isascii;
            
            spacing = other.spacing;

            linethickness = other.linethickness;

            underlinepos = other.underlinepos;
            
            return *this;
        }
        
        ~BitmapFont() {
			destroylist.Destroy();
        }
                
        /// Adds a new glyph bitmap to the list. If a previous one exists, it will be replaced.
        /// Ownership of bitmap is not transferred.
        void AddGlyph(Glyph glyph, const RectangularDrawable &bitmap, int baseline = 0);
                
        /// Adds a new glyph bitmap to the list. If a previous one exists, it will be replaced.
        /// Ownership of bitmap is not transferred.
        void AssumeGlyph(Glyph glyph, const RectangularDrawable &bitmap, int baseline = 0) {
            AddGlyph(glyph, bitmap, baseline);
            destroylist.Push(bitmap);
        }

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
        
        using BasicFont::GetSize;
        
        virtual Geometry::Size GetSize(Glyph chr) const override;
        
        virtual void Render(Glyph chr, TextureTarget &target, Geometry::Pointf location, RGBAf color) const override;

		virtual bool IsFixedWidth() const override { return isfixedw; }

		virtual bool Exists(Glyph g) const override { return glyphmap.count(g) != 0; }
		
		/// todo
		virtual int KerningDistance(Glyph chr1, Glyph chr2) const override { return 0; }
         
        virtual int GetMaxWidth() const override { return maxwidth; }
        
        virtual int GetHeight() const override { return height; }
        
		virtual int GetBaseLine() const override { return baseline; }

		virtual int GetGlyphSpacing() const override { return spacing; }

		virtual int GetDigitWidth() const override { return digw; }

		virtual int GetLineThickness() const override { return linethickness; }

		virtual int GetUnderlineOffset() const override { return underlinepos; }

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
		
		/// Changes the baseline. Might cause problems if the font already has glyphs in it.
		void SetBaseline(int value) { baseline = value; }
        
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
        
        /// Returns the image that represents a glyph
        const RectangularDrawable *GetImage(Glyph g) {
            if(glyphmap.count(g))
                return glyphmap.at(g).image;
            else
                return nullptr;
        }
        
        /// Removes a glyph from the bitmap font. If this glyph is created by font object
        /// and this glyph is the last user of that resource, it will be destroyed. Use
        /// Release to prevent this from happening.
        void Remove(Glyph g) {
            if(glyphmap.count(g)) {
                auto img = glyphmap.at(g).image;
                
                auto it = destroylist.Find(img);
                
                if(it.IsValid()) {
                    
                    int count = std::count_if(glyphmap.begin(), glyphmap.end(), 
                                [img](decltype(*glyphmap.begin()) p){ return p.second.image == img; });
                    
                    if(count == 1) {
                        it.Delete();
                    }
                }
                
                glyphmap.erase(g);
            }
        }
        
        /// If the given resource is owned by this bitmap font, its ownership will be released.
        bool Release(RectangularDrawable &img) {
            auto it = destroylist.Find(img);
            
            if(it.IsValid()) {            
                destroylist.Remove(it);
                
                return true;
            }
            else 
                return false;
        }
        
        /// Returns if the given image is owned by this bitmap font.
        bool IsOwned(RectangularDrawable &img) const {
            auto it = destroylist.Find(img);
            
            return it.IsValid();
        }
        
        std::map<Glyph, GlyphDescriptor>::iterator begin() {
            return glyphmap.begin();
        }
        
        std::map<Glyph, GlyphDescriptor>::iterator end() {
            return glyphmap.end();
        }
        
        std::map<Glyph, GlyphDescriptor>::const_iterator begin() const {
            return glyphmap.begin();
        }
        
        std::map<Glyph, GlyphDescriptor>::const_iterator end() const {
            return glyphmap.end();
        }
        
    protected:
                           
        std::map<Glyph, GlyphDescriptor> glyphmap;
		Containers::Collection<const RectangularDrawable> destroylist;
        
        int isfixedw = true;
        
        int maxwidth = 0;
        
        int height = 0;
        
        int baseline = 0;

		int digw = 0;

		bool isascii = true;
        
        int spacing = 0;

		int linethickness = 1;

		int underlinepos = 0;
    };
    
} }
