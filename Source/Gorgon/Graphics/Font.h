#pragma once

#include <stdint.h>
#include <map>
#include <limits.h>

#include "../Types.h"
#include "../Geometry/Point.h"
#include "../Geometry/Size.h"
#include "TextureTargets.h"
#include "Color.h"

namespace Gorgon { namespace Graphics {
    /// Glyph is a symbol for a character. In Gorgon, glyphs are UTF32 chars.
    using Glyph = Gorgon::Char;
    
    /// This class represents a range of glyphs. Both start and end is included.
    class GlyphRange {
    public:        
        /// Creates a range that includes a single item. Value 0 is not a valid
        /// code point 
        /*implicit*/ GlyphRange(Glyph value = 0xFFFF) : Start(value), End(value) { }
        
        GlyphRange(Glyph start, Glyph end) : Start(start), End(end) { }
        
        /// Returns the number of the glyphs in the range
        int Count() const { 
            if(Start == -1) return 0; 
            
            return End - Start + 1; 
        }
        
        void Normalize() {
            if(Start > End)
                std::swap(Start, End);
        }
        
        /// Start point of the range
        Glyph Start;
        
        /// End point of the range. This value is included in the range
        Glyph End;
    };
    
    /// Functions inside this namespace is designed for internal use, however, they might be used
    /// externally and will not have any impact on inner workings of the system.
    namespace internal {
        /// Decodes a utf-8 character from the given iterator. If char is not valid 
        /// 0xfffd is returned. \\r\\n is mapped to \\n
        Glyph decode(std::string::const_iterator &it, std::string::const_iterator end);
        bool isnewline(Glyph g);
        bool isspaced(Glyph g);
        bool isspace(Glyph g);
        bool isadjustablespace(Glyph g);
        bool isbreaking(Glyph g);

        inline int ceildiv(int v, float f) { return (int)std::ceil(v/f); }
        inline int rounddiv(int v, float f) { return (int)std::round(v/f); }

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
        
        /// This function should return the number of pixels the cursor should advance after this
        /// glyph. This value will be added to kerning distance.
        virtual float GetCursorAdvance(Glyph g) const = 0;

        /// Returns true if the glyph exists
        virtual bool Exists(Glyph g) const = 0;

        /// This function should return true if this font renderer supports only 7-bit ASCII
        virtual bool IsASCII() const = 0;

        /// This function should return true if this font is fixed width. This will suppress calls
        /// to GetSize function.
        virtual bool IsFixedWidth() const = 0;
        
        /// This function should return the additional distance between given glyphs. Returned value
        /// could be (in most cases it is) negative. Left and right are visual locations, they will
        /// not be reverted for right to left rendering.
        virtual Geometry::Pointf KerningDistance(Glyph left, Glyph right) const = 0;
        
        /// Returns the size of the EM dash
        virtual int GetEMSize() const = 0;
        
        /// Returns the width of widest glyph.
        virtual int GetMaxWidth() const = 0;

        /// Height of glyphs, actual size could be smaller but all glyphs should have the same virtual
        /// height. When drawn on the same y position, all glyphs should line up. Renderer can change
        /// actual draw location to compensate.
        virtual int GetHeight() const = 0;

        /// Returns the offset (first) and maximum height (second) that is used by letters. Offset 
        /// is the distance of the letter with max height to the top. This function uses Â and j
        /// to calculate letter height when ascii only is not set. If ascii only is set, it uses
        /// A, f, j. If Â is not found, this function simply reverts to using A.
        virtual std::pair<int, int> GetLetterHeight(bool asciionly = false) const = 0;

        /// Width of a digit, if digits do not have the same width, maximum should be returned. For
        /// practical reasons, this function is expected to consider arabic numerals.
        virtual int GetDigitWidth() const = 0;
        
        /// Baseline point of glyphs from the top.
        virtual float GetBaseLine() const = 0;
        
        /// This is the default distance between two consecutive lines. This distance can be modified
        /// by text renderers
        virtual float GetLineGap() const = 0;

        /// Should return the average thickness of a line. This information can be used to construct
        /// underline and strike through.
        virtual float GetLineThickness() const = 0;

        /// The position of the underline, if it is to be drawn.
        virtual int GetUnderlineOffset() const = 0;
        
        /// Should return if the glyph renderer requires preparation regarding the text given.
        virtual bool NeedsPrepare() const { return false; }
        
        /// Notifies glyph renderer about a text to be rendered. If renderers require modification
        /// to their internal structures, they should mark them 
        virtual void Prepare(const std::string &) const { }
    };

    /**
    * This class allows printing text on the screen. All fonts should support basic left aligned
    * print, aligned rectangular area printing. Additionally, all fonts should support basic info
    * functions. TextRenderers must be accept utf-8. internal::decode function should be preferred
    * to decode utf-8, among regular utf-8 decoding, this function will map \r\n to \n.
    */
    class TextRenderer {
    public:
        virtual ~TextRenderer() { }
        
        /// Prints the given text to the target. y coordinate is the top if the text. However, depending
        /// on the font, this value might exclude uppercase accents.
        void Print(TextureTarget &target, const std::string &text, Geometry::Pointf location) const {
            print(target, text, location);
        }
        
        void Print(TextureTarget &target, const std::string &text, Geometry::Point location) const {
            print(target, text, location);
        }
        
        void Print(TextureTarget &target, const std::string &text, int x, int y) const {
            print(target, text, {x, y});
        }
        
        void Print(TextureTarget &target, const std::string &text, Geometry::Point location, int w, TextAlignment align_override) const {
            print(target, text, {location, w, 0}, align_override);
        }
        
        void Print(TextureTarget &target, const std::string &text, Geometry::Point location, int w) const {
            print(target, text, {location, w, 0});
        }
                
        void Print(TextureTarget &target, const std::string &text, int x, int y, int w, TextAlignment align_override) const {
            print(target, text, {x, y, w, 0}, align_override);
        }
        
        void Print(TextureTarget &target, const std::string &text, int x, int y, int w) const {
            print(target, text, {x, y, w, 0});
        }

        void PrintNoWrap(TextureTarget &target, const std::string &text, Geometry::Point location, int w, TextAlignment align_override) const {
            printnowrap(target, text, {location, w, 0}, align_override);
        }
        
        void PrintNoWrap(TextureTarget &target, const std::string &text, Geometry::Point location, int w) const {
            printnowrap(target, text, {location, w, 0});
        }
                
        void PrintNoWrap(TextureTarget &target, const std::string &text, int x, int y, int w, TextAlignment align_override) const {
            printnowrap(target, text, {x, y, w, 0}, align_override);
        }
        
        void PrintNoWrap(TextureTarget &target, const std::string &text, int x, int y, int w) const {
            printnowrap(target, text, {x, y, w, 0});
        }

        void Print(TextureTarget &target, const std::string &text) {
            print(target, text, {0, 0, target.GetTargetSize()});
        }
        
        /// Whether the render can render text
        virtual bool IsReady() const = 0;

        /// Returns the glyphrenderer that is used by this text renderer. It might be the text renderer itself. It is only safe
        /// to call this function if IsReady function has returned true.
        virtual const GlyphRenderer &GetGlyphRenderer() const = 0;
        
        /// Returns the size of the EM dash
        virtual int GetEMSize() const = 0;
        
        /// Get the distance of baseline from the top of the text
        virtual float GetBaseLine() const = 0;
        
        /// Get the distance of baseline from the top of the text
        virtual int GetHeight() const = 0;
        
        /// Returns the size of the given text
        virtual Geometry::Size GetSize(const std::string &text) const = 0;
        
        /// Returns the size of the given text
        virtual Geometry::Size GetSize(const std::string &text, int width) const = 0;

        /// Returns the character index of glyph immediately after the given location. This function is Unicode aware.
        virtual int GetCharacterIndex(const std::string &text, Geometry::Point location) const = 0;
        
        /// Returns the character index of glyph immediately after the given location. This function is Unicode aware.
        virtual int GetCharacterIndex(const std::string &text, int w, Geometry::Point location, bool wrap = true) const = 0;
        
        /// Returns the position of the glyph at the character index. If the character is not found, this will return
        /// std::numeric_limit<int>::min for x and y position. Size could be 0 if it cannot be determined.
        virtual Geometry::Rectangle GetPosition(const std::string &text, int index) const = 0;
        
        /// Returns the position of the glyph at the character index. If the character is not found, this will return
        /// std::numeric_limit<int>::min for x and y position. Size could be 0 if it cannot be determined.
        virtual Geometry::Rectangle GetPosition(const std::string &text, int w, int index, bool wrap = true) const = 0;

    protected:
        virtual void print(TextureTarget &target, const std::string &text, Geometry::Point location) const = 0;
        
        /// Should print the given text to the specified location and color. Width should be used to 
        /// align the text. Unless width is 0, text should be wrapped. Even if width is 0, the alignment
        /// should be respected. For instance if width is 0 and align is right, text should end at
        /// the given location. Height of the rectangle can be left 0, thus unless explicitly requested,
        /// it should be ignored.
        virtual void print(TextureTarget &target, const std::string &text,
                        Geometry::Rectangle location, TextAlignment align_override) const = 0;

        virtual void print(TextureTarget &target, const std::string &text,
                        Geometry::Rectangle location) const = 0;

        /// Should print the given text to the specified location and color. Width should be used to 
        /// align the text. Automatic wrapping should not be used.
        virtual void printnowrap(TextureTarget &target, const std::string &text,
                        Geometry::Rectangle location) const = 0;

        /// Should print the given text to the specified location and color. Width should be used to 
        /// align the text. Automatic wrapping should not be used.
        virtual void printnowrap(TextureTarget &target, const std::string &text,
                        Geometry::Rectangle location, TextAlignment align_override) const = 0;
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


        void Print(TextureTarget &target, const std::string &text, RGBAf color) const {
            print(target, text, {0, 0, target.GetTargetSize()}, color);
        }

        void Print(TextureTarget &target, const std::string &text, Geometry::Point location, RGBAf color) const {
            print(target, text, location, color);
        }

        void Print(TextureTarget &target, const std::string &text, int x, int y, RGBAf color) const {
            print(target, text, {x, y}, color);
        }
        
        void Print(TextureTarget &target, const std::string &text, Geometry::Point location, int w, TextAlignment align_override, RGBAf color) const {
            print(target, text, {location, w, 0}, align_override, color);
        }

        void Print(TextureTarget &target, const std::string &text, Geometry::Point location, int w, RGBAf color) const {
            print(target, text, {location, w, 0}, color);
        }
        
        void Print(TextureTarget &target, const std::string &text, int x, int y, int w, TextAlignment align_override, RGBAf color) const {
            print(target, text, {x, y, w, 0}, align_override, color);
        }

        void Print(TextureTarget &target, const std::string &text, int x, int y, int w, RGBAf color) const {
            print(target, text, {x, y, w, 0}, color);
        }

        void PrintNoWrap(TextureTarget &target, const std::string &text, Geometry::Point location, int w, TextAlignment align_override, RGBAf color) const {
            printnowrap(target, text, {location, w, 0}, align_override, color);
        }

        void PrintNoWrap(TextureTarget &target, const std::string &text, Geometry::Point location, int w, RGBAf color) const {
            printnowrap(target, text, {location, w, 0}, color);
        }
        
        void PrintNoWrap(TextureTarget &target, const std::string &text, int x, int y, int w, TextAlignment align_override, RGBAf color) const {
            printnowrap(target, text, {x, y, w, 0}, align_override, color);
        }

        void PrintNoWrap(TextureTarget &target, const std::string &text, int x, int y, int w, RGBAf color) const {
            printnowrap(target, text, {x, y, w, 0}, color);
        }

        /// Changes the default alignment. It is possible to override default alignment through TextRenderer interface.
        void SetDefaultAlignment(TextAlignment value) {
            defaultalign = value;
        }

        /// Returns the current default alignment.
        TextAlignment GetDefaultAlignment() const {
            return defaultalign;
        }

        /// Changes the the color of the text. Color can only be overridden through BasicFont interface.
        void SetColor(RGBAf value) {
            color = value;
        }

        /// Returns the current text color
        RGBAf GetColor() const {
            return color;
        }
        
        virtual bool IsReady() const override {
            return renderer != nullptr;
        }
        
        virtual const GlyphRenderer &GetGlyphRenderer() const override {
            return *renderer;
        }
        
        virtual int GetEMSize() const override {
            return renderer->GetEMSize();
        }
        
        virtual float GetBaseLine() const override {
            return renderer->GetBaseLine();
        }
        
        virtual int GetHeight() const override {
            return renderer->GetHeight();
        }
        
        virtual Geometry::Size GetSize(const std::string &text) const override;
        
        virtual Geometry::Size GetSize(const std::string &text, int width) const override;
        
        virtual int GetCharacterIndex(const std::string &text, Geometry::Point location) const override;
        
        virtual int GetCharacterIndex(const std::string &text, int w, Geometry::Point location, bool wrap = true) const override;
        
        virtual Geometry::Rectangle GetPosition(const std::string& text, int index) const override;
        
        virtual Geometry::Rectangle GetPosition(const std::string& text, int w, int index, bool wrap = true) const override;
        
    protected:
        virtual void print(TextureTarget &target, const std::string &text, Geometry::Point location) const override {
            print(target, text, location, color);
        }

        virtual void print(TextureTarget &target, const std::string &text,
                        Geometry::Rectangle location, TextAlignment align) const override {
            print(target, text, location, align, color);
        }

        virtual void print(TextureTarget &target, const std::string &text, Geometry::Point location, RGBAf color) const;

        virtual void print(TextureTarget &target, const std::string &text,
                        Geometry::Rectangle location, TextAlignment align, RGBAf color) const;

        virtual void printnowrap(TextureTarget &target, const std::string &text,
                        Geometry::Rectangle location, TextAlignment align, RGBAf color) const;

        virtual void printnowrap(TextureTarget &target, const std::string &text,
                        Geometry::Rectangle location, TextAlignment align) const override {
            printnowrap(target, text, location, align, color);
        }

        virtual void print(TextureTarget &target, const std::string &text,
                        Geometry::Rectangle location) const override {
            print(target, text, location, defaultalign);
        }

        virtual void print(TextureTarget &target, const std::string &text,
                        Geometry::Rectangle location, RGBAf color) const {
            print(target, text, location, defaultalign, color);
        }

        virtual void printnowrap(TextureTarget &target, const std::string &text,
                        Geometry::Rectangle location) const override {
            printnowrap(target, text, location, defaultalign);
        }

        virtual void printnowrap(TextureTarget &target, const std::string &text,
                        Geometry::Rectangle location, RGBAf color) const {
            printnowrap(target, text, location, defaultalign, color);
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

        StyledRenderer() = default;

        GlyphRenderer &GetGlyphRenderer() {
            return *renderer;
        }
        
        void SetGlyphRenderer(GlyphRenderer &renderer) {
            this->renderer = &renderer;
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

        /// Disables text shadow
        void DisableShadow() {
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

        /// Underlines the text
        void Underline() {
            underline = true;
        }

        /// Underlines the text with the given color
        void Underline(RGBAf color) {
            underline = true;
            underlinecolor = color;
        }

        /// Returns whether the text is underlined
        bool GetUnderline() const {
            return underline;
        }

        /// Changes the underline color of the text. By default underline color
        /// will be the same as text color. To get default value, use
        /// ResetUnderlineColor function
        void SetUnderlineColor(RGBAf value) {
            underlinecolor = value;
        }

        /// Returns the current underline color.
        RGBAf GetUnderlineColor() const {
            if(underlinecolor.R == -1)
                return color;
            else
                return underlinecolor;
        }

        /// Sets underline color to match with text color
        void ResetUnderlineColor() {
            underlinecolor = -1.f;
        }

        /// Sets whether the text would be stroked
        void SetStrike(bool value) {
            strike = value;
        }

        /// Strikes the text
        void Strike() {
            strike = true;
        }

        /// Strikes the text with the given color
        void Strike(RGBAf color) {
            strike = true;
            strikecolor = color;
        }

        /// Returns whether the text would stroked
        bool GetStrike() const {
            return strike;
        }

        /// Changes the strike color of the text. By default strike color
        /// will be the same as text color. To get default value, use
        /// ResetStrikeColor function
        void SetStrikeColor(RGBAf value) {
            strikecolor = value;
        }
        
        /// Returns the current strike color.
        RGBAf GetStrikeColor() const {
            if(strikecolor.R == -1)
                return color;
            else
                return strikecolor;
        }

        /// Sets strike color to match with text color
        void ResetStrikeColor() {
            strikecolor = -1.f;
        }

        /// Changes the strike position to the given value. Default value for strike
        /// position is automatically calculated, use ResetStrikePosition to get
        /// back to the default
        void SetStrikePosition(int value) {
            strikepos = value;
        }
        
        /// Returns current strike position
        int GetStrikePosition() const {
            if(strikepos == INT_MIN)
                return (int)std::round( (renderer->GetHeight() - renderer->GetLineThickness()) * .6f );
            else
                return strikepos;
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
            justify = true;
        }

        /// Aligns the text to the right, sets justify
        void JustifyRight() {
            defaultalign  = TextAlignment::Right;
            justify = true;
        }

        /// Aligns the text to the center, sets justify
        void JustifyCenter() {
            defaultalign = TextAlignment::Center;
            justify = true;
        }
        
        /// Returns whether the text would be justified
        bool GetJustify() const {
            return justify;
        }

        /// Sets the line spacing in pixels, this spacing is the space between
        /// two lines, from the descender of the first line to the ascender of
        /// the second.
        void SetLineSpacingPixels(int value) {
            vspace = (float)value / renderer->GetHeight();
        }
        
        /// Returns the line spacing in pixels
        int GetLineSpacingPixels() const {
            return (int)std::round(vspace * renderer->GetHeight());
        }

        /// Sets the line spacing as percentage of line gap. A value of 
        /// one will use the default state by the font, where as a value of two
        /// will leave a large gap between two lines. This will round the final
        /// result to the nearest pixel. 
        void SetLineSpacing(float value) {
            vspace = value;
        }
        
        /// Returns the line spacing as percentage of line gap
        float GetLineSpacing() const {
            return vspace;
        }

        /// Spacing between letters of the text, in pixels. This is in addition to
        /// the regular character spacing.
        void SetLetterSpacing(int value) {
            hspace = value;
        }
        
        /// Returns the spacing between the letters in pixels
        int GetLetterSpacing() const {
            return hspace;
        }

        /// Distance between tab stops. This value is in pixels. Default value is
        /// 8 * A width. Tabbing is only fully effective when text is
        /// left aligned.
        void SetTabWidth(int value) {
            tabwidth = value;
        }

        /// Sets the tab width in digit widths.
        void SetTabWidthInLetters(float value) {
            tabwidth = (int)std::round(value * renderer->GetCursorAdvance('A'));
        }
        
        /// Returns tab width in pixels.
        int GetTabWidth() const {
            return tabwidth;
        }

        /// Changes the additional space between paragraphs. A paragraph is stared by a manual
        /// line break. This distance is in pixels.
        void SetParagraphSpacing(int value) {
            pspace = value;
        }
        
        /// Get the space between paragraphs in pixels.
        int GetParagraphSpacing() const {
            return pspace;
        }
        
        virtual bool IsReady() const override {
            return renderer != nullptr;
        }
        
        virtual int GetEMSize() const override {
            return renderer->GetEMSize();
        }
        
        virtual float GetBaseLine() const override {
            return renderer->GetBaseLine();
        }
        
        virtual int GetHeight() const override {
            return renderer->GetHeight();
        }
        
        virtual const GlyphRenderer &GetGlyphRenderer() const override {
            return *renderer;
        }

        virtual Geometry::Size GetSize(const std::string &text) const override;

        virtual Geometry::Size GetSize(const std::string &text, int width) const override;

        virtual int GetCharacterIndex(const std::string &text, Geometry::Point location) const override;

        virtual Geometry::Rectangle GetPosition(const std::string& text, int index) const override;

        virtual int GetCharacterIndex(const std::string &text, int w, Geometry::Point location, bool wrap = true) const override;

        virtual Geometry::Rectangle GetPosition(const std::string &text, int w, int index, bool wrap = true) const override;

    protected:
        virtual void print(TextureTarget &target, const std::string &text, Geometry::Point location) const override;

        virtual void print(TextureTarget &target, const std::string &text, Geometry::Rectangle location) const override {
            print(target, text, location, defaultalign);
        }

        virtual void printnowrap(TextureTarget &target, const std::string &text,
                        Geometry::Rectangle location, TextAlignment align) const override;

        virtual void print(TextureTarget &target, const std::string &text, 
                        Geometry::Rectangle location, TextAlignment align_override) const override;



        virtual void printnowrap(TextureTarget &target, const std::string &text,
                        Geometry::Rectangle location) const override {
            printnowrap(target, text, location, defaultalign);
        }

    private:
        //internal, float to facilitate shadow offset
        void print(TextureTarget &target, const std::string &text, Geometry::Pointf location, RGBAf color, RGBAf strikecolor, RGBAf underlinecolor) const;

        void print(TextureTarget &target, const std::string &text, Geometry::Rectanglef location, TextAlignment align, RGBAf color, RGBAf strikecolor, RGBAf underlinecolor) const;

        GlyphRenderer *renderer = nullptr;

        RGBAf color = 1.f;

        TextShadow shadow = {};
        bool underline = false;
        RGBAf underlinecolor = -1.f;
        bool strike = false;
        RGBAf strikecolor = -1.f;
        int strikepos = INT_MIN;
        TextAlignment defaultalign = TextAlignment::Left;
        bool  justify = false;
        float vspace = 1.0f;
        int   hspace = 0;
        int   pspace = 0;
        int   tabwidth = 0;
    };
    
} }
