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

    SimpleGenerator::SimpleGenerator(int fontsize, std::string fontname, bool activate) : Generator(activate) {
        Init(fontsize, fontname);
    }
    
    void SimpleGenerator::Init(int fontsize, std::string fontname) {
#ifdef WIN32
        if(fontname.find_last_of('.') == fontname.npos)
            fontname = Filesystem::Join(Filesystem::Join(OS::GetEnvVar("WINDIR"), "Fonts"), fontname == "" ? "tahoma.ttf" : fontname + ".ttf");
#else
        if(fontname.find_last_of('.') == fontname.npos) {
            bool found = false;
            
            try {
                std::streambuf *buf;
                OS::Start("fc-match", buf, {"-v", fontname == "" ? "sans" : fontname});

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
                fontname = "/usr/share/fonts/gnu-free/FreeSans.ttf";
        }
#endif

        auto &regular = *new Graphics::FreeType();
        regular.LoadFile(fontname, fontsize);

        RegularFont.SetGlyphRenderer(regular);
        CenteredFont.SetGlyphRenderer(regular);
        CenteredFont.AlignCenter();

        /*if(!regular.HasKerning()) {
            auto &bmpfnt = *new Graphics::BitmapFont(regular.MoveOutBitmap());
            RegularFont.SetGlyphRenderer(bmpfnt);
            bmpfnt.AutoKern();
            regularrenderer = &bmpfnt;

            delete &regular;
        }
        else {*/
        regularrenderer = &regular;
        //}
        
        UpdateDimensions();
        UpdateBorders();
        UpdateDimensions();
    }

    SimpleGenerator::~SimpleGenerator() {
        if(regularrenderer)
            delete regularrenderer;

        providers.DeleteAll();
        drawables.DeleteAll();
        
        delete normalborder;
        delete hoverborder;
        delete downborder;
        delete disabledborder;
        delete normaleditborder;
        delete hovereditborder;
        delete readonlyborder;
        delete focusborder;
        delete normalemptyborder;
        delete normalbg;
        delete hoverbg;
        delete downbg;
        delete disabledbg;
        delete objectshape;
        delete innerobjectshape;
        
        for(auto p : panelborders) {
            delete p;
        }
    }
    
    void SimpleGenerator::UpdateDimensions() {
        lettervsize = regularrenderer->GetLetterHeight();
        asciivsize = regularrenderer->GetLetterHeight(true);
        int totalh = lettervsize.first + lettervsize.second - 1;

        Spacing = (int)std::round((float)totalh / 5);
        Focus.Spacing = std::max(1, Spacing / 2);

        BorderedWidgetHeight = 
            totalh + Border.Width * 2 + 
            std::max(Border.Radius, Focus.Spacing * 2) + 
            Focus.Width * 2 + Focus.Spacing * 2
        ;
        
        WidgetHeight = totalh + Focus.Width * 2 + Focus.Spacing * 2;
        
        WidgetWidth = 3 * BorderedWidgetHeight + 2 * Spacing;
        
        ObjectHeight = totalh;
    }
    
    void SimpleGenerator::UpdateBorders(bool smooth) {
        Border.Width  = (int)std::max(std::round(regularrenderer->GetLineThickness()*2.6f), 1.f);
        ShapeBorder   = std::max(regularrenderer->GetLineThickness()*2.6f, 1.f);

        //limit the thickness after 2.
        if(Border.Width > 2) {
            Border.Width = (int)std::max(std::round(regularrenderer->GetLineThickness()*2.4f), 1.f);
            ShapeBorder  = std::max(regularrenderer->GetLineThickness()*2.4f, 1.f);
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
            auto c = Background.Regular;
            c.Blend(Background.Disabled);
            auto c2 = Border.Color;
            c2.Blend(Border.Disabled);

            disabledborder = makeborder(c2, c);
        }

        return *disabledborder;
    }
    
    Graphics::BitmapRectangleProvider &SimpleGenerator::PanelBorder(int missingedge) {
        if(!panelborders[missingedge])
            panelborders[missingedge] = makeborder(Border.Color, Background.Panel, missingedge);
        
        return *panelborders[missingedge];
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
        
        int bsize = (w + r + 2) * 2 + 16;
        float off = float(w / 2.0f); //prefer integers
        
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
            int(r+w+2), 
            int(r+w+2), 
            int(bsize-r-w-2),
            int(bsize-r-w-2)
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
    
    void SimpleGenerator::setupfocus(UI::ContainerTemplate &foc) {
        foc.Background.SetAnimation(FocusBorder());
        foc.SetSize(100, 100, UI::Dimension::Percent);
        foc.SetPositioning(UI::ComponentTemplate::Absolute);
        foc.SetAnchor(UI::Anchor::None, UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter);
    }
    
    UI::Template SimpleGenerator::Button() {
        Geometry::Size defsize = {WidgetWidth, BorderedWidgetHeight};
        
        UI::Template temp;
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

        setupborder(NormalBorder(), UI::ComponentCondition::Always);
        setupborder(HoverBorder(), UI::ComponentCondition::Hover);
        setupborder(DownBorder(), UI::ComponentCondition::Down);
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
        
        //Contents
        auto &content = temp.AddContainer(5, UI::ComponentCondition::Always)
            .AddIndex(6) //icon
            .AddIndex(7) //text
        ;
        content.SetSize(100, 100, UI::Dimension::Percent);
        content.SetSize(100, 100, UI::Dimension::Percent);
        content.SetSizing(UI::ComponentTemplate::Automatic);
        content.SetPositioning(UI::ComponentTemplate::Absolute);
        content.SetAnchor(UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter);
        
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
            txt.SetSize(100, 100, UI::Dimension::Percent);
            txt.SetSizing(UI::ComponentTemplate::ShrinkOnly);
        };
        
        setuptext(Forecolor.Regular, UI::ComponentCondition::Always);
        setuptext(Forecolor.Regular.BlendWith(Forecolor.Hover), UI::ComponentCondition::Hover);
        setuptext(Forecolor.Regular.BlendWith(Forecolor.Down), UI::ComponentCondition::Down);
        setuptext(Forecolor.Regular.BlendWith(Forecolor.Disabled), UI::ComponentCondition::Disabled);

        setupfocus(temp.AddContainer(4, UI::ComponentCondition::Focused));
        
        temp.Name = String::Concat("Button ", rand()%100);

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
        
        UI::Template temp;
        
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
        
        setupfocus(temp.AddContainer(4, UI::ComponentCondition::Focused));
        
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

    UI::Template SimpleGenerator::Checkbox() {
        Geometry::Size defsize = {WidgetWidth * 2 + Spacing, WidgetHeight};
        
        UI::Template temp;
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
                {ShapeBorder*2.4f, ObjectHeight/2.f},
                {ObjectHeight*0.45f, ObjectHeight-ShapeBorder*2.4f},
                {ObjectHeight-ShapeBorder*2.5f, ShapeBorder*2.4f}
            };
            
            if(ObjectHeight - ShapeBorder*4.8f < 3) {
                CGI::Polyfill(*icon, Geometry::PointList<Geometry::Pointf>{
                    {ObjectBorder*2.f, ObjectBorder*2.f},
                    {ObjectHeight - ObjectBorder*2.f, ObjectBorder*2.f},
                    {ObjectHeight - ObjectBorder*2.f, ObjectHeight - ObjectBorder*2.f},
                    {ObjectBorder*2.f, ObjectHeight - ObjectBorder*2.f},
                }, CGI::SolidFill<>(color));
            }
            else {
                CGI::DrawLines(*icon, tick, 1.2*ShapeBorder, CGI::SolidFill<>(color));
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
        
        setupfocus(temp.AddContainer(2, UI::ComponentCondition::Focused));
        
        return temp;
    }
    
    UI::Template SimpleGenerator::CheckboxButton() {
        
        UI::Template temp;
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
        border.SetValueRange(0, 0.5, 1);
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
        
        setupfocus(temp.AddContainer(4, UI::ComponentCondition::Focused));
        
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
        
        UI::Template temp;
        temp.SetSize(defsize);
        
        auto &cont = temp.AddContainer(0, UI::ComponentCondition::Always);
        cont.AddIndex(1);
        cont.AddIndex(3);
        cont.AddIndex(4);
        cont.SetPadding(Focus.Width);
        
        auto &cont2 = temp.AddContainer(0, UI::ComponentCondition::State2);
        cont2.AddIndex(2);
        cont2.AddIndex(3);
        cont2.AddIndex(4);
        cont2.SetPadding(Focus.Width);
        
        int outer_r = ObjectHeight / 2;
        int borderstart_r = outer_r - ObjectBorder;
        float inner_r = borderstart_r - Spacing / 2.f;
        
        Geometry::Size bmpsize = {ObjectHeight + 2, ObjectHeight + 2};
        Geometry::Pointf center = {float(outer_r + 1), float(outer_r + 1)};
        
        auto blank = [&](auto border) {
            auto icon = new Graphics::Bitmap(bmpsize);
            
            icon->Clear();
            
            CGI::Circle<16>(*icon, center, (float)borderstart_r, (float)ObjectBorder, CGI::SolidFill<>(border));

            icon->Prepare();
            drawables.Add(icon);
            
            return icon;
        };
        
        auto filled = [&](auto color) {
            auto icon = blank( color);
            
            CGI::Circle<16>(*icon, center, (float)inner_r, CGI::SolidFill<>(color));
            icon->Prepare();
            
            return icon;
        };
        
        auto makestate = [&](auto color, UI::ComponentCondition condition) {
            auto &it = temp.AddGraphics(1, condition);
            it.Content.SetDrawable(*blank(color));
            it.SetSizing(it.Automatic);
            it.SetIndent(Spacing, 0, 0, 0);
            
            auto &it2 = temp.AddGraphics(2, condition);
            it2.Content.SetDrawable(*filled(color));
            it2.SetSizing(it2.Automatic);
            it2.SetIndent(Spacing, 0, 0, 0);
            
            auto &tt = temp.AddTextholder(3, condition);
            tt.SetRenderer(RegularFont);
            tt.SetColor(color);
            tt.SetMargin(Spacing, 0, 0, 0);
            tt.SetAnchor(UI::Anchor::MiddleRight, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
            tt.SetDataEffect(UI::ComponentTemplate::Text);
            tt.SetClip(true);
            tt.SetSize(100, 100, UI::Dimension::Percent);
            tt.SetSizing(UI::ComponentTemplate::ShrinkOnly, UI::ComponentTemplate::ShrinkOnly);
        };

        makestate(Forecolor.Regular, UI::ComponentCondition::Always);
        makestate(Forecolor.Regular.BlendWith(Forecolor.Hover), UI::ComponentCondition::Hover);
        makestate(Forecolor.Regular.BlendWith(Forecolor.Down), UI::ComponentCondition::Down);
        makestate(Forecolor.Regular.BlendWith(Forecolor.Disabled), UI::ComponentCondition::Disabled);

        {
            auto &foc = temp.AddContainer(4, UI::ComponentCondition::Focused);
            foc.Background.SetAnimation(FocusBorder());
            foc.SetSize(100, 100, UI::Dimension::Percent);
            foc.SetIndent(-Focus.Width);
            foc.SetPositioning(foc.Absolute);
            foc.SetAnchor(UI::Anchor::None, UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter);
        }
        return temp;
    }
    
    UI::Template SimpleGenerator::Label() {
        Geometry::Size defsize = {WidgetWidth * 2 + Spacing, WidgetHeight};
        
        UI::Template temp;
        temp.SetSize(defsize);
        
        auto &bi = *new Graphics::BlankImage({Border.Width, Border.Width}, Border.Color);
        drawables.Add(bi);
        
        {
            auto &bg_n = temp.AddContainer(0, UI::ComponentCondition::Always);
            bg_n.SetSizing(UI::ComponentTemplate::Automatic);
            //assuming border radius = 0
            bg_n.SetPadding(0);
            bg_n.AddIndex(1);
            bg_n.AddIndex(2);
            bg_n.AddIndex(3);
        }
        
        {
            auto &icon = temp.AddPlaceholder(1, UI::ComponentCondition::Icon1IsSet);
            icon.SetDataEffect(UI::ComponentTemplate::Icon);
            icon.SetAnchor(UI::Anchor::MiddleRight, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
            icon.SetSize(100, 100, UI::Dimension::Percent);
            icon.SetSizing(UI::ComponentTemplate::ShrinkOnly);
            icon.SetMargin(0, 0, Spacing, 0);
        }
        
        {
            auto &foc = temp.AddContainer(2, UI::ComponentCondition::Focused);
            foc.Background.SetAnimation(FocusBorder());
            foc.SetMargin(Spacing / 2);
            foc.SetSize(100, 100, UI::Dimension::Percent);
            foc.SetPositioning(foc.Absolute);
            foc.SetAnchor(UI::Anchor::None, UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter);
        }

        {
            auto &txt_n = temp.AddTextholder(3, UI::ComponentCondition::Always);
            txt_n.SetRenderer(RegularFont);
            txt_n.SetColor(Forecolor.Regular);
            txt_n.SetAnchor(UI::Anchor::MiddleRight, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
            txt_n.SetDataEffect(UI::ComponentTemplate::Text);
            txt_n.SetClip(true);
            txt_n.SetSize(100, 100, UI::Dimension::Percent);
            txt_n.SetSizing(UI::ComponentTemplate::ShrinkOnly);
        }

        {
            auto &txt_n = temp.AddTextholder(3, UI::ComponentCondition::Disabled);
            txt_n.SetRenderer(RegularFont);
            auto c = Forecolor.Regular;
            c.Blend(Forecolor.Disabled);
            txt_n.SetColor(c);
            txt_n.SetAnchor(UI::Anchor::MiddleRight, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
            txt_n.SetDataEffect(UI::ComponentTemplate::Text);
            txt_n.SetClip(true);
            txt_n.SetSize(100, 100, UI::Dimension::Percent);
            txt_n.SetSizing(UI::ComponentTemplate::ShrinkOnly);
        }

        return temp;
    }

    UI::Template SimpleGenerator::ErrorLabel() {
        Geometry::Size defsize = {WidgetWidth * 2 + Spacing, WidgetHeight};
        
        UI::Template temp;
        temp.SetSize(defsize);

        auto &bi = *new Graphics::BlankImage({ Border.Width, Border.Width }, Border.Color);
        drawables.Add(bi);

        {
            auto &bg_n = temp.AddContainer(0, UI::ComponentCondition::Always);
            bg_n.SetSizing(UI::ComponentTemplate::Automatic);
            //assuming border radius = 0
            bg_n.SetPadding(0);
            bg_n.AddIndex(1);
            bg_n.AddIndex(2);
            bg_n.AddIndex(3);
        }

        {
            auto &icon = temp.AddPlaceholder(1, UI::ComponentCondition::Icon1IsSet);
            icon.SetDataEffect(UI::ComponentTemplate::Icon);
            icon.SetAnchor(UI::Anchor::MiddleRight, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
            icon.SetSize(100, 100, UI::Dimension::Percent);
            icon.SetSizing(UI::ComponentTemplate::ShrinkOnly);
            icon.SetMargin(0, 0, Spacing, 0);
        }

        {
            auto &foc = temp.AddContainer(2, UI::ComponentCondition::Focused);

            auto &ci = Graphics::EmptyImage::Instance();


            auto &hi = *new Graphics::Bitmap({ 2, Focus.Width });
            hi.Clear();
            for (auto i = 0; i < Focus.Width; i++)
                hi.SetRGBAAt(0, i, Focus.Color);
            hi.Prepare();
            drawables.Add(hi);

            auto &vi = *new Graphics::Bitmap({ Focus.Width, 2 });
            vi.Clear();
            for (auto i = 0; i < Focus.Width; i++)
                vi.SetRGBAAt(i, 0, Focus.Color);
            vi.Prepare();
            drawables.Add(vi);

            auto &cri = *new Graphics::BlankImage(Focus.Width, Focus.Width, Focus.Color);

            auto &rect = *new Graphics::RectangleProvider(cri, hi, cri, vi, ci, vi, cri, hi, cri);

            foc.Background.SetAnimation(rect);
            providers.Add(rect);
            foc.SetMargin(Spacing / 2);
            foc.SetSize(100, 100, UI::Dimension::Percent);
            foc.SetPositioning(foc.Absolute);
            foc.SetAnchor(UI::Anchor::None, UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter);
        }

        {
            auto &txt_n = temp.AddTextholder(3, UI::ComponentCondition::Always);
            txt_n.SetRenderer(RegularFont);
            txt_n.SetColor(Forecolor.Error);
            txt_n.SetAnchor(UI::Anchor::MiddleRight, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
            txt_n.SetDataEffect(UI::ComponentTemplate::Text);
            txt_n.SetClip(true);
            txt_n.SetSize(100, 100, UI::Dimension::Percent);
            txt_n.SetSizing(UI::ComponentTemplate::ShrinkOnly);
        }

        return temp;
    }
    
    UI::Template SimpleGenerator::BlankPanel() {
        Geometry::Size defsize = {WidgetWidth * 4 + Spacing * 5, WidgetHeight * 10 + Spacing * 11};
        
        UI::Template temp;
        temp.SetSize(defsize);
        
        
        auto &bg = temp.AddContainer(0, UI::ComponentCondition::Always);
        bg.SetClip(true);
        
        bg.AddIndex(1);
        
        auto &cont = temp.AddContainer(1, UI::ComponentCondition::Always);
        cont.SetTag(UI::ComponentTemplate::ContentsTag);
        cont.SetValueModification(cont.ModifyPosition, cont.UseXY);
        cont.SetSize(100, 100, UI::Dimension::Percent);
        cont.SetPositioning(cont.Absolute);
        cont.SetAnchor(UI::Anchor::TopLeft, UI::Anchor::TopLeft, UI::Anchor::TopLeft);
        cont.SetPosition(0, 0);
        //bg.Background.SetAnimation(NormalBorder());
        
        return temp;
    }
    
    UI::Template SimpleGenerator::makepanel(int missingedge) {
        Geometry::Size defsize = {
            WidgetWidth * 4 + Spacing * 3 + Border.Width * 2 + Spacing * 2, 
            BorderedWidgetHeight * 10 + Spacing * 9 + Border.Width * 2 + Spacing * 2
        };
        
        if(missingedge == 1 || missingedge == 3) {
            defsize.Height = BorderedWidgetHeight + Border.Width + Spacing * 2;
        }
        
        if(missingedge == 2 || missingedge == 4) {
            defsize.Width = WidgetWidth * 2 + Spacing + Border.Width + Spacing * 2;
        }
        
        UI::Template temp;
        temp.SetSize(defsize);
        
        
        auto &bg = temp.AddContainer(0, UI::ComponentCondition::Always);
        bg.Background.SetAnimation(PanelBorder(missingedge));
        bg.AddIndex(1);
        
        
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
        }
        
        bg.SetPadding(padding);
        
        auto &vp = temp.AddContainer(1, UI::ComponentCondition::Always);
        vp.SetTag(UI::ComponentTemplate::ViewPortTag);
        vp.SetSize(100, 100, UI::Dimension::Percent);
        vp.SetPositioning(vp.Absolute);
        vp.SetAnchor(UI::Anchor::TopLeft, UI::Anchor::TopLeft, UI::Anchor::TopLeft);
        vp.SetPosition(0, 0);
        vp.SetClip(true);
        vp.AddIndex(2);
        //vp.Background.SetAnimation(HoverBorder());
        
        auto &cont = temp.AddContainer(2, UI::ComponentCondition::Always);
        cont.SetTag(UI::ComponentTemplate::ContentsTag);
        //cont.SetValueModification(cont.ModifyPosition, cont.UseXY);
        cont.SetSize(0, 0, UI::Dimension::Percent);
        cont.SetSizing(UI::ComponentTemplate::Fixed);
        cont.SetPositioning(cont.Absolute);
        cont.SetAnchor(UI::Anchor::TopLeft, UI::Anchor::TopLeft, UI::Anchor::TopLeft);
        cont.SetPosition(0, 0);
        //cont.Background.SetAnimation(DownBorder());
        
        return temp;
    }
    
    UI::Template SimpleGenerator::Panel() {
        return makepanel(0);
    }
    
    UI::Template SimpleGenerator::TopPanel() {
        return makepanel(1);
    }
    
    UI::Template SimpleGenerator::LeftPanel() {
        return makepanel(2);
    }
    
    UI::Template SimpleGenerator::RightPanel() {
        return makepanel(4);
    }
    
    UI::Template SimpleGenerator::BottomPanel() {
        return makepanel(3);
    }
    
    UI::Template SimpleGenerator::Inputbox() {
        Geometry::Size defsize = {WidgetWidth * 2 + Spacing, BorderedWidgetHeight};
        
        UI::Template temp;
        temp.SetSize(defsize);

        {
            auto &bg = temp.AddContainer(0, UI::ComponentCondition::Always);
            bg.AddIndex(5);
        }

        {
            auto &bg = temp.AddContainer(0, UI::ComponentCondition::Readonly);
            bg.AddIndex(6);
        }
        
        {
            auto &bg_n = temp.AddContainer(5, UI::ComponentCondition::Always);
            bg_n.SetPadding(Spacing + Border.Width , UI::Dimension::Pixel);
            bg_n.AddIndex(1);
            bg_n.AddIndex(2);
            bg_n.AddIndex(3);
            bg_n.AddIndex(4);
            bg_n.SetSize(100, 100, UI::Dimension::Percent);
            bg_n.SetClip(true); //!Shadow
            bg_n.Background.SetAnimation(NormalEditBorder());
        }

        {
            auto &bg_h = temp.AddContainer(5, UI::ComponentCondition::Hover);
            bg_h.SetPadding(Spacing + Border.Width);
            bg_h.AddIndex(1);
            bg_h.AddIndex(2);
            bg_h.AddIndex(3);
            bg_h.AddIndex(4);
            bg_h.SetSize(100, 100, UI::Dimension::Percent);
            bg_h.SetClip(true); //!Shadow
            bg_h.Background.SetAnimation(HoverEditBorder());
        }

        {
            auto &bg_r = temp.AddContainer(6, UI::ComponentCondition::Readonly);
            bg_r.SetPadding(Spacing + Border.Width);
            bg_r.AddIndex(1);
            bg_r.AddIndex(2);
            bg_r.AddIndex(3);
            bg_r.AddIndex(4);
            bg_r.SetSize(100, 100, UI::Dimension::Percent);
            bg_r.SetClip(true); //!Shadow
            bg_r.Background.SetAnimation(DisabledBorder());
        }

        {
            auto &bg_d = temp.AddContainer(5, UI::ComponentCondition::Disabled);
            bg_d.SetPadding(Spacing + Border.Width);
            bg_d.AddIndex(1);
            bg_d.AddIndex(2);
            bg_d.AddIndex(3);
            bg_d.AddIndex(4);
            bg_d.SetSize(100, 100, UI::Dimension::Percent);
            bg_d.SetClip(true); //!Shadow
            bg_d.Background.SetAnimation(DisabledBorder());
        }
        
        {
            auto &foc = temp.AddContainer(1, UI::ComponentCondition::Focused);
            foc.Background.SetAnimation(FocusBorder());
            foc.SetIndent(-Focus.Width*2);
            foc.SetSize(100, 100, UI::Dimension::Percent);
            foc.SetPositioning(foc.Absolute);
            foc.SetAnchor(UI::Anchor::None, UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter);
        }

        {
            auto &txt_n = temp.AddTextholder(2, UI::ComponentCondition::Always);
            txt_n.SetRenderer(RegularFont);
            txt_n.SetColor(Forecolor.Regular);
            txt_n.SetAnchor(UI::Anchor::MiddleRight, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
            txt_n.SetDataEffect(UI::ComponentTemplate::Text);
            txt_n.SetTag(txt_n.ContentsTag);
            txt_n.SetSize({100, UI::Dimension::Percent}, lettervsize.first+lettervsize.second);
            txt_n.SetSizing(UI::ComponentTemplate::Fixed);
        }

        {
            
            auto c = Forecolor.Regular;
            c.Blend(Forecolor.Hover);
            auto &txt_h = temp.AddTextholder(2, UI::ComponentCondition::Hover);
            txt_h.SetRenderer(RegularFont);
            txt_h.SetColor(c);
            txt_h.SetAnchor(UI::Anchor::MiddleRight, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
            txt_h.SetDataEffect(UI::ComponentTemplate::Text);
            txt_h.SetTag(txt_h.ContentsTag);
            txt_h.SetSize({100, UI::Dimension::Percent}, lettervsize.first+lettervsize.second);
            txt_h.SetSizing(UI::ComponentTemplate::Fixed);
        }

        {
            auto c = Forecolor.Regular;
            c.Blend(Forecolor.Disabled);
            auto &txt_d = temp.AddTextholder(2, UI::ComponentCondition::Disabled);
            txt_d.SetRenderer(RegularFont);
            txt_d.SetColor(c);
            txt_d.SetAnchor(UI::Anchor::MiddleRight, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
            txt_d.SetDataEffect(UI::ComponentTemplate::Text);
            txt_d.SetTag(txt_d.ContentsTag);
            txt_d.SetSize({100, UI::Dimension::Percent}, lettervsize.first+lettervsize.second);
            txt_d.SetSizing(UI::ComponentTemplate::Fixed);
        }

        {
            auto &anim = *new Graphics::BitmapAnimationProvider();
            int h = lettervsize.first + lettervsize.second;
            auto &img = *new Graphics::Bitmap({std::min((int)std::round(Border.Width/2.f), 1), h});
            img.ForAllPixels([&img, this, h](int x, int y) {
                img(x, y, 0) = Border.Color.R;
                img(x, y, 1) = Border.Color.G;
                img(x, y, 2) = Border.Color.B;
                img(x, y, 3) = (y >= (asciivsize.first - 1) && y <= (lettervsize.first + regularrenderer->GetBaseLine()+1)) * Border.Color.A;
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
            
            auto &caret = temp.AddGraphics(3, UI::ComponentCondition::Focused);
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
            
            int h = RegularFont.GetGlyphRenderer().GetHeight();
            
            auto &selection = temp.AddGraphics(4, UI::ComponentCondition::Focused);
            selection.Content.SetDrawable(img);
            selection.SetPosition(0, 0, UI::Dimension::Pixel);
            selection.SetPositioning(selection.Absolute);
            selection.SetAnchor(UI::Anchor::None, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
            selection.SetTag(selection.SelectionTag);
            selection.SetSize(10, lettervsize.second + lettervsize.first + 1);
            selection.SetSizing(UI::ComponentTemplate::Fixed);
        }
        
        return temp;
    }

    UI::Template SimpleGenerator::Progressbar() {
        Geometry::Size defsize = {WidgetWidth * 2 + Spacing, WidgetHeight};
        
        UI::Template temp;
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
    
}}
