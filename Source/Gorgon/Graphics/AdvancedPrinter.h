#pragma once

#include "Font.h"
#include "Color.h"
#include "../Graphics.h"
#include "../Geometry/Margin.h"
#include "../Containers/Hashmap.h"

#include <string>

#include "../String.h"

namespace Gorgon { namespace Graphics {
            
    /// Constants for colors. Color indices are 7-bit integers 0-127. First few are named as
    /// follows. Each named color has forecolor and backcolor pairs.
    enum class NamedFontColors {
        Regular,
        Title,
        Emphasis,
        Info,
        Inverted,
        Code,
        Keyword,
        Comment,
    };
    
    /// Constants for headings
    enum class HeaderLevel {
        H1 = 1,
        H2,
        H3,
        H4
    };
            
    /// Constants for fonts. Font indices are 7-bit integers 0-127. First few are named as
    /// follows.
    enum class NamedFonts {
        /// Default font
        Normal,
        
        /// Default font
        Regular = Normal,
        
        /// Bold font
        Bold,
        
        /// First level heading
        H1,
        
        /// Second level heading
        H2,
        
        /// Third level heading
        H3,
        
        /// Fourth level heading
        H4,
        
        /// Italic font
        Italic,
        
        /// Smaller font, usually 75% of full size
        Small,
        
        /// Bold and italic font
        BoldItalic,
        
        /// Font style used to display information, usually smaller and uses different colors.
        Info,
        
        /// A large font, usually 125% of the original size
        Larger,
        
        /// Small font that will be used in super and subscripts. Could also be used for other purposes.
        Script,
        
        /// Small font that will be used in super and subscripts for fonts that is known to be bold.
        /// Could also be used for other purposes.
        BoldScript,
        
        /// Small font that will be used in super and subscripts. Could also be used for other purposes.
        /// This script font will be used for small, info and script fonts
        SmallScript
    };
        

    /**
     * This class helps building strings to be used with AdvancedRenderer. Advanced rendering is
     * capable of modifying fonts, offsets, displaying images, handing regions, selection, and 
     * tables. You should prefer using this class instead of fixed unicode points as the advanced
     * renderer notation can change between revisions. For external storage, we recommend using
     * markdown. This class uses fluent interface. All integers are 16 bit integers including the
     * integers in points and sizes.
     */
    class AdvancedTextBuilder {
    public:
        /// Defines how an image will be aligned
        enum ImageAlign {
            Inline,
            Left,
            Right
        };
        
        /// Defines a table column. If both pixel width and relative width are 0, the column is auto
        /// sized.
        class TableColumn {
        public:
            /// Alignment of text in the column
            TextAlignment Align = TextAlignment::Left;
            /// Width in pixels
            int Width = 0;
            /// Width in percentage
            int RelWidth = 0;
        };
        
        /// Constructor, optionally initial string can be supplied.
        AdvancedTextBuilder(const std::string &text = "") : text(text)
        { }
        
        /// Appends the given data to the builder
        template<class T_>
        AdvancedTextBuilder &operator += (const T_ &v) {
            text += String::From(v);
            
            return *this;
        }
        
        /// Appends the given data to the builder
        template<class T_>
        AdvancedTextBuilder &Append(const T_ &v) {
            text += String::From(v);
            
            return *this;
        }
        
        /// Returns the string built by this builder.
        operator std::string&() {
            return text;
        }
        
        /// Returns the string built by this builder.
        operator const std::string&() const {
            return text;
        }
        
        /// Returns the string built by this builder. You may change it.
        std::string &Get() {
            return text;
        }
        
        /// Returns the string built by this builder.
        const std::string &Get() const {
            return text;
        }
        
        /// Appends a line break that does not start a new paragraph.
        AdvancedTextBuilder &LineBreak() { return C2(0x85); }
        
        /// Resets all formatting instructions.
        AdvancedTextBuilder &ResetFormatting() { return SCI(0x04); }
        
        /// @name Font style switching
        /// Use to change the current font. Bold and italic are exclusive and may not be available.
        /// Unless overridden, all styles can modify color, underline, strikethrough and paragraph
        /// and line spacing.
        /// @{ BEGIN
        
        AdvancedTextBuilder &UseDefaultFont() { return C1(0xf); }
        
        AdvancedTextBuilder &UseBoldFont() { return C1(0xe); }
        
        AdvancedTextBuilder &UseItalicFont() { return C2(0x91); }
        
        AdvancedTextBuilder &UseBoldItalicFont() { return SetFont(8); }
        
        AdvancedTextBuilder &UseSmallFont() { return C2(0x92); }
        
        AdvancedTextBuilder &UseInfoFont() { return SetFont(9); }
        
        AdvancedTextBuilder &UseHeader(HeaderLevel level) { return C1(0x10 + char(level)); }
        
        /// Switch to superscript, use ScriptOff to switch off
        AdvancedTextBuilder &UseSuperscript() { return C1(5); }
        
        /// Switch to subscript, use ScriptOff to switch off
        AdvancedTextBuilder &UseSubscript() { return C1(6); }
        
        /// Switches sub and superscript off
        AdvancedTextBuilder &ScriptOff() { return C1(7); }

        /// Switches to the given font index. If it doesn't exist, default font will be used.
        AdvancedTextBuilder &SetFont(Byte fontindex) { CSI(0x15); Index(fontindex); return ST(); }
        /// END @}
        
        
        /// @name Color control
        /// These functions control the color of different parts of the system. Default color for
        /// text and border is set by font style. Default color for background is transparent.
        /// @{ BEGIN
        
        AdvancedTextBuilder &UseDefaultColor() { CSI(0x01); return ST(); }
        
        /// Sets the forecolor to the given 7-bit index. 
        AdvancedTextBuilder &SetColor(Byte index, Byte alpha = 255) { CSI(0x01); Index(index); if(alpha != 255) Byte(alpha); return ST(); }
        
        /// Sets the forecolor to the given index name. 
        AdvancedTextBuilder &SetColor(NamedFontColors index, Byte alpha = 255) { return SetColor(Gorgon::Byte(index), alpha); }
        
        /// Sets the forecolor to the given color. If precise is not set, 7-bits per channel will be
        /// used.
        AdvancedTextBuilder &SetColor(RGBA color, bool precise = false) { CSI(0x02); Color(color, precise); return ST(); }
        
        /// Removes tint color that is used for images.
        AdvancedTextBuilder &RemoveTint() { CSI(0x18); return ST(); }
        
        /// Sets the tint color that is used for images to the given 7-bit index. 
        AdvancedTextBuilder &SetTint(Byte index, Byte alpha = 255) { CSI(0x18); Index(index); if(alpha != 255) Byte(alpha); return ST(); }
        
        /// Sets the tint color that is used for images to the given index name. 
        AdvancedTextBuilder &SetTint(NamedFontColors index, Byte alpha = 255) { return SetTint(Gorgon::Byte(index), alpha); }
        
        /// Sets the tint color that is used for images to the given color. If precise is not set, 
        /// 7-bits per channel will be used.
        AdvancedTextBuilder &SetTint(RGBA color, bool precise = false) { CSI(0x18); Color(color, precise); return ST(); }
        
        /// Sets the alpha that is used for images to the given color. If precise is not set, 
        /// 7-bits per channel will be used.
        AdvancedTextBuilder &SetAlpha(Byte alpha, bool precise = false) { return SetTint(RGBA{255, 255, 255, alpha}, precise); }
        
        /// Sets the background color to the given 7-bit index. 
        AdvancedTextBuilder &SetBackgroundColor(Byte index, Byte alpha = 255) { CSI(0x03); Index(index); if(alpha != 255) Byte(alpha); return ST(); }
        
        /// Sets the background color to the given index name. 
        AdvancedTextBuilder &SetBackgroundColor(NamedFontColors index, Byte alpha = 255) { return SetBackgroundColor(Gorgon::Byte(index), alpha); }
        
        /// Sets the background color to the given color. If precise is not set, 7-bits per channel 
        /// will be used.
        AdvancedTextBuilder &SetBackgroundColor(RGBA color, bool precise = false) { CSI(0x04); Color(color, precise); return ST(); }
        
        
        /// Sets the border color to the given 7-bit index. 
        AdvancedTextBuilder &SetBorderColor(Byte index, Byte alpha = 255) { CSI(0x06); Index(index); if(alpha != 255) Byte(alpha); return ST(); }
        
        /// Sets the border color to the given index name. 
        AdvancedTextBuilder &SetBorderColor(NamedFontColors index, Byte alpha = 255) { return SetBorderColor(Gorgon::Byte(index), alpha); }
        
        /// Sets the border color to the given color. If precise is not set, 7-bits per channel 
        /// will be used.
        AdvancedTextBuilder &SetBorderColor(RGBA color, bool precise = false) { CSI(0x07); Color(color, precise); return ST(); }
        
        /// END @}
        
        
        /// @name Alignment
        /// These functions modify alignment of text and images. Default horizontal alignment is
        /// dictated by the font style. Default vertical alignment is baseline.
        /// @{ BEGIN
        
        /// Disables horizontal alignment override.
        AdvancedTextBuilder &UseDefaultHorizontalAlignment() { return SCI(0x29); }
        
        /// Aligns text to left. Disables justify.
        AdvancedTextBuilder &AlignLeft() { SCI(0x21); return SCI(0x22); }
        
        /// Aligns text to center. Disables justify.
        AdvancedTextBuilder &AlignCenter() { SCI(0x21); return SCI(0x24); }
        
        /// Aligns text to right. Disables justify.
        AdvancedTextBuilder &AlignRight() { SCI(0x21); return SCI(0x23); }
        
        /// Aligns text to left. Enables justify.
        AdvancedTextBuilder &JustifyLeft() { SCI(0x20); return SCI(0x22); }
        
        /// Aligns text to center. Enables justify.
        AdvancedTextBuilder &JustifyCenter() { SCI(0x20); return SCI(0x24); }
        
        /// Aligns text to right. Enables justify.
        AdvancedTextBuilder &JustifyRight() { SCI(0x20); return SCI(0x23); }
        
        /// Sets the text alignment without changing justify.
        AdvancedTextBuilder &SetAlignment(TextAlignment align) {
            switch(align) {
            case TextAlignment::Left: return SCI(0x22);
            case TextAlignment::Right: return SCI(0x23);
            case TextAlignment::Center: return SCI(0x24);
            }
            return *this;
        }
        
        /// Turns justify on/off
        AdvancedTextBuilder &Justify(bool enable = true) { return SCI(enable ? 0x20 : 0x21); }
        
        /// Modifies vertical alignment
        AdvancedTextBuilder &AlignToTop() { return SCI(0x25); }
        
        /// Modifies vertical alignment
        AdvancedTextBuilder &AlignToMiddle() { return SCI(0x26); }
        
        /// Modifies vertical alignment
        AdvancedTextBuilder &AlignToBottom() { return SCI(0x27); }
        
        /// Modifies vertical alignment. This is the default alignment.
        AdvancedTextBuilder &AlignToBaseline() { return SCI(0x28); }
        
        /// END @}
        
        
        /// @name Marking
        /// @{ BEGIN
        
        AdvancedTextBuilder &Underline(bool enable = true) { return SCI(enable ? 0x10 : 0x11); }
        
        AdvancedTextBuilder &Strikethrough(bool enable = true) { return SCI(enable ? 0x12 : 0x13); }
        
        /// END @}
        
        
        /// @name Positioning
        /// @{ BEGIN
        
        AdvancedTextBuilder &WordWrap(bool enable) { return SCI(enable ? 0x30 : 0x31); }
        
        /// Sets the width that the words will wrap from. Pixel and char widths will be added
        /// together. For char width letter A is often used.
        AdvancedTextBuilder &SetWrapWidth(short pixels, short rel) { CSI(0x0e); ValAndRel(pixels, rel); return ST(); }
        
        AdvancedTextBuilder &DefaultWrapWidth() { CSI(0x0e); return ST(); }
        
        /// Changes the offset that will be added to each letter. Relative sizing is relative to
        /// character width and line height, the value is in percentage.
        AdvancedTextBuilder &SetLetterOffset(const Geometry::Point &pixels, const Geometry::Point &rel) { 
            CSI(0x14); 
            ValAndRel(pixels.X, rel.X); GS(); 
            ValAndRel(pixels.Y, rel.Y);
            return ST(); 
        }

        AdvancedTextBuilder &DefaultLetterOffset() { CSI(0x14); return ST(); }
        
        /// Changes the offset of underline. rel is relative to line height and in percentage.
        AdvancedTextBuilder &SetUnderlineOffset(short pixels, short rel) { CSI(0x12); ValAndRel(pixels, rel); return ST(); }
        
        AdvancedTextBuilder &DefaultUnderlineOffset() { CSI(0x12); return ST(); }
        
        /// Changes the offset of strike. rel is relative to line height and in percentage.
        AdvancedTextBuilder &SetStrikethroughOffset(short pixels, short rel) { CSI(0x13); ValAndRel(pixels, rel); return ST(); }
        
        AdvancedTextBuilder &DefaultStrikethroughOffset() { CSI(0x13); return ST(); }
        
        /// Add letters that will be used to break text from. Useful for code views.
        AdvancedTextBuilder &AddBreakingLetters(std::vector<Char> letters) {
            CSI(0x23);
            for(auto c : letters) String::AppendUnicode(text, c);
            return ST();
        }
        
        /// Removes letters that will be used to break text from. Useful for code views.
        AdvancedTextBuilder &RemoveBreakingLetters(std::vector<Char> letters) {
            CSI(0x24);
            for(auto c : letters) String::AppendUnicode(text, c);
            return ST();
        }
        
        /// END @}
        
        
        /// @name Spacing
        /// @{ BEGIN
        
        /// Changes the spacing between paragraphs. rel is relative to line height and in percentage.
        AdvancedTextBuilder &SetParagraphSpacing(short pixels, short rel = 0) { CSI(0x09); ValAndRel(pixels, rel); return ST(); }
        
        AdvancedTextBuilder &DefaultParagraphSpacing() { CSI(0x09); return ST(); }
        
        /// Changes the spacing between lines. rel is relative to line height and in percentage.
        AdvancedTextBuilder &SetLineSpacing(short pixels, short rel = 0) { CSI(0x0d); ValAndRel(pixels, rel); return ST(); }
        
        AdvancedTextBuilder &DefaultLineSpacing() { CSI(0x0d); return ST(); }
        
        /// Changes the spacing between the letters. rel is relative to em width and in 
        /// percentage.
        AdvancedTextBuilder &SetLetterSpacing(short pixels, short rel = 0) { CSI(0x0c); ValAndRel(pixels, rel); return ST(); }
        
        AdvancedTextBuilder &DefaultLetterSpacing() { CSI(0x0c); return ST(); }
        
        /// Changes the indent. rel is relative to em width and in percentage.
        AdvancedTextBuilder &SetIndent(short pixels, short rel = 0) { CSI(0x0a); ValAndRel(pixels, rel); return ST(); }
        
        AdvancedTextBuilder &RemoveIndent() { CSI(0x0a); return ST(); }
        
        /// Changes the indent of the start of paragraphs. rel is relative to em width and in 
        /// percentage.
        AdvancedTextBuilder &SetHangingIndent(short pixels, short rel) { CSI(0x0b); ValAndRel(pixels, rel); return ST(); }
        
        AdvancedTextBuilder &RemoveHangingIndent() { CSI(0x0b); return ST(); }

        
        /// Place the requested amount of space horizontally. rel is relative to em width and in 
        /// percentage.
        AdvancedTextBuilder &HorizontalSpace(short pixels, short rel = 0) { CSI(0x40); ValAndRel(pixels, rel); return ST(); }
        
        /// Place the requested amount of space vertically. rel is relative to line height and in 
        /// percentage.
        AdvancedTextBuilder &VerticalSpace(short pixels, short rel = 0) { CSI(0x41); ValAndRel(pixels, rel); return ST(); }
        
        /// Changes the spacing between the tab stops. rel is in space widths. per is percentage of
        /// wrap width
        AdvancedTextBuilder &SetTabWidth(short pixels, short rel = 0, short per = 0) { CSI(0x17); ValAndRel(pixels, rel); Int(per); return ST(); }
        
        /// Adds a tabstop. The tabstop with the given index will be located at the specified location.
        /// It replaces nearest tabstop. rel is in space widths.
        AdvancedTextBuilder &AddTabStop(Byte index, short pixels, short rel = 0, short per = 0) { CSI(0x25); Index(index); ValAndRel(pixels, rel); RS(); Int(per); return ST(); }
        
        /// Removes the tabstop at the given index.
        AdvancedTextBuilder &RemoveTabStop(Byte index) { CSI(0x26); Index(index); return ST(); }
        
        /// END @}
        
        
        /// @name Box
        /// Controls box features such as background and border. Default background color is 
        /// transparent, it must be changed if background is to be used. Default border thickness
        /// is set to underline thickness of the default font. Border and background are off by
        /// default. They will be turned on and used for tables.
        /// @{ BEGIN
        
        AdvancedTextBuilder &ShowBackground() { return SCI(0x00); }
        
        AdvancedTextBuilder &RemoveBackground() { return SCI(0x01); }
        
        AdvancedTextBuilder &ShowBorder() { return SCI(0x02); }
        
        AdvancedTextBuilder &RemoveBorder() { return SCI(0x03); }
        
        /// Sets the border thickness. rel is relative to underline thickness and is in percentage.
        /// Default thickness is pixels = 0, rel = 100
        AdvancedTextBuilder &SetBorderThickness(short pixels, short rel) { CSI(0x05); ValAndRel(pixels, rel); return ST(); }
        
        /// Sets the padding of the text from the border. rel is relative to underline thickness 
        /// and is in percentage. Default padding is pixels = 0, rel = 100
        AdvancedTextBuilder &SetPadding(short pixels, short rel) { CSI(0x08); ValAndRel(pixels, rel); return ST(); }
        
        /// Sets the padding of the text from the border. rel is relative to underline thickness 
        /// and is in percentage. Default padding is pixels = 0, rel = 100
        AdvancedTextBuilder &SetPadding(const Gorgon::Geometry::Margin &pixels, const Gorgon::Geometry::Margin rel) { 
            CSI(0x08); 
            ValAndRel(pixels.Left, rel.Left); GS();
            ValAndRel(pixels.Top, rel.Top); GS();
            ValAndRel(pixels.Right, rel.Right); GS();
            ValAndRel(pixels.Bottom, rel.Bottom);
            return ST(); 
        }
        
        /// Sets the padding of the selected text from the selected image border. This includes the width of the border.
        /// rel is relative to underline thickness and is in percentage. Default is pixels = 0, rel = 200.
        AdvancedTextBuilder &SetSelectionPadding(const Gorgon::Geometry::Margin &pixels, const Gorgon::Geometry::Margin rel) { 
            CSI(0x016); 
            ValAndRel(pixels.Left, rel.Left); GS();
            ValAndRel(pixels.Top, rel.Top); GS();
            ValAndRel(pixels.Right, rel.Right); GS();
            ValAndRel(pixels.Bottom, rel.Bottom);
            return ST(); 
        }
        
        /// END @}
        
        
        /// @name Image
        /// @{ BEGIN
        
        /// Displays the image with the given ID aIf the image is larger than the wrap width
        AdvancedTextBuilder &InlineImage(Byte index) { 
            CSI(0x10);
            Index(index);
            Byte(0);
            
            return ST();
        }
        
        /// Displays the image with the given ID and offset. If the image is larger than the wrap
        /// width, it will be shrunk.
        AdvancedTextBuilder &InlineImage(Byte index, Geometry::Point offset) { 
            CSI(0x10);
            Index(index);
            Byte(0);
            Int((short)offset.X);
            US();
            Int((short)offset.Y);
            RS();
            
            return ST();
        }
        
        /// Displays the image with the given ID and size. Image will be scale proportionally to
        /// this area. If any dimension is 0, it will be ignored. relsize is relative to wrap width
        /// and line height and is in percentage.
        AdvancedTextBuilder &InlineImage(Byte index, Geometry::Size pixelsize, Geometry::Size relsize, Geometry::Point offset = {0, 0}) {
            CSI(0x10);
            Index(index);
            Byte(0);
            if(offset != Geometry::Point{0, 0}) {
                Int((short)offset.X);
                US();
                Int((short)offset.Y);
            }
            RS();
            Int((short)pixelsize.Width);
            US();
            Int((short)pixelsize.Height);
            US();
            Int((short)relsize.Width);
            US();
            Int((short)relsize.Height);
            
            return ST();
        }

        
        /// Displays the image with the given ID, side and margin. If the image is larger than the wrap
        /// width, it will be shrunk.
        AdvancedTextBuilder &AlignedImage(Byte index, ImageAlign side, Geometry::Point offset = {0, 0}, Geometry::Margin margins = {0}) {
            CSI(0x10);
            Index(index);
            Byte(side);
            if(offset != Geometry::Point{0, 0}) {
                Int((short)offset.X);
                US();
                Int((short)offset.Y);
            }
            RS();
            RS();
            if(margins != Geometry::Margin{0}) {
                if(margins.Left == margins.Top == margins.Right == margins.Bottom) {
                    Int((short)margins.Left);
                }
                else {
                    Int((short)margins.Left);
                    US();
                    Int((short)margins.Top);
                    US();
                    Int((short)margins.Right);
                    US();
                    Int((short)margins.Bottom);
                }
            }
            
            return ST();
        }
        
        /// Displays the image with the given ID and offset. If the image is larger than the wrap
        /// width, it will be shrunk.
        AdvancedTextBuilder &AlignedImage(Byte index, ImageAlign side, Geometry::Margin margins) {
            return AlignedImage(index, side, Geometry::Point{0, 0}, margins);
        }
        
        
        /// Displays the image with the given ID, side and margin. If the image is larger than the wrap
        /// width, it will be shrunk. If any dimension of the size is zero, it will be ignored.
        AdvancedTextBuilder &AlignedImage(Byte index, ImageAlign side,  Geometry::Size pixelsize, Geometry::Size relsize, Geometry::Point offset = {0, 0}, Geometry::Margin margins = {0}) {
            CSI(0x10);
            Index(index);
            Byte(side);
            if(offset != Geometry::Point{0, 0}) {
                Int((short)offset.X);
                US();
                Int((short)offset.Y);
            }
            RS();
            Int((short)pixelsize.Width);
            US();
            Int((short)pixelsize.Height);
            US();
            Int((short)relsize.Width);
            US();
            Int((short)relsize.Height);
            RS();
            if(margins != Geometry::Margin{0}) {
                if(margins.Left == margins.Top == margins.Right == margins.Bottom) {
                    Int((short)margins.Left);
                }
                else {
                    Int((short)margins.Left);
                    US();
                    Int((short)margins.Top);
                    US();
                    Int((short)margins.Right);
                    US();
                    Int((short)margins.Bottom);
                }
            }
            
            return ST();
        }
        
        /// Displays the image with the given ID and offset. If the image is larger than the wrap
        /// width, it will be shrunk. If any dimension of the size is zero, it will be ignored.
        AdvancedTextBuilder &AlignedImage(Byte index, ImageAlign side, Geometry::Size pixelsize, Geometry::Size relsize, Geometry::Margin margins) {
            return AlignedImage(index, side, pixelsize, relsize, {0, 0}, margins);
        }
        
        /// END @}
        
        
        /// @name Table
        /// These functions allow generation of tables
        /// @{ BEGIN
        
        /// Starts a table. Tables use background and border settings. Set border thickness to 0
        /// to disable table border. relwidth is relative to wrap width and in percentage.
        AdvancedTextBuilder &BeginTable(std::vector<TableColumn> columns, short pixelwidth, short relwidth, bool drawouterborder = true) {
            CSI(0x20);
            Byte(drawouterborder);
            ValAndRel(pixelwidth, relwidth);
            for(auto c : columns) {
                RS();
                Byte((Gorgon::Byte)c.Align);
                if(c.Width != 0 || c.RelWidth != 0)
                    ValAndRel(c.Width, c.RelWidth);
            }
            return ST();
        }
        
        /// Go to next cell. If colspan is set, span align is used to align the new joined column
        AdvancedTextBuilder &NextCell(Byte colspan = 1, Byte rowspan = 1, TextAlignment spanalign = TextAlignment::Left) { 
            if(colspan > 1) {
                CSI(0x21);
                Byte(colspan);
                Byte(Gorgon::Byte(spanalign));
                ST();
            }
            if(rowspan > 1) {
                CSI(0x22);
                Byte(rowspan);
                ST();
            }
            
            US(); return *this; 
        }
        
        AdvancedTextBuilder &NextRow() { RS(); return *this; }
        
        AdvancedTextBuilder &EndTable() { return SCI(0x33); }
        
        /// END @}
        
        
        /// @name Selection
        /// These functions help to display selection marker
        /// @{
        
        AdvancedTextBuilder &SetSelectionImage(Byte index) { CSI(0x11); Index(index); return ST(); }
        
        AdvancedTextBuilder &StartSelection() { return C2(0x86); }
        
        AdvancedTextBuilder &EndSelection() { return C2(0x87); }
        
        /// @}
        
        
        /// @name Regions
        /// Regions are ranges of text. AdvancedPrint function will return a list of region 
        /// boundaries. Each region will have one or more boundaries. Multiple boundaries are 
        /// returned if a region spans multiple lines. You may use same region id multiple times.
        /// Regions can overlap.
        /// @{
        
        AdvancedTextBuilder &StartRegion(Byte index) { CSI(0x30); Index(index); return ST(); }
        
        AdvancedTextBuilder &EndRegion(Byte index) { CSI(0x31); Index(index); return ST(); }
        
        /// Places a placeholder space. Placeholder will be wrapped as if it is a single glyph. Relative
        /// is relative to wrap width and line height and is in percentage.
        AdvancedTextBuilder &Placeholder(Geometry::Size pixels, Geometry::Size rel) { 
            CSI(0x42); 
            ValAndRel(pixels.Width, rel.Width); GS();
            ValAndRel(pixels.Height, rel.Height);
            
            return ST(); 
        }
        
        /// @}
        
    protected:
        
        AdvancedTextBuilder &C1(char c) {
            text.push_back(c);
            return *this;
        }
        
        AdvancedTextBuilder &C2(char c) {
            text.push_back('\xc2');
            text.push_back(c);
            return *this;
        }
        
        void Int(int16_t val) {
            char b1 = val & 0x7f;
            char b2 = (val >> 7) & 0x7f;
            char b3 = (val >> 14) & 0x03;
            
            text.push_back(b1);
            if(b2 != 0 || b3 != 0)
                text.push_back(b2);
            if(b3 != 0)
                text.push_back(b3);
        }
        
        void Index(Byte ind) {
            text.push_back(char(ind & 0x7f));
        }
        
        void Byte(Byte val, bool precise = false) {
            if(precise) {
                text.push_back(char(val & 0xf));
                text.push_back(char(val >> 4));
            }
            else
                text.push_back(char(val >> 1));
        }
        
        void Color(RGBA color, bool precise) {
            Byte(color.R, precise); Byte(color.G, precise); Byte(color.B, precise);
            if(color.A != 255) Byte(color.A, precise);
        }
        
        void ValAndRel(short value, short rel) {
            if(value != 0 || rel == 0)
                Int(value);
            
            if(value != 0 && rel != 0)
                RS();
            
            if(rel != 0)
                Int(rel);
        }
        
        AdvancedTextBuilder &SCI(char cmd) { C2('\x9a'); text.push_back(cmd); return *this; }
        
        void CSI(char cmd) { C2('\x9b'); text.push_back(cmd); }
        
        AdvancedTextBuilder &ST()  { return C2('\x9c'); }
        
        void FS()  { C2('\x1c'); }
        
        void GS()  { C2('\x1d'); }
        
        void RS()  { C2('\x1e'); }
        
        void US()  { C2('\x1f'); }

    private:
        std::string text;
    };
    
    /**
     * Advanced renderer allows AdvancedPrint that allows unicode based markup that can change every
     * aspect of text rendering. It allows images and tables to be placed. Use AdvancedTextBuilder
     * to easily build advanced markup. Unlike other text printers AdvancedRenderer is a heavy object
     * and should not be copied around.
     */
    class AdvancedPrinter : public TextPrinter {
    public:
        
        AdvancedPrinter(AdvancedPrinter &&) = default;
        
        AdvancedPrinter &operator =(AdvancedPrinter &&) = default;
        
        class Region {
        public:
            Byte id;
            Geometry::Bounds bounds;
        };
        
        void RegisterFont(Byte index, const StyledPrinter &renderer);
        
        void RegisterFont(NamedFonts index, const StyledPrinter &renderer) { 
            RegisterFont((Byte)index, renderer); 
        }
        
        void RegisterColor(Byte index, const RGBA &forecolor, const RGBA &backcolor);
        
        void RegisterColor(NamedFonts index, const RGBA &forecolor, const RGBA &backcolor) { 
            RegisterColor((Byte)index, forecolor, backcolor); 
        }
        
        /// Registers the given image to be used in the advanced print
        void RegisterImage(Byte index, const RectangularDrawable &image) {
            images.Add(index, image);
        }
        
        /// This is the advanced operation which allows user to submit functions that will perform the
        /// rendering. First one is glyph render, the second is box renderer, starting point, size,
        /// background color, border thickness and border color will be given to this function. The 
        /// final one draws a horizontal line from a point with the given width and thickness.
        template<class GF_, class BF_, class LF_>
        std::vector<Region> AdvancedOperation(
            GF_ glyphr, BF_ boxr, LF_ liner, 
            const std::string &text, Geometry::Point location, int width
        );
        
        std::vector<Region> AdvancedPrint(
            Layer &target, const std::string &text, 
            Geometry::Point location, int width
        );
        
    protected:
        /// Indexed fonts, some of these are named
        std::map<Byte, StyledPrinter> fonts;
        
        /// Indexed images
        Containers::Hashmap<Byte, const RectangularDrawable> images;
        
        /// Tabstops
        std::map<Byte, std::tuple<int, int, int>> tabstops;
        
        /// Indexed foreground colors
        std::map<Byte, RGBA> colors;
        
        /// Indexed background colors
        std::map<Byte, RGBA> background;
    };
    
} }
