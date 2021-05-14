#include "AdvancedPrinterImpl.h"

#pragma warning(disable:4003)
#define MOVEIT(x) ++it; if(it == end) { --it; return x; }

namespace Gorgon { namespace Graphics {

    std::vector<AdvancedPrinter::Region> AdvancedPrinter::AdvancedPrint(
        TextureTarget &target, const std::string &text, 
        Geometry::Point location, int width, bool wrap
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
            text, location, width, wrap
        );
    }
    
    Geometry::Size AdvancedPrinter::GetSize(const std::string &text) const {
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
            [](const Geometry::Bounds &, const RGBAf &, int, RGBAf) {
            },
            [](int, int, int, int, RGBAf) {
            },
            [](Byte, const Geometry::Bounds &, const RGBAf &, bool) {
            },
            text, {0,0}, 0, false
        );

        return sz;
    }
    
    Geometry::Size AdvancedPrinter::GetSize(const std::string &text, int width) const {
        Geometry::Size sz = {0, 0};

        AdvancedOperation(
            [&sz](
                const GlyphRenderer &renderer, Glyph g,
                const Geometry::Point &location, const RGBAf &, int
                ) {
            if(g != 0xffff) {
                auto p = location + (Geometry::Point)renderer.GetSize(g) + renderer.GetOffset(g);
                p.Y += int(renderer.GetBaseLine());

                if(p.X > sz.Width)
                    sz.Width = p.X;

                if(p.Y > sz.Height)
                    sz.Height = p.Y;
            }
            return true;
        },
            [](const Geometry::Bounds &, const RGBAf &, int, RGBAf) {
        },
            [](int, int, int, int, RGBAf) {
        },
            [](Byte, const Geometry::Bounds &, const RGBAf &, bool) {
        },
            text, {0,0}, width, true
            );

        return sz;
    }
    
    int AdvancedPrinter::GetCharacterIndex(const std::string &text, Geometry::Point location) const {
        int maxdoney = -1;
        int nearestind = -1;
        bool done = false;

        AdvancedOperation(
            [&](
                const GlyphRenderer &renderer, Glyph g,
                const Geometry::Point &l, const RGBAf &, int index
                ) {
            if(done)
                return false;

            if(l.Y >= location.Y) {
                done = true;
                return false;
            }

            if(maxdoney < l.Y) {
                if(l.X >= location.X+renderer.GetCursorAdvance(g)/2) {
                    maxdoney = l.Y;
                }
                else {
                    nearestind = index;
                }
            }

            return true;
        },
            [](const Geometry::Bounds &, const RGBAf &, int, RGBAf) {
        },
            [](int, int, int, int, RGBAf) {
        },
            [](Byte, const Geometry::Bounds &, const RGBAf &, bool) {
        },
            text, {0,0}, 0, false
            );

        return nearestind;
    }
    
    int AdvancedPrinter::GetCharacterIndex(const std::string &text, int w, Geometry::Point location, bool wrap) const {
        int maxdoney = -1;
        int nearestind = -1;
        bool done = false;

        AdvancedOperation(
            [&](
                const GlyphRenderer &renderer, Glyph g,
                const Geometry::Point &l, const RGBAf &, int index
                ) {
            if(done)
                return false;

            if(l.Y >= location.Y) {
                done = true;
                return false;
            }

            if(maxdoney < l.Y) {
                if(l.X >= location.X+renderer.GetCursorAdvance(g)/2) {
                    maxdoney = l.Y;
                }
                else {
                    nearestind = index;
                }
            }

            return true;
        },
            [](const Geometry::Bounds &, const RGBAf &, int, RGBAf) {
        },
            [](int, int, int, int, RGBAf) {
        },
            [](Byte, const Geometry::Bounds &, const RGBAf &, bool) {
        },
            text, {0,0}, w, wrap
            );

        return nearestind;
    }

    Geometry::Rectangle AdvancedPrinter::GetPosition(const std::string &text, int index) const {
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
            [](const Geometry::Bounds &, const RGBAf &, int, RGBAf) {
        },
            [](int, int, int, int, RGBAf) {
        },
            [](Byte, const Geometry::Bounds &, const RGBAf &, bool) {
        },
            text, {0,0}, 0, false
            );

        return cur;
    }

    Geometry::Rectangle AdvancedPrinter::GetPosition(const std::string &text, int w, int index, bool wrap) const {

        Geometry::Rectangle cur = {std::numeric_limits<int>::min(), std::numeric_limits<int>::min(), 0, 0};

        AdvancedOperation(
            [index, &cur](
                const GlyphRenderer &renderer, Glyph g,
                const Geometry::Point &location, const RGBAf &, long ind
                ) {
            if(index == ind) {
                cur.Move(location + renderer.GetOffset(g) + Geometry::Point(0, (int)renderer.GetBaseLine()));
                cur.Resize(renderer.GetSize(g));

                return false;
            }

            return true;
        },
            [](const Geometry::Bounds &, const RGBAf &, int, RGBAf) {
        },
            [](int, int, int, int, RGBAf) {
        },
            [](Byte, const Geometry::Bounds &, const RGBAf &, bool) {
        },
            text, {0,0}, w, wrap
            );

        return cur;
    }

    inline const StyledPrinter *AdvancedPrinter::findfont(int f) const {
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

} }
