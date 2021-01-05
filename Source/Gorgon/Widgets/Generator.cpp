#include "Generator.h"
#include "../OS.h"
#include "../Filesystem.h"
#include "../Graphics/FreeType.h"
#include "../Graphics/BitmapFont.h"
#include "../Graphics/BlankImage.h"
#include "../Graphics/Animations.h"
#include "../CGI/Line.h"
#include "../CGI/Polygon.h"
#include "../CGI/Circle.h"

//This should be fixed
#ifdef LINUX
#include <unistd.h>
#include <wait.h>

namespace Gorgon { namespace OS {
    bool Start(const std::string &name, std::streambuf *&buf, const std::vector<std::string> &args);
} }

#endif
#include "../Graphics/EmptyImage.h"

namespace Gorgon { namespace Widgets { 

    SimpleGenerator::SimpleGenerator(int fontsize, std::string fontname, std::string boldfontname, bool activate, float density) : Generator(activate), Density(density) {
        Init(fontsize, fontname, boldfontname);
    }
    
    std::string findfontfile(std::string fontname, bool bold) {
#ifdef WIN32
        if(fontname.find_last_of('.') == fontname.npos)
            fontname = Filesystem::Join(Filesystem::Join(OS::GetEnvVar("WINDIR"), "Fonts"), fontname == "" ? (bold ? "Tahomabd.ttf" : "Tahoma.ttf") : fontname + ".ttf");
#else
        if(fontname.find_last_of('.') == fontname.npos) {
            bool found = false;
            
            try {
                std::streambuf *buf;
                OS::Start("fc-match", buf, {"-v", fontname == "" ? (bold ? "sans:bold" : "sans") : fontname});

                if(buf) {
                    std::istream in(buf);
                    std::string line;
                    while(getline(in, line)) {
                        line = String::Trim(line);
                        auto name = String::Extract(line, ':', true);
                        if(name == "file") {
                            String::Extract(line, '"', true);
                            auto fname = String::Extract(line, '"', true);
                            fontname = fname;
                            found = true;
                            break;
                        }
                    }
                    
                    delete buf;
                }
            }
            catch(...) {
                found = false;
            }

            if(!found)
                fontname = bold ? "/usr/share/fonts/gnu-free/FreeSansBold.ttf" : "/usr/share/fonts/gnu-free/FreeSans.ttf";
        }
#endif
        
        return fontname;
    }
    
    void SimpleGenerator::Init(int fontsize, std::string fontname, std::string boldfontname) {
        auto &regular = *new Graphics::FreeType();
        regular.LoadFile(findfontfile(fontname, false), fontsize);
        regularrenderer = &regular;
        
        RegularFont.SetGlyphRenderer(regular);
        CenteredFont.SetGlyphRenderer(regular);
        CenteredFont.AlignCenter();
        
        auto &bold = *new Graphics::FreeType();
        bold.LoadFile(findfontfile(boldfontname, true), fontsize);
        boldrenderer = &bold;
        BoldFont.SetGlyphRenderer(bold);
        
        auto &title = *new Graphics::FreeType();
        title.LoadFile(findfontfile(boldfontname, true), int(std::round(fontsize*1.2)));
        titlerenderer = &title;
        TitleFont.SetGlyphRenderer(title);
        
        auto &subtitle = *new Graphics::FreeType();
        subtitle.LoadFile(findfontfile(boldfontname, true), int(std::round(fontsize*1.1)));
        subtitlerenderer = &subtitle;
        SubtitleFont.SetGlyphRenderer(subtitle);
        
        UpdateDimensions();
        UpdateBorders();
        UpdateDimensions();
    }
    
    UI::Template SimpleGenerator::maketemplate() {
        UI::Template temp;
        temp.SetSpacing(Spacing);
        temp.SetUnitWidth(BorderedWidgetHeight); //BorderedHeight = UnitWidth
        
        return temp;
    }

    SimpleGenerator::~SimpleGenerator() {
        delete regularrenderer;
        delete boldrenderer;
        delete titlerenderer;
        delete subtitlerenderer;
        
        providers.DeleteAll();
        drawables.DeleteAll();
        
        delete normalborder;
        delete hoverborder;
        delete downborder;
        delete disabledborder;
        delete passivewindowborder;
        delete activewindowborder;
        delete grooveborder;
        delete normaleditborder;
        delete hovereditborder;
        delete readonlyborder;
        delete focusborder;
        delete normalemptyborder;
        delete normalbg;
        delete hoverbg;
        delete downbg;
        delete disabledbg;
        delete normalstraight;
        delete altstraight;
        delete hoverstraight;
        delete downstraight;
        delete disabledstraight;
        delete normalrbg;
        delete hoverrbg;
        delete downrbg;
        delete disabledrbg;
        delete objectshape;
        delete innerobjectshape;
        delete groovebg;
        
        for(auto p : panelborders) {
            delete p;
        }
        for(auto p : panelbgs) {
            delete p;
        }
    }
    
    void SimpleGenerator::UpdateDimensions() {
        lettervsize = regularrenderer->GetLetterHeight();
        asciivsize = regularrenderer->GetLetterHeight(true);
        
        int totalh = (int)regularrenderer->GetLineGap();
        
        Spacing = (int)std::round((float)totalh / (2 * Density / 3));
        Focus.Spacing = std::max(1, Spacing / 2);

        BorderedWidgetHeight = 
            totalh + Border.Width * 2 + 
            std::max(Border.Radius/2, Focus.Spacing) * 2 + 
            Focus.Width * 2 + Focus.Spacing * 2
        ;
        
        WidgetHeight = totalh + Focus.Width * 2 + Focus.Spacing * 2;
        
        WidgetWidth = 3 * BorderedWidgetHeight + 2 * Spacing;
        
        ObjectHeight = totalh;
        
        RegularFont.SetTabWidth(totalh + Spacing + Focus.Spacing + Focus.Width); //Align with checkbox radio button
    }
    
    void SimpleGenerator::UpdateBorders(bool smooth) {
        Border.Width  = (int)std::max(std::round(regularrenderer->GetLineThickness()*2.6f), 1.f);
        ShapeBorder   = std::max((regularrenderer->GetLineThickness()+ObjectHeight/18.f)*1.3f, 1.f);

        //limit the thickness after 2.
        if(Border.Width > 2) {
            Border.Width = (int)std::max(std::round(regularrenderer->GetLineThickness()*2.4f), 1.f);
        }
        if(ShapeBorder > 2.f) {
            ShapeBorder = std::max((regularrenderer->GetLineThickness()+ObjectHeight/18.f)*1.2f, 1.f);

            if(ShapeBorder < 2.f)
                ShapeBorder = 2.f;
        }

        ObjectBorder  = Border.Width;
        Border.Radius = (int)std::round(asciivsize.second / 4.f);
        Border.Divisions = smooth * Border.Radius  / 2;
        
        Focus.Width = std::max(1, Border.Width / 2);
    }
    
    Graphics::BitmapRectangleProvider &SimpleGenerator::NormalBorder() {
        if(!normalborder)
            normalborder = makeborder(Border.Color, Background.Regular);
        
        return *normalborder;
    }
    
    Graphics::BitmapRectangleProvider &SimpleGenerator::HoverBorder() {
        if(!hoverborder) {
            auto c = Background.Regular;
            c.Blend(Background.Hover);
            hoverborder = makeborder(Border.Color, c);
        }
        
        return *hoverborder;
    }

    Graphics::BitmapRectangleProvider &SimpleGenerator::DownBorder() {
        if(!downborder) {
            auto c = Background.Regular;
            c.Blend(Background.Down);
            downborder = makeborder(Border.Color, c);
        }

        return *downborder;
    }

    Graphics::BitmapRectangleProvider &SimpleGenerator::DisabledBorder() {
        if(!disabledborder) {
            auto c = Background.Disabled;
            auto c2 = Border.Disabled;

            disabledborder = makeborder(c2, c);
        }

        return *disabledborder;
    }
    
    Graphics::BitmapRectangleProvider &SimpleGenerator::ActiveWindowBorder() {
        if(!activewindowborder)
            activewindowborder = makeborder(0x0, Border.ActiveWindow, 0, 0, Border.Radius ? Border.Radius+Border.Width*2 : 0);
        
        return *activewindowborder;
    }
    
    Graphics::BitmapRectangleProvider &SimpleGenerator::PassiveWindowBorder() {
        if(!passivewindowborder)
            passivewindowborder = makeborder(0x0, Border.PassiveWindow, 0, 0, Border.Radius ? Border.Radius+Border.Width*2 : 0);
        
        return *passivewindowborder;
    }
    
    Graphics::BitmapRectangleProvider &SimpleGenerator::PanelBorder(int missingedge) {
        if(!panelborders[missingedge])
            panelborders[missingedge] = makeborder(Border.Color, Background.Panel, missingedge);
        
        return *panelborders[missingedge];
    }
    
    Graphics::BitmapRectangleProvider &SimpleGenerator::PanelBG(int missingedge) {
        if(!panelbgs[missingedge])
            panelbgs[missingedge] = makeborder(0x0, Background.Panel, missingedge);
        
        return *panelbgs[missingedge];
    }
    
    Graphics::BitmapRectangleProvider &SimpleGenerator::GrooveBorder() {
        if(!grooveborder)
            grooveborder = makeborder(Border.Color, Background.Groove);
        
        return *grooveborder;
    }

    Graphics::BitmapRectangleProvider &SimpleGenerator::NormalEditBorder() {
        if(!normaleditborder)
            normaleditborder = makeborder(Border.Color, Background.Edit);
        
        return *normaleditborder;
    }

    Graphics::BitmapRectangleProvider &SimpleGenerator::HoverEditBorder() {
        if(!hovereditborder) {
            auto c = Background.Edit;
            c.Blend(Background.Hover);

            hovereditborder = makeborder(Border.Color, c);
        }

        return *hovereditborder;
    }

    Graphics::BitmapRectangleProvider &SimpleGenerator::ReadonlyBorder() {
        if(!readonlyborder) {
            auto c = Background.Edit;
            c.Blend(Background.Disabled);

            readonlyborder = makeborder(Border.Color, c);
        }

        return *hovereditborder;
    }

    Graphics::BitmapRectangleProvider &SimpleGenerator::NormalBG() {
        if(!normalbg)
            normalbg = makeborder(0x0, Background.Regular);
        
        return *normalbg;
    }
    
    Graphics::BitmapRectangleProvider &SimpleGenerator::HoverBG() {
        if(!hoverbg) {
            auto c = Background.Regular;
            c.Blend(Background.Hover);
            hoverbg = makeborder(0x0, c);
        }
        
        return *hoverbg;
    }

    Graphics::BitmapRectangleProvider &SimpleGenerator::DownBG() {
        if(!downbg) {
            auto c = Background.Regular;
            c.Blend(Background.Down);
            downbg = makeborder(0x0, c);
        }

        return *downbg;
    }
    
    Graphics::BitmapRectangleProvider &SimpleGenerator::DisabledBG() {
        if(!disabledbg) {
            auto c = Background.Regular;
            c.Blend(Background.Disabled);
            disabledbg = makeborder(0x0, c);
        }

        return *disabledbg;
    }

    Graphics::BitmapRectangleProvider &SimpleGenerator::NormalStraightBG() {
        if(!normalstraight)
            normalstraight = makeborder(0x0, Background.Regular, 0, -1, 0);
        
        return *normalstraight;
    }
    
    Graphics::BitmapRectangleProvider &SimpleGenerator::AltStraightBG() {
        if(!altstraight) {
            auto c = Background.Regular;
            c.Blend(Background.Alternate);
            altstraight = makeborder(0x0, c, 0, -1, 0);
        }
        
        return *altstraight;
    }
    
    Graphics::BitmapRectangleProvider &SimpleGenerator::HoverStraightBG() {
        if(!hoverstraight) {
            auto c = Background.Regular;
            c.Blend(Background.Hover);
            hoverstraight = makeborder(0x0, c, 0, -1, 0);
        }
        
        return *hoverstraight;
    }

    Graphics::BitmapRectangleProvider &SimpleGenerator::DownStraightBG() {
        if(!downstraight) {
            auto c = Background.Regular;
            c.Blend(Background.Down);
            downstraight = makeborder(0x0, c, 0, -1, 0);
        }

        return *downstraight;
    }
    
    Graphics::BitmapRectangleProvider &SimpleGenerator::DisabledStraightBG() {
        if(!disabledstraight) {
            auto c = Background.Regular;
            c.Blend(Background.Disabled);
            disabledstraight = makeborder(0x0, c, 0, -1, 0);
        }

        return *disabledstraight;
    }
    
    Graphics::BitmapRectangleProvider &SimpleGenerator::NormalRBG() {
        if(!normalrbg)
            normalrbg = makeborder(0x0, Background.Regular, 0, 0, Border.Radius/2);
        
        return *normalrbg;
    }
    
    Graphics::BitmapRectangleProvider &SimpleGenerator::HoverRBG() {
        if(!hoverrbg) {
            auto c = Background.Regular;
            c.Blend(Background.Hover);
            hoverrbg = makeborder(0x0, c, 0, 0, Border.Radius/2);
        }
        
        return *hoverrbg;
    }

    Graphics::BitmapRectangleProvider &SimpleGenerator::DownRBG() {
        if(!downrbg) {
            auto c = Background.Regular;
            c.Blend(Background.Down);
            downrbg = makeborder(0x0, c, 0, 0, Border.Radius/2);
        }

        return *downrbg;
    }
    
    Graphics::BitmapRectangleProvider &SimpleGenerator::DisabledRBG() {
        if(!disabledrbg) {
            auto c = Background.Regular;
            c.Blend(Background.Disabled);
            disabledrbg = makeborder(0x0, c, 0, 0, Border.Radius/2);
        }

        return *disabledrbg;
    }
    
    Graphics::BitmapRectangleProvider &SimpleGenerator::GrooveBG() {
        if(!groovebg) {
            auto c = Background.Groove;
            groovebg = makeborder(0x0, c);
        }

        return *groovebg;
    }
    
    Graphics::BitmapRectangleProvider &SimpleGenerator::ObjectShape() {
        if(!objectshape) {
            auto c = Forecolor.Regular;
            objectshape = makeborder(0x0, c);
        }

        return *objectshape;
    }
    
    Graphics::MaskedObjectProvider &SimpleGenerator::InnerObjectShape() {
        if(!innerobjectshape) {
            auto c = Forecolor.Regular;
            auto shape = makeborder(0x0, c, 0, Border.Width, Border.Radius / 3);
            
            providers.Add(shape);
            
            innerobjectshape = new Graphics::MaskedObjectProvider(shape, shape);
        }

        return *innerobjectshape;
    }

    Graphics::RectangleProvider &SimpleGenerator::FocusBorder() {
        if(!focusborder)
            focusborder = makefocusborder();
        
        return *focusborder;
    }
    
    Graphics::BitmapRectangleProvider &SimpleGenerator::NormalEmptyBorder() {
        if(!normalemptyborder)
            normalemptyborder = makeborder(Border.Color, 0x0);
        
        return *normalemptyborder;
    }
    
    Graphics::BitmapRectangleProvider *SimpleGenerator::makeborder(Graphics::RGBA border, Graphics::RGBA bg, int missingedge, int w, int r) {
        if(w == -1)
            w = Border.Width;
        
        if(r == -1)
            r = Border.Radius;
        
        int coff = r + (border.A > 0 ? w+1 : 0);
        int bsize = coff * 2 + 16;
        float off = (border.A ? float(w / 2.0f) : 0);
        
        auto &bi = *new Graphics::Bitmap({bsize, bsize}, Graphics::ColorMode::RGBA);
        bi.Clear();
        
        if(r == 0) {
            Geometry::PointList<Geometry::Pointf> list = {{off,off}, {off, bsize-off}, {bsize-off, bsize-off}, {bsize-off, off}};
            
            CGI::Polyfill(bi.GetData(), list, CGI::SolidFill<>(bg));
            
            list = {
                {off, off}, 
                {off, bsize-off},
                {bsize-off, bsize-off},
                {bsize-off, off}, 
                {off, off}
            };
            
            if(missingedge) {
                list.Pop();
            }
            
            if(border.A != 0)
                CGI::DrawLines(bi.GetData(), list, (float)w, CGI::SolidFill<>(border));
        }
        else {
            Geometry::PointList<Geometry::Pointf> list;
            
            int div = Border.Divisions+1;
            float angperdivision = -PI/2/div;
            float angstart = -PI/2;
            
            if(missingedge) {
                list.Push({off, 0});
            }
            else {
                for(int i=0; i<=div; i++) {
                    float ang = angstart + angperdivision*i;
                    list.Push(Geometry::Pointf::FromVector((float)r, ang, Geometry::Pointf{off+r, off+r}));
                }
            }
            
            angstart = PI;
            for(int i=0; i<=div; i++) {
                float ang = angstart + angperdivision*i;
                list.Push(Geometry::Pointf::FromVector((float)r, ang, Geometry::Pointf{off+r, bsize-off-r}));
            }
            
            angstart = PI/2;
            for(int i=0; i<=div; i++) {
                float ang = angstart + angperdivision*i;
                list.Push(Geometry::Pointf::FromVector((float)r, ang, Geometry::Pointf{bsize-off-r, bsize-off-r}));
            }
            
            if(missingedge) {
                list.Push({bsize-off, 0});
            }
            else {
                angstart = 0;
                for(int i=0; i<=div; i++) {
                    float ang = angstart + angperdivision*i;
                    list.Push(Geometry::Pointf::FromVector((float)r, ang, Geometry::Pointf{bsize-off-r, off+r}));
                }
            }
            
            CGI::Polyfill(bi.GetData(), list, CGI::SolidFill<>(bg));
            
            
            if(!missingedge) {
                list.Push(list.Front());
            }
            
            if(border.A != 0)
                CGI::DrawLines(bi.GetData(), list, (float)w, CGI::SolidFill<>(border));
        }
        
        if(missingedge == 2) {
            bi = bi.Rotate90();
        }
        else if(missingedge == 3) {
            bi = bi.Rotate180();
        }
        else if(missingedge == 4) {
            bi = bi.Rotate270();
        }
        
        drawables.Add(bi);
        

        auto ret = new Graphics::BitmapRectangleProvider(Graphics::Slice(bi, {
            coff, 
            coff, 
            bsize-coff,
            bsize-coff
        }));
        
        ret->Prepare();
        
        return ret;
    }
    
    Graphics::RectangleProvider *SimpleGenerator::makefocusborder() {
        auto &ci = Graphics::EmptyImage::Instance();

        auto &hi = *new Graphics::Bitmap({2, Focus.Width});
        hi.Clear();
        for(auto i=0; i<Focus.Width; i++)
            hi.SetRGBAAt(0, i, Focus.Color);
        hi.Prepare();
        drawables.Add(hi);

        auto &vi = *new Graphics::Bitmap({Focus.Width, 2});
        vi.Clear();
        for(auto i=0; i<Focus.Width; i++)
            vi.SetRGBAAt(i, 0, Focus.Color);
        vi.Prepare();
        drawables.Add(vi);

        auto &cri = *new Graphics::BlankImage(Focus.Width, Focus.Width, Focus.Color);

        return new Graphics::RectangleProvider(cri, hi, cri, vi, ci, vi, cri, hi, cri);
    }
    
    void SimpleGenerator::setupfocus(UI::GraphicsTemplate &foc) {
        foc.Content.SetAnimation(FocusBorder());
        foc.SetSize(100, 100, UI::Dimension::Percent);
        foc.SetPositioning(UI::ComponentTemplate::Absolute);
        foc.SetAnchor(UI::Anchor::None, UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter);
    }
    
    UI::Template SimpleGenerator::Button(bool border) {
        Geometry::Size defsize = {WidgetWidth, border ? BorderedWidgetHeight : WidgetHeight};
        
        UI::Template temp = maketemplate();
        temp.SetSize(defsize);
        
        
        temp.AddContainer(0, UI::ComponentCondition::Always)
            .AddIndex(1) //border
            .AddIndex(2) //boxed content
        ;
        
        auto setupborder = [&](auto &anim, UI::ComponentCondition condition) {
            auto &bg = temp.AddContainer(1, condition);
            bg.Background.SetAnimation(anim);
            bg.SetPositioning(UI::ComponentTemplate::Absolute);
        };

        if(border) {
            setupborder(NormalBorder(), UI::ComponentCondition::Always);
            setupborder(HoverBorder(), UI::ComponentCondition::Hover);
            setupborder(DownBorder(), UI::ComponentCondition::Down);
            setupborder(DisabledBorder(), UI::ComponentCondition::Disabled);
        }
        else {
            setupborder(NormalBG(), UI::ComponentCondition::Always);
            setupborder(HoverBG(), UI::ComponentCondition::Hover);
            setupborder(DownBG(), UI::ComponentCondition::Down);
            setupborder(DisabledBG(), UI::ComponentCondition::Disabled);
        }
        
        auto &boxed = temp.AddContainer(2, UI::ComponentCondition::Always)
            .AddIndex(3) //clip
            .AddIndex(4) //focus
        ;
        if(border)
            boxed.SetBorderSize(Border.Width);
        boxed.SetPadding(std::max(Border.Radius / 2, Focus.Spacing));
        boxed.SetPositioning(UI::ComponentTemplate::Absolute);
        
        auto &clip = temp.AddContainer(3, UI::ComponentCondition::Always)
            .AddIndex(5)
        ;
        clip.SetClip(true);
        clip.SetPadding(Focus.Spacing + Focus.Width);
        
        //Contents
        auto &content = temp.AddContainer(5, UI::ComponentCondition::Always)
            .AddIndex(6) //icon
            .AddIndex(7) //text
        ;
        content.SetPositioning(UI::ComponentTemplate::Absolute);
        content.SetAnchor(UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter);
        content.SetSizing(UI::ComponentTemplate::Automatic);
        
        //Icon container
        auto &iconcont = temp.AddContainer(6, UI::ComponentCondition::Icon1IsSet)
            .AddIndex(8)
        ;
        iconcont.SetMargin(0, 0, Spacing, 0);
        iconcont.SetAnchor(UI::Anchor::MiddleRight, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
        iconcont.SetSizing(UI::ComponentTemplate::Automatic);
        
        auto setupicon = [&](auto &icon) -> auto& {
            icon.SetDataEffect(UI::ComponentTemplate::Icon1);
            icon.SetSizing(UI::ComponentTemplate::Automatic);
            
            return icon;
        };
        
        setupicon(temp.AddGraphics(8, UI::ComponentCondition::Always));
        setupicon(temp.AddGraphics(8, UI::ComponentCondition::Disabled)).SetColor({1.0f, 0.5f});
        
        //Text
        auto setuptext = [&](Graphics::RGBA color, UI::ComponentCondition condition) {
            auto &txt = temp.AddTextholder(7, condition);
            txt.SetRenderer(CenteredFont);
            txt.SetColor(color);
            txt.SetAnchor(UI::Anchor::MiddleRight, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
            txt.SetDataEffect(UI::ComponentTemplate::Text);
            txt.SetSizing(UI::ComponentTemplate::ShrinkOnly);
        };
        
        setuptext(Forecolor.Regular, UI::ComponentCondition::Always);
        setuptext(Forecolor.Regular.BlendWith(Forecolor.Hover), UI::ComponentCondition::Hover);
        setuptext(Forecolor.Regular.BlendWith(Forecolor.Down), UI::ComponentCondition::Down);
        setuptext(Forecolor.Regular.BlendWith(Forecolor.Disabled), UI::ComponentCondition::Disabled);

        setupfocus(temp.AddGraphics(4, UI::ComponentCondition::Focused));

        return temp;
    }
    
    UI::Template SimpleGenerator::IconButton(Geometry::Size iconsize) {
        
        if(iconsize.Width == -1) {
            iconsize.Width = WidgetHeight;
        }
        else {
            iconsize.Width += Focus.Spacing * 2 + Focus.Width * 2;
        }
        
        if(iconsize.Height == -1) {
            iconsize.Height = WidgetHeight;
        }
        else {
            iconsize.Height += Focus.Spacing * 2 + Focus.Width * 2;
        }
        
        auto externalspacing = Border.Width + std::max(Border.Radius / 2, Focus.Spacing);
        
        iconsize += Geometry::Size(externalspacing) * 2;
        
        UI::Template temp = maketemplate();
        temp.SetSpacing(Spacing);
        
        temp.SetSize(iconsize);
        
        temp.AddContainer(0, UI::ComponentCondition::Always)
            .AddIndex(1) //background
            .AddIndex(2) //boxed content
        ;
        
        //background
        auto setupbg = [&](auto &anim, UI::ComponentCondition condition) {
            auto &bg = temp.AddContainer(1, condition);
            bg.Background.SetAnimation(anim);
            bg.SetSize(100, 100, UI::Dimension::Percent);
            bg.SetPositioning(UI::ComponentTemplate::Absolute);
        };
        
        setupbg(NormalBG(), UI::ComponentCondition::Always);
        setupbg(HoverBG(), UI::ComponentCondition::Hover);
        setupbg(DownBG(), UI::ComponentCondition::Down);
        setupbg(DisabledBG(), UI::ComponentCondition::Disabled);
        
        //boxed content
        auto &boxed = temp.AddContainer(2, UI::ComponentCondition::Always)
            .AddIndex(3) //contents
            .AddIndex(4) //focus
        ;
        boxed.SetBorderSize(Border.Width);
        boxed.SetPadding(std::max(Border.Radius / 2, Focus.Spacing));
        boxed.SetPositioning(UI::ComponentTemplate::Absolute);
        
        setupfocus(temp.AddGraphics(4, UI::ComponentCondition::Focused));
        
        //contents
        auto &contents = temp.AddContainer(3, UI::ComponentCondition::Always)
            .AddIndex(5) //Icon or text, if icon exists text will not be displayed
        ;
        contents.SetClip(true);
        contents.SetPadding(Focus.Spacing + Focus.Width);
        
        //Icon container
        temp.AddContainer(5, UI::ComponentCondition::Icon1IsSet)
            .AddIndex(6)
        ;
        
        //Icon
        auto setupicon = [&](auto &icon) -> auto& {
            icon.SetDataEffect(icon.Icon);
            icon.SetAnchor(UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter);
            icon.SetSize(100, 100, UI::Dimension::Percent);
            icon.SetPositioning(icon.Absolute);
            icon.SetSizing(icon.ShrinkOnly);
            icon.SetFillArea(false);
            
            return icon;
        };
        
        setupicon(temp.AddGraphics(6, UI::ComponentCondition::Always));
        setupicon(temp.AddGraphics(6, UI::ComponentCondition::Disabled)).SetColor({1.0f, 0.5f});

        
        //Text container
        temp.AddContainer(5, UI::ComponentCondition::Always)
            .AddIndex(7)
        ;
        
        //Text only visible when no icon is set
        auto setuptext = [&](Graphics::RGBA color, UI::ComponentCondition condition) {
            auto &txt = temp.AddTextholder(7, condition);
            txt.SetRenderer(CenteredFont);
            txt.SetColor(color);
            txt.SetAnchor(UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter);
            txt.SetDataEffect(UI::ComponentTemplate::Text);
            txt.SetSize(100, 100, UI::Dimension::Percent);
            txt.SetSizing(UI::ComponentTemplate::ShrinkOnly);
        };
        
        setuptext(Forecolor.Regular, UI::ComponentCondition::Always);
        setuptext(Forecolor.Regular.BlendWith(Forecolor.Hover), UI::ComponentCondition::Hover);
        setuptext(Forecolor.Regular.BlendWith(Forecolor.Down), UI::ComponentCondition::Down);
        setuptext(Forecolor.Regular.BlendWith(Forecolor.Disabled), UI::ComponentCondition::Disabled);

        return temp;
    }
    
    UI::Template SimpleGenerator::DialogButton() {
        return Button(false);
    }

    UI::Template SimpleGenerator::Checkbox() {
        Geometry::Size defsize = {WidgetWidth * 2 + Spacing, WidgetHeight};
        
        UI::Template temp = maketemplate();
        temp.SetSpacing(Spacing);
        temp.SetSize(defsize);
        
        temp.AddContainer(0, UI::ComponentCondition::Always)
            .AddIndex(1) //Content
            .AddIndex(2) //Focus
        ;
        
        
        auto &cont = temp.AddContainer(1, UI::ComponentCondition::Always)
            .AddIndex(3) //Box symbol
            .AddIndex(4) //Tick
            .AddIndex(5) //Text
        ;
        cont.SetClip(true);
        cont.SetPadding(Focus.Spacing + Focus.Width);
        cont.SetSizing(UI::ComponentTemplate::Fixed);
        cont.SetSize(100, 100, UI::Dimension::Percent);
        
        //tick container, will be used for animation
        auto &state2 = temp.AddContainer(4, UI::ComponentCondition::Always, UI::ComponentCondition::State2)
            .AddIndex(6) //Tick
        ;
        state2.SetValueModification(UI::ComponentTemplate::ModifyAlpha, UI::ComponentTemplate::UseTransition);
        state2.SetReversible(true);
        state2.SetPositioning(UI::ComponentTemplate::Absolute);
        state2.SetAnchor(UI::Anchor::None, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
        
        auto box = [&](auto color) {
            auto icon = new Graphics::Bitmap({ObjectHeight, ObjectHeight});
            
            icon->ForAllPixels([&](auto x, auto y) {
                if(x>=ObjectBorder && x<ObjectHeight-ObjectBorder && y>=ObjectBorder && y<ObjectHeight-ObjectBorder)
                    icon->SetRGBAAt(x, y, Background.Regular);
                else
                    icon->SetRGBAAt(x, y, 0x0);
            });
            
            Geometry::PointList<Geometry::Pointf> border ={
                {ObjectBorder/2.f, ObjectBorder/2.f},
                {ObjectHeight-ObjectBorder/2.f, ObjectBorder/2.f},
                {ObjectHeight-ObjectBorder/2.f, ObjectHeight-ObjectBorder/2.f},
                {ObjectBorder/2.f, ObjectHeight-ObjectBorder/2.f},
                {ObjectBorder/2.f, ObjectBorder/2.f},
            };
            
            CGI::DrawLines(*icon, border, (float)ObjectBorder, CGI::SolidFill<>(color));
            icon->Prepare();
            drawables.Add(icon);
            
            return icon;
        };
        
        auto tick = [&](auto color) {
            auto icon = new Graphics::Bitmap({ObjectHeight, ObjectHeight});
            
            icon->ForAllPixels([&](auto x, auto y) {
                icon->SetRGBAAt(x, y, 0x0);
            });
            
            //these coordinates are designed to look good across many different
            //sizes covering a wide range
            Geometry::PointList<Geometry::Pointf> tick ={
                {ShapeBorder*2.2f, ObjectHeight/2.f},
                {ObjectHeight*0.45f, ObjectHeight-ShapeBorder*2.2f},
                {ObjectHeight-ShapeBorder*2.4f, ShapeBorder*2.2f}
            };
            
            if(ObjectHeight - ShapeBorder*4.6f < 3) {
                CGI::Polyfill(*icon, Geometry::PointList<Geometry::Pointf>{
                    {ObjectBorder*2.f, ObjectBorder*2.f},
                    {ObjectHeight - ObjectBorder*2.f, ObjectBorder*2.f},
                    {ObjectHeight - ObjectBorder*2.f, ObjectHeight - ObjectBorder*2.f},
                    {ObjectBorder*2.f, ObjectHeight - ObjectBorder*2.f},
                }, CGI::SolidFill<>(color));
            }
            else {
                CGI::DrawLines(*icon, tick, 1.2f*ShapeBorder, CGI::SolidFill<>(color));
            }
            icon->Prepare();
            
            return icon;
        };
        
        auto makestate = [&](auto color, UI::ComponentCondition condition) {
            //box
            auto &bx = temp.AddGraphics(3, condition);
            bx.Content.SetDrawable(*box(color));
            bx.SetAnchor(UI::Anchor::MiddleRight, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
            
            auto &tic = temp.AddGraphics(6, condition);
            tic.Content.SetDrawable(*tick(color));
            tic.SetAnchor(UI::Anchor::MiddleRight, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
            
            auto &tt = temp.AddTextholder(5, condition);
            tt.SetRenderer(RegularFont);
            tt.SetColor(color);
            tt.SetMargin(Spacing, 0, 0, 0);
            tt.SetAnchor(UI::Anchor::MiddleRight, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
            tt.SetDataEffect(UI::ComponentTemplate::Text);
            tt.SetSize(100, 100, UI::Dimension::Percent);
            tt.SetSizing(UI::ComponentTemplate::ShrinkOnly, UI::ComponentTemplate::ShrinkOnly);
        };
        
        makestate(Forecolor.Regular, UI::ComponentCondition::Always);
        makestate(Forecolor.Regular.BlendWith(Forecolor.Hover), UI::ComponentCondition::Hover);
        makestate(Forecolor.Regular.BlendWith(Forecolor.Down), UI::ComponentCondition::Down);
        makestate(Forecolor.Regular.BlendWith(Forecolor.Disabled), UI::ComponentCondition::Disabled);
        
        setupfocus(temp.AddGraphics(2, UI::ComponentCondition::Focused));
        
        return temp;
    }
    
    UI::Template SimpleGenerator::CheckboxButton() {
        
        UI::Template temp = maketemplate();
        temp.SetSpacing(Spacing);
        temp.SetSize(BorderedWidgetHeight, BorderedWidgetHeight);
        

        temp.AddContainer(0, UI::ComponentCondition::Always)
            .AddIndex(1) //background
            .AddIndex(8) //checked border
            .AddIndex(2) //boxed content
        ;
        
        //background
        auto setupbg = [&](auto &anim, UI::ComponentCondition condition) {
            auto &bg = temp.AddContainer(1, condition);
            bg.Background.SetAnimation(anim);
            bg.SetSize(100, 100, UI::Dimension::Percent);
            bg.SetPositioning(UI::ComponentTemplate::Absolute);
        };
        
        setupbg(NormalBG(), UI::ComponentCondition::Always);
        setupbg(HoverBG(), UI::ComponentCondition::Hover);
        setupbg(DownBG(), UI::ComponentCondition::Down);
        setupbg(DisabledBG(), UI::ComponentCondition::Disabled);
        
        //checked border
        auto &border = temp.AddContainer(8, UI::ComponentCondition::Always, UI::ComponentCondition::State2);
        border.SetValueModification(UI::ComponentTemplate::ModifyAlpha, UI::ComponentTemplate::UseTransition);
        border.SetValueRange(0, 0.25, 1);
        border.SetReversible(true);
        border.Background.SetAnimation(NormalEmptyBorder());        
        
        //boxed content
        auto &boxed = temp.AddContainer(2, UI::ComponentCondition::Always)
            .AddIndex(3) //contents
            .AddIndex(4) //focus
        ;
        boxed.SetBorderSize(Border.Width);
        boxed.SetPadding(std::max(Border.Radius / 2, Focus.Spacing));
        boxed.SetPositioning(UI::ComponentTemplate::Absolute);
        
        setupfocus(temp.AddGraphics(4, UI::ComponentCondition::Focused));
        
        //contents
        auto &contents = temp.AddContainer(3, UI::ComponentCondition::Always)
            .AddIndex(5) //Icon or text, if icon exists text will not be displayed
        ;
        contents.SetClip(true);
        contents.SetPadding(Focus.Spacing + Focus.Width);
        
        //Icon container
        temp.AddContainer(5, UI::ComponentCondition::Icon1IsSet)
            .AddIndex(6)
        ;
        
        //Icon
        auto setupicon = [&](auto &icon) -> auto& {
            icon.SetDataEffect(icon.Icon);
            icon.SetAnchor(UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter);
            icon.SetSize(100, 100, UI::Dimension::Percent);
            icon.SetPositioning(icon.Absolute);
            icon.SetSizing(icon.ShrinkOnly);
            icon.SetFillArea(false);
            
            return icon;
        };
        
        setupicon(temp.AddGraphics(6, UI::ComponentCondition::Always));
        setupicon(temp.AddGraphics(6, UI::ComponentCondition::Disabled)).SetColor({1.0f, 0.5f});

        
        //Text container
        temp.AddContainer(5, UI::ComponentCondition::Always)
            .AddIndex(7)
        ;
        
        //Text only visible when no icon is set
        auto setuptext = [&](Graphics::RGBA color, UI::ComponentCondition condition) {
            auto &txt = temp.AddTextholder(7, condition);
            txt.SetRenderer(CenteredFont);
            txt.SetColor(color);
            txt.SetAnchor(UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter);
            txt.SetDataEffect(UI::ComponentTemplate::Text);
            txt.SetSize(100, 100, UI::Dimension::Percent);
            txt.SetSizing(UI::ComponentTemplate::ShrinkOnly);
        };
        
        setuptext(Forecolor.Regular, UI::ComponentCondition::Always);
        setuptext(Forecolor.Regular.BlendWith(Forecolor.Hover), UI::ComponentCondition::Hover);
        setuptext(Forecolor.Regular.BlendWith(Forecolor.Down), UI::ComponentCondition::Down);
        setuptext(Forecolor.Regular.BlendWith(Forecolor.Disabled), UI::ComponentCondition::Disabled);

        return temp;
    }
    
    UI::Template SimpleGenerator::RadioButton() {
        Geometry::Size defsize = {WidgetWidth * 2 + Spacing, WidgetHeight};
        
        UI::Template temp = maketemplate();
        temp.SetSpacing(Spacing);
        temp.SetSize(defsize);
        
        temp.AddContainer(0, UI::ComponentCondition::Always)
            .AddIndex(1) //Content
            .AddIndex(2) //Focus
        ;
        
        auto &cont = temp.AddContainer(1, UI::ComponentCondition::Always)
            .AddIndex(3) //Box symbol
            .AddIndex(4) //Tick
            .AddIndex(5) //Text
        ;
        cont.SetClip(true);
        cont.SetPadding(Focus.Spacing + Focus.Width);
        cont.SetSizing(UI::ComponentTemplate::Fixed);
        cont.SetSize(100, 100, UI::Dimension::Percent);
        
        //tick container, will be used for animation
        auto &state2 = temp.AddContainer(4, UI::ComponentCondition::Always, UI::ComponentCondition::State2)
            .AddIndex(6) //Tick
        ;
        state2.SetValueModification(UI::ComponentTemplate::ModifyAlpha, UI::ComponentTemplate::UseTransition);
        state2.SetReversible(true);
        state2.SetPositioning(UI::ComponentTemplate::Absolute);
        state2.SetAnchor(UI::Anchor::None, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
        
        float outer_r = ObjectHeight / 2.f - 0.5f;
        int borderstart_r = int(ceil(outer_r - ObjectBorder));
        float inner_r = std::max(outer_r - ObjectBorder - std::max(Spacing / 2.f, 1.5f), 1.5f);
        
        Geometry::Size bmpsize = {ObjectHeight + 2, ObjectHeight + 2};
        Geometry::Pointf center = {float(outer_r + 1.5f), float(outer_r + 1.5f)};
        
        auto box = [&](auto color) {
            auto icon = new Graphics::Bitmap(bmpsize);
            
            icon->Clear();
            
            CGI::Circle<16>(*icon, center, (float)borderstart_r, (float)ObjectBorder, CGI::SolidFill<>(color));

            icon->Prepare();
            drawables.Add(icon);
            
            return icon;
        };
        
        auto tick = [&](auto color) {
            auto icon = new Graphics::Bitmap(bmpsize);
            
            icon->ForAllPixels([&](auto x, auto y) {
                icon->SetRGBAAt(x, y, 0x0);
            });
            
            CGI::Circle<16>(*icon, center, (float)inner_r, CGI::SolidFill<>(color));
            icon->Prepare();
            
            return icon;
        };
        
        auto makestate = [&](auto color, UI::ComponentCondition condition) {
            //box
            auto &bx = temp.AddGraphics(3, condition);
            bx.Content.SetDrawable(*box(color));
            bx.SetAnchor(UI::Anchor::MiddleRight, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
            bx.SetMargin(-1, 0, -1, 0);
            
            auto &tic = temp.AddGraphics(6, condition);
            tic.Content.SetDrawable(*tick(color));
            tic.SetAnchor(UI::Anchor::MiddleRight, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
            tic.SetMargin(-1, 0, -1, 0);
            
            auto &tt = temp.AddTextholder(5, condition);
            tt.SetRenderer(RegularFont);
            tt.SetColor(color);
            tt.SetMargin(Spacing, 0, 0, 0);
            tt.SetAnchor(UI::Anchor::MiddleRight, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
            tt.SetDataEffect(UI::ComponentTemplate::Text);
            tt.SetSize(100, 100, UI::Dimension::Percent);
            tt.SetSizing(UI::ComponentTemplate::ShrinkOnly, UI::ComponentTemplate::ShrinkOnly);
        };
        
        makestate(Forecolor.Regular, UI::ComponentCondition::Always);
        makestate(Forecolor.Regular.BlendWith(Forecolor.Hover), UI::ComponentCondition::Hover);
        makestate(Forecolor.Regular.BlendWith(Forecolor.Down), UI::ComponentCondition::Down);
        makestate(Forecolor.Regular.BlendWith(Forecolor.Disabled), UI::ComponentCondition::Disabled);
        
        setupfocus(temp.AddGraphics(2, UI::ComponentCondition::Focused));
        
        return temp;
    }
    
    UI::Template SimpleGenerator::Label() {
        Geometry::Size defsize = {WidgetWidth * 2 + Spacing, WidgetHeight};
        
        UI::Template temp = maketemplate();
        temp.SetSpacing(Spacing);
        temp.SetSize(defsize);
        
        auto &cont = temp.AddContainer(0, UI::ComponentCondition::Always, UI::ComponentCondition::Disabled)
            .AddIndex(1) //icon
            .AddIndex(2) //text
        ;
        cont.SetValueModification(UI::ComponentTemplate::ModifyAlpha, UI::ComponentTemplate::UseTransition);
        cont.SetValueRange(0, 1, 0.5);
        cont.SetReversible(true);
        cont.SetClip(true);
        
        auto &icon = temp.AddPlaceholder(1, UI::ComponentCondition::Icon1IsSet);
        icon.SetDataEffect(UI::ComponentTemplate::Icon);
        icon.SetAnchor(UI::Anchor::MiddleRight, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
        icon.SetSize(100, 100, UI::Dimension::Percent);
        icon.SetSizing(UI::ComponentTemplate::ShrinkOnly);
        icon.SetMargin(0, 0, Spacing, 0);
        
        auto &txt = temp.AddTextholder(2, UI::ComponentCondition::Always);
        txt.SetRenderer(RegularFont);
        txt.SetColor(Forecolor.Regular);
        txt.SetAnchor(UI::Anchor::MiddleRight, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
        txt.SetDataEffect(UI::ComponentTemplate::Text);
        txt.SetSize(100, 100, UI::Dimension::Percent);
        txt.SetSizing(UI::ComponentTemplate::ShrinkOnly);
        
        return temp;
    }

    UI::Template SimpleGenerator::ErrorLabel() {
        Geometry::Size defsize = {WidgetWidth * 2 + Spacing, WidgetHeight};
        
        UI::Template temp = maketemplate();
        temp.SetSpacing(Spacing);
        temp.SetSize(defsize);
        
        auto &cont = temp.AddContainer(0, UI::ComponentCondition::Always, UI::ComponentCondition::Disabled)
            .AddIndex(1) //icon
            .AddIndex(2) //text
        ;
        cont.SetValueModification(UI::ComponentTemplate::ModifyAlpha, UI::ComponentTemplate::UseTransition);
        cont.SetValueRange(0, 1, 0.5);
        cont.SetReversible(true);
        cont.SetClip(true);
        
        auto &icon = temp.AddPlaceholder(1, UI::ComponentCondition::Icon1IsSet);
        icon.SetDataEffect(UI::ComponentTemplate::Icon);
        icon.SetAnchor(UI::Anchor::MiddleRight, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
        icon.SetSize(100, 100, UI::Dimension::Percent);
        icon.SetSizing(UI::ComponentTemplate::ShrinkOnly);
        icon.SetMargin(0, 0, Spacing, 0);
        
        auto &txt = temp.AddTextholder(2, UI::ComponentCondition::Always);
        txt.SetRenderer(RegularFont);
        txt.SetColor(Forecolor.Regular.BlendWith(Forecolor.Error));
        txt.SetAnchor(UI::Anchor::MiddleRight, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
        txt.SetDataEffect(UI::ComponentTemplate::Text);
        txt.SetSize(100, 100, UI::Dimension::Percent);
        txt.SetSizing(UI::ComponentTemplate::ShrinkOnly);
        
        return temp;
    }
    
    UI::Template SimpleGenerator::BoldLabel() {
        Geometry::Size defsize = {WidgetWidth * 2 + Spacing, WidgetHeight};
        
        UI::Template temp = maketemplate();
        temp.SetSpacing(Spacing);
        temp.SetSize(defsize);
        
        auto &cont = temp.AddContainer(0, UI::ComponentCondition::Always, UI::ComponentCondition::Disabled)
            .AddIndex(1) //icon
            .AddIndex(2) //text
        ;
        cont.SetValueModification(UI::ComponentTemplate::ModifyAlpha, UI::ComponentTemplate::UseTransition);
        cont.SetValueRange(0, 1, 0.5);
        cont.SetReversible(true);
        cont.SetClip(true);
        
        auto &icon = temp.AddPlaceholder(1, UI::ComponentCondition::Icon1IsSet);
        icon.SetDataEffect(UI::ComponentTemplate::Icon);
        icon.SetAnchor(UI::Anchor::MiddleRight, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
        icon.SetSize(100, 100, UI::Dimension::Percent);
        icon.SetSizing(UI::ComponentTemplate::ShrinkOnly);
        icon.SetMargin(0, 0, Spacing, 0);
        
        auto &txt = temp.AddTextholder(2, UI::ComponentCondition::Always);
        txt.SetRenderer(BoldFont);
        txt.SetColor(Forecolor.Regular);
        txt.SetAnchor(UI::Anchor::MiddleRight, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
        txt.SetDataEffect(UI::ComponentTemplate::Text);
        txt.SetSize(100, 100, UI::Dimension::Percent);
        txt.SetSizing(UI::ComponentTemplate::ShrinkOnly);
        
        return temp;
    }

    UI::Template SimpleGenerator::TitleLabel() {
        Geometry::Size defsize = {WidgetWidth * 2 + Spacing, WidgetHeight + TitleFont.GetHeight() - RegularFont.GetHeight()};
        
        UI::Template temp = maketemplate();
        temp.SetSpacing(Spacing);
        temp.SetSize(defsize);
        
        auto &cont = temp.AddContainer(0, UI::ComponentCondition::Always, UI::ComponentCondition::Disabled)
            .AddIndex(1) //icon
            .AddIndex(2) //text
            .AddIndex(3) //line
        ;
        cont.SetValueModification(UI::ComponentTemplate::ModifyAlpha, UI::ComponentTemplate::UseTransition);
        cont.SetValueRange(0, 1, 0.5);
        cont.SetReversible(true);
        cont.SetClip(true);
        
        auto &icon = temp.AddPlaceholder(1, UI::ComponentCondition::Icon1IsSet);
        icon.SetDataEffect(UI::ComponentTemplate::Icon);
        icon.SetAnchor(UI::Anchor::MiddleRight, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
        icon.SetSize(100, 100, UI::Dimension::Percent);
        icon.SetSizing(UI::ComponentTemplate::ShrinkOnly);
        icon.SetMargin(0, 0, Spacing, 0);
        
        auto &txt = temp.AddTextholder(2, UI::ComponentCondition::Always);
        txt.SetRenderer(TitleFont);
        txt.SetColor(Forecolor.Title);
        txt.SetAnchor(UI::Anchor::MiddleRight, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
        txt.SetDataEffect(UI::ComponentTemplate::Text);
        txt.SetSize(100, 100, UI::Dimension::Percent);
        txt.SetSizing(UI::ComponentTemplate::ShrinkOnly);
        
        auto &ln = temp.AddGraphics(3, UI::ComponentCondition::Always);
        ln.Content.SetAnimation(Graphics::BlankImage::Get());
        ln.SetColor(Forecolor.Title);
        ln.SetSize({100, UI::Dimension::Percent}, Border.Width);
        ln.SetMargin(Spacing*2, 0, 0, 0);
        ln.SetAnchor(UI::Anchor::FirstBaselineRight, UI::Anchor::BottomLeft, UI::Anchor::BottomLeft);
        
        return temp;
    }

    UI::Template SimpleGenerator::SubtitleLabel() {
        Geometry::Size defsize = {WidgetWidth * 2 + Spacing, WidgetHeight + SubtitleFont.GetHeight() - RegularFont.GetHeight()};
        
        UI::Template temp = maketemplate();
        temp.SetSpacing(Spacing);
        temp.SetSize(defsize);
        
        auto &cont = temp.AddContainer(0, UI::ComponentCondition::Always, UI::ComponentCondition::Disabled)
            .AddIndex(1) //icon
            .AddIndex(2) //text
            .AddIndex(3) //line
        ;
        cont.SetValueModification(UI::ComponentTemplate::ModifyAlpha, UI::ComponentTemplate::UseTransition);
        cont.SetValueRange(0, 1, 0.5);
        cont.SetReversible(true);
        cont.SetClip(true);
        
        auto &icon = temp.AddPlaceholder(1, UI::ComponentCondition::Icon1IsSet);
        icon.SetDataEffect(UI::ComponentTemplate::Icon);
        icon.SetAnchor(UI::Anchor::MiddleRight, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
        icon.SetSize(100, 100, UI::Dimension::Percent);
        icon.SetSizing(UI::ComponentTemplate::ShrinkOnly);
        icon.SetMargin(0, 0, Spacing, 0);
        
        auto &txt = temp.AddTextholder(2, UI::ComponentCondition::Always);
        txt.SetRenderer(SubtitleFont);
        txt.SetColor(Forecolor.Title);
        txt.SetAnchor(UI::Anchor::MiddleRight, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
        txt.SetDataEffect(UI::ComponentTemplate::Text);
        txt.SetSize(100, 100, UI::Dimension::Percent);
        txt.SetSizing(UI::ComponentTemplate::ShrinkOnly);
        
        auto &ln = temp.AddGraphics(3, UI::ComponentCondition::Always);
        ln.Content.SetAnimation(Graphics::BlankImage::Get());
        ln.SetColor(Forecolor.Title);
        ln.SetSize({100, UI::Dimension::Percent}, std::max(1, int(std::round(Border.Width/2.f))));
        ln.SetMargin(Spacing*2, 0, BorderedWidgetHeight, 0);
        ln.SetAnchor(UI::Anchor::FirstBaselineRight, UI::Anchor::BottomLeft, UI::Anchor::BottomLeft);
        
        
        return temp;
    }

    UI::Template SimpleGenerator::LeadingLabel() {
        Geometry::Size defsize = {WidgetWidth * 2 + Spacing, WidgetHeight + SubtitleFont.GetHeight() - RegularFont.GetHeight()};
        
        UI::Template temp = maketemplate();
        temp.SetSpacing(Spacing);
        temp.SetSize(defsize);
        
        auto &cont = temp.AddContainer(0, UI::ComponentCondition::Always, UI::ComponentCondition::Disabled)
            .AddIndex(1) //icon
            .AddIndex(2) //text
            .AddIndex(3) //line
        ;
        cont.SetValueModification(UI::ComponentTemplate::ModifyAlpha, UI::ComponentTemplate::UseTransition);
        cont.SetValueRange(0, 1, 0.5);
        cont.SetReversible(true);
        cont.SetClip(true);
        
        auto &icon = temp.AddPlaceholder(1, UI::ComponentCondition::Icon1IsSet);
        icon.SetDataEffect(UI::ComponentTemplate::Icon);
        icon.SetAnchor(UI::Anchor::MiddleRight, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
        icon.SetSize(100, 100, UI::Dimension::Percent);
        icon.SetSizing(UI::ComponentTemplate::ShrinkOnly);
        icon.SetMargin(0, 0, Spacing, 0);
        
        auto &txt = temp.AddTextholder(2, UI::ComponentCondition::Always);
        txt.SetRenderer(BoldFont);
        txt.SetColor(Forecolor.Title);
        txt.SetAnchor(UI::Anchor::MiddleRight, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
        txt.SetDataEffect(UI::ComponentTemplate::Text);
        txt.SetSize(100, 100, UI::Dimension::Percent);
        txt.SetSizing(UI::ComponentTemplate::ShrinkOnly);
        
        auto &ln = temp.AddGraphics(3, UI::ComponentCondition::Always);
        ln.Content.SetAnimation(Graphics::BlankImage::Get());
        ln.SetColor(Forecolor.Title);
        ln.SetSize({100, UI::Dimension::Percent}, std::max(1, int(std::round(Border.Width/2.f))));
        ln.SetMargin(Spacing*2, 0, BorderedWidgetHeight, 0);
        ln.SetAnchor(UI::Anchor::FirstBaselineRight, UI::Anchor::BottomLeft, UI::Anchor::BottomLeft);
        
        
        return temp;
    }

    UI::Template SimpleGenerator::makepanel(int missingedge, bool scrollers) {
        Geometry::Size defsize = {
            WidgetWidth * 2 + Spacing + Border.Width * 2 + Spacing * 2, 
            BorderedWidgetHeight * 10 + Spacing * 9 + Border.Width * 2 + Spacing * 2
        };
        
        if(missingedge == 1 || missingedge == 3 || missingedge == -1) {
            defsize.Height = BorderedWidgetHeight + Border.Width + Spacing * 2;
        }
        
        if(missingedge == 2 || missingedge == 4 || missingedge == -1) {
            defsize.Width = WidgetWidth * 2 + Spacing + Border.Width + Spacing * 2;
        }
        
        UI::Template temp = maketemplate();
        temp.SetSize(defsize);
        
        
        auto &bg = temp.AddContainer(0, UI::ComponentCondition::Always)
            .AddIndex(1)
        ;
        
        if(missingedge > -1)
            bg.Background.SetAnimation(PanelBorder(missingedge));
        
        
        Geometry::Margin padding(Border.Width + Spacing);
        switch(missingedge) {
        case 1:
            padding.Top = Spacing;
            break;
        case 2:
            padding.Left = Spacing;
            break;
        case 3:
            padding.Bottom = Spacing;
            break;
        case 4:
            padding.Right = Spacing;
            break;
        case -1:
            padding = Spacing;
            break;
        case -2:
            padding = 0;
            break;
        }
        
        bg.SetPadding(padding);
        
        auto &vp = temp.AddContainer(1, UI::ComponentCondition::Always)
            .AddIndex(2)
        ;
        vp.SetTag(UI::ComponentTemplate::ViewPortTag);
        vp.SetSize(100, 100, UI::Dimension::Percent);
        vp.SetAnchor(UI::Anchor::TopLeft, UI::Anchor::TopLeft, UI::Anchor::TopLeft);
        vp.SetClip(true);
        
        auto &cont = temp.AddContainer(2, UI::ComponentCondition::Always);
        cont.SetTag(UI::ComponentTemplate::ContentsTag);
        cont.SetSize(0, 0, UI::Dimension::Percent);
        cont.SetSizing(UI::ComponentTemplate::Fixed);
        cont.SetPositioning(cont.Absolute);
        cont.SetAnchor(UI::Anchor::TopLeft, UI::Anchor::TopLeft, UI::Anchor::TopLeft);
        
        if(scrollers) {
            auto &vst = operator[](Scrollbar_Vertical);
            auto &hst = operator[](Scrollbar_Horizontal);
            
            temp.SetSize(temp.GetWidth()+vst.GetWidth()+Spacing, temp.GetHeight());
            
            bg
                .AddIndex(3) //VScroll
                .AddIndex(4) //HScroll
            ;
            
            auto &vs = temp.AddPlaceholder(3, UI::ComponentCondition::VScroll);
            vs.SetTemplate(vst);
            vs.SetTag(UI::ComponentTemplate::VScrollTag);
            vs.SetSize(vst.GetWidth(), {100, UI::Dimension::Percent});
            vs.SetSizing(UI::ComponentTemplate::Fixed);
            vs.SetAnchor(UI::Anchor::TopRight, UI::Anchor::TopRight, UI::Anchor::TopLeft);
            vs.SetMargin(Spacing, 0, 0, 0);
            
            auto &hs = temp.AddPlaceholder(4, UI::ComponentCondition::HScroll);
            hs.SetPositioning(UI::ComponentTemplate::Absolute);
            hs.SetTemplate(hst);
            hs.SetTag(UI::ComponentTemplate::HScrollTag);
            hs.SetSize({100, UI::Dimension::Percent}, hst.GetHeight());
            hs.SetSizing(UI::ComponentTemplate::Fixed);
            hs.SetAnchor(UI::Anchor::None, UI::Anchor::BottomCenter, UI::Anchor::BottomCenter);
            hs.SetMargin(0, Spacing, vst.GetWidth()+Spacing, 0);
            
            auto &vp = temp.AddContainer(1, UI::ComponentCondition::HScroll)
                .AddIndex(2)
            ;
            vp.SetTag(UI::ComponentTemplate::ViewPortTag);
            vp.SetSize(100, 100, UI::Dimension::Percent);
            vp.SetAnchor(UI::Anchor::TopLeft, UI::Anchor::TopLeft, UI::Anchor::TopLeft);
            vp.SetClip(true);
            vp.SetIndent(0, 0, 0, hst.GetHeight()+Spacing);
        }
        
        return temp;
    }
    
    UI::Template SimpleGenerator::BlankPanel() {
        auto tmp = makepanel(-2, true);
        
        return tmp;
    }
    
    UI::Template SimpleGenerator::Panel() {
        auto tmp = makepanel(0, true);
        
        return tmp;
    }
    
    UI::Template SimpleGenerator::TopPanel() {
        return makepanel(1, false);
    }
    
    UI::Template SimpleGenerator::LeftPanel() {
        return makepanel(2, true);
    }
    
    UI::Template SimpleGenerator::RightPanel() {
        return makepanel(4, true);
    }
    
    UI::Template SimpleGenerator::BottomPanel() {
        return makepanel(3, false);
    }
    
    UI::Template SimpleGenerator::Inputbox() {
        Geometry::Size defsize = {WidgetWidth, BorderedWidgetHeight};
        
        UI::Template temp = maketemplate();
        temp.SetSpacing(Spacing);
        temp.SetSize(defsize);
        
        
        temp.AddContainer(0, UI::ComponentCondition::Always)
            .AddIndex(1) //border
            .AddIndex(2) //boxed content
        ;
        
        auto setupborder = [&](auto &anim, UI::ComponentCondition condition) {
            auto &bg = temp.AddContainer(1, condition);
            bg.Background.SetAnimation(anim);
            bg.SetSize(100, 100, UI::Dimension::Percent);
            bg.SetPositioning(UI::ComponentTemplate::Absolute);
        };

        setupborder(NormalEditBorder(), UI::ComponentCondition::Always);
        setupborder(HoverEditBorder(), UI::ComponentCondition::Hover);
        setupborder(NormalBorder(), UI::ComponentCondition::Readonly);
        setupborder(DisabledBorder(), UI::ComponentCondition::Disabled);
        
        auto &boxed = temp.AddContainer(2, UI::ComponentCondition::Always)
            .AddIndex(3) //clip
            .AddIndex(4) //focus
        ;
        boxed.SetSize(100, 100, UI::Dimension::Percent);
        boxed.SetBorderSize(Border.Width);
        boxed.SetPadding(std::max(Border.Radius / 2, Focus.Spacing));
        boxed.SetPositioning(UI::ComponentTemplate::Absolute);
        
        auto &clip = temp.AddContainer(3, UI::ComponentCondition::Always)
            .AddIndex(5)
        ;
        clip.SetClip(true);
        clip.SetPadding(Focus.Spacing + Focus.Width);
        clip.SetSize(100, 100, UI::Dimension::Percent);
        clip.SetAnchor(UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter);
        
        //Contents
        auto &content = temp.AddContainer(5, UI::ComponentCondition::Always)
            .AddIndex(6) //text
            .AddIndex(7) //selection
            .AddIndex(8) //caret
        ;
        content.SetSize(100, 100, UI::Dimension::Percent);
        content.SetPositioning(UI::ComponentTemplate::Absolute);
        content.SetAnchor(UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter);
        content.SetTag(UI::ComponentTemplate::ViewPortTag);
        
        
        //Text
        auto setuptext = [&](Graphics::RGBA color, UI::ComponentCondition condition) {
            auto &txt = temp.AddTextholder(6, condition);
            txt.SetRenderer(RegularFont);
            txt.SetColor(color);
            txt.SetAnchor(UI::Anchor::MiddleRight, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
            txt.SetDataEffect(UI::ComponentTemplate::Text);
            txt.SetSize(100, 100, UI::Dimension::Percent);
            txt.SetPositioning(UI::ComponentTemplate::Absolute);
            txt.SetTag(UI::ComponentTemplate::ContentsTag);
        };
        
        setuptext(Forecolor.Regular, UI::ComponentCondition::Always);
        setuptext(Forecolor.Regular.BlendWith(Forecolor.Hover), UI::ComponentCondition::Hover);
        setuptext(Forecolor.Regular.BlendWith(Forecolor.Down), UI::ComponentCondition::Down);
        setuptext(Forecolor.Regular.BlendWith(Forecolor.Disabled), UI::ComponentCondition::Disabled);
        
        {
            auto &anim = *new Graphics::BitmapAnimationProvider();
            auto &img = *new Graphics::Bitmap({std::min((int)std::round(Border.Width/2.f), 1), ObjectHeight});
            img.ForAllPixels([&img, this](int x, int y) {
                img(x, y, 0) = Border.Color.R;
                img(x, y, 1) = Border.Color.G;
                img(x, y, 2) = Border.Color.B;
                img(x, y, 3) = /*(y >= (asciivsize.first - 1) && y <= (lettervsize.first + regularrenderer->GetBaseLine()+1)) **/ Border.Color.A;
            });
            drawables.Add(img);
            img.Prepare();
            auto &img2 = *new Graphics::Bitmap({std::min((int)std::round(Border.Width/2.f), 1), RegularFont.GetGlyphRenderer().GetHeight()});
            img2.Clear();
            img2.Prepare();
            drawables.Add(img2);
            
            anim.Add(img, 700);
            anim.Add(img2, 300);
            providers.Add(anim);
            
            auto &caret = temp.AddGraphics(8, UI::ComponentCondition::Focused);
            caret.Content.SetAnimation(anim);
            caret.SetPosition(0, 0, UI::Dimension::Pixel);
            caret.SetPositioning(caret.Absolute);
            caret.SetAnchor(UI::Anchor::None, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
            caret.SetTag(caret.CaretTag);
            caret.SetSizing(caret.Fixed);
            caret.SetSize(img.GetWidth(), img.GetHeight());
        }
        
        {
            auto &img = *new Graphics::BlankImage(8, 8, Background.Selected);
            drawables.Add(img);
            
            auto &selection = temp.AddGraphics(7, UI::ComponentCondition::Focused);
            selection.Content.SetDrawable(img);
            selection.SetPosition(0, 0, UI::Dimension::Pixel);
            selection.SetPositioning(selection.Absolute);
            selection.SetAnchor(UI::Anchor::None, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
            selection.SetTag(selection.SelectionTag);
            selection.SetSize(0, ObjectHeight);
            selection.SetSizing(UI::ComponentTemplate::Fixed);
        }
        
        setupfocus(temp.AddGraphics(4, UI::ComponentCondition::Focused));
        
        return temp;
    }

    UI::Template SimpleGenerator::Progressbar() {
        int h = std::max(Border.Radius * 2 + Border.Width * 2 + 4, Spacing * 3);
        Geometry::Size defsize = {WidgetWidth * 2 + Spacing, h};
        
        UI::Template temp = maketemplate();
        temp.SetSpacing(Spacing);
        temp.SetSize(defsize);
        
        {
            auto &bg = temp.AddContainer(0, UI::ComponentCondition::Always);
         
            bg.Background.SetAnimation(NormalBorder());
            bg.SetPadding(Border.Width + Spacing/2);
            
            bg.AddIndex(1);
        }
        
        {
            auto &bar = temp.AddGraphics(1, UI::ComponentCondition::Always);
            bar.SetSizing(UI::ComponentTemplate::Fixed);
            bar.SetSize({Border.Width*2}, {100, UI::Dimension::Percent});
            bar.SetPositioning(UI::ComponentTemplate::AbsoluteSliding);
            bar.SetValueModification(UI::ComponentTemplate::ModifyWidth);
            bar.Content.SetAnimation(InnerObjectShape());
            bar.SetAnchor(UI::Anchor::MiddleRight, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
        }
        
        {
            auto &bar = temp.AddGraphics(1, UI::ComponentCondition::Ch1V0);
        }
        
        return temp;
    }
    
    UI::Template SimpleGenerator::BlankLayerbox() {
        Geometry::Size defsize = {WidgetWidth * 4 + Spacing * 3, WidgetHeight * 4 + Spacing * 3};
        
        UI::Template temp = maketemplate();
        temp.SetSize(defsize);
        
        
        auto &bg = temp.AddContainer(0, UI::ComponentCondition::Always);
        bg.SetClip(true);
        
        bg.AddIndex(1);
        
        auto &cont = temp.AddContainer(1, UI::ComponentCondition::Always);
        cont.SetTag(UI::ComponentTemplate::ContentsTag);
        cont.SetSize(100, 100, UI::Dimension::Percent);
        cont.SetPositioning(cont.Absolute);
        cont.SetAnchor(UI::Anchor::TopLeft, UI::Anchor::TopLeft, UI::Anchor::TopLeft);
        cont.SetPosition(0, 0);
        
        return temp;
    }
    
    UI::Template SimpleGenerator::Layerbox() {
        Geometry::Size defsize = {WidgetWidth * 4 + Spacing * 3, WidgetHeight * 4 + Spacing * 3};
        
        UI::Template temp = maketemplate();
        temp.SetSize(defsize);
        
        
        auto &bg = temp.AddContainer(0, UI::ComponentCondition::Always);
        bg.Background.SetAnimation(PanelBorder(0));
        bg.AddIndex(1);
        bg.SetClip(true);
        
        bg.SetPadding(Border.Width + Spacing);
        
        auto &cont = temp.AddContainer(1, UI::ComponentCondition::Always);
        cont.SetTag(UI::ComponentTemplate::ContentsTag);
        cont.SetSize(100, 100, UI::Dimension::Percent);
        cont.SetSizing(UI::ComponentTemplate::Fixed);
        cont.SetPositioning(cont.Absolute);
        cont.SetAnchor(UI::Anchor::TopLeft, UI::Anchor::TopLeft, UI::Anchor::TopLeft);
        cont.SetPosition(0, 0);
        
        return temp;
    }
    
    UI::Template SimpleGenerator::VScrollbar() {
        auto dist = int(std::round(Spacing / 3.f));
        int w = std::max(Border.Radius * 2 + std::max(0, dist - Border.Radius / 2) * 2, Spacing * 2);
        
        Geometry::Size defsize = {w, BorderedWidgetHeight * 3 - Border.Width * 2};
        
        UI::Template temp = maketemplate();
        temp.SetSize(defsize);
        
        auto &cont = temp.AddContainer(0, UI::ComponentCondition::Always)
            .AddIndex(1) //handle control
        ;
        cont.Background.SetAnimation(GrooveBG());
        cont.SetPadding(dist, dist + Border.Radius/2);
        cont.SetTag(UI::ComponentTemplate::DragBarTag);
        cont.SetOrientation(Graphics::Orientation::Vertical);
        
        auto setupbar = [&](auto &border, auto cond) {
            auto &size = temp.AddGraphics(1, cond);
            size.SetValueModification(UI::ComponentTemplate::ModifyPositionAndSize, UI::ComponentTemplate::UseXW);
            size.SetPositioning(UI::ComponentTemplate::AbsoluteSliding);
            size.SetSize({100, UI::Dimension::Percent}, {w, UI::Dimension::Pixel});
            size.SetTag(UI::ComponentTemplate::DragTag);
            size.SetAnchor(UI::Anchor::None, UI::Anchor::TopCenter, UI::Anchor::MiddleCenter);
            size.Content.SetAnimation(border);
        };
        
        setupbar(NormalRBG(), UI::ComponentCondition::Always);
        setupbar(HoverRBG(), UI::ComponentCondition::Hover);
        setupbar(DownRBG(), UI::ComponentCondition::Down);
        setupbar(DisabledRBG(), UI::ComponentCondition::Disabled);
        
        //remove handle when there is nothing to scroll
        temp.AddIgnored(1, UI::ComponentCondition::Ch4V1);
        
        return temp;
    }
    
    UI::Template SimpleGenerator::HScrollbar() {
        auto dist = int(std::round(Spacing / 3.f));
        int h = std::max(Border.Radius * 2 + std::max(0, dist - Border.Radius / 2) * 2, Spacing * 2);
        
        Geometry::Size defsize = {WidgetWidth * 2 + Spacing, h};
        
        UI::Template temp = maketemplate();
        temp.SetSize(defsize);
        
        auto &cont = temp.AddContainer(0, UI::ComponentCondition::Always)
            .AddIndex(1) //handle control
        ;
        cont.Background.SetAnimation(GrooveBG());
        cont.SetPadding(dist + Border.Radius/2, dist);
        cont.SetTag(UI::ComponentTemplate::DragBarTag);
        
        auto setupbar = [&](auto &border, auto cond) {
            auto &size = temp.AddGraphics(1, cond);
            size.SetValueModification(UI::ComponentTemplate::ModifyPositionAndSize, UI::ComponentTemplate::UseXW);
            size.SetPositioning(UI::ComponentTemplate::AbsoluteSliding);
            size.SetSize({h, UI::Dimension::Pixel}, {100, UI::Dimension::Percent});
            size.SetTag(UI::ComponentTemplate::DragTag);
            size.SetAnchor(UI::Anchor::None, UI::Anchor::MiddleLeft, UI::Anchor::MiddleCenter);
            size.Content.SetAnimation(border);
        };
        
        setupbar(NormalRBG(), UI::ComponentCondition::Always);
        setupbar(HoverRBG(), UI::ComponentCondition::Hover);
        setupbar(DownRBG(), UI::ComponentCondition::Down);
        setupbar(DisabledRBG(), UI::ComponentCondition::Disabled);
        
        //remove handle when there is nothing to scroll
        temp.AddIgnored(1, UI::ComponentCondition::Ch4V1);
        
        return temp;
    }
    
    UI::Template SimpleGenerator::Listbox() {
        Geometry::Size defsize = {WidgetWidth*2+Spacing, BorderedWidgetHeight*8};
        
        UI::Template temp = maketemplate();
        
        temp.SetSpacing(Spacing);
        temp.SetSize(defsize);
        
        auto &vst = operator[](Scrollbar_Vertical);
        
        auto &bg = temp.AddContainer(0, UI::ComponentCondition::Always)
            .AddIndex(1) //content
            .AddIndex(3) //focused border
            .AddIndex(4) //listitem
            .AddIndex(5) //scrollbar
        ;
        bg.Background.SetAnimation(PanelBG());
        bg.SetPadding(std::max(Border.Radius / 2, Focus.Spacing)+Border.Width);
        
        auto &viewport = temp.AddContainer(1, UI::ComponentCondition::Always)
            .AddIndex(2)
        ;
        viewport.SetTag(UI::ComponentTemplate::ContentsTag);
        viewport.SetClip(true);
        
        auto &contents = temp.AddContainer(2, UI::ComponentCondition::Always);
        contents.SetTag(UI::ComponentTemplate::ContentsTag);
        
        //focused border
        auto &border = temp.AddContainer(3, UI::ComponentCondition::Always, UI::ComponentCondition::Focused);
        border.SetValueModification(UI::ComponentTemplate::ModifyAlpha, UI::ComponentTemplate::UseTransition);
        border.SetValueRange(0, 0.5, 1);
        border.SetReversible(true);
        border.SetPositioning(UI::ComponentTemplate::Absolute);
        border.SetMargin(-Border.Width-std::max(Border.Radius / 2, Focus.Spacing));
        border.Background.SetAnimation(NormalEmptyBorder());
        
        auto &item = temp.AddPlaceholder(4, UI::ComponentCondition::Always);
        item.SetTemplate(listbox_listitem);
        item.SetTag(UI::ComponentTemplate::ItemTag);
        
        auto &vs = temp.AddPlaceholder(5, UI::ComponentCondition::VScroll);
        vs.SetTemplate(vst);
        vs.SetTag(UI::ComponentTemplate::VScrollTag);
        vs.SetSize(vst.GetWidth(), {100, UI::Dimension::Percent});
        vs.SetSizing(UI::ComponentTemplate::Fixed);
        vs.SetAnchor(UI::Anchor::TopRight, UI::Anchor::TopRight, UI::Anchor::TopLeft);
        vs.SetMargin(Spacing, 0, 0, 0);
        
        
        //****** listitem
        
        //TODO fix height
        listbox_listitem.SetSize(defsize.Width - (Border.Width + std::max(Border.Radius / 2, Focus.Spacing)) * 2, WidgetHeight);
        
        listbox_listitem.AddContainer(0, UI::ComponentCondition::Always)
            .AddIndex(1) //background
            .AddIndex(2) //cliped content
            .AddIndex(3) //focus
        ;
        
        auto setupborder = [&](auto &anim, UI::ComponentCondition condition) {
            auto &bg = listbox_listitem.AddContainer(1, condition);
            bg.Background.SetAnimation(anim);
            bg.SetSize(100, 100, UI::Dimension::Percent);
            bg.SetPositioning(UI::ComponentTemplate::Absolute);
        };

        setupborder(AltStraightBG(), UI::ComponentCondition::Even);
        setupborder(HoverStraightBG(), UI::ComponentCondition::Hover);
        setupborder(DownStraightBG(), UI::ComponentCondition::Down);
        setupborder(DisabledStraightBG(), UI::ComponentCondition::Disabled);
        
        auto &clip = listbox_listitem.AddContainer(2, UI::ComponentCondition::Always)
            .AddIndex(5)
        ;
        clip.SetClip(true);
        clip.SetPadding(Focus.Width);
        clip.SetSize(100, 100, UI::Dimension::Percent);
        
        //Contents
        auto &content = listbox_listitem.AddContainer(5, UI::ComponentCondition::Always)
            .AddIndex(6) //icon
            .AddIndex(7) //text
        ;
        content.SetSize(100, 100, UI::Dimension::Percent);
        content.SetPositioning(UI::ComponentTemplate::Absolute);
        content.SetAnchor(UI::Anchor::None, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
        content.SetPadding(Focus.Spacing);
        
        //Contents when selected
        auto &contentsel = listbox_listitem.AddContainer(5, UI::ComponentCondition::State2)
            .AddIndex(6) //icon
            .AddIndex(9) //bg
        ;
        contentsel.SetSize(100, 100, UI::Dimension::Percent);
        contentsel.SetPositioning(UI::ComponentTemplate::Absolute);
        contentsel.SetAnchor(UI::Anchor::None, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
        contentsel.SetPadding(Focus.Spacing);
        
        
        //Icon container
        auto &iconcont = listbox_listitem.AddContainer(6, UI::ComponentCondition::Icon1IsSet)
            .AddIndex(8)
        ;
        iconcont.SetMargin(0, 0, Spacing, 0);
        iconcont.SetAnchor(UI::Anchor::MiddleRight, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
        iconcont.SetSizing(UI::ComponentTemplate::Automatic);
        
        auto setupicon = [&](auto &icon) -> auto& {
            icon.SetDataEffect(UI::ComponentTemplate::Icon1);
            icon.SetSizing(UI::ComponentTemplate::Automatic);
            
            return icon;
        };
        
        setupicon(listbox_listitem.AddGraphics(8, UI::ComponentCondition::Always));
        setupicon(listbox_listitem.AddGraphics(8, UI::ComponentCondition::Disabled)).SetColor({1.0f, 0.5f});
        
        //Text
        auto setuptext = [&](Graphics::RGBA color, UI::ComponentCondition condition, int index = 7) {
            auto &txt = listbox_listitem.AddTextholder(index, condition);
            txt.SetRenderer(RegularFont);
            txt.SetColor(color);
            txt.SetAnchor(UI::Anchor::MiddleRight, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
            txt.SetDataEffect(UI::ComponentTemplate::Text);
            txt.SetSize(100, 100, UI::Dimension::Percent);
            txt.SetMargin(0, Focus.Spacing * 2);
        };
        
        setuptext(Forecolor.Regular, UI::ComponentCondition::Always);
        setuptext(Forecolor.Regular.BlendWith(Forecolor.Hover), UI::ComponentCondition::Hover);
        setuptext(Forecolor.Regular.BlendWith(Forecolor.Down), UI::ComponentCondition::Down);
        setuptext(Forecolor.Regular.BlendWith(Forecolor.Disabled), UI::ComponentCondition::Disabled);
        
        //Text bg
        auto &textbg = listbox_listitem.AddContainer(9, UI::ComponentCondition::Always)
            .AddIndex(7)
        ;            
        auto &img = *new Graphics::BlankImage(8, 8, Background.Selected);
        drawables.Add(img);
        textbg.Background.SetAnimation(img);
        textbg.SetMargin(0, -Focus.Spacing);
        textbg.SetPadding(0, Focus.Spacing);

        setupfocus(listbox_listitem.AddGraphics(3, UI::ComponentCondition::Focused));
        
        return temp;
    }
    
    UI::Template SimpleGenerator::Dropdown() {
        Geometry::Size defsize = {BorderedWidgetHeight * 4 + Spacing * 3, BorderedWidgetHeight};
        
        UI::Template temp = maketemplate();
        temp.SetSize(defsize);
        
        
        temp.AddContainer(0, UI::ComponentCondition::Always)
            .AddIndex(1) //border
            .AddIndex(2) //boxed content
            .AddIndex(3) //Button
            .AddIndex(8) //list
        ;
        
        auto setupborder = [&](auto &anim, UI::ComponentCondition condition) {
            auto &bg = temp.AddContainer(1, condition);
            bg.Background.SetAnimation(anim);
            bg.SetSize(100, 100, UI::Dimension::Percent);
            bg.SetPositioning(UI::ComponentTemplate::Absolute);
        };

        setupborder(NormalBorder(), UI::ComponentCondition::Always);
        setupborder(HoverBorder(), UI::ComponentCondition::Hover);
        setupborder(DownBorder(), UI::ComponentCondition::Opened);
        setupborder(DisabledBorder(), UI::ComponentCondition::Disabled);
        
        auto &boxed = temp.AddContainer(2, UI::ComponentCondition::Always)
            .AddIndex(4) //clip
            .AddIndex(5) //focus
        ;
        boxed.SetSize(100, 100, UI::Dimension::Percent);
        boxed.SetBorderSize(Border.Width);
        boxed.SetPadding(std::max(Border.Radius / 2, Focus.Spacing));
        boxed.SetAnchor(UI::Anchor::None, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
        
        
        auto arrow = [&](auto color, bool upwards) {
            auto icon = new Graphics::Bitmap({int(std::round(ObjectHeight*0.9f)), int(std::round(ObjectHeight*0.67f))});
            
            icon->Clear();
            
            Geometry::PointList<Geometry::Pointf> border;
            float off = 0;
            float w = (float)icon->GetWidth();
            float h = (float)icon->GetHeight()-off;
            
            if(upwards) {   
                border = {
                    {w/2.f, 0},
                    {w, h-off},
                    {0, h-off},
                };
            }
            else {   
                border = {
                    {w/2.f, h-off},
                    {0, 0},
                    {w, 0},
                };
            }
            
            CGI::Polyfill(*icon, border, CGI::SolidFill<>(color));
            icon->Prepare();
            drawables.Add(icon);
            
            return icon;
        };
        
        auto cross = [&](auto color) {
            auto icon = new Graphics::Bitmap({int(std::round(ObjectHeight*0.7)), int(std::round(ObjectHeight*0.7))});
            
            icon->Clear();
            
            float off = ObjectBorder/2.f;
            float s = float(int(std::round(ObjectHeight*0.7)));
            float mid = s/2.f;
            
            Geometry::PointList<Geometry::Pointf> border = {
                {off, 0},
                {mid,mid-off},
                {s-off, 0},
                {s, off},
                {mid+off, mid},
                {s, s-off},
                {s-off, s},
                {mid, mid+off},
                {off, s},
                {0, s-off},
                {mid-off, mid},
                {0, off},
                {off, 0}
            };
            
            CGI::Polyfill(*icon, border, CGI::SolidFill<>(color));
            icon->Prepare();
            drawables.Add(icon);
            
            return icon;
        };
        
        auto makebutton = [&](auto color, int icon, UI::ComponentCondition cond) {
            auto &btn = temp.AddGraphics(3, cond);
            switch(icon) {
            case 0:
                btn.Content.SetAnimation(*arrow(color, false));
                break;
            case 1:
                btn.Content.SetAnimation(*arrow(color, true));
                break;
            case 2:
                btn.Content.SetAnimation(*cross(color));
                break;
            }
            btn.SetAnchor(UI::Anchor::MiddleRight, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
            btn.SetMargin(0, icon!=2 ? Spacing/2 : 0, Spacing*2, 0);
            btn.SetSizing(UI::ComponentTemplate::Automatic);
        };
        
        //TODO expand
        makebutton(Forecolor.Regular, 0, UI::ComponentCondition::Always);
        makebutton(Forecolor.Regular, 1, UI::ComponentCondition::Reversed);
        makebutton(Forecolor.Regular, 2, UI::ComponentCondition::Opened);
        
        setupfocus(temp.AddGraphics(5, UI::ComponentCondition::Focused));
        
        auto &clip = temp.AddContainer(4, UI::ComponentCondition::Always)
            .AddIndex(6) //text
            .AddIndex(7) //icon
        ;
        clip.SetClip(true);
        
        auto maketext = [&](auto color, UI::ComponentCondition cond) {
            auto &tt = temp.AddTextholder(6, cond);
            tt.SetRenderer(RegularFont);
            tt.SetColor(color);
            tt.SetAnchor(UI::Anchor::MiddleRight, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
            tt.SetDataEffect(UI::ComponentTemplate::Text);
            tt.SetSize(100, 100, UI::Dimension::Percent);
            tt.SetSizing(UI::ComponentTemplate::Fixed, UI::ComponentTemplate::Automatic);
        };
        
        maketext(Forecolor.Regular, UI::ComponentCondition::Always);
        maketext(Forecolor.Regular.BlendWith(Forecolor.Hover), UI::ComponentCondition::Hover);
        maketext(Forecolor.Regular.BlendWith(Forecolor.Down), UI::ComponentCondition::Down);
        maketext(Forecolor.Regular.BlendWith(Forecolor.Disabled), UI::ComponentCondition::Disabled);
        
        //TODO setup size
        auto &list = temp.AddPlaceholder(8, UI::ComponentCondition::Always);
        list.SetTemplate((*this)[Listbox_Regular]);
        list.SetTag(UI::ComponentTemplate::ListTag);
        list.SetPositioning(UI::ComponentTemplate::Absolute);
        
        return temp;
        
    }
    
    UI::Template SimpleGenerator::Window() {
        auto tmp = makepanel(-1, true);
        tmp.SetSize({WidgetWidth*3+Spacing*2+BorderedWidgetHeight, BorderedWidgetHeight*10});
        
        auto &cbg = dynamic_cast<UI::ContainerTemplate&>(tmp[0]);
        cbg.Background.SetAnimation(PanelBG());
        cbg.SetIndex(6);
        cbg.SetSize(100, 100, UI::Dimension::Percent);
        cbg.SetPositioning(UI::ComponentTemplate::Relative);
        cbg.SetAnchor(UI::Anchor::BottomCenter, UI::Anchor::TopCenter, UI::Anchor::TopCenter);
        
        auto addborder = [&](auto condition, auto &image) {
            auto &bg = tmp.AddContainer(0, condition)
                .AddIndex(5) //title
                .AddIndex(6) //panel
            ;
            bg.Background.SetAnimation(image);
            bg.SetPadding(Border.Width*2, Border.Width*2, Border.Width*2, Border.Width*2);
            bg.SetOrientation(Graphics::Orientation::Vertical);
            bg.SetTag(UI::ComponentTemplate::ResizeTag);            
        };
        
        addborder(UI::ComponentCondition::Always, PassiveWindowBorder());
        addborder(UI::ComponentCondition::Focused, ActiveWindowBorder());
        
        auto &titlebar = tmp.AddContainer(5, UI::ComponentCondition::Always)
            .AddIndex(7) //icon
            .AddIndex(8) //text
            .AddIndex(9) //close button
        ;
        titlebar.SetTag(UI::ComponentTemplate::DragTag);
        titlebar.SetSize({100, UI::Dimension::Percent}, WidgetHeight);
        //titlebar.Background.SetAnimation(NormalBG());
        titlebar.SetMargin(0, 0, 0, Border.Width);
        
        auto &icon = tmp.AddGraphics(7, UI::ComponentCondition::Always);
        icon.SetDataEffect(UI::ComponentTemplate::Icon);
        icon.SetAnchor(UI::Anchor::None, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
        
        auto &text = tmp.AddTextholder(8, UI::ComponentCondition::Always);
        text.SetRenderer(CenteredFont);
        text.SetSize(100, 100, UI::Dimension::Percent);
        text.SetDataEffect(UI::ComponentTemplate::Title);
        text.SetSizing(UI::ComponentTemplate::Fixed);
        text.SetTag(UI::ComponentTemplate::DragTag);
        text.SetColor(Forecolor.Inverted);
        text.SetAnchor(UI::Anchor::MiddleRight, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
        
        //close button
        auto &cb = tmp.AddPlaceholder(9, UI::ComponentCondition::Always);
        cb.SetSize(ObjectHeight, ObjectHeight);
        cb.SetSizing(UI::ComponentTemplate::Fixed);
        cb.SetTag(UI::ComponentTemplate::CloseTag);
        cb.SetAnchor(UI::Anchor::MiddleRight, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
        
        
        //close button template
        UI::Template &closebtn = *new UI::Template;
        closebtn.SetSize(ObjectHeight, ObjectHeight);
        closebtn.SetSpacing(Spacing);
        closebtn.SetUnitWidth(BorderedWidgetHeight);
        closebtn.AddContainer(0, UI::ComponentCondition::Always)
            .AddIndex(1)
            .Background.SetAnimation(NormalBorder())
        ;
        closebtn.AddContainer(0, UI::ComponentCondition::Hover)
            .AddIndex(1)
            .Background.SetAnimation(HoverBorder())
        ;
        closebtn.AddContainer(0, UI::ComponentCondition::Down)
            .AddIndex(1)
            .Background.SetAnimation(DownBorder())
        ;
        closebtn.AddContainer(0, UI::ComponentCondition::Disabled)
            .AddIndex(1)
            .Background.SetAnimation(DisabledBorder())
        ;
        closebtn.AddContainer(0, UI::ComponentCondition::Hidden);
        
        auto cross = [&](auto color) {
            int bs = int(std::round(ObjectHeight*0.6));
            if(bs%2 != ObjectHeight%2)
                bs++;
            
            auto icon = new Graphics::Bitmap({bs, bs});
            
            icon->Clear();
            
            float off = (float)ObjectBorder;
            float s = float(bs);
            float mid = s/2.f;
            
            Geometry::PointList<Geometry::Pointf> border = {
                {off, 0},
                {mid,mid-off},
                {s-off, 0},
                {s, off},
                {mid+off, mid},
                {s, s-off},
                {s-off, s},
                {mid, mid+off},
                {off, s},
                {0, s-off},
                {mid-off, mid},
                {0, off},
                {off, 0}
            };
            
            CGI::Polyfill(*icon, border, CGI::SolidFill<>(color));
            icon->Prepare();
            drawables.Add(icon);
            
            return icon;
        };
        
        {
        auto &sym = closebtn.AddGraphics(1, UI::ComponentCondition::Always);
        sym.Content.SetAnimation(*cross(Forecolor.Regular));
        sym.SetAnchor(UI::Anchor::None, UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter);
        }
        
        {
        auto &sym = closebtn.AddGraphics(1, UI::ComponentCondition::Disabled);
        sym.Content.SetAnimation(*cross(Forecolor.Regular));
        sym.SetAnchor(UI::Anchor::None, UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter);
        sym.SetColor({1.0f, 0.5f});
        }
        
        cb.OwnTemplate(closebtn);
        
        
        return tmp;
    }
    
    UI::Template SimpleGenerator::DialogWindow() {
        auto temp = Window();
        temp.SetSize({WidgetWidth*2+Spacing+BorderedWidgetHeight, BorderedWidgetHeight*5});
        
        auto &cbg = dynamic_cast<UI::ContainerTemplate&>(temp[0]);
        cbg.Background.SetAnimation(PanelBG(3));
        
        int maxind = 0;
        for(auto &c : temp) {
            if(maxind < c.GetIndex())
                maxind = c.GetIndex();
        }
        
        maxind++;
        
        for(auto &c : temp) {
            if(c.GetIndex() == 0) {
                auto rootp = dynamic_cast<UI::ContainerTemplate*>(&c);
                if(rootp) {
                    rootp->AddIndex(maxind); //dialog button place
                }
            }
        }
        
        auto &btndiag = operator[](Button_Dialog);
        
        auto &btnplace = temp.AddContainer(maxind, UI::ComponentCondition::Always);
        btnplace.SetSize({100, UI::Dimension::Percent}, btndiag.GetHeight());
        btnplace.SetMargin(0, Spacing, 0, 0);
        btnplace.SetPositioning(UI::ComponentTemplate::Relative);
        btnplace.SetAnchor(UI::Anchor::BottomCenter, UI::Anchor::TopCenter, UI::Anchor::TopCenter);
        btnplace.SetTag(UI::ComponentTemplate::DialogButtonsTag);

        auto &btn = temp.AddPlaceholder(maxind+1, UI::ComponentCondition::Always);
        btn.SetTemplate(btndiag);
        btn.SetTag(UI::ComponentTemplate::ButtonTag);
        
        
        return temp;
    }
    
}}
