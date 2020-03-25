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
        
        for(auto p : panelborders) {
            delete p;
        }
    }
    
    void SimpleGenerator::UpdateDimensions() {
        lettervsize = regularrenderer->GetLetterHeight();
        asciivsize = regularrenderer->GetLetterHeight(true);
        int totalh = lettervsize.first + lettervsize.second;

        Spacing = (int)std::round((float)totalh / 4);

        WidgetWidth = regularrenderer->GetDigitWidth() * 8 + Border.Width * 2 + Border.Radius + Spacing * 3;
        BorderedWidgetHeight = totalh + Border.Radius / 2 + Spacing * 2 + Border.Width * 2;
        WidgetHeight = totalh + Focus.Width * 2;
        
        ObjectHeight = asciivsize.second;
    }
    
    void SimpleGenerator::UpdateBorders(bool smooth) {
        Border.Width  = (int)std::max(std::round(regularrenderer->GetLineThickness()*2.6f), 1.f);
        ObjectBorder  = Border.Width;
        Border.Radius = (int)std::round(asciivsize.second / 4.f);
        Border.Divisions = smooth * Border.Radius  / 2;
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
    
    Graphics::BitmapRectangleProvider *SimpleGenerator::makeborder(Graphics::RGBA border, Graphics::RGBA bg, int missingedge) {
        int bsize = (Border.Width + Border.Radius + 2) * 2 + 16;
        float off = float((Border.Width + 1) / 2 + 1); //prefer integers
        
        auto &bi = *new Graphics::Bitmap({bsize, bsize}, Graphics::ColorMode::RGBA);
        bi.Clear();
        
        if(Border.Radius == 0) {
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
            
            CGI::DrawLines(bi.GetData(), list, (float)Border.Width, CGI::SolidFill<>(border));
        }
        else {
            auto r = Border.Radius;
            
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
            
            CGI::DrawLines(bi.GetData(), list, (float)Border.Width, CGI::SolidFill<>(border));
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
            int(Border.Radius+Border.Width+1), 
            int(Border.Radius+Border.Width+1), 
            int(bsize-Border.Radius-Border.Width-1),
            int(bsize-Border.Radius-Border.Width-1)
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
    
    UI::Template SimpleGenerator::Button() {
        Geometry::Size defsize = {WidgetWidth, BorderedWidgetHeight};
        
        UI::Template temp;
        temp.SetSize(defsize);

        {
            auto &bg_n = temp.AddContainer(0, UI::ComponentCondition::Always);
            bg_n.SetSizing(UI::ComponentTemplate::Automatic);

            bg_n.Background.SetAnimation(NormalBorder());
            bg_n.SetPadding(Spacing);
            bg_n.AddIndex(1);
            bg_n.AddIndex(2);
        }

        {
            auto &txt_n = temp.AddTextholder(1, UI::ComponentCondition::Always);
            txt_n.SetRenderer(RegularFont);
            txt_n.SetColor(Forecolor.Regular);
            txt_n.SetAnchor(UI::Anchor::None, UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter);
            txt_n.SetDataEffect(UI::ComponentTemplate::Text);
            txt_n.SetClip(true);
            txt_n.SetSize(100, 100, UI::Dimension::Percent);
            txt_n.SetSizing(UI::ComponentTemplate::ShrinkOnly);
        }

        {
            auto &bg_h = temp.AddContainer(0, UI::ComponentCondition::Hover);
            bg_h.SetSizing(UI::ComponentTemplate::Automatic);
            
            bg_h.Background.SetAnimation(HoverBorder());
            bg_h.SetPadding(Spacing);
            bg_h.AddIndex(1);
            bg_h.AddIndex(2);
        }

        {
            auto &txt_h = temp.AddTextholder(1, UI::ComponentCondition::Hover);
            
            auto c = Forecolor.Regular;
            c.Blend(Forecolor.Hover);
            txt_h.SetRenderer(RegularFont);
            txt_h.SetColor(c);
            txt_h.SetAnchor(UI::Anchor::None, UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter);
            txt_h.SetDataEffect(UI::ComponentTemplate::Text);
            txt_h.SetClip(true);
            txt_h.SetSize(100, 100, UI::Dimension::Percent);
            txt_h.SetSizing(UI::ComponentTemplate::ShrinkOnly, UI::ComponentTemplate::ShrinkOnly);
        }

        {
            auto &bg_d = temp.AddContainer(0, UI::ComponentCondition::Down);
            bg_d.SetSizing(UI::ComponentTemplate::Automatic);

            bg_d.Background.SetAnimation(DownBorder());
            bg_d.SetPadding(Spacing);
            bg_d.AddIndex(1);
            bg_d.AddIndex(2);
        }

        {
            auto &txt_d = temp.AddTextholder(1, UI::ComponentCondition::Down);

            auto c = Forecolor.Regular;
            c.Blend(Forecolor.Down);
            txt_d.SetRenderer(RegularFont);
            txt_d.SetColor(c);
            txt_d.SetAnchor(UI::Anchor::None, UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter);
            txt_d.SetDataEffect(UI::ComponentTemplate::Text);
            txt_d.SetClip(true);
            txt_d.SetSize(100, 100, UI::Dimension::Percent);
            txt_d.SetSizing(UI::ComponentTemplate::ShrinkOnly, UI::ComponentTemplate::ShrinkOnly);
        }

        {
            auto &bg_d = temp.AddContainer(0, UI::ComponentCondition::Disabled);
            bg_d.SetSizing(UI::ComponentTemplate::Automatic);

            bg_d.Background.SetAnimation(DisabledBorder());
            bg_d.SetPadding(Spacing);
            bg_d.AddIndex(1);
            bg_d.AddIndex(2);
        }

        {
            auto &txt_d = temp.AddTextholder(1, UI::ComponentCondition::Disabled);

            auto c = Forecolor.Regular;
            c.Blend(Forecolor.Disabled);
            txt_d.SetRenderer(RegularFont);
            txt_d.SetColor(c);
            txt_d.SetAnchor(UI::Anchor::None, UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter);
            txt_d.SetDataEffect(UI::ComponentTemplate::Text);
            txt_d.SetClip(true);
            txt_d.SetSize(100, 100, UI::Dimension::Percent);
            txt_d.SetSizing(UI::ComponentTemplate::ShrinkOnly, UI::ComponentTemplate::ShrinkOnly);
        }

        {
            auto &foc = temp.AddContainer(2, UI::ComponentCondition::Focused);

            foc.Background.SetAnimation(FocusBorder());
            foc.SetMargin(Border.Width + Border.Radius / 2);
            foc.SetSize(100, 100, UI::Dimension::Percent);
            foc.SetPositioning(foc.Absolute);
            foc.SetAnchor(UI::Anchor::None, UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter);
        }

        return temp;
    }
    
    UI::Template SimpleGenerator::IconButton(Geometry::Size iconsize) {
        
        if(iconsize.Width == -1) {
            iconsize.Width = BorderedWidgetHeight - Spacing * 2;
        }
        
        if(iconsize.Height == -1) {
            iconsize.Height = BorderedWidgetHeight - Spacing * 2;
        }
        
        
        UI::Template temp;
        
        auto bgsize = iconsize + Geometry::Size(Spacing * 2, Spacing * 2);
        temp.SetSize(bgsize);
        {
            auto &bg = temp.AddContainer(0, UI::ComponentCondition::Always);
            
            bg.SetPadding(Spacing);
            bg.Background.SetAnimation(NormalBG());
            bg.AddIndex(1);
            bg.AddIndex(2);
            bg.AddIndex(3);
        }
        
        {
            auto &bg = temp.AddContainer(0, UI::ComponentCondition::Hover);
            
            bg.SetPadding(Spacing);
            bg.Background.SetAnimation(HoverBG());
            bg.AddIndex(1);
            bg.AddIndex(2);
            bg.AddIndex(3);
        }
        
        {
            auto &bg = temp.AddContainer(0, UI::ComponentCondition::Down);
            
            bg.SetPadding(Spacing);
            bg.Background.SetAnimation(DownBG());
            bg.AddIndex(1);
            bg.AddIndex(2);
            bg.AddIndex(3);
        }

        {
            auto &bg = temp.AddContainer(0, UI::ComponentCondition::Disabled);

            bg.SetPadding(Spacing);
            bg.Background.SetAnimation(DisabledBG());
            bg.AddIndex(1);
            bg.AddIndex(2);
            bg.AddIndex(3);
            bg.AddIndex(4);
        }

        {
            auto &overlay = temp.AddContainer(4, UI::ComponentCondition::Disabled);

            auto c = Forecolor.Regular;
            c.Blend(Forecolor.Disabled);
            c.A /= 2;

            auto &im = *new Graphics::BlankImage(bgsize, c);
            drawables.Add(im);

            overlay.Background.SetDrawable(im);
            overlay.SetPositioning(overlay.Absolute);
            overlay.SetSize(100, 100, UI::Dimension::Percent);
            overlay.SetSizing(overlay.Fixed);
            overlay.SetAnchor(UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter);
            overlay.SetClip(true);
        }

        auto &icon = temp.AddPlaceholder(1, UI::ComponentCondition::Always);
        icon.SetDataEffect(icon.Icon);
        icon.SetAnchor(UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter);
        icon.SetSize(100, 100, UI::Dimension::Percent);
        icon.SetPositioning(icon.Absolute);
        icon.SetSizing(icon.Fixed);
        icon.SetClip(true);

        {
            auto &txt_n = temp.AddTextholder(3, UI::ComponentCondition::Always);
            txt_n.SetRenderer(RegularFont);
            txt_n.SetColor(Forecolor.Regular);
            txt_n.SetAnchor(UI::Anchor::None, UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter);
            txt_n.SetDataEffect(UI::ComponentTemplate::Text);
            txt_n.SetClip(true);
            txt_n.SetSize(100, 100, UI::Dimension::Percent);
            txt_n.SetSizing(UI::ComponentTemplate::ShrinkOnly);
            txt_n.SetPositioning(txt_n.Absolute);
        }

        
        {
            auto &foc = temp.AddContainer(2, UI::ComponentCondition::Focused);
            foc.Background.SetAnimation(FocusBorder());
            foc.SetMargin(1);
            foc.SetSize(100, 100, UI::Dimension::Percent);
            foc.SetPositioning(foc.Absolute);
            foc.SetAnchor(UI::Anchor::None, UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter);
        }
        
        return temp;
    }

    UI::Template SimpleGenerator::Checkbox() {
        Geometry::Size defsize = {WidgetWidth * 2 + Spacing, WidgetHeight};
        
        UI::Template temp;
        temp.SetSize(defsize);
        
        {
            auto &cont = temp.AddContainer(0, UI::ComponentCondition::Always);
            cont.SetPadding(Focus.Width);
            cont.AddIndex(1);
            cont.AddIndex(3);
            cont.AddIndex(4);
        }
        
        {
            auto &cont = temp.AddContainer(0, UI::ComponentCondition::State2);
            cont.SetPadding(Focus.Width);
            cont.AddIndex(2);
            cont.AddIndex(3);
            cont.AddIndex(4);
        }
        
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
            auto icon = box(color);
            
            Geometry::PointList<Geometry::Pointf> tick ={
                {ObjectBorder*2.f, ObjectHeight/2.f},
                {ObjectHeight/2.f, ObjectHeight-ObjectBorder*2.f},
                {ObjectHeight-ObjectBorder*2.f, ObjectBorder*2.f}
            };
            
            CGI::DrawLines(*icon, tick, (float)ObjectBorder, CGI::SolidFill<>(color));
            icon->Prepare();
            
            return icon;
        };
        
        auto makestate = [&](auto color, UI::ComponentCondition condition) {
            auto &it = temp.AddGraphics(1, condition);
            it.Content.SetDrawable(*box(color));
            it.SetSizing(it.Automatic);
            it.SetIndent(Spacing, 0, 0, 0);
            
            auto &it2 = temp.AddGraphics(2, condition);
            it2.Content.SetDrawable(*tick(color));
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
    
    UI::Template SimpleGenerator::CheckboxButton() {
        
        UI::Template temp;
        temp.SetSize((WidgetWidth-Spacing*3)/4, BorderedWidgetHeight);
        
        auto bgsize = temp.GetSize();
        
        {
            auto &bg = temp.AddContainer(0, UI::ComponentCondition::Always);
            
            bg.Background.SetAnimation(NormalBG());
            bg.AddIndex(1);
            bg.AddIndex(2);
            bg.AddIndex(3);
        }
        
        {
            auto &bg = temp.AddContainer(0, UI::ComponentCondition::Hover);
            
            bg.Background.SetAnimation(HoverBG());
            bg.AddIndex(1);
            bg.AddIndex(2);
            bg.AddIndex(3);
        }

        {
            auto &bg = temp.AddContainer(0, UI::ComponentCondition::Down);

            bg.Background.SetAnimation(DownBG());
            bg.AddIndex(1);
            bg.AddIndex(2);
            bg.AddIndex(3);
        }

        {
            auto &bg = temp.AddContainer(0, UI::ComponentCondition::Disabled);

            bg.Background.SetAnimation(DisabledBG());
            bg.AddIndex(1);
            bg.AddIndex(2);
            bg.AddIndex(3);
            bg.AddIndex(4);
        }

        {
            auto &overlay = temp.AddContainer(4, UI::ComponentCondition::Disabled);

            auto c = Forecolor.Regular;
            c.Blend(Forecolor.Disabled);
            c.A /= 2;

            auto &im = *new Graphics::BlankImage(bgsize, c);
            drawables.Add(im);

            overlay.Background.SetDrawable(im);
            overlay.SetPositioning(overlay.Absolute);
            overlay.SetSize(100, 100, UI::Dimension::Percent);
            overlay.SetSizing(overlay.Fixed);
            overlay.SetAnchor(UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter);
            overlay.SetClip(true);
        }

        auto &icon = temp.AddPlaceholder(1, UI::ComponentCondition::Always);
        icon.SetDataEffect(icon.Icon);
        icon.SetAnchor(UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter);
        icon.SetSize(100, 100, UI::Dimension::Percent);
        icon.SetPositioning(icon.Absolute);
        icon.SetSizing(icon.Fixed);
        icon.SetClip(true);

        {
            auto &txt_n = temp.AddTextholder(3, UI::ComponentCondition::Always);
            txt_n.SetRenderer(RegularFont);
            txt_n.SetColor(Forecolor.Regular);
            txt_n.SetAnchor(UI::Anchor::None, UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter);
            txt_n.SetDataEffect(UI::ComponentTemplate::Text);
            txt_n.SetClip(true);
            txt_n.SetSize(100, 100, UI::Dimension::Percent);
            txt_n.SetSizing(UI::ComponentTemplate::ShrinkOnly);
            txt_n.SetPositioning(txt_n.Absolute);
        }

        
        {
            auto &foc = temp.AddContainer(2, UI::ComponentCondition::Focused);
            foc.Background.SetAnimation(FocusBorder());
            foc.SetMargin(1);
            foc.SetSize(100, 100, UI::Dimension::Percent);
            foc.SetPositioning(foc.Absolute);
            foc.SetAnchor(UI::Anchor::None, UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter);
        }
        
        {
            auto &sel = temp.AddContainer(2, UI::ComponentCondition::State2);
            sel.Background.SetAnimation(NormalEmptyBorder());
            sel.SetSize(100, 100, UI::Dimension::Percent);
            sel.SetPositioning(sel.Absolute);
            sel.SetAnchor(UI::Anchor::None, UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter);
        }
        
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
        int inner_r = borderstart_r - Spacing / 2;
        
        Geometry::Size bmpsize = {ObjectHeight + 2, ObjectHeight + 2};
        Geometry::Pointf center = {float(outer_r + 1), float(outer_r + 1)};
        
        auto blank = [&](auto border) {
            auto icon = new Graphics::Bitmap(bmpsize);
            
            icon->Clear();
            
            CGI::Circle<16>(*icon, center, outer_r - 0.5f, CGI::SolidFill<>(Background.Regular));
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
        
        auto &cont = temp.AddContainer(2, UI::ComponentCondition::Always);
        cont.SetTag(UI::ComponentTemplate::ContentsTag);
        //cont.SetValueModification(cont.ModifyPosition, cont.UseXY);
        cont.SetSize(100, 100, UI::Dimension::Percent);
        cont.SetSizing(UI::ComponentTemplate::Fixed);
        cont.SetPositioning(cont.Absolute);
        cont.SetAnchor(UI::Anchor::TopLeft, UI::Anchor::TopLeft, UI::Anchor::TopLeft);
        cont.SetPosition(0, 0);
        
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
        Geometry::Size defsize = {WidgetWidth * 2 + Spacing, BorderedWidgetHeight};
        
        UI::Template temp;
        temp.SetSize(defsize);
        
        {
            auto &bg = temp.AddContainer(0, UI::ComponentCondition::Always);
         
            bg.Background.SetAnimation(NormalBorder());
            bg.SetPadding(Border.Width + Spacing);
            
            bg.AddIndex(1);
        }
        
        {
            auto &bar = temp.AddGraphics(1, UI::ComponentCondition::Always);
            bar.SetSizing(UI::ComponentTemplate::Fixed);
            bar.SetSize(100, 100, UI::Dimension::Percent);
            bar.SetPositioning(UI::ComponentTemplate::AbsoluteSliding);
            //bar.SetValueModification(UI::ComponentTemplate::ModifyX);
            bar.Content.SetAnimation(ObjectShape());
            bar.SetAnchor(UI::Anchor::MiddleRight, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
        }
        
        return temp;
    }
    
}}
