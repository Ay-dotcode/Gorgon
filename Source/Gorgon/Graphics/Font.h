#pragma once

#include <stdint.h>
#include <map>

#include "../Geometry/Point.h"
#include "../Geometry/Size.h"
#include "TextureTargets.h"
#include "Color.h"

namespace Gorgon { namespace Graphics {
	using Glyph = int32_t;
	
    namespace internal {
        Glyph decode(std::string::const_iterator &it, std::string::const_iterator end);
        bool isnewline(Glyph g, std::string::const_iterator &it, std::string::const_iterator end);
        bool isspaced(Glyph g);
		bool isspace(Glyph g);
		bool isadjusablespace(Glyph g);
		bool isbreaking(Glyph g);
    }

	/**
	 * Should be implemented by the systems aimed to render fonts on the screen. Glyph renderer
     * should be capable of rendering single glyphs at the screen. These systems should also
     * provide info about glyphs.
     */
	class GlyphRenderer {
	public:
		virtual ~GlyphRenderer() { }

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
        
		/// Returns the width of widest glyph.
        virtual int GetMaxWidth() const = 0;
        
        /// Height of glyphs, actual size could be smaller but all glyphs should have the same virtual
        /// height. When drawn on the same y position, all glyphs should line up. Renderer can change
        /// actual draw location to compensate.
        virtual int GetLineHeight() const = 0;
        
        /// Baseline point of glyphs from the top.
        virtual int GetBaseLine() const = 0;
	};

    /**
     * This class allows printing text on the screen. All fonts should support basic left aligned
     * print, aligned rectangular area printing. Additionally, all fonts should support basic info
     * functions. TextRenderers must be unicode aware.
     */
    class TextRenderer {
    public:
        virtual ~TextRenderer() { }
        
		/// Prints the given text to the target
		void Print(TextureTarget &target, const std::string &text, Geometry::Pointf location) const {
            print(target, text, location);
        }
        
        void Print(TextureTarget &target, const std::string &text, Geometry::Point location) const {
            print(target, text, (Geometry::Pointf)location);
        }
        
        void Print(TextureTarget &target, const std::string &text, int x, int y) const {
            print(target, text, {(float)x, (float)y});
        }
        
        void Print(TextureTarget &target, const std::string &text, float x, float y) const {
            print(target, text, {x, y});
        }
        
        void Print(TextureTarget &target, const std::string &text, Geometry::Rectanglef location, TextAlignment align_override) const {
            print(target, text, location, align_override);
        }
        
        void Print(TextureTarget &target, const std::string &text, Geometry::Rectanglef location) const {
            print(target, text, location);
        }
        
        void Print(TextureTarget &target, const std::string &text, Geometry::Pointf location, float w, TextAlignment align_override) const {
            print(target, text, {location, w, 0.f}, align_override);
        }
        
        void Print(TextureTarget &target, const std::string &text, Geometry::Pointf location, float w) const {
            print(target, text, {location, w, 0.f});
        }
        
        void Print(TextureTarget &target, const std::string &text, Geometry::Point location, int w, TextAlignment align_override) const {
            print(target, text, {Geometry::Pointf(location), float(w), 0.f}, align_override);
        }
        
        void Print(TextureTarget &target, const std::string &text, Geometry::Point location, int w) const {
            print(target, text, {Geometry::Pointf(location), float(w), 0.f});
        }
        
        void Print(TextureTarget &target, const std::string &text, float x, float y, float w, TextAlignment align_override) const {
            print(target, text, {x, y, w, 0.f}, align_override);
        }
        
        void Print(TextureTarget &target, const std::string &text, float x, float y, float w) const {
            print(target, text, {x, y, w, 0.f});
        }
        
        void Print(TextureTarget &target, const std::string &text, int x, int y, int w, TextAlignment align_override) const {
            print(target, text, {(float)x, (float)y, float(w), 0.f}, align_override);
        }
        
        void Print(TextureTarget &target, const std::string &text, int x, int y, int w) const {
            print(target, text, {(float)x, (float)y, float(w), 0.f});
        }

		void Print(TextureTarget &target, const std::string &text) {
			print(target, text, {0.f, 0.f, target.GetTargetSize()});
		}
        
        virtual Geometry::Size GetSize(const std::string &text) const = 0;
        
        virtual Geometry::Size GetSize(const std::string &text, float width) const = 0;
        
        virtual int GetCharacterIndex(const std::string &text, Geometry::Pointf location) const = 0;
        
        virtual Geometry::Pointf GetPosition(const std::string &text, int index) const = 0;
        
        virtual int GetCharacterIndex(const std::string &text, float w, Geometry::Pointf location) const = 0;
        
        virtual Geometry::Pointf GetPosition(const std::string &text, float w, int index) const = 0;

    protected:
        virtual void print(TextureTarget &target, const std::string &text, Geometry::Pointf location) const = 0;
        
        /// Should print the given text to the specified location and color. Width should be used to 
        /// align the text. Unless width is 0, text should be wrapped. Even if width is 0, the alignment
        /// should be respected. For instance if width is 0 and align is right, text should end at
        /// the given location. Height of the rectangle can be left 0, thus unless explicitly requested,
		/// it should be ignored.
		virtual void print(TextureTarget &target, const std::string &text,
						   Geometry::Rectanglef location, TextAlignment align_override) const = 0;

		virtual void print(TextureTarget &target, const std::string &text,
						   Geometry::Rectanglef location) const = 0;
	};
    
    /**
     * This is the basic font, performing the minimal amount of operations necessary to render
     * text on the screen. It requires a single GlyphRenderer to work.
     */
    class BasicFont : public TextRenderer {
    public:
        BasicFont(const GlyphRenderer &renderer, RGBAf color = 1.f, TextAlignment defaultalign = TextAlignment::Left) : 
			defaultalign(defaultalign), color(color), renderer(&renderer) {
		}

		using TextRenderer::Print;


		void Print(TextureTarget &target, const std::string &text, Geometry::Pointf location, RGBAf color) const {
			print(target, text, location, color);
		}

		void Print(TextureTarget &target, const std::string &text, RGBAf color) const {
			print(target, text, {0.f, 0.f, target.GetTargetSize()}, color);
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

		void Print(TextureTarget &target, const std::string &text, Geometry::Rectanglef location, TextAlignment align_override, RGBAf color) const {
			print(target, text, location, align_override, color);
		}

		void Print(TextureTarget &target, const std::string &text, Geometry::Rectanglef location, RGBAf color) const {
			print(target, text, location, color);
		}

		void Print(TextureTarget &target, const std::string &text, Geometry::Pointf location, float w, TextAlignment align_override, RGBAf color) const {
			print(target, text, {location, w, 0.f}, align_override, color);
		}

		void Print(TextureTarget &target, const std::string &text, Geometry::Pointf location, float w, RGBAf color) const {
			print(target, text, {location, w, 0.f}, color);
		}

		void Print(TextureTarget &target, const std::string &text, Geometry::Point location, int w, TextAlignment align_override, RGBAf color) const {
			print(target, text, {Geometry::Pointf(location), float(w), 0.f}, align_override, color);
		}

		void Print(TextureTarget &target, const std::string &text, Geometry::Point location, int w, RGBAf color) const {
			print(target, text, {Geometry::Pointf(location), float(w), 0.f}, color);
		}

		void Print(TextureTarget &target, const std::string &text, float x, float y, float w, TextAlignment align_override, RGBAf color) const {
			print(target, text, {x, y, w, 0.f}, align_override, color);
		}

		void Print(TextureTarget &target, const std::string &text, float x, float y, float w, RGBAf color) const {
			print(target, text, {x, y, w, 0.f}, color);
		}

		void Print(TextureTarget &target, const std::string &text, int x, int y, int w, TextAlignment align_override, RGBAf color) const {
			print(target, text, {(float)x, (float)y, float(w), 0.f}, align_override, color);
		}

		void Print(TextureTarget &target, const std::string &text, int x, int y, int w, RGBAf color) const {
			print(target, text, {(float)x, (float)y, float(w), 0.f}, color);
		}

		/// Changes the default alignment. It is possible to override default alignment through TextRenderer interface.
		void SetDefaultAlignment(TextAlignment value) {
			defaultalign = value;
		}

		/// Returns the current default alignment.
		TextAlignment GetDefaultAlignment() const {
			return defaultalign;
		}

		/// Changes the the color of the text. Color can only be overriden through BasicFont interface.
		void SetColor(RGBAf value) {
			color = value;
		}

		/// Returns the current text color
		RGBAf GetColor() const {
			return color;
		}

        virtual Geometry::Size GetSize(const std::string &text) const override;
        
        virtual Geometry::Size GetSize(const std::string &text, float width) const override { return {0,0}; }
        
        virtual int GetCharacterIndex(const std::string &text, Geometry::Pointf location) const override { return 0; }
        
        virtual Geometry::Pointf GetPosition(const std::string &text, int index) const override { return {0.f, 0.f}; }
        
        virtual int GetCharacterIndex(const std::string &text, float w, Geometry::Pointf location) const override { return 0; }
        
        virtual Geometry::Pointf GetPosition(const std::string &text, float w, int index) const override { return {0.f, 0.f}; }
        
    protected:
		virtual void print(TextureTarget &target, const std::string &text, Geometry::Pointf location) const override {
			print(target, text, location, color);
		}

		virtual void print(TextureTarget &target, const std::string &text,
						   Geometry::Rectanglef location, TextAlignment align) const override {
			print(target, text, location, align, color);
		}

		virtual void print(TextureTarget &target, const std::string &text, Geometry::Pointf location, RGBAf color) const;

		virtual void print(TextureTarget &target, const std::string &text,
						   Geometry::Rectanglef location, TextAlignment align, RGBAf color) const;

		virtual void print(TextureTarget &target, const std::string &text,
						   Geometry::Rectanglef location) const override {
			print(target, text, location, defaultalign);
		}

		virtual void print(TextureTarget &target, const std::string &text,
						   Geometry::Rectanglef location, RGBAf color) const {
			print(target, text, location, defaultalign, color);
		}

		/// Default alignment if none is specified
		TextAlignment defaultalign;

		/// Color of this renderer, can be overridden.
		RGBAf color;

    private:
        const GlyphRenderer *renderer;
    };

    /**
     * Describes how a text shadow should be. Default is no shadow.
     */
	class TextShadow {
	public:

		enum ShadowType {
			None,
			Flat
		};

        TextShadow() = default;

		TextShadow(ShadowType type) : type(type) { }

		TextShadow(ShadowType type, RGBAf color, Geometry::Pointf offset) :
			type(type), color(color), offset(offset) {}

		TextShadow(RGBAf color, Geometry::Pointf offset = {1.f, 1.f}) :
			type(Flat), color(color), offset(offset) {}

		ShadowType type = None;
		RGBAf color = 0x80000000;
		Geometry::Pointf offset = {1.f, 1.f};
	};

    /**
     * This text renderer can style text according to the set parameters. It can draw shadow
     * modify spacings and tabwidth and is capable of rendering underline. This object is not
	 * heavy, and could be copied. When setting up spacing, try to avoid non-integer values,
	 * as they would cause text to blur.
     */
	class StyledRenderer : public TextRenderer {
	public:
		/// Renderer must be ready in order to calculate spacings correctly. If it will be
		/// initialized later, call ResetSpacing to reset all to defaults.
		StyledRenderer(GlyphRenderer &renderer, RGBAf color = 1.f, TextShadow shadow = {}) : 
			renderer(&renderer), color(color), shadow(shadow) { 
			tabwidth = renderer.GetMaxWidth() * 8;
		}

        /// Changes the color of the text
        void SetColor(RGBAf value) {
        	color = value;
        }
        
        /// Returns color of the text
        RGBAf GetColor() const {
        	return color;
        }

		/// Changes text shadow
		void SetShadow(TextShadow value) {
			shadow = value;
		}

		/// Remove text shadow
		void RemoveShadow() {
			shadow = TextShadow::None;
		}

		/// Uses flat shadow for text
		void UseFlatShadow(RGBAf color, Geometry::Pointf offset = {1.f, 1.f}) {
			shadow = {TextShadow::Flat, color, offset};
		}
		
		/// Returns text shadow
		TextShadow GetShadow() const {
			return shadow;
		}

		/// Sets underlining for the text
		void SetUnderline(bool value) {
			underline = value;
		}
		
		/// Returns whether the text is underlined
		bool GetUnderline() const {
			return underline;
		}

		/// Sets whether the text would be stroked
		void SetStrikeThrough(bool value) {
			strikethrough = value;
		}
		
		/// Returns whether the text would stroked
		bool GetStrikeThrough() const {
			return strikethrough;
		}

		/// Sets the default alignment for the text
		void SetDefaultAlign(TextAlignment value) {
			defaultalign = value;
		}

		/// Aligns the text to the left, removes justify
		void AlignLeft() {
			defaultalign = TextAlignment::Left;
			justify = false;
		}

		/// Aligns the text to the right, removes justify
		void AlignRight() {
			defaultalign  = TextAlignment::Right;
			justify = false;
		}

		/// Aligns the text to the center, removes justify
		void AlignCenter() {
			defaultalign = TextAlignment::Center;
			justify = false;
		}
		
		/// Returns the default alignment for the text
		TextAlignment GetDefaultAlign() const {
			return defaultalign;
		}

		/// Sets whether the text would be justified. Justify will not affect single line
		/// text as well last line of a paragraph.
		void SetJustify(bool value) {
			justify = value;
		}

		/// Aligns the text to the left, sets justify
		void JustifyLeft() {
			defaultalign = TextAlignment::Left;
		}

		/// Aligns the text to the right, sets justify
		void JustifyRight() {
			defaultalign  = TextAlignment::Right;
		}

		/// Aligns the text to the center, sets justify
		void JustifyCenter() {
			defaultalign = TextAlignment::Center;
		}
		
		/// Returns whether the text would be justified
		bool GetJustify() const {
			return justify;
		}

		/// Sets the line spacing in pixels, this spacing is the space between
		/// two lines, from the descender of the first line to the ascender of
		/// the second.
		void SetLineSpacingPixels(float value) {
			vspace = value;
		}
		
		/// Returns the line spacing in pixels
		float GetLineSpacingPixels() const {
			return vspace;
		}

		/// Sets the line spacing as percentage of glyph height. A value of 
		/// one will make the lines touch each other, where as a value of two
		/// will leave a full line empty between two lines. Line spacing stored 
		/// in pixels to be added to the glyph height. This will round the final
		/// result to the nearest pixel. Unless the font is large enough, 
		/// non-integer pixel coordinates should be avoided to keep text crisp
		void SetLineSpacing(float value) {
			vspace = std::round((value-1) * renderer->GetLineHeight());
		}

		/// Sets the line spacing as percentage of glyph height. A value of 
		/// one will make the lines touch each other, where as a value of two
		/// will leave a full line empty between two lines. Line spacing stored 
		/// in pixels to be added to the glyph height. This variant does not
		/// perform rounding. Unless the font is large enough, non-integer
		/// pixel coordinates should be avoided to keep text crisp
		void SetLineSpacingNoRounding(float value) {
			vspace = (value-1) * renderer->GetLineHeight();
		}

		/// Returns the line spacing as percentage of glyph height
		float GetLineSpacing() const {
			return (vspace / renderer->GetLineHeight()) + 1;
		}

		/// Spacing between letters of the text, in pixels
		void SetLetterSpacing(float value) {
			hspace = value;
		}
		
		/// Returns the spacing between the letters in pixels
		float GetLetterSpacing() const {
			return hspace;
		}

		/// Distance between tab stops. This value is in pixels. Default value is
		/// 8 * widest glyph's width.
		void SetTabWidth(int value) {
			tabwidth = value;
		}

		/// Sets the tab width in widest glyph's multiples.
		void SetTabWidthInLetters(float value) {
			tabwidth = (int)std::round(value * renderer->GetMaxWidth());
		}
		
		/// Returns tab width in pixels.
		int GetTabWidth() const {
			return tabwidth;
		}

		/// Changes the additional space between paragraphs. A paragraph is stared by a manual
		/// line break.
		void SetParagraphSpacing(float value) {
			pspace = value;
		}
		
		/// Get the space between paragraphs.
		float GetParagraphSpacing() const {
			return pspace;
		}

		virtual Geometry::Size GetSize(const std::string &text) const override { return {0,0}; }

		virtual Geometry::Size GetSize(const std::string &text, float width) const override { return{0,0}; }

		virtual int GetCharacterIndex(const std::string &text, Geometry::Pointf location) const override { return 0; }

		virtual Geometry::Pointf GetPosition(const std::string &text, int index) const override { return{0.f, 0.f}; }

		virtual int GetCharacterIndex(const std::string &text, float w, Geometry::Pointf location) const override { return 0; }

		virtual Geometry::Pointf GetPosition(const std::string &text, float w, int index) const override { return{0.f, 0.f}; }

	protected:
		virtual void print(TextureTarget &target, const std::string &text, Geometry::Pointf location) const override;

		virtual void print(TextureTarget &target, const std::string &text, Geometry::Rectanglef location) const override { }

		virtual void print(TextureTarget &target, const std::string &text, 
						   Geometry::Rectanglef location, TextAlignment align_override) const override { }


	private:
		void print(TextureTarget &target, const std::string &text, Geometry::Pointf location, RGBAf color) const;

		GlyphRenderer *renderer = nullptr;

		RGBAf color = 1.f;
		TextShadow shadow = {};
		bool underline = false;
		bool strikethrough = false;
		TextAlignment defaultalign = TextAlignment::Left;
		bool justify = false;
		float vspace = 0;
		float hspace = 0;
		float pspace = 0;
		int   tabwidth = 0;
	};
    
} }
