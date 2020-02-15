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

        if(!regular.HasKerning()) {
            auto &bmpfnt = *new Graphics::BitmapFont(regular.MoveOutBitmap());
            RegularFont.SetGlyphRenderer(bmpfnt);
            bmpfnt.AutoKern();
            regularrenderer = &bmpfnt;

            delete &regular;
        }
        else {
            regularrenderer = &regular;
        }
        
        //RegularFont.AlignRight();
    }

    SimpleGenerator::~SimpleGenerator() {
        if(regularrenderer)
            delete regularrenderer;

        providers.DeleteAll();
        drawables.DeleteAll();
    }
    
    Graphics::BitmapRectangleProvider &SimpleGenerator::NormalBorder() {
        static Graphics::BitmapRectangleProvider border = makeborder(Border.Color, Background.Regular);
        
        return border;
    }
    
    Graphics::BitmapRectangleProvider &SimpleGenerator::PanelBorder() {
        static Graphics::BitmapRectangleProvider border = makeborder(Border.Color, Background.Panel);
        
        return border;
    }
    
    Graphics::BitmapRectangleProvider &SimpleGenerator::NormalEditBorder() {
        static Graphics::BitmapRectangleProvider border = makeborder(Border.Color, Background.Edit);
        
        return border;
    }
    
    Graphics::BitmapRectangleProvider &SimpleGenerator::HoverBorder() {
        auto c = Background.Regular;
        c.Blend(Background.Hover);
        static Graphics::BitmapRectangleProvider border = makeborder(Border.Color, c);
        
        return border;
    }
    
    Graphics::BitmapRectangleProvider &SimpleGenerator::DownBorder() {
        auto c = Background.Regular;
        c.Blend(Background.Down);
        static Graphics::BitmapRectangleProvider border = makeborder(Border.Color, c);
        
        return border;
    }
    
    Graphics::BitmapRectangleProvider SimpleGenerator::makeborder(Graphics::RGBA border, Graphics::RGBA bg) {
        int bsize = (Border.Width + Border.Radius + 1) * 2 + 16;
        float off = (float)(Border.Width + 1) / 2; //round up
        
        auto &bi = *new Graphics::Bitmap({bsize, bsize}, Graphics::ColorMode::RGBA);
        bi.Clear();
        
        if(Border.Radius == 0) {
            CGI::Polyfill(bi.GetData(), {{off,off}, {off, bsize-off}, {bsize-off, bsize-off}, {bsize-off, off}}, CGI::SolidFill<>(bg));
            
            CGI::DrawLines(bi.GetData(), {
                {off, off}, 
                {off, bsize-off},
                {bsize-off, bsize-off},
                {bsize-off, off}, 
                {off, off}
            }, (float)Border.Width, CGI::SolidFill<>(border));
        }
        else {
            auto r = Border.Radius;
            
            CGI::Polyfill(bi.GetData(), {
                {off + r, off}, 
                {off, off + r}, 
                {off, bsize - off - r}, 
                {off + r, bsize - off}, 
                {bsize - off - r, bsize - off},
                {bsize - off, bsize - off - r},
                {bsize - off, off + r}, 
                {bsize - off - r, off}, 
            }, CGI::SolidFill<>(bg));
            
            CGI::DrawLines(bi.GetData(), {
                {off + r, off}, 
                {off, off + r}, 
                {off, bsize - off - r}, 
                {off + r, bsize - off}, 
                {bsize - off - r, bsize - off},
                {bsize - off, bsize - off - r},
                {bsize - off, off + r}, 
                {bsize - off - r, off}, 
                {off + r, off}
            }, (float)Border.Width, CGI::SolidFill<>(Border.Color));
        }
        
        drawables.Add(bi);

        Graphics::BitmapRectangleProvider ret = Graphics::Slice(bi, {int(off*2+Border.Radius), int(off*2+Border.Radius), int(bsize-off*2-Border.Radius), int(bsize-off*2-Border.Radius)});
        ret.Prepare();
        
        return ret;
    }
    
    
    UI::Template SimpleGenerator::Button(Geometry::Size defsize) {

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
            auto &foc = temp.AddContainer(2, UI::ComponentCondition::Focused);

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

            auto &rect = *new Graphics::RectangleProvider(cri, hi, cri, vi, ci, vi, cri, hi, cri);

            foc.Background.SetAnimation(rect);
            providers.Add(rect);
            foc.SetMargin(Spacing / 2);
            foc.SetSize(100, 100, UI::Dimension::Percent);
            foc.SetPositioning(foc.Absolute);
            foc.SetAnchor(UI::Anchor::None, UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter);
        }

        return temp;
    }
    
    UI::Template SimpleGenerator::IconButton(Geometry::Size defsize) {
        
        UI::Template temp;
        temp.SetSize(defsize);
        
        auto bgsize = defsize - Geometry::Size(8, 8);
        
        {
            auto &bg = temp.AddContainer(0, UI::ComponentCondition::Always);
            
            auto c = Background.Regular;
            
            auto &im = *new Graphics::BlankImage(bgsize, c);
            drawables.Add(im);
            
            bg.SetPadding(4);
            bg.Background.SetDrawable(im);
            bg.AddIndex(1);
            bg.AddIndex(2);
            bg.AddIndex(3);
        }
        
        {
            auto &bg = temp.AddContainer(0, UI::ComponentCondition::Hover);
            
            auto c = Background.Regular;
            c.Blend(Background.Hover);
            
            auto &im = *new Graphics::BlankImage(bgsize, c);
            drawables.Add(im);
            
            bg.SetPadding(4);
            bg.Background.SetDrawable(im);
            bg.AddIndex(1);
            bg.AddIndex(2);
            bg.AddIndex(3);
        }
        
        {
            auto &bg = temp.AddContainer(0, UI::ComponentCondition::Down);
            
            auto c = Background.Regular;
            c.Blend(Background.Down);
            
            auto &im = *new Graphics::BlankImage(defsize - bgsize, c);
            drawables.Add(im);
            
            bg.SetPadding(4);
            bg.Background.SetDrawable(im);
            bg.AddIndex(1);
            bg.AddIndex(2);
            bg.AddIndex(3);
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
            
            auto &rect = *new Graphics::RectangleProvider(cri, hi, cri, vi, ci, vi, cri, hi, cri);
            
            foc.Background.SetAnimation(rect);
            providers.Add(rect);
            foc.SetMargin(1);
            foc.SetSize(100, 100, UI::Dimension::Percent);
            foc.SetPositioning(foc.Absolute);
            foc.SetAnchor(UI::Anchor::None, UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter);
        }
        
        return temp;
    }

    UI::Template SimpleGenerator::Checkbox(Geometry::Size defsize) {
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
            });
            icon2.ForAllPixels([&](auto x, auto y) {
                if(x>=ObjectBorder && x<ObjectHeight-ObjectBorder && y>=ObjectBorder && y<ObjectHeight-ObjectBorder)
                    icon2.SetRGBAAt(x, y, Background.Regular);
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
            });
            icon2.ForAllPixels([&](auto x, auto y) {
                if(x>=ObjectBorder && x<ObjectHeight-ObjectBorder && y>=ObjectBorder && y<ObjectHeight-ObjectBorder)
                    icon2.SetRGBAAt(x, y, Background.Regular);
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
            });
            icon2.ForAllPixels([&](auto x, auto y) {
                if(x>=ObjectBorder && x<ObjectHeight-ObjectBorder && y>=ObjectBorder && y<ObjectHeight-ObjectBorder)
                    icon2.SetRGBAAt(x, y, Background.Regular);
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
            auto &foc = temp.AddContainer(4, UI::ComponentCondition::Focused);
            
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
            
            auto &rect = *new Graphics::RectangleProvider(cri, hi, cri, vi, ci, vi, cri, hi, cri);
            
            foc.Background.SetAnimation(rect);
            //providers.Add(rect);
            //foc.SetMargin(Spacing / 2);
            foc.SetSize(100, 100, UI::Dimension::Percent);
            foc.SetPositioning(foc.Absolute);
            foc.SetAnchor(UI::Anchor::None, UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter);
        }
        return temp;
    }
    
    UI::Template SimpleGenerator::RadioButton(Geometry::Size defsize) {
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
            auto &foc = temp.AddContainer(4, UI::ComponentCondition::Focused);
            
            auto &ci = Graphics::EmptyImage::Instance();
            
            auto &hi = *new Graphics::Bitmap({2, Focus.Width});
            hi.Clear();
            for(auto i=0; i<Focus.Width; i++)
                hi.SetRGBAAt(0, i, Focus.Color);
            hi.Prepare();
            
            auto &vi = *new Graphics::Bitmap({Focus.Width, 2});
            vi.Clear();
            for(auto i=0; i<Focus.Width; i++)
                vi.SetRGBAAt(i, 0, Focus.Color);
            vi.Prepare();
            
            auto &cri = *new Graphics::BlankImage(Focus.Width, Focus.Width, Focus.Color);
            
            auto &rect = *new Graphics::RectangleProvider(cri, hi, cri, vi, ci, vi, cri, hi, cri);
            
            foc.Background.SetAnimation(rect);
            providers.Add(rect);
            foc.SetSize(100, 100, UI::Dimension::Percent);
            foc.SetPositioning(foc.Absolute);
            foc.SetAnchor(UI::Anchor::None, UI::Anchor::MiddleCenter, UI::Anchor::MiddleCenter);
        }
        return temp;
    }
    
    UI::Template SimpleGenerator::Label(Geometry::Size defsize) {
        UI::Template temp;
        temp.SetSize(defsize);
        
        auto &bi = *new Graphics::BlankImage({Border.Width, Border.Width}, Border.Color);
        drawables.Add(bi);
        
        {
            auto &bg_n = temp.AddContainer(0, UI::ComponentCondition::Always);
            bg_n.SetSizing(UI::ComponentTemplate::Automatic);
            //assuming border radius = 0
            bg_n.SetPadding(Spacing);
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
            txt_n.SetColor(Forecolor.Regular);
            txt_n.SetAnchor(UI::Anchor::MiddleRight, UI::Anchor::MiddleLeft, UI::Anchor::MiddleLeft);
            txt_n.SetDataEffect(UI::ComponentTemplate::Text);
            txt_n.SetClip(true);
            txt_n.SetSize(100, 100, UI::Dimension::Percent);
            txt_n.SetSizing(UI::ComponentTemplate::ShrinkOnly);
        }
        
        return temp;
    }

    UI::Template SimpleGenerator::ErrorLabel(Geometry::Size defsize) {
        UI::Template temp;
        temp.SetSize(defsize);

        auto &bi = *new Graphics::BlankImage({ Border.Width, Border.Width }, Border.Color);
        drawables.Add(bi);

        {
            auto &bg_n = temp.AddContainer(0, UI::ComponentCondition::Always);
            bg_n.SetSizing(UI::ComponentTemplate::Automatic);
            //assuming border radius = 0
            bg_n.SetPadding(Spacing);
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
    
    UI::Template SimpleGenerator::BlankPanel(Geometry::Size defsize) {
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
    
    UI::Template SimpleGenerator::Panel(Geometry::Size defsize) {
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
    
    //TODO: fix me
    UI::Template SimpleGenerator::TopPanel(Geometry::Size defsize) {
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
        cont.Background.SetAnimation(PanelBorder());
        
        return temp;
    }
    
    UI::Template SimpleGenerator::LeftPanel(Geometry::Size defsize) {
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
        cont.Background.SetAnimation(PanelBorder());
        
        return temp;
    }
    
    UI::Template SimpleGenerator::RightPanel(Geometry::Size defsize) {
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
        cont.Background.SetAnimation(PanelBorder());
        
        return temp;
    }
    
    UI::Template SimpleGenerator::BottomPanel(Geometry::Size defsize) {
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
        cont.Background.SetAnimation(PanelBorder());
        
        return temp;
    }
    
    UI::Template SimpleGenerator::Inputbox(Geometry::Size defsize) {
        UI::Template temp;
        temp.SetSize(defsize);
        
        auto &bi = *new Graphics::BlankImage({Border.Width, Border.Width}, Border.Color);
        drawables.Add(bi);
        
        {
            auto &bg_n = temp.AddContainer(0, UI::ComponentCondition::Always);
            bg_n.SetPadding(Spacing);
            //bg_n.SetTag(UI::ComponentTemplate::ViewPortTag);
            bg_n.AddIndex(1);
            bg_n.AddIndex(2);
            bg_n.AddIndex(3);
            bg_n.AddIndex(4);
            bg_n.SetClip(true); //!Shadow
            bg_n.Background.SetAnimation(NormalEditBorder());
        }
        
        {
            auto &foc = temp.AddContainer(1, UI::ComponentCondition::Focused);
            
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
            
            auto &rect = *new Graphics::RectangleProvider(cri, hi, cri, vi, ci, vi, cri, hi, cri);
            
            foc.Background.SetAnimation(rect);
            providers.Add(rect);
            foc.SetMargin(Spacing / 2);
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
            txt_n.SetSize({100, UI::Dimension::Percent}, RegularFont.GetGlyphRenderer().GetHeight()*5/6);
            txt_n.SetSizing(UI::ComponentTemplate::Fixed);
        }
        
        {
            auto &anim = *new Graphics::BitmapAnimationProvider();
            int h = RegularFont.GetGlyphRenderer().GetHeight();
            auto &img = *new Graphics::Bitmap({std::min(Border.Width/2, 1), h});
            img.ForAllPixels([&img, this, h](int x, int y) {
                img(x, y, 0) = Border.Color.R;
                img(x, y, 1) = Border.Color.G;
                img(x, y, 2) = Border.Color.B;
                img(x, y, 3) = (y >= h/6 && y <= 5*h/6) * Border.Color.A;
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
            caret.SetTag(caret.CaretTag);
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
            selection.SetSize(10, 4*h/6+1);
            selection.SetSizing(UI::ComponentTemplate::Fixed);
        }
        
        return temp;
    }

    
}}
