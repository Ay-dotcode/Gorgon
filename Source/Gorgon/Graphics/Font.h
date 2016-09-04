#pragma once

#include <stdint.h>
#include <map>

#include "../Geometry/Point.h"
#include "../Geometry/Size.h"
#include "TextureTargets.h"
#include "Drawables.h"
#include "Color.h"
#include "Bitmap.h"

namespace Gorgon { namespace Graphics {

	using Glyph = int32_t;
	
	/**
	 * Should be implemented by the systems aimed to render fonts on the screen. Glyph renderer
     * should be capable of rendering single glyphs at the screen. These systems should also
     * provide info about glyphs.
     */
	class GlyphRenderer {
	public:
		/// This function should render the given character to the target at the specified location
		/// and color. If chr does not exists, this function should perform no action. location and
		/// color can be modified as per the needs of renderer. If the kerning returns integers
		/// location will always be an integer. Additionally, text renderers will place glyphs on
        /// 0 y position from the top. It is glyph renderer's task to ensure baseline of glyphs to 
        /// line up. 
		virtual void Render(Glyph chr, TextureTarget &target, Geometry::Pointf location, RGBAf color) const = 0;

		/// This function should return the size of the requested glyph. If it does not exists,
		/// 0x0 should be returned
		virtual Geometry::Size GetSize(Glyph chr) const = 0;

		/// This function should return true if this font renderer supports only 7-bit ASCII
		virtual bool IsASCII() const = 0;

		/// This function should return true if this font is fixed width. This will suppress calls
		/// to GetSize function.
		virtual bool IsFixedWidth() const = 0;
		
		/// This function should return the additional distance between given glyphs. Returned value
		/// could be (in most cases it is) negative. Non-integer values would break pixel perfect
		/// rendering.
		virtual float KerningDistance(Glyph chr1, Glyph chr2) const = 0;
        
        virtual int MaxWidth() const = 0;
        
        /// Height of glyphs, actual size could be smaller but all glyphs should have the same virtual
        /// height. When drawn on the same y position, all glyphs should line up. Renderer can change
        /// actual draw location to compansate.
        virtual int LineHeight() const = 0;
        
        /// Baseline point of glyphs from the top.
        virtual int BaseLine() const = 0;
	};

    /**
     * This class allows printing text on the screen. All fonts should support basic left aligned
     * print, aligned rectangular area printing. Additionally, all fonts should support basic info
     * functions. TextRenderers must be unicode aware.
     */
    class TextRenderer {
    public:
        virtual ~TextRenderer() { }
        
        void Print(TextureTarget &target, const std::string &text, Geometry::Pointf location, RGBAf color) const {
            print(target, text, location, color);
        }
        
        void Print(TextureTarget &target, const std::string &text, Geometry::Point location, RGBAf color) const {
            print(target, text, (Geometry::Pointf)location, color);
        }
        
        void Print(TextureTarget &target, const std::string &text, int x, int y, RGBAf color) const {
            print(target, text, {(float)x, (float)y}, color);
        }
        
        void Print(TextureTarget &target, const std::string &text, float x, float y, RGBAf color) const {
            print(target, text, {x, y}, color);
        }
        
        void Print(TextureTarget &target, const std::string &text, Geometry::Rectanglef location, TextAlignment align, RGBAf color) const {
            print(target, text, location, align, color);
        }
        
        //...
        
        
        virtual Geometry::Size GetSize(const std::string &text) const = 0;
        
        virtual Geometry::Size GetSize(const std::string &text, float width) const = 0;
        
        virtual int GetCharacterIndex(const std::string &text, Geometry::Pointf location) const = 0;
        
        virtual Geometry::Pointf GetPosition(const std::string &text, int index) const = 0;

    protected:
        virtual void print(TextureTarget &target, const std::string &text, Geometry::Pointf location, RGBAf color) const = 0;
        
        /// Should print the given text to the specified location and color. Width should be used to 
        /// align the text. Unless width is 0, text should be wrapped. Even if width is 0, the alignment
        /// should be respected. For instance if width is 0 and align is right, text should end at
        /// the given location. Height of the rectangle should be ignored.
        virtual void print(TextureTarget &target, const std::string &text, 
                           Geometry::Rectanglef location, TextAlignment align, RGBAf color) const = 0;
    };
    
    /**
     * This is the basic font, performing the minimal amount of operations necessary to render
     * text on the screen. It requires a single GlyphRenderer to work.
     */
    class BasicFont : public TextRenderer {
    public:
        BasicFont(const GlyphRenderer &renderer) : renderer(&renderer) { }
        
        virtual Geometry::Size GetSize(const std::string &text) const override { return {0,0}; }
        
        virtual Geometry::Size GetSize(const std::string &text, float width) const override { return {0,0}; }
        
        virtual int GetCharacterIndex(const std::string &text, Geometry::Pointf location) const override { return 0; }
        
        virtual Geometry::Pointf GetPosition(const std::string &text, int index) const override { return {0.f, 0.f}; }
        
    protected:
        virtual void print(TextureTarget &target, const std::string &text, Geometry::Pointf location, RGBAf color) const override;
        
        virtual void print(TextureTarget &target, const std::string &text, 
                           Geometry::Rectanglef location, TextAlignment align, RGBAf color) const override { }
         
    private:
        const GlyphRenderer *renderer;
    };
    
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
        
        BitmapFont(int baseline) : baseline(baseline) { }
        
        BitmapFont(const BitmapFont &) = delete;
        
        //duplicate and move constructor
        
        BitmapFont &operator =(const BitmapFont &) = delete;
        
        ~BitmapFont() {
            delete renderer;
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
        
        virtual bool IsASCII() const override {
            return isascii;
        }
        
        virtual Geometry::Size GetSize(Glyph chr) const override {
            if(glyphmap.count(chr))
                return glyphmap.at(chr).image->GetSize();
            else
                return {0, 0};
        }
        
        virtual void Render(Glyph chr, TextureTarget &target, Geometry::Pointf location, RGBAf color) const override;

		virtual bool IsFixedWidth() const override { return isfixedw; }
		
		/// todo
		virtual float KerningDistance(Glyph chr1, Glyph chr2) const override { return spacing; }
         
        virtual int MaxWidth() const override { return maxwidth; }
        
        virtual int LineHeight() const override { return lineheight; }
        
        virtual int BaseLine() const override { return baseline; }
        
        /// Changes the spacing between glyphs
        void SetGlyphSpacing(float spacing) { this->spacing = spacing; }
       
    protected:
        virtual void print(TextureTarget &target, const std::string &text, Geometry::Pointf location, RGBAf color) const override {
            renderer->Print(target, text, location, color);
        }
        
        virtual void print(TextureTarget &target, const std::string &text, 
                           Geometry::Rectanglef location, TextAlignment align, RGBAf color) const override {
            renderer->Print(target, text, location, align, color);
        }
                           
                           
        std::map<Glyph, GlyphDescriptor> glyphmap;
        
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
