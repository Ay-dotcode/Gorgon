#pragma once

#include "Font.h"
#include "Color.h"
#include "../Graphics.h"
#include "../Geometry/Margin.h"
#include "../Containers/Hashmap.h"

#include <string>

#include "../String.h"

#define MOVEIT(x) ++it; if(it == end) { --it; return x; }

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
        Selection,
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
    enum class NamedFont {
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
        SmallScript,
        
        /// Fixed width font to be used in programming
        FixedWidth,
        
        /// Fixed width font to be used in programming
        FixedWidthBold,
        
        /// Fixed width font to be used in programming
        FixedWidthItalic,
    };
    
    inline bool operator ==(int l, NamedFont r) {
        return l == (int)r;
    }

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
        
        /// Appends a zero width breaking space.
        AdvancedTextBuilder &ZeroWidthSpace() { String::AppendUnicode(text, 0x200b); return *this; }
        
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
        AdvancedTextBuilder &UseSuperscript() { return SCI(6); }
        
        /// Switch to subscript, use ScriptOff to switch off
        AdvancedTextBuilder &UseSubscript() { return SCI(5); }
        
        /// Switches sub and superscript off
        AdvancedTextBuilder &ScriptOff() { return SCI(7); }

        /// Switches to the given font index. If it doesn't exist, default font will be used.
        AdvancedTextBuilder &SetFont(Byte fontindex) { CSI(0x15); Index(fontindex); return ST(); }
        /// END @}
        
        
        /// @name Color control
        /// These functions control the color of different parts of the system. Default color for
        /// text and border is set by font style. Default color for background is transparent.
        /// @{ BEGIN
        
        AdvancedTextBuilder &UseDefaultColor() { CSI(0x01); return ST(); }
        
        /// Sets the forecolor to the given 7-bit index. 
        AdvancedTextBuilder &SetColor(Byte index, Byte alpha = 255) { CSI(0x01); Index(index); if(alpha != 255) Alpha(alpha); return ST(); }
        
        /// Sets the forecolor to the given index name. 
        AdvancedTextBuilder &SetColor(NamedFontColors index, Byte alpha = 255) { return SetColor(Gorgon::Byte(index), alpha); }
        
        /// Sets the forecolor to the given color.
        AdvancedTextBuilder &SetColor(RGBA color) { CSI(0x02); Color(color); return ST(); }
        
        /// Removes tint color that is used for images.
        AdvancedTextBuilder &RemoveTint() { CSI(0x18); return ST(); }
        
        /// Sets the tint color that is used for images to the given 7-bit index. 
        AdvancedTextBuilder &SetTint(Byte index, Byte alpha = 255) { CSI(0x18); Index(index); if(alpha != 255) Alpha(alpha); return ST(); }
        
        /// Sets the tint color that is used for images to the given index name. 
        AdvancedTextBuilder &SetTint(NamedFontColors index, Byte alpha = 255) { return SetTint(Gorgon::Byte(index), alpha); }
        
        /// Sets the tint color that is used for images to the given color. 
        AdvancedTextBuilder &SetTint(RGBA color) { CSI(0x18); Color(color); return ST(); }
        
        /// Sets the alpha that is used for images to the given color. 
        AdvancedTextBuilder &SetAlpha(Byte alpha) { return SetTint(RGBA{255, 255, 255, alpha}); }
        
        /// Sets the background color to the given 7-bit index. 
        AdvancedTextBuilder &SetBackgroundColor(Byte index, Byte alpha = 255) { CSI(0x03); Index(index); if(alpha != 255) Alpha(alpha); return ST(); }
        
        /// Sets the background color to the given index name. 
        AdvancedTextBuilder &SetBackgroundColor(NamedFontColors index, Byte alpha = 255) { return SetBackgroundColor(Gorgon::Byte(index), alpha); }
        
        /// Sets the background color to the given color.
        AdvancedTextBuilder &SetBackgroundColor(RGBA color) { CSI(0x04); Color(color); return ST(); }
        
        
        /// Sets the border color to the given 7-bit index. 
        AdvancedTextBuilder &SetBorderColor(Byte index, Byte alpha = 255) { CSI(0x06); Index(index); if(alpha != 255) Alpha(alpha); return ST(); }
        
        /// Sets the border color to the given index name. 
        AdvancedTextBuilder &SetBorderColor(NamedFontColors index, Byte alpha = 255) { return SetBorderColor(Gorgon::Byte(index), alpha); }
        
        /// Sets the border color to the given color.
        AdvancedTextBuilder &SetBorderColor(RGBA color) { CSI(0x07); Color(color); return ST(); }
        
        
        AdvancedTextBuilder &UseDefaultUnderlineColor() { CSI(0x1c); return ST(); }
        
        /// Sets the underline color to the given 7-bit index. 
        AdvancedTextBuilder &SetUnderlineColor(Byte index, Byte alpha = 255) { CSI(0x1c); Index(index); if(alpha != 255) Alpha(alpha); return ST(); }
        
        /// Sets the underline color to the given index name. 
        AdvancedTextBuilder &SetUnderlineColor(NamedFontColors index, Byte alpha = 255) { return SetUnderlineColor(Gorgon::Byte(index), alpha); }
        
        /// Sets the underline color to the given color.
        AdvancedTextBuilder &SetUnderlineColor(RGBA color) { CSI(0x1d); Color(color); return ST(); }
        
        AdvancedTextBuilder &UseDefaultStrikethroughColor() { CSI(0x1e); return ST(); }
        
        /// Sets the strikethrough color to the given 7-bit index. 
        AdvancedTextBuilder &SetStrikethroughColor(Byte index, Byte alpha = 255) { CSI(0x1e); Index(index); if(alpha != 255) Alpha(alpha); return ST(); }
        
        /// Sets the strikethrough color to the given index name. 
        AdvancedTextBuilder &SetStrikethroughColor(NamedFontColors index, Byte alpha = 255) { return SetStrikethroughColor(Gorgon::Byte(index), alpha); }
        
        /// Sets the strikethrough collor to the given color.
        AdvancedTextBuilder &SetStrikethroughColor(RGBA color) { CSI(0x1f); Color(color); return ST(); }
        
        /// Sets the text color of the selection. Default is NamedFontColors::Selection
        AdvancedTextBuilder &SetSelectedTextColor(Byte index, Byte alpha = 255) { CSI(0x11); Index(0b00010); Index(index); if(alpha != 255) Alpha(alpha); return ST(); }
        
        /// Sets the text color of the selection. Default is NamedFontColors::Selection
        AdvancedTextBuilder &SetSelectedTextColor(NamedFontColors index, Byte alpha = 255) { return SetSelectedTextColor(Gorgon::Byte(index), alpha); }
        
        /// Sets the text color of the selection. Default is NamedFontColors::Selection
        AdvancedTextBuilder &SetSelectedTextColor(RGBA color) { CSI(0x11); Index(0b00100); Color(color); return ST(); }
        
        /// Sets the background color of the selection. Default is NamedFontColors::Selection. 
        /// Disables selection image
        AdvancedTextBuilder &SetSelectedBackgroundColor(Byte index, Byte alpha = 255) { CSI(0x11); Index(0b01000); Index(index); if(alpha != 255) Alpha(alpha); return ST(); }
        
        /// Sets the background color of the selection. Default is NamedFontColors::Selection. 
        /// Disables selection image
        AdvancedTextBuilder &SetSelectedBackgroundColor(NamedFontColors index, Byte alpha = 255) { return SetSelectedBackgroundColor(Gorgon::Byte(index), alpha); }
        
        /// Sets the background color of the selection. Default is NamedFontColors::Selection. 
        /// Disables selection image
        AdvancedTextBuilder &SetSelectedBackgroundColor(RGBA color) { CSI(0x11); Index(0b10000); Color(color); return ST(); }
        
        /// Uses default background color if no image is set.
        AdvancedTextBuilder &DefaultSelectedBackgroundColor() { CSI(0x11); Index(0b10000); return ST(); }
        
        /// END @}
        
        
        /// @name Alignment
        /// These functions modify alignment of text and images. Default horizontal alignment is
        /// dictated by the font style. Default vertical alignment is baseline.
        /// @{ BEGIN
        
        /// Disables horizontal alignment and justify override.
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
        
        /// Uses the style default for underline on/off state
        AdvancedTextBuilder &DefaultUnderline() { return SCI(0x16); }
        
        AdvancedTextBuilder &Strikethrough(bool enable = true) { return SCI(enable ? 0x12 : 0x13); }

        /// Uses the style default for strikethrough on/off state
        AdvancedTextBuilder &DefaultStrikethrough() { return SCI(0x17); }
        
        /// END @}
        
        
        /// @name Positioning
        /// @{ BEGIN
        
        AdvancedTextBuilder &WordWrap(bool enable) { return SCI(enable ? 0x30 : 0x31); }
        
        /// Sets the width that the words will wrap from. Pixel and em widths will be added
        /// together. rel is relative to em size.
        AdvancedTextBuilder &SetWrapWidth(short pixels, short rel = 0) { CSI(0x0e); ValAndRel(pixels, rel); return ST(); }
        
        AdvancedTextBuilder &DefaultWrapWidth() { CSI(0x0e); ValAndRel(); return ST(); }
        
        /// Changes the offset that will be added to each letter. Relative sizing is relative to
        /// character width and line height, the value is in percentage.
        AdvancedTextBuilder &SetLetterOffset(const Geometry::Point &pixels, const Geometry::Point &rel = {0, 0}) { 
            CSI(0x14); 
            ValAndRel(pixels.X, rel.X); 
            ValAndRel(pixels.Y, rel.Y);
            return ST(); 
        }

        AdvancedTextBuilder &DefaultLetterOffset() { CSI(0x14); ValAndRel(); return ST(); }
        
        /// Changes the offset of underline. rel is relative to line height and in percentage.
        AdvancedTextBuilder &SetUnderlineOffset(short pixels, short rel) { CSI(0x12); ValAndRel(pixels, rel); return ST(); }
        
        AdvancedTextBuilder &DefaultUnderlineOffset() { CSI(0x12); return ST(); }
        
        /// Changes underline settings. Thickness will be set to default. If no parameters are given,
        /// will set all of them to defaults.
        AdvancedTextBuilder &UnderlineSettings(bool descenders = false, bool spaces = true, bool tabs = false, bool gaps = false, bool placeholders = false) { 
            CSI(0x1a); 
            Index(1 | descenders<<2 | spaces<<3 | tabs<<4 | gaps<<5 | placeholders<<6);
            return ST();
        }
        
        /// Changes underline thickness. rel is relative to line thickness and is in percentage
        AdvancedTextBuilder &UnderlineThickness(short pixels, short rel) { 
            CSI(0x1a); 
            Index(0b10);
            ValAndRel(pixels, rel);
            return ST();
        }
        
        /// Changes the offset of strike. rel is relative to line height and in percentage.
        AdvancedTextBuilder &SetStrikethroughOffset(short pixels, short rel) { CSI(0x13); ValAndRel(pixels, rel); return ST(); }
        
        AdvancedTextBuilder &DefaultStrikethroughOffset() { CSI(0x13); ValAndRel(); return ST(); }
        
        /// Changes strike settings. If no parameters are given, will set them to defaults.
        AdvancedTextBuilder &StrikeSettings(bool spaces = true, bool tabs = false, bool gaps = false, bool placeholders = false) { 
            CSI(0x1b); 
            Index(1 | 1<<2 | spaces<<3 | tabs<<4 | gaps<<5 | placeholders<<6);
            return ST();
        }
        
        /// Changes strike thickness. rel is relative to line thickness and is in percentage
        AdvancedTextBuilder &StrikethroughThickness(short pixels, short rel) { 
            CSI(0x1b); 
            Index(0b10);
            ValAndRel(pixels, rel);
            return ST();
        }
        
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
        
        /// Changes the spacing between paragraphs. rel is relative to line height and in percentage. This value is applied after line spacing.
        AdvancedTextBuilder &SetParagraphSpacing(short pixels, short rel = 0) { CSI(0x09); ValAndRel(pixels, rel); return ST(); }
        
        AdvancedTextBuilder &DefaultParagraphSpacing() { CSI(0x09); ValAndRel(); return ST(); }
        
        /// Changes the spacing between lines. rel is relative to line height and in percentage.
        AdvancedTextBuilder &SetLineSpacing(short pixels, short rel = 0) { CSI(0x0d); ValAndRel(pixels, rel); return ST(); }
        
        AdvancedTextBuilder &DefaultLineSpacing() { CSI(0x0d); ValAndRel(); return ST(); }
        
        /// Changes the spacing between the letters. rel is relative to em width and in 
        /// percentage.
        AdvancedTextBuilder &SetLetterSpacing(short pixels, short rel = 0) { CSI(0x0c); ValAndRel(pixels, rel); return ST(); }
        
        AdvancedTextBuilder &DefaultLetterSpacing() { CSI(0x0c); ValAndRel(); return ST(); }
        
        /// Changes the indent. rel is relative to em width and in percentage.
        AdvancedTextBuilder &SetIndent(short pixels, short rel = 0) { CSI(0x0a); ValAndRel(pixels, rel); return ST(); }
        
        AdvancedTextBuilder &RemoveIndent() { CSI(0x0a); ValAndRel(); return ST(); }
        
        /// Changes the indent of the start of paragraphs. rel is relative to em width and in 
        /// percentage.
        AdvancedTextBuilder &SetHangingIndent(short pixels, short rel) { CSI(0x0b); ValAndRel(pixels, rel); return ST(); }
        
        AdvancedTextBuilder &RemoveHangingIndent() { CSI(0x0b); ValAndRel(); return ST(); }

        
        /// Place the requested amount of space horizontally. rel is relative to em width and in 
        /// percentage. per is relative to wrap width and in basis points (1/10000). This space will
        /// not be breaking. If breaking is desired, you may insert zero width space.
        AdvancedTextBuilder &HorizontalSpace(short pixels, short rel = 0, short per = 0) { CSI(0x40); ValRelAndPer(pixels, rel, per); return ST(); }
        
        /// Place the requested amount of space vertically. rel is relative to line height and in 
        /// percentage.
        AdvancedTextBuilder &VerticalSpace(short pixels, short rel = 0) { CSI(0x41); ValAndRel(pixels, rel); return ST(); }
        
        /// Changes the spacing between the tab stops. rel is in space widths. per is basis points of
        /// wrap width
        AdvancedTextBuilder &SetTabWidth(short pixels, short rel = 0, short per = 0) { CSI(0x17); ValRelAndPer(pixels, rel, per); return ST(); }
        
        /// Adds a tabstop. The tabstop with the given index will be located at the specified location.
        /// It replaces nearest tabstop. rel is in space widths. per is basis point of wrap width
        AdvancedTextBuilder &AddTabStop(Byte index, short pixels, short rel = 0, short per = 0) { CSI(0x25); Index(index); ValRelAndPer(pixels, rel, per); return ST(); }
        
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
        AdvancedTextBuilder &SetPadding(const Gorgon::Geometry::Margin &pixels, const Gorgon::Geometry::Margin &rel = {0}) { 
            CSI(0x08); 
            ValAndRel(pixels.Left, rel.Left);
            ValAndRel(pixels.Top, rel.Top);
            ValAndRel(pixels.Right, rel.Right);
            ValAndRel(pixels.Bottom, rel.Bottom);
            return ST(); 
        }
        
        /// Sets the padding of the selected text from the selected image border. This includes the width of the border.
        /// rel is relative to line thickness and is in percentage. Default is pixels = 0, rel = 0.
        AdvancedTextBuilder &SetSelectionPadding(const Gorgon::Geometry::Margin &pixels, const Gorgon::Geometry::Margin &rel = {0}) { 
            CSI(0x16); 
            ValAndRel(pixels.Left, rel.Left);
            ValAndRel(pixels.Top, rel.Top);
            ValAndRel(pixels.Right, rel.Right);
            ValAndRel(pixels.Bottom, rel.Bottom);
            return ST(); 
        }
        
        /// END @}
        
        
        /// @name Image
        /// @{ BEGIN
        
        //Info map bits: 0-1: align, 2: offset, 3: size, 4: short margins, 5: full margins
        
        /// Displays the image with the given ID aIf the image is larger than the wrap width
        AdvancedTextBuilder &InlineImage(Byte index) { 
            CSI(0x10);
            Index(index);
            Index(0); //no additional info
            
            return ST();
        }
        
        /// Displays the image with the given ID and offset. If the image is larger than the wrap
        /// width, it will be shrunk.
        AdvancedTextBuilder &InlineImage(Byte index, Geometry::Point offset) { 
            CSI(0x10);
            Index(index);
            Index(0b100); //only offset is set
            Int((short)offset.X);
            Int((short)offset.Y);
            
            return ST();
        }
        
        /// Displays the image with the given ID and size. Image will be scale proportionally to
        /// this area. If any dimension is 0, it will be ignored. relsize is relative to wrap width
        /// and line height and is in percentage.
        AdvancedTextBuilder &InlineImage(Byte index, Geometry::Size pixelsize, Geometry::Size relsize, Geometry::Point offset = {0, 0}) {
            CSI(0x10);
            Index(index);
            if(offset == Geometry::Point(0, 0)) {
                Index(0b1000); //only size
            }
            else {
                Index(0b1100); //size and offset
                Int((short)offset.X);
                Int((short)offset.Y);
            }
            ValAndRel(pixelsize.Width, relsize.Width);
            ValAndRel(pixelsize.Height, relsize.Height);
            
            return ST();
        }

        
        /// Displays the image with the given ID, side and margin. If the image is larger than the wrap
        /// width, it will be shrunk.
        AdvancedTextBuilder &AlignedImage(Byte index, ImageAlign side, Geometry::Point offset = {0, 0}, Geometry::Margin margins = {0}) {
            CSI(0x10);
            Index(index);
            
            int margintype = margins == Geometry::Margin{0} ? 0 : ((margins.Left == margins.Top == margins.Right == margins.Bottom) ? 1 : 2);
            
            if(offset == Geometry::Point(0, 0)) {
                Index(side | (margintype << 4)); //side and maybe margin
            }
            else {
                Index(side | 0b100 | (margintype << 4)); //side, offset and maybe margin
                Int((short)offset.X);
                Int((short)offset.Y);
            }
            
            if(margintype == 1) {
                    Int((short)margins.Left);
            }
            else if(margintype == 2) {
                Int((short)margins.Left);
                Int((short)margins.Top);
                Int((short)margins.Right);
                Int((short)margins.Bottom);
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
            
            int margintype = margins == Geometry::Margin{0} ? 0 : ((margins.Left == margins.Top == margins.Right == margins.Bottom) ? 1 : 2);
            
            if(offset == Geometry::Point(0, 0)) {
                Index(side | 0b1000 | (margintype << 4)); //side, size and maybe margin
            }
            else {
                Index(side | 0b1100 | (margintype << 4)); //side, offset, size and maybe margin
                Int((short)offset.X);
                Int((short)offset.Y);
            }
            
            ValAndRel(pixelsize.Width, relsize.Width);
            ValAndRel(pixelsize.Height, relsize.Height);
            
            if(margintype == 1) {
                    Int((short)margins.Left);
            }
            else if(margintype == 2) {
                Int((short)margins.Left);
                Int((short)margins.Top);
                Int((short)margins.Right);
                Int((short)margins.Bottom);
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
            Index(drawouterborder);
            ValAndRel(pixelwidth, relwidth);
            for(auto c : columns) {
                RS();
                Index((Gorgon::Byte)c.Align);
                ValAndRel(c.Width, c.RelWidth);
            }
            return ST();
        }
        
        /// Go to next cell. If colspan is set, span align is used to align the new joined column
        AdvancedTextBuilder &NextCell(Byte colspan = 1, Byte rowspan = 1, TextAlignment spanalign = TextAlignment::Left) { 
            if(colspan > 1) {
                CSI(0x21);
                Index(colspan);
                Index(Gorgon::Byte(spanalign));
                ST();
            }
            if(rowspan > 1) {
                CSI(0x22);
                Index(rowspan);
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
        
        /// Sets the selection image, disables selection background color.
        AdvancedTextBuilder &SetSelectionImage(Byte index) { CSI(0x11); Index(0b01001); Index(index); return ST(); }
        
        /// Removes the selection image and color. Calling this function will effectively remove all
        /// background from the selected text.
        AdvancedTextBuilder &NoSelectionBackground() { CSI(0x11); Index(0b100000); return ST(); }
        
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
            ValAndRel(pixels.Width, rel.Width);
            ValAndRel(pixels.Height, rel.Height);
            ST();
            
            String::AppendUnicode(text, 0xfffc);
            
            return *this;
        }
        
        /// Places a placeholder space. Placeholder will be wrapped as if it is a single glyph. This
        /// overload will have the same size as the previous placeholder.
        AdvancedTextBuilder &Placeholder() {
            String::AppendUnicode(text, 0xfffc);
            
            return *this;
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
            String::AppendUnicode(text, (uint16_t)val);
        }
        
        void Index(Byte ind) {
            text.push_back(ind & 0x7f);
        }
        
        void Alpha(Byte alpha) {
            text.push_back(alpha >> 1);
        }
        
        void Color(RGBA color) {
            String::AppendUnicode(text, (unsigned int)color&0xffff);
            String::AppendUnicode(text, ((unsigned int)color>>16)&0xffff);
        }
        
        void ValAndRel() {
            Index(127);
        }
        
        void ValAndRel(short value, short rel) {
            Index((value != 0) | (rel != 0)<<1);
            
            if(value)
                Int(value);
            
            if(rel)
                Int(rel);
        }
        
        void ValRelAndPer() {
            Index(127);
        }
        
        void ValRelAndPer(short value, short rel, short per) {
            Index((value != 0) | (rel != 0)<<1 | (per != 0)<<2);
            
            if(value)
                Int(value);
            
            if(rel)
                Int(rel);
            
            if(per)
                Int(per);
        }
        
        AdvancedTextBuilder &SCI(char cmd) { C2('\x9a'); text.push_back(cmd); return *this; }
        
        void CSI(char cmd) { C2('\x9b'); text.push_back(cmd); }
        
        AdvancedTextBuilder &ST()  { return C2('\x9c'); }
        
        //no longer rs and us as they clash with numbers
        void RS()  { C2('\x8e'); }
        
        void US()  { C2('\x8f'); }

    private:
        std::string text;
    };
    
    /**
     * Advanced renderer allows AdvancedPrint that allows unicode based markup that can change every
     * aspect of text rendering. It allows images and tables to be placed. Use AdvancedTextBuilder
     * to easily build advanced markup. Unlike other text printers AdvancedRenderer is a heavy object
     * and should not be copied around. Normal font must be set in order to print anything.
     */
    class AdvancedPrinter : public TextPrinter {
    public:
        
        class Region {
        public:
            Region() = default;
            
            Region(Byte id, const Geometry::Bounds &bounds) : 
                ID(id), Bounds(bounds) 
            { }
            
            Byte ID;
            Geometry::Bounds Bounds;
        };
        
        
        AdvancedPrinter() = default;
        
        AdvancedPrinter(AdvancedPrinter &&) = default;

        
        AdvancedPrinter &operator =(AdvancedPrinter &&) = default;
        
        
        void RegisterFont(Byte index, const StyledPrinter &renderer) {
            fonts[index] = renderer;
        }
        
        void RegisterFont(NamedFont index, const StyledPrinter &renderer) { 
            RegisterFont((Byte)index, renderer); 
        }
        
        void RegisterColor(Byte index, const RGBA &forecolor, const RGBA &backcolor) {
            colors[index] = forecolor;
            backcolors[index] = backcolor;
        }
        
        void RegisterColor(NamedFont index, const RGBA &forecolor, const RGBA &backcolor) { 
            RegisterColor((Byte)index, forecolor, backcolor); 
        }
        
        /// Registers the given image to be used in the advanced print
        void RegisterImage(Byte index, const RectangularDrawable &image) {
            images.Add(index, image);
        }
        
        /// Registers the given image to be used in the advanced print
        void RegisterImage(Byte index, RectangularDrawable &&image) = delete;
        
        /// Adds breaking glyphs. A line can be wrapped after a breaking letter. Spaces are breaking
        /// and cannot be removed from the list.
        void SetBreakingLetters(std::vector<Char> letters) {
            breakingchars = letters;
        }
        
        /// Returns the list of breaking glyphs. You can change the returned vector.
        std::vector<Char> &GetBreakingLetters() {
            return breakingchars;
        }
        
        /// Returns the list of breaking glyphs.
        const std::vector<Char> &GetBreakingLetters() const {
            return breakingchars;
        }
        
        /// This is the advanced operation which allows user to submit functions that will perform the
        /// rendering. First one is glyph render. It will be given the renderer to be used, the 
        /// second is box renderer, bounds, background color, border thickness and 
        /// border color will be given to this function. The final one draws a horizontal line from 
        /// a point with the given width and thickness. For wordwrap to work, width should be set
        /// to a positive value
        template<class GF_, class BF_, class LF_, class IF_>
        std::vector<Region> AdvancedOperation(
            GF_ glyphr, BF_ boxr, LF_ liner, IF_ imgr,
            const std::string &text, Geometry::Point location, int width, bool wrap = true
        ) const {
            
            for(auto &sty : fonts) {
                if(sty.second.IsReady() && sty.second.GetGlyphRenderer().NeedsPrepare())
                    sty.second.GetGlyphRenderer().Prepare(text);
            }
            
            Glyph prev = 0;
            int ind = 0;
            
            struct openregioninfo : public Region {
                openregioninfo(Byte id, const Geometry::Bounds &bounds, long startat) :
                    Region(id, bounds), startat(startat) 
                { }
                    
                long    startat  =  0;
                long    finishat = -1;
            };
            
            struct lineinfo {
                lineinfo(long startat, int thickness, int yoffset, const RGBAf &color) : 
                    startat(startat), thickness(thickness), yoffset(yoffset), color(color)
                { }
                
                long startat;
                long finishat = -1;
                
                int thickness;
                int yoffset;
                
                int start, end;
                int y;
                
                RGBAf color;
            };

            struct selectioninfo {
                long                startat;
                
                setval<RGBAf>       selcolor;
                RGBAf               selbg;
                bool                noselbg;
                setval<Byte>        selimg;
                Geometry::Margin    selpadding;
                long                finishat = -1;
            };
            
            
            std::vector<Glyph> breaking = breakingchars;
            std::sort(breaking.begin(), breaking.end());
            
            //state machine
            setvalrel               letterspacing;
            int                     wrapwidth = width; //relative will be calculated instantly
            setvalrel               hangingindent;
            setvalrel               indent;
            setvalrel               paragraphspacing;
            setvalrel               linespacing;
            setvalrel               xoffset;
            setvalrel               yoffset;
            setval<int>             tabwidth;
            setval<bool>            justify;
            setval<TextAlignment>   align;
            setval<RGBAf>           color;

            setval<bool>            underline;
            setval<bool>            strike;
            
            setval<RGBAf>           selcolor;
            setval<RGBAf>           selbg;
            bool                    noselbg = false;
            setval<Byte>            selimg;
            setvalrelmargin         selpadding;
            
            linesettings            underlinesettings;
            linesettings            strikesettings;
            
            wrap = wrap && (width != 0);
            
            //for current font, use changeprinter to update all
            int   fontid = 0;
            const StyledPrinter *printer = &fonts.at(0);
            const GlyphRenderer *renderer = nullptr;
            
            int   height = 0; //current font height, line gap is used
            int   baseline = 0; //current font baseline
            int   em = 0; //current font size em size
            int   lineth = 0; //current line thickness
            float baselineoffsetatlastspace = 0;
            bool  underlineon = false;
            bool  strikeon = false;
            int   curunderlineoff = 0;
            int   curstrikeoff = 0;
            
            //for sub/superscript
            float baselineoffset = 0;
            
            //added to lines for sub/superscript
            int extralineoffset = 0;
            int extralineheight = 0;
            
            //for placing letters
            Geometry::Point cur = location;
            int  starty = cur.Y;
            int  lastbreak = 0;
            bool beginparag = true;
            bool newline    = true;
            int  lastadvance = 0;
            
            std::vector<openregioninfo> openregions; 
            std::vector<Region> regions;
            
            std::vector<lineinfo> underlines;
            std::vector<lineinfo> strikes;
            
            std::vector<selectioninfo> selections;
            
            //used in the parsers too
            auto end = text.end();
            
            std::vector<glyphmark> acc;
            int maxh = 0; //maximum height of a line
            int maxb = 0; //maximum baseline of a line
            long curindex = 0;
            
            auto changeprinter = [&](auto p) {
                printer = p;
                renderer = &printer->GetGlyphRenderer();
                
                if(baseline != renderer->GetBaseLine())
                    prev = 0; //font size changed, do not use kerning
                    
                height   = renderer->GetLineGap();
                baseline = renderer->GetBaseLine();
                em       = renderer->GetEMSize();
                lineth   = std::min(1, (int)std::round(renderer->GetLineThickness()));
                
                if(underlineon) {
                    underlines.back().finishat = (long)acc.size();
                    underlineon = false;
                }
                if(strikeon) {
                    strikes.back().finishat = (long)acc.size();
                    strikeon = false;
                }
            };
            
            auto switchtoscript = [&] {
                if(
                    fontid == NamedFont::Info || fontid == NamedFont::Small ||  fontid == NamedFont::Script || 
                    fontid == NamedFont::BoldScript || fontid == NamedFont::SmallScript) 
                {
                    changeprinter(findfont(NamedFont::SmallScript));
                }
                else if(
                    fontid == NamedFont::Bold || fontid == NamedFont::BoldItalic || fontid == NamedFont::H3 || 
                    fontid == NamedFont::H4
                ) {
                    changeprinter(findfont(NamedFont::BoldScript));
                }
                else if(
                    fontid == NamedFont::Larger
                ) {
                    changeprinter(&fonts.at(0));
                }
                else if(
                    fontid == NamedFont::H1 || fontid == NamedFont::H2
                ) {
                    changeprinter(findfont(NamedFont::Bold));
                }
                else {
                    changeprinter(findfont(NamedFont::Script));
                }
            };
            
            //parse multi character command
            auto CSI = [&](auto &it, auto end) {
                MOVEIT();
                Glyph cmd = internal::decode_impl(it, end);
                curindex++;
                
                if(cmd == ST)
                    return;
                
                Glyph p = 0;
                
                MOVEIT();
                p = internal::decode_impl(it, end);
                curindex++;
                
                switch(cmd) {
                case 0x01: {
                    int ind = readindex(it, end, p, curindex);
                    if(colors.count(ind)) {
                        color = setval<RGBAf>{true, colors.at(ind)};
                        color.val.A = readalpha(it, end, p, curindex)/255.f;
                    }
                    else {
                        color.set = false;
                    }
                    break;
                }
                case 0x02: {
                    color = setval<RGBAf>{true, readcolor(it, end, p, curindex)};
                    break;
                }
                case 0x09: //paragraph spacing
                    paragraphspacing = readvalrel(it, end, p, true, curindex);
                    break;
                case 0x0a: //set indent
                    indent = readvalrel(it, end, p, true, curindex);
                    break;
                case 0x0b:
                    hangingindent = readvalrel(it, end, p, true, curindex);
                    break;
                case 0x0c: //letter spacing
                    letterspacing = readvalrel(it, end, p, true, curindex);
                    break;
                case 0x0d: //line spacing
                    linespacing = readvalrel(it, end, p, true, curindex);
                    break;
                case 0x0e: //wrap width
                    wrapwidth = readvalrel(it, end, p, false, curindex)(renderer->GetEMSize(), width);
                    break;
                case 0x11: {
                    auto bits = readindex(it, end, p, curindex);
                    
                    if(bits&0b100000) {
                        noselbg = true;
                    }
                    else {
                        noselbg = false;
                    }
                    
                    if(bits & 0b00001) {
                        auto ind = readindex(it, end, p, curindex);
                        if(ind != -1) {
                            selimg.set = true;
                            selimg.val = ind;
                        }
                        else {
                            selimg.set = false;
                        }
                    }
                    if(bits & 0b00010) {
                        int ind = readindex(it, end, p, curindex);
                        if(colors.count(ind)) {
                            selcolor = setval<RGBAf>{true, colors.at(ind)};
                            selcolor.val.A = readalpha(it, end, p, curindex)/255.f;
                        }
                        else {
                            selcolor.set = false;
                        }
                    }
                    if(bits & 0b00100) {
                        selcolor = setval<RGBAf>{true, readcolor(it, end, p, curindex)};
                    }
                    if(bits & 0b01000) {
                        int ind = readindex(it, end, p, curindex);
                        if(backcolors.count(ind)) {
                            selbg= setval<RGBAf>{true, backcolors.at(ind)};
                            selbg.val.A = readalpha(it, end, p, curindex)/255.f;
                        }
                        else {
                            selbg.set = false;
                        }
                    }
                    if(bits & 0b10000) {
                        selbg = setval<RGBAf>{true, readcolor(it, end, p, curindex)};
                    }
                    break;
                }
                case 0x14: //letter offset
                    xoffset = readvalrel(it, end, p, true, curindex);
                    yoffset = readvalrel(it, end, p, true, curindex);
                    break;
                case 0x15:
                    fontid = readindex(it, end, p, curindex);
                    if(baselineoffset != 0)
                        switchtoscript();
                    else
                        changeprinter(findfont(fontid));
                    break;
                case 0x16: {
                    selpadding = setvalrelmargin(
                        readvalrel(it, end, p, true, curindex), readvalrel(it, end, p, true, curindex), 
                        readvalrel(it, end, p, true, curindex), readvalrel(it, end, p, true, curindex)
                    );
                    
                    break;
                }
                case 0x17: { //set tab width
                    auto val = readvalrelper(it, end, p, false, curindex);
                    tabwidth = setval<int>{val.set, val(em, wrapwidth, printer->GetTabWidth())};
                    break;
                }
                case 0x1a: {
                    auto m = readindex(it, end, p, curindex);
                    if(m&1) {
                        underlinesettings.descenders   = m&0b0000100;
                        underlinesettings.spaces       = m&0b0001000;
                        underlinesettings.tabs         = m&0b0010000;
                        underlinesettings.gaps         = m&0b0100000;
                        underlinesettings.placeholders = m&0b1000000;
                    }
                    if(m&2) {
                        underlinesettings.thickness = readvalrel(it, end, p, true, curindex);
                    }
                    break;
                }
                case 0x1b: {
                    auto m = readindex(it, end, p, curindex);
                    if(m&1) {
                        strikesettings.descenders   = 1;
                        strikesettings.spaces       = m&0b0001000;
                        strikesettings.tabs         = m&0b0010000;
                        strikesettings.gaps         = m&0b0100000;
                        strikesettings.placeholders = m&0b1000000;
                    }
                    if(m&2) {
                        strikesettings.thickness = readvalrel(it, end, p, true, curindex);
                    }
                    break;
                }
                case 0x1c: {
                    int ind = readindex(it, end, p, curindex);
                    if(colors.count(ind)) {
                        underlinesettings.color = setval<RGBAf>{true, colors.at(ind)};
                        underlinesettings.color.val.A = readalpha(it, end, p, curindex)/255.f;
                    }
                    else {
                        underlinesettings.color.set = false;
                    }
                    break;
                }
                case 0x1d: {
                    underlinesettings.color = setval<RGBAf>{true, readcolor(it, end, p, curindex)};
                    break;
                }
                case 0x1e: {
                    int ind = readindex(it, end, p, curindex);
                    if(colors.count(ind)) {
                        strikesettings.color = setval<RGBAf>{true, colors.at(ind)};
                        strikesettings.color.val.A = readalpha(it, end, p, curindex)/255.f;
                    }
                    else {
                        strikesettings.color.set = false;
                    }
                    break;
                }
                case 0x1f: {
                    strikesettings.color = setval<RGBAf>{true, readcolor(it, end, p, curindex)};
                    break;
                }
                case 0x23: {
                    while(p != ST) {
                        breaking.insert(std::upper_bound(breaking.begin(), breaking.end(), p), p);
                        MOVEIT();
                        p = internal::decode_impl(it, end);
                        curindex++;
                    }
                    break;
                }
                case 0x24: {
                    std::vector<Glyph> rem;
                    while(p != ST) {
                        rem.insert(std::upper_bound(rem.begin(), rem.end(), p), p);
                        MOVEIT();
                        p = internal::decode_impl(it, end);
                        curindex++;
                    }
                    auto rit = rem.begin();
                    breaking.erase(std::remove_if(breaking.begin(), breaking.end(), [&](auto v) {
                        while(*rit < v && rit != rem.end())
                            rit++;
                        
                        if(rit == rem.end())
                            return false;
                        else
                            return *rit == v;
                    }), breaking.end());
                    break;
                }
                case 0x30:
                    openregions.push_back({readindex(it, end, p, curindex), {cur, 0, 0}, (long)acc.size()});
                    break;
                case 0x31: {
                    auto ind = readindex(it, end, p, curindex);
                    
                    for(int i=openregions.size()-1; i>=0; i--) {
                        if(openregions[i].ID == ind && openregions[i].finishat == -1) {
                            openregions[i].finishat = (int)acc.size();
                            break;
                        }
                    }
                    break;
                }
                case 0x40: //horizontal spacing
                    cur.X += readvalrelper(it, end, p, false, curindex)(em, wrapwidth, 0);
                    prev = 0; //no kerning after a spacing like this
                    
                    break;
                case 0x41: //vertical spacing
                    cur.Y += readvalrel(it, end, p, true, curindex)(height, 0);
                    break;
                }
                
                //if extra data at the end, read them
                while(p != ST) {
                    MOVEIT();
                    p = internal::decode_impl(it, end);
                    curindex++;
                }
            };
            
            auto SCI = [&](auto &it, auto end) {
                MOVEIT();
                Glyph cmd = internal::decode_impl(it, end);
                curindex++;
                
                switch(cmd) {
                case 0x4:
                    letterspacing.set       = false;
                    hangingindent.set       = false;
                    indent.set              = false;
                    paragraphspacing.set    = false;
                    linespacing.set         = false;
                    justify.set             = false;
                    align.set               = false;
                    color.set               = false;
                    tabwidth.set            = false;
                    xoffset.set             = false;
                    yoffset.set             = false;
                    changeprinter(&fonts.at(0));
                    fontid = 0;
                    baselineoffset = 0.0f;
                case 0x5:
                case 0x6:
                    switchtoscript();
                    break;
                case 0x7:
                    changeprinter(findfont(fontid));
                    break;
                case 0x10:
                    underline = setval<bool>{true, true};
                    break;
                case 0x11:
                    underline = setval<bool>{true, false};
                    if(underlineon) {
                        underlines.back().finishat = (long)acc.size();
                    }
                    break;
                case 0x12:
                    strike = setval<bool>{true, true};
                    break;
                case 0x13:
                    strike = setval<bool>{true, false};
                    if(strikeon) {
                        strikes.back().finishat = (long)acc.size();
                    }
                    break;
                case 0x16:
                    underline.set = false;
                    if(!printer->GetUnderline() && underlineon) {
                        underlines.back().finishat = (long)acc.size();
                    }
                    break;
                case 0x17:
                    strike.set = false;
                    if(!printer->GetStrike() && strikeon) {
                        strikes.back().finishat = (long)acc.size();
                    }
                    break;
                case 0x20:
                    justify = setval<bool>{true, true};
                    break;
                case 0x21:
                    justify = setval<bool>{true, false};
                    break;
                case 0x22:
                    align = setval<TextAlignment>{true, TextAlignment::Left};
                    break;
                case 0x23:
                    align = setval<TextAlignment>{true, TextAlignment::Right};
                    break;
                case 0x24:
                    align = setval<TextAlignment>{true, TextAlignment::Center};
                    break;
                case 0x29:
                    justify = setval<bool>{false};
                    align = setval<TextAlignment>{false};
                    break;
                case 0x30:
                    wrap = true;
                    break;
                case 0x31:
                    wrap = false;
                    break;
                }
                
                switch(cmd) {
                case 0x5: {
                    baselineoffset = -0.3f;
                    
                    auto height = renderer->GetBaseLine() * 0.3f;
                    if(height > extralineheight)
                        extralineheight = height;
                    
                    break;
                }
                case 0x6: {
                    baselineoffset = 0.4f;
                    
                    auto offset = renderer->GetBaseLine() * 0.4f;
                    if(offset > extralineoffset)
                        extralineoffset = offset;
                    
                    break;
                }
                case 0x7:
                    baselineoffset = 0.0f;
                    break;
                }
            };
            
            auto othercmd = [&](Glyph g) {
                switch(g) {
                case 0x0e:
                    changeprinter(findfont(NamedFont::Bold));
                    fontid = (int)NamedFont::Bold;
                    return true;
                case 0x0f:
                    changeprinter(findfont(NamedFont::Regular));
                    fontid = (int)NamedFont::Regular;
                    return true;
                case 0x91:
                    changeprinter(findfont(NamedFont::Italic));
                    fontid = (int)NamedFont::Italic;
                    return true;
                case 0x92:
                    changeprinter(findfont(NamedFont::Small));
                    fontid = (int)NamedFont::Small;
                    return true;
                case 0x11:
                    changeprinter(findfont(NamedFont::H1));
                    fontid = (int)NamedFont::H1;
                    return true;
                case 0x12:
                    changeprinter(findfont(NamedFont::H2));
                    fontid = (int)NamedFont::H2;
                    return true;
                case 0x13:
                    changeprinter(findfont(NamedFont::H3));
                    fontid = (int)NamedFont::H3;
                    return true;
                case 0x14:
                    changeprinter(findfont(NamedFont::H4));
                    fontid = (int)NamedFont::H4;
                    return true;
                case 0x86: {
                    RGBAf curbgcol = color(printer->GetColor());
                    curbgcol.A *= 0.2;
                    
                    auto curim = selimg;
                    if(selbg.set) {
                        curim.set = false;
                        curbgcol = selbg({0.0f, 0.0f});
                    }
                    else if(backcolors.count((int)NamedFontColors::Selection)) {
                        curbgcol = backcolors.at((int)NamedFontColors::Selection);
                    }
                    
                    selections.push_back({
                        (long)acc.size(), selcolor, 
                        curbgcol, noselbg, curim, selpadding(lineth, 0), -1
                    });
                    return true;
                }
                case 0x87:
                    if(selections.size()) {
                        selections.back().finishat = (long)acc.size();
                    }
                    return true;
                }
                
                return false;
            };
            
            auto doline = [&](Glyph nl) {
                int end = nl == 0 ? lastbreak : (int)acc.size();
                
                int totalw = acc[end-1].location.X + acc[end-1].width - location.X;
                int xoff = 0;
                int lineend = 0;
                
                if(nl == 0 && justify(printer->GetJustify()) && wrapwidth) {
                    //count spaces and letters
                    int sps = 0;
                    int letters = 0;
                    Glyph prev = 0;

                    for(auto it=acc.begin(); it!=acc.begin()+end; ++it) {
                        if(internal::isadjustablespace(it->g))
                            sps++;

                        //ignore before and after tabs
                        if(it->g == '\t') {
                            prev = 0;
                        }
                        else {
                            if(prev && internal::isspaced(prev))
                                letters++;

                            prev = it->g;
                        }
                    }

                    auto target = wrapwidth - totalw;
                    int gs = 0; //glyph spacing
                    int spsp = 0; //space spacing
                    int extraspsp = 0; //extra spaced spaces

                    //try stretching up to 1 full digit if that would be enough.
                    //this may reduce the amount of spacing per letter.
                    if(sps > 0) {
                        spsp = target/sps;

                        //max 1em
                        if(spsp > renderer->GetDigitWidth()) {
                            spsp = renderer->GetDigitWidth();
                            target -= spsp*sps;
                        }
                        else {
                            target -= spsp*sps;

                            extraspsp = target;
                            target = 0;
                        }
                    }

                    if(letters && target/letters >= 1) { //we can increase glyph spacing
                        gs = target/letters;
                        if(gs > 1 && gs > renderer->GetHeight()/3) //1 is always usable
                            gs = renderer->GetHeight()/3;

                        target -= gs*letters;
                    }

                    if(sps > 0 && target > 0) {
                        target += spsp*sps; //roll back and allow upto 2em now
                        spsp = target/sps;

                        //max 2em
                        if(spsp > renderer->GetHeight()*2) {
                            spsp = renderer->GetHeight()*2;
                            target -= spsp*sps;
                        }
                        else {
                            target -= spsp*sps;

                            extraspsp = target;
                            target = 0;
                        }
                    }

                    if(target == 0) {
                        //go over all glyphs and set widths
                        int off = 0;
                        for(auto it=acc.begin(); it!=acc.begin()+end; ++it) {
                            it->location.X += off;

                            if(internal::isadjustablespace(it->g)) {
                                off += spsp;

                                if(extraspsp-->0)
                                    off++;
                            }

                            if(it->g != '\t' && internal::isspaced(it->g)) {
                                off += gs;
                            }
                        }

                        totalw = wrapwidth - target;
                    }
                }
                
                if(wrapwidth > 0) {
                    switch(align(printer->GetDefaultAlign())) {
                    case TextAlignment::Right:
                        xoff = wrapwidth - totalw;
                        lineend = wrapwidth + location.X;
                        
                        break;
                    case TextAlignment::Center:
                        xoff = (wrapwidth - totalw) / 2;
                        lineend = wrapwidth - xoff + location.X;
                        
                        break;
                    default:
                        lineend = totalw + location.X;
                        
                        break;
                    }
                }
                
                int lineh = linespacing(maxh, printer->GetLineSpacing() * (maxh + extralineoffset + extralineheight));

                //selection handling
                for(auto &s : selections) {
                    Geometry::Bounds bnds;
                    //not started yet
                    if(s.startat >= end) {
                        continue;
                    }
                    
                    //find left
                    if(s.startat >= 0 && s.startat < end) {
                        bnds.Left   = acc[s.startat].location.X;
                        s.startat = -1;
                    }
                    else if(s.startat == -1 && end != 0) {
                        bnds.Left = acc[0].location.X + xoff;
                    }
                    else {
                        bnds.Left = location.X + xoff;
                    }
                    
                    //find right
                    if(s.finishat != -1 && s.finishat < end) {
                        bnds.Right = acc[s.finishat].location.X + xoff;
                    }
                    else {
                        bnds.Right = lineend;
                    }
                    
                    bnds.Top     = starty;
                    bnds.Bottom  = cur.Y+lineh;
                    
                    //add margins
                    bnds = bnds + s.selpadding;
                    
                    //render
                    if(s.selimg.set) {
                        imgr(s.selimg.val, bnds, 1.0f, false);
                    }
                    else {
                        boxr(bnds, s.selbg, 0, {0.0f});
                    }
                }
                
                
                //render
                for(int i=0; i<end; i++) {
                    Translate(acc[i].location, xoff, maxb-acc[i].baseline+extralineoffset);
                    if(acc[i].g != '\t' && (!internal::isspace(acc[i].g) || renderer->Exists(acc[i].g)))
                        if(!
                            glyphr(
                                *acc[i].renderer, acc[i].g, 
                                acc[i].location + acc[i].offset, 
                                acc[i].color, acc[i].index
                            )
                        ) {
                            acc.clear();
                            return false;
                        }
                }
                
                if(nl == 0) {
                    //clean spaces at the start of the next line
                    for(; end<acc.size(); end++) {
                        if(!internal::isspace(acc[end].g) && acc[end].g != '\t')
                            break;
                        else {
                            Translate(acc[end].location, xoff, maxb-acc[end].baseline+extralineoffset);
                        }
                    }
                }
                
                //cleanup selections
                for(auto &s : selections) {
                    if(s.startat >= end) {
                        s.startat -= end;
                    }
                    
                    if(s.finishat > end) {
                        s.finishat -= end;
                    }
                    else if(s.finishat != -1) {
                        s.finishat = -2;
                    }
                }
                
                selections.erase(
                    std::remove_if(
                        selections.begin(), selections.end(), 
                        [](auto s) { return s.finishat == -2; }
                    ), selections.end()
                );

                //region X locations determined here
                for(auto &r : openregions) {
                    if(r.startat >= 0 && r.startat < end) {
                        r.Bounds.Left = acc[r.startat].location.X;
                    }
                    if(r.finishat >= 0 && r.finishat < end) {
                        r.Bounds.Right = acc[r.finishat].location.X;
                    }
                }
                
                //line X locations determined here
                for(auto &u : underlines) {
                    if(u.startat >= 0 && u.startat < end) {
                        u.start = acc[u.startat].location.X;
                        u.y     = acc[u.startat].location.Y;
                    }
                    else if(u.startat == -1 && end != 0) {
                        u.y = acc[0].location.Y;
                    }
                    
                    if(u.finishat >= 0 && u.finishat < end) {
                        u.end = acc[u.finishat].location.X;
                    }
                }
                
                for(auto &s : strikes) {
                    if(s.startat >= 0 && s.startat < end) {
                        s.start = acc[s.startat].location.X;
                        s.y     = acc[s.startat].location.Y;
                    }
                    else if(s.startat == -1 && end != 0) {
                        s.y = acc[0].location.Y;
                    }
                    
                    if(s.finishat >= 0 && s.finishat < end) {
                        s.end = acc[s.finishat].location.X;
                    }
                }
                
                //clean consumed glyphs
                acc.erase(acc.begin(), acc.begin()+end);
                
                beginparag = nl != 0 && nl != 0x85;
                
                cur.X = location.X;
                
                //if not empty we need to translate the remaining glyphs to next line
                if(!acc.empty()) {
                    if(beginparag) {
                        cur.X += hangingindent(em, 0);
                    }
                    
                    cur.X += indent(em, 0);
                    
                    //offset every glyph back this amount
                    int xoff = cur.X - acc[0].location.X;
                    
                    //move remaining glyphs to next line
                    for(auto &gm : acc) {
                        gm.location.Y += lineh;
                        gm.location.X += xoff;
                    }
                    
                    cur.X = acc.back().location.X + acc.back().width;
                }
                
                ind = acc.size();
                newline = ind == 0;

                cur.Y += lineh;
                
                int nextlinexstart = location.X + indent(em, 0) + hangingindent(em, 0) * beginparag;
                
                //BEGIN finalize regions before paragraph spacing
                for(auto &r : openregions) {
                    if(r.startat >= end) {
                        r.startat -= end;
                        
                        continue;
                    }
                    else {
                        r.startat = -1;
                    }
                    
                    r.Bounds.Bottom = cur.Y;
                    r.Bounds.Top = starty;
                    
                    if(r.finishat != -1 && r.finishat <= end) {
                        if(r.finishat == end) {
                            r.Bounds.Right = lineend;
                        }
                        
                        regions.push_back(r);
                        r.finishat = -2;
                    }
                    else {
                        r.Bounds.Right = lineend;
                        
                        regions.push_back(r);
                        
                        r.Bounds.Left = nextlinexstart;
                        r.Bounds.Top  = cur.Y;
                        
                        if(r.finishat != -1) {
                            r.finishat -= end;
                        }
                    }
                }
                
                openregions.erase(
                    std::remove_if(
                        openregions.begin(), openregions.end(), 
                        [](auto r) { return r.finishat == -2; }
                    ), openregions.end()
                );
                //END
                
                //BEGIN Draw underlines
                for(auto &u : underlines) {
                    if(u.startat >= end) {
                        u.startat -= end;
                        
                        if(u.finishat >= end)
                            u.finishat -= end;
                        
                        continue;
                    }
                    else {
                        u.startat = 0;
                    }
                    
                    if(u.finishat != -1 && u.finishat <= end) {
                        if(u.finishat == end) {
                            u.end = lineend;
                        }
                        
                        if(u.start < u.end)
                            liner(u.start, u.end, u.y + u.yoffset, u.thickness, u.color);
                        
                        u.finishat = -2;
                    }
                    else {
                        u.end = lineend;
                        
                        if(u.start < u.end)
                            liner(u.start, u.end, u.y + u.yoffset, u.thickness, u.color);
                        
                        u.start = nextlinexstart;
                        
                        if(u.finishat != -1) {
                            u.finishat -= end;
                        }
                    }
                }
                
                underlines.erase(
                    std::remove_if(
                        underlines.begin(), underlines.end(), 
                        [](auto u) { return u.finishat == -2; }
                    ), underlines.end()
                );
                //END
                
                //BEGIN Draw strikes
                for(auto &s : strikes) {
                    if(s.startat >= end) {
                        s.startat -= end;
                        
                        if(s.finishat >= end)
                            s.finishat -= end;
                        
                        continue;
                    }
                    else {
                        s.startat = 0;
                    }
                    
                    if(s.finishat != -1 && s.finishat <= end) {
                        if(s.finishat == end) {
                            s.end = lineend;
                        }
                        
                        if(s.start < s.end)
                            liner(s.start, s.end, s.y + s.yoffset, s.thickness, s.color);
                        
                        s.finishat = -2;
                    }
                    else {
                        s.end = lineend;
                        
                        if(s.start < s.end)
                            liner(s.start, s.end, s.y + s.yoffset, s.thickness, s.color);
                        
                        s.start = nextlinexstart;
                        
                        if(s.finishat != -1) {
                            s.finishat -= end;
                        }
                    }
                }
                
                strikes.erase(
                    std::remove_if(
                        strikes.begin(), strikes.end(), 
                        [](auto u) { return u.finishat == -2; }
                    ), strikes.end()
                );
                //END
                
                //if requested do paragraph
                if(beginparag)
                    cur.Y += paragraphspacing(maxh, printer->GetParagraphSpacing());
                
                //BEGIN Reset
                maxh = 0;
                maxb = 0;
                lastbreak = 0;
                extralineoffset = 0;
                extralineheight = 0;
                starty = cur.Y;
                baselineoffsetatlastspace = 0;
                
                auto backup = printer;
                changeprinter(findfont(fontid));
                
                //if still doing scripts, readjust exta line height and offset
                if(baselineoffset < 0) {
                    auto height = renderer->GetBaseLine() * -baselineoffset;
                    if(height > extralineheight)
                        extralineheight = height;
                    
                    changeprinter(backup);
                }
                else if(baselineoffset > 0) {
                    auto offset = renderer->GetBaseLine() * baselineoffset;
                    
                    if(offset > extralineoffset)
                        extralineoffset = offset;
                    
                    changeprinter(backup);
                }
                //END
                
                return true;
            }; //do line
            
            //Iterate glyphs
            changeprinter(printer);
            for(auto it=text.begin(); it!=end; ++it) {
                Glyph g = internal::decode_impl(it, end);
                curindex++;
                
                if(g == 0xffff)
                    continue;
                
                // **** Commands
                if(g == this->CSI) {
                    CSI(it, end);
                    continue;
                }
                
                if(g == this->SCI) {
                    SCI(it, end);
                    continue;
                }
                
                if(othercmd(g))
                    continue;
                
                //used to detect 0 char line wraps
                ind++;
                
                //horizontal space between the previous glyph to this one
                //will be ignored after a break
                int hspace = 0;
                
                //width of the current glyph
                int gw     = 0;

                
                // **** Breaking character check
                if(internal::isbreaking(g)) {
                    lastbreak = (int)acc.size();
                    baselineoffsetatlastspace = baselineoffset;
                }
                
                // **** Determine spacing
                
                //indent if the first character of the new line, otherwise indent will be
                //handled by doline
                if(newline) {
                    if(beginparag) {
                        hspace = hangingindent(em, 0);
                    }
                    
                    hspace += indent(em, 0);
                }
                
                if(g == '\t') {
                    auto off = cur.X + hspace - location.X;
                    hspace = 0;
                    off += tabwidth(printer->GetTabWidth());
                    off /= tabwidth(printer->GetTabWidth());
                    off *= tabwidth(printer->GetTabWidth());
                    
                    gw = off - cur.X + location.X;
                    
                    //TODO tab stops
                }
                else if(prev && !newline && g != '\t') {
                    hspace = (int)renderer->KerningDistance(prev, g).X;
                    
                    if(!internal::isspaced(g)) { //will be drawn on top of previous glyph
                        hspace -= lastadvance;
                    }
                    else {
                        hspace += letterspacing(em, printer->GetLetterSpacing());
                    }
                }
                
                
                // **** Determine glyph size
                if(internal::isnewline(g)) {
                    if(!doline(g))
                        break;
                    
                    continue;
                }
                else if(internal::isspace(g)) {
                    if(renderer->Exists(g)) {
                        gw = renderer->GetCursorAdvance(g);
                    }
                    else {
                        gw = (int)internal::defaultspace(g, *renderer);
                    }
                }
                else if(g != '\t') {
                    gw = renderer->GetCursorAdvance(g);
                }
                
                newline = false;

                // **** Accumulate
                cur.X += hspace;
                
                Geometry::Point curoff = {xoffset(em, 0), yoffset(em, 0)};
                auto curcolor = color(printer->GetColor());
                
                //TODO gaps
                
                //BEGIN underline
                
                bool dounderline = underline(printer->GetUnderline());
                
                if(dounderline) {
                    if(g == '\t' && !underlinesettings.tabs) {
                        dounderline = false;
                    }
                    else if(!underlinesettings.descenders && renderer->GetOffset(g).Y+renderer->GetSize(g).Height > 0) {
                        dounderline = false;
                    }
                    else if(!underlinesettings.spaces && internal::isspace(g)) {
                        dounderline = false;
                    }
                }
                
                if(dounderline) {
                    auto underlineoffset = underlinesettings.offset(height, renderer->GetUnderlineOffset()) + curoff.Y;
                    auto ucolor = underlinesettings.color(curcolor);
                    
                    if(!underlineon) {
                        underlines.push_back({
                            (long)acc.size(),
                            underlinesettings.thickness(renderer->GetLineThickness(), lineth),
                            underlineoffset, ucolor
                        });
                        
                        underlineon = true;
                        curunderlineoff = underlineoffset + cur.Y;
                    }
                    else if(underlineoffset + cur.Y != curunderlineoff || ucolor != underlines.back().color) {
                        underlines.back().finishat = (long)acc.size();
                        
                        underlines.push_back({
                            (long)acc.size(),
                            underlinesettings.thickness(renderer->GetLineThickness(), lineth),
                            underlineoffset, ucolor
                        });
                        
                        curunderlineoff = underlineoffset + cur.Y;
                    }
                }
                else {
                    if(underlineon) {
                        underlines.back().finishat = (long)acc.size();
                        underlineon = false;
                    }
                }
                //END
                
                //BEGIN strike                
                bool dostrike = strike(printer->GetStrike());
                
                if(dostrike) {
                    if(g == '\t' && !strikesettings.tabs) {
                        dostrike = false;
                    }
                    else if(!strikesettings.spaces && internal::isspace(g)) {
                        dostrike = false;
                    }
                }

                if(dostrike) {
                    auto strikeoffset = strikesettings.offset(height, printer->GetStrikePosition()) + curoff.Y;
                    auto ucolor = strikesettings.color(curcolor);
                    
                    if(!strikeon) {
                        strikes.push_back({
                            (long)acc.size(),
                            strikesettings.thickness(renderer->GetLineThickness(), lineth),
                            strikeoffset, ucolor
                        });
                        
                        strikeon = true;
                        curstrikeoff = strikeoffset + cur.Y;
                    }
                    else if(strikeoffset + cur.Y != curstrikeoff || ucolor != strikes.back().color) {
                        strikes.back().finishat = (long)acc.size();
                        
                        strikes.push_back({
                            (long)acc.size(),
                            strikesettings.thickness(renderer->GetLineThickness(), lineth),
                            strikeoffset, ucolor
                        });
                        
                        curstrikeoff = strikeoffset + cur.Y;
                    }
                }
                else {
                    if(strikeon) {
                        strikes.back().finishat = (long)acc.size();
                        strikeon = false;
                    }
                }
                //END
                
                
                if(baselineoffset != 0) {
                    acc.push_back({
                        (!selections.empty() && selections.back().finishat == -1) ? 
                            selcolor(curcolor) :
                            curcolor
                        , 
                        renderer, cur, curoff, 
                        g, curindex-1, gw, 
                        (int)std::round(baseline*(1+baselineoffset)), 
                        (int)std::round(height + baseline*fabs(baselineoffset))
                    });
                }
                else {
                    acc.push_back({ 
                        (!selections.empty() && selections.back().finishat == -1) ? 
                            selcolor(curcolor) :
                            curcolor
                        , 
                        renderer, cur, curoff, 
                        g, curindex-1, gw, baseline, height
                    });
                }
                
                if(baseline > maxb)
                    maxb = baseline;
                if(height > maxh)
                    maxh = height;
                
                cur.X += gw;
                
                
                prev = g;
                
                
                if(wrapwidth && wrap && cur.X > wrapwidth + location.X) {
                    //emergency break, no spaces
                    if(lastbreak == 0) {
                        if(ind == 1) { //at least one character should be processed
                            lastbreak = (int)acc.size();
                        }
                        else {
                            lastbreak = (int)acc.size()-1;
                        }
                    }
                    
                    //ignore spaces at the end of the line
                    for(; lastbreak>0; lastbreak--) {
                        if(!internal::isspace(acc[lastbreak-1].g))
                            break;
                    }
                    
                    auto blosave = baselineoffset;
                    baselineoffset = baselineoffsetatlastspace;
                    
                    if(!doline(0))
                        break;
                    
                    baselineoffset = blosave;
                    if(baselineoffset == 0 && blosave != 0) {
                        changeprinter(findfont(fontid));
                    }
                }
                
                auto br = std::lower_bound(breaking.begin(), breaking.end(), g);
                if(br != breaking.end() && *br == g) {
                    lastbreak = (int)acc.size();
                }

            }
            
            if(!acc.empty())
                doline(-1);
            
            return regions;
        }
        
        std::vector<Region> AdvancedPrint(
            TextureTarget &target, const std::string &text, 
            Geometry::Point location, int width
        ) const {
            return AdvancedOperation(
                [&target](
                    const GlyphRenderer &renderer, Glyph g, 
                    const Geometry::Point &location, const RGBAf &color, int
                ) {
                    if(g != 0xffff)
                        renderer.Render(g, target, location, color);
                    
                    return true;
                },
                [&target](const Geometry::Bounds &bounds, const RGBAf &bg, int thickness, RGBAf border) { 
                    target.Draw(bounds, bg);
                    if(thickness != 0) {
                        //TODO render border
                    }
                }, 
                [&target](int xstart, int xend, int y, int thickness, RGBAf color) {
                    target.Draw(xstart, y, xend-xstart, thickness, color);
                }, 
                [&target, this](Byte index, const Geometry::Bounds &bounds, const RGBAf &tint, bool stretch) {
                    if(images.Exists(index)) {
                        if(stretch)
                            images[index].DrawStretched(target, bounds.TopLeft(), bounds.GetSize(), tint);
                        else
                            images[index].DrawIn(target, bounds.TopLeft(), bounds.GetSize(), tint);
                    }
                },
                text, location, width, true
            );
        }
        
        bool IsReady() const override {
            return fonts.count(0) && fonts.at(0).IsReady();
        }
        
        virtual const GlyphRenderer &GetGlyphRenderer() const override {
            return fonts.at(0).GetGlyphRenderer();
        }
        
        virtual int GetEMSize() const override {
            return fonts.at(0).GetEMSize();
        }
        
        virtual float GetBaseLine() const override {
            return fonts.at(0).GetBaseLine();
        }
        
        virtual int GetHeight() const override {
            return fonts.at(0).GetHeight();
        }
        
        virtual Geometry::Size GetSize(const std::string &text) const override { 
            Geometry::Size sz = {0, 0};
            
            AdvancedOperation(
                [&sz](
                    const GlyphRenderer &renderer, Glyph g, 
                    const Geometry::Point &location, const RGBAf &, int
                ) {
                    if(g != 0xffff) {
                        auto p = location + (Geometry::Point)renderer.GetSize(g) + renderer.GetOffset(g);
                        p.Y += renderer.GetBaseLine();
                        
                        if(p.X > sz.Width)
                            sz.Width = p.X;
                        
                        if(p.Y > sz.Height)
                            sz.Height = p.Y;
                    }
                    
                    return true;
                },
                [](const Geometry::Bounds &, const RGBAf &, int , RGBAf ) {
                }, 
                [](int , int , int , int , RGBAf ) {
                }, 
                [](Byte , const Geometry::Bounds &, const RGBAf &, bool ) {
                },
                text, {0,0}, 0, false
            );
            
            return sz;
        }
        
        virtual Geometry::Size GetSize(const std::string &text, int width) const override { 
            Geometry::Size sz = {0, 0};
            
            AdvancedOperation(
                [&sz](
                    const GlyphRenderer &renderer, Glyph g, 
                    const Geometry::Point &location, const RGBAf &, int
                ) {
                    if(g != 0xffff) {
                        if(g == '!')
                            g = '!';
                        auto p = location + (Geometry::Point)renderer.GetSize(g) + renderer.GetOffset(g);
                        p.Y += renderer.GetBaseLine();
                        
                        if(p.X > sz.Width)
                            sz.Width = p.X;
                        
                        if(p.Y > sz.Height)
                            sz.Height = p.Y;
                    }
                    return true;
                },
                [](const Geometry::Bounds &, const RGBAf &, int , RGBAf ) {
                }, 
                [](int , int , int , int , RGBAf ) {
                }, 
                [](Byte , const Geometry::Bounds &, const RGBAf &, bool ) {
                },
                text, {0,0}, width, true
            );
            
            return sz;            
        }
        
        virtual int GetCharacterIndex(const std::string &text, Geometry::Point location) const override { Utils::NotImplemented(); }
        
        virtual int GetCharacterIndex(const std::string &text, int w, Geometry::Point location, bool wrap = true) const override { Utils::NotImplemented(); }
        
        virtual Geometry::Rectangle GetPosition(const std::string& text, int index) const override { 
            Geometry::Rectangle cur = {std::numeric_limits<int>::min(), std::numeric_limits<int>::min(), 0, 0};
            
            AdvancedOperation(
                [index, &cur](
                    const GlyphRenderer &renderer, Glyph g, 
                    const Geometry::Point &location, const RGBAf &, long ind
                ) {
                    if(index == ind) {
                        cur.Move(location);
                        cur.Resize(renderer.GetSize(g));
                        
                        return false;
                    }
                    
                    return true;
                },
                [](const Geometry::Bounds &, const RGBAf &, int , RGBAf ) {
                }, 
                [](int , int , int , int , RGBAf ) {
                }, 
                [](Byte , const Geometry::Bounds &, const RGBAf &, bool ) {
                },
                text, {0,0}, 0, false
            );
            
            return cur;
        }
        
        virtual Geometry::Rectangle GetPosition(const std::string& text, int w, int index, bool wrap = true) const override {
            
            Geometry::Rectangle cur = {std::numeric_limits<int>::min(), std::numeric_limits<int>::min(), 0, 0};
            
            AdvancedOperation(
                [index, &cur](
                    const GlyphRenderer &renderer, Glyph g, 
                    const Geometry::Point &location, const RGBAf &, long ind
                ) {
                    if(index == ind) {
                        cur.Move(location + renderer.GetOffset(g) + Geometry::Point(0, renderer.GetBaseLine()));
                        cur.Resize(renderer.GetSize(g));
                        
                        return false;
                    }
                    
                    return true;
                },
                [](const Geometry::Bounds &, const RGBAf &, int , RGBAf ) {
                }, 
                [](int , int , int , int , RGBAf ) {
                }, 
                [](Byte , const Geometry::Bounds &, const RGBAf &, bool ) {
                },
                text, {0,0}, w, wrap
            );
            
            return cur;            
        }
        
        
    protected:
        struct glyphmark {
            RGBAf color;
            const GlyphRenderer *renderer;
            Geometry::Point location, offset;
            Glyph g;
            long index;
            int width;
            int baseline;
            int height;
        };


        struct setvalrel {
            explicit setvalrel(bool set = false, int val = 0, float rel = 0) : set(set), val(val), rel(rel) { }
            bool set;
            int val;
            float rel;
            
            template<class T_>
            int operator()(T_ to, int def) {
                if(!set)
                    return def;
                
                return val + int(std::round(rel * to));
            }
        };

        struct setvalrelmargin {
            setvalrelmargin() = default;
            
            explicit setvalrelmargin(const setvalrel &left, const setvalrel &top, const setvalrel &right, const setvalrel &bottom) : 
                set(left.set), val({left.val, top.val, right.val, bottom.val}), 
                rel({left.rel, top.rel, right.rel, bottom.rel}) 
            { }
            
            bool set = false;
            Geometry::Margin val;
            Geometry::Marginf rel;
            
            Geometry::Margin operator()(Geometry::Margin to, Geometry::Margin def) {
                if(!set)
                    return def;
                
                return {val.Left + int(std::round(rel.Left* to.Left)), val.Top + int(std::round(rel.Top * to.Top)), val.Right + int(std::round(rel.Right * to.Right)), val.Bottom + int(std::round(rel.Bottom * to.Bottom))};
            }
        };

        struct setvalrelper {
            explicit setvalrelper(bool set = false, int val = 0, float rel = 0, float per = 0) : set(set), val(val), rel(rel), per(per) { }
            bool set;
            int val;
            float rel;
            float per;
            
            template<class T_>
            int operator()(T_ to, T_ perc, int def) {
                if(!set)
                    return def;
                
                return val + int(std::round(rel * to)) + int(std::round(per * perc));
            }
        };
        
        template<class T_>
        struct setval {
            explicit setval(bool set = false, T_ val = T_()) : set(set), val(val) { }
            
            bool set;
            T_ val;
            
            T_ operator()(T_ def) {
                if(!set)
                    return def;
                
                return val;
            }
        };
        
        struct linesettings {
            bool descenders  = false;
            bool spaces      = true;
            bool tabs        = false;
            bool gaps        = false;
            bool placeholders= false;
            setvalrel thickness;
            setvalrel offset;
            setval<RGBAf> color;
        };
        
        
        virtual void print(TextureTarget &target, const std::string &text, Geometry::Point location) const override {
            AdvancedPrint(target, text, location, 0);
        }

        virtual void print(TextureTarget &target, const std::string &text,
                        Geometry::Rectangle location) const override {
            AdvancedPrint(target, text, location.TopLeft(), location.Width);
        }

        virtual void print(TextureTarget &target, const std::string &text,
                        Geometry::Rectangle location, TextAlignment align) const override {
            AdvancedPrint(target, text, location.TopLeft(), location.Width);
        }

        virtual void printnowrap(TextureTarget &target, const std::string &text,
                        Geometry::Rectangle location) const override {
            AdvancedPrint(target, text, location.TopLeft(), location.Width);
        }

        virtual void printnowrap(TextureTarget &target, const std::string &text,
                        Geometry::Rectangle location, TextAlignment align) const override {
            AdvancedPrint(target, text, location.TopLeft(), location.Width);
        }
        
        int16_t readint(std::string::const_iterator &it, std::string::const_iterator end, Glyph &cur, long &curindex) const {
            int16_t ret = (int16_t)cur;
            
            MOVEIT(ret); //get the byte after
            cur = internal::decode_impl(it, end);
            curindex++;
            
            return ret;
        }
        
        RGBA readcolor(std::string::const_iterator &it, std::string::const_iterator end, Glyph &cur, long &curindex) const {
            uint32_t col = cur;
            
            MOVEIT(col);
            cur = internal::decode_impl(it, end);
            curindex++;
            
            col = col | (cur << 16);
            
            MOVEIT(col);
            cur = internal::decode_impl(it, end);
            curindex++;
            
            return col;
        }
        
        Byte readindex(std::string::const_iterator &it, std::string::const_iterator end, Glyph &cur, long &curindex) const {
            if(cur > 0x7f)
                return -1;
            
            Byte ret = cur;
            
            MOVEIT(ret); //get the byte after
            cur = internal::decode_impl(it, end);
            curindex++;
            
            return ret;
        }
        
        Byte readalpha(std::string::const_iterator &it, std::string::const_iterator end, Glyph &cur, long &curindex) const {
            if(cur > 0x7f)
                return 255;
            
            Byte ret = (cur<<1) | ((cur&0x40) != 0);
            
            MOVEIT(ret); //get the byte after
            cur = internal::decode_impl(it, end);
            curindex++;
            
            return ret;
        }
        
        setvalrel readvalrel(std::string::const_iterator &it, std::string::const_iterator end, Glyph &cur, bool relper, long &curindex) const {
            auto mode = readindex(it, end, cur, curindex);
            
            if(mode == 127)
                return setvalrel();
            
            int val = 0;
            float rel = 0;
            
            if(mode&0b1)
                val = readint(it, end, cur, curindex);
            
            if(mode&0b10) {
                if(relper)
                    rel = readint(it, end, cur, curindex)/100.f;
                else
                    rel = readint(it, end, cur, curindex);
            }
            
            return setvalrel(true, val, rel);
        }
        
        setvalrelper readvalrelper(std::string::const_iterator &it, std::string::const_iterator end, Glyph &cur, bool relper, long &curindex) const {
            auto mode = readindex(it, end, cur, curindex);
            
            if(mode == 127)
                return setvalrelper();
            
            int val = 0;
            float rel = 0;
            float per = 0;
            
            if(mode&0b1)
                val = readint(it, end, cur, curindex);
            
            if(mode&0b10) {
                if(relper)
                    rel = readint(it, end, cur, curindex)/100.f;
                else
                    rel = readint(it, end, cur, curindex);
            }
            
            if(mode&0b100)
                per = readint(it, end, cur, curindex)/10000.f;
            
            return setvalrelper(true, val, rel, per);
        }
        
        const StyledPrinter *findfont(int f) const {
            if(fonts.count(f))
                return &fonts.at(f);
            
            switch((NamedFont)f) {
            default:
            case NamedFont::Normal:
            case NamedFont::Bold:
            case NamedFont::Italic:
            case NamedFont::Larger:
                return &fonts.at(0);
            case NamedFont::Small:
                if(fonts.count((int)NamedFont::Info))
                    return &fonts.at((int)NamedFont::Info);
                else
                    return &fonts.at(0);
            case NamedFont::Info:
                if(fonts.count((int)NamedFont::Small))
                    return &fonts.at((int)NamedFont::Small);
                else
                    return &fonts.at(0);
            case NamedFont::H1:
            case NamedFont::H2:
                if(fonts.count((int)NamedFont::H1))
                    return &fonts.at((int)NamedFont::H1);
                else if(fonts.count((int)NamedFont::H2))
                    return &fonts.at((int)NamedFont::H2);
                else if(fonts.count((int)NamedFont::H3))
                    return &fonts.at((int)NamedFont::H3);
                else if(fonts.count((int)NamedFont::H4))
                    return &fonts.at((int)NamedFont::H4);
                else
                    return findfont(NamedFont::Larger);
            case NamedFont::H3:
            case NamedFont::H4:
                if(fonts.count((int)NamedFont::H1))
                    return &fonts.at((int)NamedFont::H1);
                else if(fonts.count((int)NamedFont::H2))
                    return &fonts.at((int)NamedFont::H2);
                else if(fonts.count((int)NamedFont::H3))
                    return &fonts.at((int)NamedFont::H3);
                else if(fonts.count((int)NamedFont::H4))
                    return &fonts.at((int)NamedFont::H4);
                else
                    return findfont(NamedFont::Bold);
            case NamedFont::BoldItalic:
                if(fonts.count((int)NamedFont::Bold))
                    return &fonts.at((int)NamedFont::Bold);
                else
                    return findfont(NamedFont::Italic);
            case NamedFont::BoldScript:
            case NamedFont::SmallScript:
                return findfont(NamedFont::Script);
                
            case NamedFont::Script:
                return findfont(NamedFont::Small);
                
            case NamedFont::FixedWidthBold:
                if(fonts.count((int)NamedFont::FixedWidth))
                    return findfont(NamedFont::FixedWidth);
                
                return findfont(NamedFont::Bold);
            case NamedFont::FixedWidthItalic:
                if(fonts.count((int)NamedFont::FixedWidth))
                    return findfont(NamedFont::FixedWidth);
                
                return findfont(NamedFont::Italic);
            }
        }
        
        const StyledPrinter *findfont(NamedFont f) const {
            return findfont((int)f);
        }

        static const int RS = 0x8e;
        static const int US = 0x8f;
        static const int SCI = 0x9a;
        static const int CSI = 0x9b;
        static const int ST = 0x9c;

        
        /// Indexed fonts, some of these are named
        std::map<Byte, StyledPrinter> fonts;
        
        /// Indexed images
        Containers::Hashmap<Byte, const RectangularDrawable> images;
        
        /// Tabstops
        std::map<Byte, std::tuple<int, int, int>> tabstops;
        
        /// Indexed foreground colors
        std::map<Byte, RGBA> colors;
        
        /// Indexed background colors
        std::map<Byte, RGBA> backcolors;
        
        std::vector<Glyph> breakingchars;
    };
    
} }

#undef MOVEIT
