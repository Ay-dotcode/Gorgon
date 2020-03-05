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

    SimpleGenerator::SimpleGenerator(int fontsize, std::string fontname) {
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
        delete panelborder;
        delete toppanelborder;
        delete bottompanelborder;
        delete leftpanelborder;
        delete rightpanelborder;
        delete normaleditborder;
        delete hovereditborder;
        delete focusborder;
        delete normalemptyborder;
        delete normalbg;
        delete hoverbg;
        delete downbg;
        delete disabledbg;
    }
    
    void SimpleGenerator::UpdateDimensions() {
        lettervsize = regularrenderer->GetLetterHeight();
        asciivsize = regularrenderer->GetLetterHeight(true);
        int totalh = lettervsize.first + lettervsize.second;

        Spacing = (int)std::round((float)totalh / 4);

        WidgetWidth = regularrenderer->GetDigitWidth() * 8 + Border.Width * 2 + Border.Radius / 2 + Spacing * 3;
        BordedWidgetHeight = totalh + Border.Radius / 2 + Spacing * 2;
        WidgetHeight = totalh + Focus.Width * 2;
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
    
    Graphics::BitmapRectangleProvider &SimpleGenerator::PanelBorder() {
        if(!panelborder)
            panelborder = makeborder(Border.Color, Background.Panel);
        
        return *panelborder;
    }
    
    Graphics::BitmapRectangleProvider &SimpleGenerator::TopPanelBorder() {
        if(!toppanelborder)
            panelborder = makeborder(Border.Color, Background.Panel, 1);
        
        return *panelborder;
    }
    
    Graphics::BitmapRectangleProvider &SimpleGenerator::BottomPanelBorder() {
        if(!panelborder)
            panelborder = makeborder(Border.Color, Background.Panel, 3);
        
        return *panelborder;
    }
    
    Graphics::BitmapRectangleProvider &SimpleGenerator::LeftPanelBorder() {
        if(!panelborder)
            panelborder = makeborder(Border.Color, Background.Panel, 2);
        
        return *panelborder;
    }
    
    Graphics::BitmapRectangleProvider &SimpleGenerator::RightPanelBorder() {
        if(!panelborder)
            panelborder = makeborder(Border.Color, Background.Panel, 4);
        
        return *panelborder;
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
    
    Graphics::BitmapRectangleProvider *SimpleGenerator::makeborder(Graphics::RGBA border, Graphics::RGBA bg, int missingside) {
        int bsize = (Border.Width + Border.Radius + 1) * 2 + 16;
        float off = (int)(Border.Width + 1) / 2; //round up
        
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
            
            if(missingside) {
                list.Pop();
            }
            
            CGI::DrawLines(bi.GetData(), list, (float)Border.Width, CGI::SolidFill<>(border));
        }
        else {
            auto r = Border.Radius;
            
            Geometry::PointList<Geometry::Pointf> list = {
                {off + r, off}, 
                {off, off + r}, 
                {off, bsize - off - r}, 
                {off + r, bsize - off}, 
                {bsize - off - r, bsize - off},
                {bsize - off, bsize - off - r},
                {bsize - off, off + r}, 
                {bsize - off - r, off}, 
            };
            
            if(missingside) {
                list[0].X = off;
                list[7].X = bsize - off;
            }
            
            CGI::Polyfill(bi.GetData(), list, CGI::SolidFill<>(bg));
            
            list = {
                {off + r, off}, 
                {off, off + r}, 
                {off, bsize - off - r}, 
                {off + r, bsize - off}, 
                {bsize - off - r, bsize - off},
                {bsize - off, bsize - off - r},
                {bsize - off, off + r}, 
                {bsize - off - r, off}, 
                {off + r, off}
            };
            
            if(missingside) {
                list.Pop();
                list[0].X = off;
                list[7].X = bsize - off;
            }
            
            CGI::DrawLines(bi.GetData(), list, (float)Border.Width, CGI::SolidFill<>(border));
        }
        
        if(missingside == 2) {
            bi = bi.Rotate90();
        }
        else if(missingside == 3) {
            bi = bi.Rotate180();
        }
        else if(missingside == 4) {
            bi = bi.Rotate270();
        }
        
        drawables.Add(bi);

        auto ret = new Graphics::BitmapRectangleProvider(Graphics::Slice(bi, {
            int(off*2+Border.Radius+1), 
            int(off*2+Border.Radius+1), 
            int(bsize-off*2-Border.Radius-1),
            int(bsize-off*2-Border.Radius-1)
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
        Geometry::Size defsize = {WidgetWidth, BordedWidgetHeight};
        
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
            foc.SetMargin(Spacing / 2);
            foc.SetSize(100, 100, UI::Dimension::Percent);
            foc.SetPositioning(foc.Absolute);
            foc.SetAnchor(UI::Anchor::None, UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter);
        }

        return temp;
    }
    
    UI::Template SimpleGenerator::IconButton(Geometry::Size iconsize) {
        
        UI::Template temp;
        temp.SetSize(iconsize + Geometry::Size(16, 16));
        
        auto bgsize = iconsize + Geometry::Size(8, 8);
        
        {
            auto &bg = temp.AddContainer(0, UI::ComponentCondition::Always);
            
            bg.SetPadding(4);
            bg.Background.SetAnimation(NormalBG());
            bg.AddIndex(1);
            bg.AddIndex(2);
            bg.AddIndex(3);
        }
        
        {
            auto &bg = temp.AddContainer(0, UI::ComponentCondition::Hover);
            
            bg.SetPadding(4);
            bg.Background.SetAnimation(HoverBG());
            bg.AddIndex(1);
            bg.AddIndex(2);
            bg.AddIndex(3);
        }
        
        {
            auto &bg = temp.AddContainer(0, UI::ComponentCondition::Down);
            
            bg.SetPadding(4);
            bg.Background.SetAnimation(DownBG());
            bg.AddIndex(1);
            bg.AddIndex(2);
            bg.AddIndex(3);
        }

        {
            auto &bg = temp.AddContainer(0, UI::ComponentCondition::Disabled);

            bg.SetPadding(4);
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
        
        auto &cont = temp.AddContainer(0, UI::ComponentCondition::Always);
        cont.AddIndex(1);
        cont.AddIndex(3);
        cont.AddIndex(4);
        
        auto &cont2 = temp.AddContainer(0, UI::ComponentCondition::State2);
        cont2.AddIndex(2);
        cont2.AddIndex(3);
        cont2.AddIndex(4);
        
        {
            auto color = Forecolor.Regular;
            
            auto &icon = *new Graphics::Bitmap({ObjectHeight, ObjectHeight});
            auto &icon2 = *new Graphics::Bitmap({ObjectHeight, ObjectHeight});
            icon.ForAllPixels([&](auto x, auto y) {
                if(x>=ObjectBorder && x<ObjectHeight-ObjectBorder && y>=ObjectBorder && y<ObjectHeight-ObjectBorder)
                    icon.SetRGBAAt(x, y, Background.Regular);
                else
                    icon.SetRGBAAt(x, y, 0x0);
            });
            icon2.ForAllPixels([&](auto x, auto y) {
                if(x>=ObjectBorder && x<ObjectHeight-ObjectBorder && y>=ObjectBorder && y<ObjectHeight-ObjectBorder)
                    icon2.SetRGBAAt(x, y, Background.Regular);
                else
                    icon2.SetRGBAAt(x, y, 0x0);
            });
            
            Geometry::PointList<Geometry::Pointf> border ={
                {ObjectBorder/2.f, ObjectBorder/2.f},
                {ObjectHeight-ObjectBorder/2.f, ObjectBorder/2.f},
                {ObjectHeight-ObjectBorder/2.f, ObjectHeight-ObjectBorder/2.f},
                {ObjectBorder/2.f, ObjectHeight-ObjectBorder/2.f},
                {ObjectBorder/2.f, ObjectBorder/2.f},
            };
            CGI::DrawLines(icon, border, (float)ObjectBorder, CGI::SolidFill<>(color));
            CGI::DrawLines(icon2, border, (float)ObjectBorder, CGI::SolidFill<>(color));
            
            Geometry::PointList<Geometry::Pointf> tick ={
                {ObjectBorder*2.f, ObjectHeight/2.f},
                {ObjectHeight/2.f, ObjectHeight-ObjectBorder*2.f},
                {ObjectHeight-ObjectBorder*2.f, ObjectBorder*2.f}
            };
            CGI::DrawLines(icon2, tick, (float)ObjectBorder, CGI::SolidFill<>(color));
            icon.Prepare();
            drawables.Add(icon);
            icon2.Prepare();
            drawables.Add(icon2);
            
            auto &it = temp.AddGraphics(1, UI::ComponentCondition::Always);
            it.Content.SetDrawable(icon);
            it.SetSizing(it.Automatic);
            it.SetIndent(Spacing, 0, 0, 0);
            
            auto &it2 = temp.AddGraphics(2, UI::ComponentCondition::Always);
            it2.Content.SetDrawable(icon2);
            it2.SetSizing(it2.Automatic);
            it2.SetIndent(Spacing, 0, 0, 0);
            
            auto &tt = temp.AddTextholder(3, UI::ComponentCondition::Always);
            tt.SetRenderer(RegularFont);
            tt.SetColor(color);
            tt.SetMargin(Spacing, 0, 0, 0);
            tt.SetAnchor(UI::Anchor::MiddleRight, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
            tt.SetDataEffect(UI::ComponentTemplate::Text);
            tt.SetClip(true);
            tt.SetSize(100, 100, UI::Dimension::Percent);
            tt.SetSizing(UI::ComponentTemplate::ShrinkOnly, UI::ComponentTemplate::ShrinkOnly);
        }
        
        {
            auto color = Forecolor.Regular;
            color.Blend(Forecolor.Hover);
            
            auto &icon = *new Graphics::Bitmap({ObjectHeight, ObjectHeight});
            auto &icon2 = *new Graphics::Bitmap({ObjectHeight, ObjectHeight});
            icon.ForAllPixels([&](auto x, auto y) {
                if(x>=ObjectBorder && x<ObjectHeight-ObjectBorder && y>=ObjectBorder && y<ObjectHeight-ObjectBorder)
                    icon.SetRGBAAt(x, y, Background.Regular);
                else
                    icon.SetRGBAAt(x, y, 0x0);
            });
            icon2.ForAllPixels([&](auto x, auto y) {
                if(x>=ObjectBorder && x<ObjectHeight-ObjectBorder && y>=ObjectBorder && y<ObjectHeight-ObjectBorder)
                    icon2.SetRGBAAt(x, y, Background.Regular);
                else
                    icon2.SetRGBAAt(x, y, 0x0);
            });
            
            Geometry::PointList<Geometry::Pointf> border ={
                {ObjectBorder/2.f, ObjectBorder/2.f},
                {ObjectHeight-ObjectBorder/2.f, ObjectBorder/2.f},
                {ObjectHeight-ObjectBorder/2.f, ObjectHeight-ObjectBorder/2.f},
                {ObjectBorder/2.f, ObjectHeight-ObjectBorder/2.f},
                {ObjectBorder/2.f, ObjectBorder/2.f},
            };
            CGI::DrawLines(icon, border, (float)ObjectBorder, CGI::SolidFill<>(color));
            CGI::DrawLines(icon2, border, (float)ObjectBorder, CGI::SolidFill<>(color));
            
            Geometry::PointList<Geometry::Pointf> tick ={
                {ObjectBorder*2.f, ObjectHeight/2.f},
                {ObjectHeight/2.f, ObjectHeight-ObjectBorder*2.f},
                {ObjectHeight-ObjectBorder*2.f, ObjectBorder*2.f}
            };
            CGI::DrawLines(icon2, tick, (float)ObjectBorder, CGI::SolidFill<>(color));
            icon.Prepare();
            drawables.Add(icon);
            icon2.Prepare();
            drawables.Add(icon2);
            
            auto &it = temp.AddGraphics(1, UI::ComponentCondition::Hover);
            it.Content.SetDrawable(icon);
            it.SetSizing(it.Automatic);
            it.SetIndent(Spacing, 0, 0, 0);
            
            auto &it2 = temp.AddGraphics(2, UI::ComponentCondition::Hover);
            it2.Content.SetDrawable(icon2);
            it2.SetSizing(it2.Automatic);
            it2.SetIndent(Spacing, 0, 0, 0);
            
            auto &tt = temp.AddTextholder(3, UI::ComponentCondition::Hover);
            tt.SetRenderer(RegularFont);
            tt.SetColor(color);
            tt.SetMargin(Spacing, 0, 0, 0);
            tt.SetAnchor(UI::Anchor::MiddleRight, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
            tt.SetDataEffect(UI::ComponentTemplate::Text);
            tt.SetClip(true);
            tt.SetSize(100, 100, UI::Dimension::Percent);
            tt.SetSizing(UI::ComponentTemplate::ShrinkOnly, UI::ComponentTemplate::ShrinkOnly);
        }

        {
            auto color = Forecolor.Regular;
            color.Blend(Forecolor.Down);

            auto &icon = *new Graphics::Bitmap({ObjectHeight, ObjectHeight});
            auto &icon2 = *new Graphics::Bitmap({ObjectHeight, ObjectHeight});
            icon.ForAllPixels([&](auto x, auto y) {
                if(x>=ObjectBorder && x<ObjectHeight-ObjectBorder && y>=ObjectBorder && y<ObjectHeight-ObjectBorder)
                    icon.SetRGBAAt(x, y, Background.Regular);
                else
                    icon.SetRGBAAt(x, y, 0x0);
            });
            icon2.ForAllPixels([&](auto x, auto y) {
                if(x>=ObjectBorder && x<ObjectHeight-ObjectBorder && y>=ObjectBorder && y<ObjectHeight-ObjectBorder)
                    icon2.SetRGBAAt(x, y, Background.Regular);
                else
                    icon2.SetRGBAAt(x, y, 0x0);
            });

            Geometry::PointList<Geometry::Pointf> border = {
                {ObjectBorder/2.f, ObjectBorder/2.f},
                {ObjectHeight-ObjectBorder/2.f, ObjectBorder/2.f},
                {ObjectHeight-ObjectBorder/2.f, ObjectHeight-ObjectBorder/2.f},
                {ObjectBorder/2.f, ObjectHeight-ObjectBorder/2.f},
                {ObjectBorder/2.f, ObjectBorder/2.f},
            };
            CGI::DrawLines(icon, border, (float)ObjectBorder, CGI::SolidFill<>(color));
            CGI::DrawLines(icon2, border, (float)ObjectBorder, CGI::SolidFill<>(color));

            Geometry::PointList<Geometry::Pointf> tick = {
                {ObjectBorder*2.f, ObjectHeight/2.f},
                {ObjectHeight/2.f, ObjectHeight-ObjectBorder*2.f},
                {ObjectHeight-ObjectBorder*2.f, ObjectBorder*2.f}
            };
            CGI::DrawLines(icon2, tick, (float)ObjectBorder, CGI::SolidFill<>(color));
            icon.Prepare();
            drawables.Add(icon);
            icon2.Prepare();
            drawables.Add(icon2);

            auto &it = temp.AddGraphics(1, UI::ComponentCondition::Down);
            it.Content.SetDrawable(icon);
            it.SetSizing(it.Automatic);
            it.SetIndent(Spacing, 0, 0, 0);

            auto &it2 = temp.AddGraphics(2, UI::ComponentCondition::Down);
            it2.Content.SetDrawable(icon2);
            it2.SetSizing(it2.Automatic);
            it2.SetIndent(Spacing, 0, 0, 0);

            auto &tt = temp.AddTextholder(3, UI::ComponentCondition::Down);
            tt.SetRenderer(RegularFont);
            tt.SetColor(color);
            tt.SetMargin(Spacing, 0, 0, 0);
            tt.SetAnchor(UI::Anchor::MiddleRight, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
            tt.SetDataEffect(UI::ComponentTemplate::Text);
            tt.SetClip(true);
            tt.SetSize(100, 100, UI::Dimension::Percent);
            tt.SetSizing(UI::ComponentTemplate::ShrinkOnly, UI::ComponentTemplate::ShrinkOnly);
        }

        {
            auto color = Forecolor.Regular;
            color.Blend(Forecolor.Disabled);

            auto &icon = *new Graphics::Bitmap({ObjectHeight, ObjectHeight});
            auto &icon2 = *new Graphics::Bitmap({ObjectHeight, ObjectHeight});
            icon.ForAllPixels([&](auto x, auto y) {
                if(x>=ObjectBorder && x<ObjectHeight-ObjectBorder && y>=ObjectBorder && y<ObjectHeight-ObjectBorder)
                    icon.SetRGBAAt(x, y, Background.Disabled);
                else
                    icon.SetRGBAAt(x, y, 0x0);
            });
            icon2.ForAllPixels([&](auto x, auto y) {
                if(x>=ObjectBorder && x<ObjectHeight-ObjectBorder && y>=ObjectBorder && y<ObjectHeight-ObjectBorder)
                    icon2.SetRGBAAt(x, y, Background.Disabled);
                else
                    icon2.SetRGBAAt(x, y, 0x0);
            });

            Geometry::PointList<Geometry::Pointf> border = {
                {ObjectBorder/2.f, ObjectBorder/2.f},
                {ObjectHeight-ObjectBorder/2.f, ObjectBorder/2.f},
                {ObjectHeight-ObjectBorder/2.f, ObjectHeight-ObjectBorder/2.f},
                {ObjectBorder/2.f, ObjectHeight-ObjectBorder/2.f},
                {ObjectBorder/2.f, ObjectBorder/2.f},
            };
            CGI::DrawLines(icon, border, (float)ObjectBorder, CGI::SolidFill<>(color));
            CGI::DrawLines(icon2, border, (float)ObjectBorder, CGI::SolidFill<>(color));

            Geometry::PointList<Geometry::Pointf> tick = {
                {ObjectBorder*2.f, ObjectHeight/2.f},
                {ObjectHeight/2.f, ObjectHeight-ObjectBorder*2.f},
                {ObjectHeight-ObjectBorder*2.f, ObjectBorder*2.f}
            };
            CGI::DrawLines(icon2, tick, (float)ObjectBorder, CGI::SolidFill<>(color));
            icon.Prepare();
            drawables.Add(icon);
            icon2.Prepare();
            drawables.Add(icon2);

            auto &it = temp.AddGraphics(1, UI::ComponentCondition::Disabled);
            it.Content.SetDrawable(icon);
            it.SetSizing(it.Automatic);
            it.SetIndent(Spacing, 0, 0, 0);

            auto &it2 = temp.AddGraphics(2, UI::ComponentCondition::Disabled);
            it2.Content.SetDrawable(icon2);
            it2.SetSizing(it2.Automatic);
            it2.SetIndent(Spacing, 0, 0, 0);

            auto &tt = temp.AddTextholder(3, UI::ComponentCondition::Disabled);
            tt.SetRenderer(RegularFont);
            tt.SetColor(color);
            tt.SetMargin(Spacing, 0, 0, 0);
            tt.SetAnchor(UI::Anchor::MiddleRight, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
            tt.SetDataEffect(UI::ComponentTemplate::Text);
            tt.SetClip(true);
            tt.SetSize(100, 100, UI::Dimension::Percent);
            tt.SetSizing(UI::ComponentTemplate::ShrinkOnly, UI::ComponentTemplate::ShrinkOnly);
        }
        
        {
            auto &foc = temp.AddContainer(4, UI::ComponentCondition::Focused);
            foc.Background.SetAnimation(FocusBorder());
            foc.SetSize(100, 100, UI::Dimension::Percent);
            foc.SetPositioning(foc.Absolute);
            foc.SetAnchor(UI::Anchor::None, UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter);
        }
        return temp;
    }
    
    UI::Template SimpleGenerator::CheckboxButton() {
        
        UI::Template temp;
        temp.SetSize((WidgetWidth-Spacing*3)/4, BordedWidgetHeight);
        
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


        {
            auto color = Forecolor.Regular;
            
            auto &icon = *new Graphics::Bitmap({ObjectHeight, ObjectHeight});
            auto &icon2 = *new Graphics::Bitmap({ObjectHeight, ObjectHeight});

            icon.Clear();
            icon2.Clear();
            
            Float center = Float(ObjectHeight-ObjectBorder*2.5f) / 2.f;
            CGI::Circle(icon, {center+ObjectBorder*1.25f, center+ObjectBorder*1.25f}, center+ObjectBorder, CGI::SolidFill<>(Background.Regular));
            CGI::Circle(icon2, {center+ObjectBorder*1.25f, center+ObjectBorder*1.25f}, center+ObjectBorder, CGI::SolidFill<>(Background.Regular));
            
            CGI::Circle(icon, {center+ObjectBorder*1.25f, center+ObjectBorder*1.25f}, center, (float)ObjectBorder, CGI::SolidFill<>(color));
            CGI::Circle(icon2, {center+ObjectBorder*1.25f, center+ObjectBorder*1.25f}, center, (float)ObjectBorder, CGI::SolidFill<>(color));
            
            CGI::Circle(icon2, {center+ObjectBorder*1.25f, center+ObjectBorder*1.25f}, center - Spacing / 2, CGI::SolidFill<>(color));
            icon.Prepare();
            drawables.Add(icon);
            icon2.Prepare();
            drawables.Add(icon2);
            
            auto &it = temp.AddGraphics(1, UI::ComponentCondition::Always);
            it.Content.SetDrawable(icon);
            it.SetSizing(it.Automatic);
            it.SetIndent(Spacing, 0, 0, 0);
            
            auto &it2 = temp.AddGraphics(2, UI::ComponentCondition::Always);
            it2.Content.SetDrawable(icon2);
            it2.SetSizing(it2.Automatic);
            it2.SetIndent(Spacing, 0, 0, 0);
            
            auto &tt = temp.AddTextholder(3, UI::ComponentCondition::Always);
            tt.SetRenderer(RegularFont);
            tt.SetColor(color);
            tt.SetMargin(Spacing, 0, 0, 0);
            tt.SetAnchor(UI::Anchor::MiddleRight, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
            tt.SetDataEffect(UI::ComponentTemplate::Text);
            tt.SetClip(true);
            tt.SetSize(100, 100, UI::Dimension::Percent);
            tt.SetSizing(UI::ComponentTemplate::ShrinkOnly, UI::ComponentTemplate::ShrinkOnly);
        }
        
        {
            auto color = Forecolor.Regular;
            color.Blend(Forecolor.Hover);
            
            auto &icon = *new Graphics::Bitmap({ObjectHeight, ObjectHeight});
            auto &icon2 = *new Graphics::Bitmap({ObjectHeight, ObjectHeight});

            icon.Clear();
            icon2.Clear();

            Float center = Float(ObjectHeight-ObjectBorder*2.5f) / 2.f;
            CGI::Circle(icon, {center+ObjectBorder*1.25f, center+ObjectBorder*1.25f}, center+ObjectBorder, CGI::SolidFill<>(Background.Regular));
            CGI::Circle(icon2, {center+ObjectBorder*1.25f, center+ObjectBorder*1.25f}, center+ObjectBorder, CGI::SolidFill<>(Background.Regular));
            
            CGI::Circle(icon, {center+ObjectBorder*1.25f, center+ObjectBorder*1.25f}, center, (float)ObjectBorder, CGI::SolidFill<>(color));
            CGI::Circle(icon2, {center+ObjectBorder*1.25f, center+ObjectBorder*1.25f}, center, (float)ObjectBorder, CGI::SolidFill<>(color));
            
            CGI::Circle(icon2, {center+ObjectBorder*1.25f, center+ObjectBorder*1.25f}, center - Spacing / 2, CGI::SolidFill<>(color));
            icon.Prepare();
            drawables.Add(icon);
            icon2.Prepare();
            drawables.Add(icon2);
            
            auto &it = temp.AddGraphics(1, UI::ComponentCondition::Hover);
            it.Content.SetDrawable(icon);
            it.SetSizing(it.Automatic);
            it.SetIndent(Spacing, 0, 0, 0);
            
            auto &it2 = temp.AddGraphics(2, UI::ComponentCondition::Hover);
            it2.Content.SetDrawable(icon2);
            it2.SetSizing(it2.Automatic);
            it2.SetIndent(Spacing, 0, 0, 0);
            
            auto &tt = temp.AddTextholder(3, UI::ComponentCondition::Hover);
            tt.SetRenderer(RegularFont);
            tt.SetColor(color);
            tt.SetMargin(Spacing, 0, 0, 0);
            tt.SetAnchor(UI::Anchor::MiddleRight, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
            tt.SetDataEffect(UI::ComponentTemplate::Text);
            tt.SetClip(true);
            tt.SetSize(100, 100, UI::Dimension::Percent);
            tt.SetSizing(UI::ComponentTemplate::ShrinkOnly, UI::ComponentTemplate::ShrinkOnly);
        }

        {
            auto color = Forecolor.Regular;
            color.Blend(Forecolor.Down);

            auto &icon = *new Graphics::Bitmap({ObjectHeight, ObjectHeight});
            auto &icon2 = *new Graphics::Bitmap({ObjectHeight, ObjectHeight});

            icon.Clear();
            icon2.Clear();

            Float center = Float(ObjectHeight-ObjectBorder*2.5f) / 2.f;
            CGI::Circle(icon, {center+ObjectBorder*1.25f, center+ObjectBorder*1.25f}, center+ObjectBorder, CGI::SolidFill<>(Background.Regular));
            CGI::Circle(icon2, {center+ObjectBorder*1.25f, center+ObjectBorder*1.25f}, center+ObjectBorder, CGI::SolidFill<>(Background.Regular));

            CGI::Circle(icon, {center+ObjectBorder*1.25f, center+ObjectBorder*1.25f}, center, (float)ObjectBorder, CGI::SolidFill<>(color));
            CGI::Circle(icon2, {center+ObjectBorder*1.25f, center+ObjectBorder*1.25f}, center, (float)ObjectBorder, CGI::SolidFill<>(color));

            CGI::Circle(icon2, {center+ObjectBorder*1.25f, center+ObjectBorder*1.25f}, center - Spacing / 2, CGI::SolidFill<>(color));
            icon.Prepare();
            drawables.Add(icon);
            icon2.Prepare();
            drawables.Add(icon2);

            auto &it = temp.AddGraphics(1, UI::ComponentCondition::Down);
            it.Content.SetDrawable(icon);
            it.SetSizing(it.Automatic);
            it.SetIndent(Spacing, 0, 0, 0);

            auto &it2 = temp.AddGraphics(2, UI::ComponentCondition::Down);
            it2.Content.SetDrawable(icon2);
            it2.SetSizing(it2.Automatic);
            it2.SetIndent(Spacing, 0, 0, 0);

            auto &tt = temp.AddTextholder(3, UI::ComponentCondition::Down);
            tt.SetRenderer(RegularFont);
            tt.SetColor(color);
            tt.SetMargin(Spacing, 0, 0, 0);
            tt.SetAnchor(UI::Anchor::MiddleRight, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
            tt.SetDataEffect(UI::ComponentTemplate::Text);
            tt.SetClip(true);
            tt.SetSize(100, 100, UI::Dimension::Percent);
            tt.SetSizing(UI::ComponentTemplate::ShrinkOnly, UI::ComponentTemplate::ShrinkOnly);
        }

        {
            auto color = Forecolor.Regular;
            color.Blend(Forecolor.Disabled);

            auto &icon = *new Graphics::Bitmap({ObjectHeight, ObjectHeight});
            auto &icon2 = *new Graphics::Bitmap({ObjectHeight, ObjectHeight});

            icon.Clear();
            icon2.Clear();

            Float center = Float(ObjectHeight-ObjectBorder*2.5f) / 2.f;
            CGI::Circle(icon, {center+ObjectBorder*1.25f, center+ObjectBorder*1.25f}, center+ObjectBorder, CGI::SolidFill<>(Background.Regular));
            CGI::Circle(icon2, {center+ObjectBorder*1.25f, center+ObjectBorder*1.25f}, center+ObjectBorder, CGI::SolidFill<>(Background.Regular));

            CGI::Circle(icon, {center+ObjectBorder*1.25f, center+ObjectBorder*1.25f}, center, (float)ObjectBorder, CGI::SolidFill<>(color));
            CGI::Circle(icon2, {center+ObjectBorder*1.25f, center+ObjectBorder*1.25f}, center, (float)ObjectBorder, CGI::SolidFill<>(color));

            CGI::Circle(icon2, {center+ObjectBorder*1.25f, center+ObjectBorder*1.25f}, center - Spacing / 2, CGI::SolidFill<>(color));
            icon.Prepare();
            drawables.Add(icon);
            icon2.Prepare();
            drawables.Add(icon2);

            auto &it = temp.AddGraphics(1, UI::ComponentCondition::Disabled);
            it.Content.SetDrawable(icon);
            it.SetSizing(it.Automatic);
            it.SetIndent(Spacing, 0, 0, 0);

            auto &it2 = temp.AddGraphics(2, UI::ComponentCondition::Disabled);
            it2.Content.SetDrawable(icon2);
            it2.SetSizing(it2.Automatic);
            it2.SetIndent(Spacing, 0, 0, 0);

            auto &tt = temp.AddTextholder(3, UI::ComponentCondition::Disabled);
            tt.SetRenderer(RegularFont);
            tt.SetColor(color);
            tt.SetMargin(Spacing, 0, 0, 0);
            tt.SetAnchor(UI::Anchor::MiddleRight, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
            tt.SetDataEffect(UI::ComponentTemplate::Text);
            tt.SetClip(true);
            tt.SetSize(100, 100, UI::Dimension::Percent);
            tt.SetSizing(UI::ComponentTemplate::ShrinkOnly, UI::ComponentTemplate::ShrinkOnly);
        }

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
    
    UI::Template SimpleGenerator::Panel() {
        Geometry::Size defsize = {WidgetWidth * 4 + Spacing * 5 + Border.Width * 2 + Border.Radius + Spacing * 2, WidgetHeight * 10 + Spacing * 11 + Border.Width * 2 + Border.Radius + Spacing * 2};
        
        UI::Template temp;
        temp.SetSize(defsize);
        
        
        auto &bg = temp.AddContainer(0, UI::ComponentCondition::Always);
        bg.SetPadding(Border.Width+Border.Radius);
        bg.Background.SetAnimation(PanelBorder());
        bg.AddIndex(1);
        
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
        cont.SetValueModification(cont.ModifyPosition, cont.UseXY);
        cont.SetSize(100, 100, UI::Dimension::Percent);
        cont.SetPositioning(cont.Absolute);
        cont.SetAnchor(UI::Anchor::TopLeft, UI::Anchor::TopLeft, UI::Anchor::TopLeft);
        cont.SetPosition(0, 0);
        
        return temp;
    }
    
    UI::Template SimpleGenerator::TopPanel() {
        Geometry::Size defsize = {WidgetWidth * 4 + Spacing * 5 + Border.Width * 2 + Border.Radius + Spacing * 2, WidgetHeight * 2 + Spacing * 3 + Border.Width * 2 + Border.Radius + Spacing * 2};
        
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
        cont.Background.SetAnimation(TopPanelBorder());
        
        return temp;
    }
    
    UI::Template SimpleGenerator::LeftPanel() {
        Geometry::Size defsize = {WidgetWidth * 4 + Spacing * 5 + Border.Width * 2 + Border.Radius + Spacing * 2, WidgetHeight * 10 + Spacing * 11 + Border.Width * 2 + Border.Radius + Spacing * 2};
        
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
        cont.Background.SetAnimation(LeftPanelBorder());
        
        return temp;
    }
    
    UI::Template SimpleGenerator::RightPanel() {
        Geometry::Size defsize = {WidgetWidth * 4 + Spacing * 5 + Border.Width * 2 + Border.Radius + Spacing * 2, WidgetHeight * 10 + Spacing * 11 + Border.Width * 2 + Border.Radius + Spacing * 2};
        
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
        cont.Background.SetAnimation(RightPanelBorder());
        
        return temp;
    }
    
    UI::Template SimpleGenerator::BottomPanel() {
        Geometry::Size defsize = {WidgetWidth * 4 + Spacing * 5 + Border.Width * 2 + Border.Radius + Spacing * 2, WidgetHeight * 2 + Spacing * 3 + Border.Width * 2 + Border.Radius + Spacing * 2};
        
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
        cont.Background.SetAnimation(BottomPanelBorder());
        
        return temp;
    }
    
    UI::Template SimpleGenerator::Inputbox() {
        Geometry::Size defsize = {WidgetWidth * 2 + Spacing, BordedWidgetHeight};
        
        UI::Template temp;
        temp.SetSize(defsize);
        
        auto &bi = *new Graphics::BlankImage({Border.Width, Border.Width}, Border.Color);
        drawables.Add(bi);
        
        {
            auto &bg_n = temp.AddContainer(0, UI::ComponentCondition::Always);
            bg_n.SetPadding(Spacing, UI::Dimension::Pixel);
            bg_n.AddIndex(1);
            bg_n.AddIndex(2);
            bg_n.AddIndex(3);
            bg_n.AddIndex(4);
            bg_n.SetClip(true); //!Shadow
            bg_n.Background.SetAnimation(NormalEditBorder());
        }

        {
            auto &bg_h = temp.AddContainer(0, UI::ComponentCondition::Hover);
            bg_h.SetPadding(Spacing);
            bg_h.AddIndex(1);
            bg_h.AddIndex(2);
            bg_h.AddIndex(3);
            bg_h.AddIndex(4);
            bg_h.SetClip(true); //!Shadow
            bg_h.Background.SetAnimation(HoverEditBorder());
        }

        {
            auto &bg_d = temp.AddContainer(0, UI::ComponentCondition::Disabled);
            bg_d.SetPadding(Spacing);
            bg_d.AddIndex(1);
            bg_d.AddIndex(2);
            bg_d.AddIndex(3);
            bg_d.AddIndex(4);
            bg_d.SetClip(true); //!Shadow
            bg_d.Background.SetAnimation(DisabledBorder());
        }
        
        {
            auto &foc = temp.AddContainer(1, UI::ComponentCondition::Focused);
            foc.Background.SetAnimation(FocusBorder());
            foc.SetIndent(-Focus.Width);
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
            auto &img = *new Graphics::Bitmap({std::min(Border.Width/2, 1), h});
            img.ForAllPixels([&img, this, h](int x, int y) {
                img(x, y, 0) = Border.Color.R;
                img(x, y, 1) = Border.Color.G;
                img(x, y, 2) = Border.Color.B;
                img(x, y, 3) = (y >= (asciivsize.first - 1) && y <= (lettervsize.first + regularrenderer->GetBaseLine()+1)) * Border.Color.A;
            });
            drawables.Add(img);
            img.Prepare();
            auto &img2 = *new Graphics::Bitmap({std::min(Border.Width/2, 1), RegularFont.GetGlyphRenderer().GetHeight()});
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

    
}}
