#include "ColorPlane.h"
#include "../Graphics/ColorSpaces.h"
#include "../CGI/Polygon.h"
#include "../CGI/Line.h"

#include <map>

//TODO move halftable lc values out
namespace Gorgon { namespace Widgets {
    
    ColorPlane::ColorPlane(const UI::Template &temp) : 
        ComponentStackComposer(temp),
        layerbox(Registry::Layerbox_Blank)
    {
        layerbox.Resize(GetInteriorSize());
        
        layerbox.GetLayer().Add(inputlayer);
        layerbox.GetLayer().Add(displaylayer);
        Add(layerbox);
        Refresh();
        
        inputlayer.SetClick(this, &ColorPlane::click);
    }
    
    void ColorPlane::Resize(const Geometry::Size &size) {
        auto oldsz = layerbox.GetSize();

        ComponentStackComposer::Resize(size);
        
        auto newsz = GetInteriorSize();

        if(oldsz != newsz) {
            layerbox.Resize(GetInteriorSize());
            Refresh();
        }
    }
    
    void ColorPlane::click(Geometry::Point l) {
        auto &huetable = *htbl;
        auto &lctable = *lctbl;
        
        Geometry::Pointf location = l;
        
        bool found = false;
        ColorType color;
        
        if(location.Y > halftableoffset.Y) {
            location -= halftableoffset;
            
            l = Round(location) / stride;
            int halftablerow = -1;
            
            if(lctable.size()>8) {
                if(l.Y == 0) {
                    if(l.X >= huetable.size() / 2) {
                        halftablerow = 1;
                        l.X -= huetable.size() / 2;
                    }
                    else if(l.X < huetable.size()){
                        halftablerow = 0;
                    }
                }
                if(l.Y == 1) {
                    if(l.X >= huetable.size() / 2) {
                        halftablerow = 3;
                        l.X -= huetable.size() / 2;
                    }
                    else if(l.X < huetable.size()){
                        halftablerow = 2;
                    }
                }
            }
            else if(l.Y == 0) {
                if(l.X >= huetable.size() / 2) {
                    halftablerow = 3;
                    l.X -= huetable.size() / 2;
                }
                else if(l.X < huetable.size()){
                    halftablerow = 2;
                }
            }
            
            std::cout << l.X << std::endl;
            
            found = true;
            switch(halftablerow) {
            case 0:
                color = Graphics::LChAf(20, 10, huetable[l.X * 2]);
                break;
            case 1:
                color = Graphics::LChAf(80, 15, huetable[l.X * 2]);
                break;
            case 2:
                color = Graphics::LChAf(8, 25, huetable[l.X * 2]);
                break;
            case 3:
                color = Graphics::LChAf(5, 75, huetable[l.X * 2]);
                break;
                
            default:
                found = false;
            }
        }
        else if(location.Y > colortableoffset.Y) {
            location -= colortableoffset;
            l = Round(location) / stride;
            
            if(l.Y < lctable.size() && l.X < huetable.size()) {
                found = true;
                color = Graphics::LChAf(lctable[l.Y].first, lctable[l.Y].second, huetable[l.X]);
            }
        }
        else if(location.Y > grayscaleoffset.Y) {
            location -= grayscaleoffset;
            l = Round(location) / stride;
            
            if(l.Y == 0 && l.X < huetable.size()) {
                found = true;
                color = Graphics::LChAf(100.f/(huetable.size()-1) * l.X, 0, 0);
            }
        }
        
        if(found && color != this->color) {
            this->color = color;
            Refresh();
            ChangedEvent(color);
        }
        
        ClickedEvent();
    }
    
    void ColorPlane::Refresh() {
        static const std::map<Density, const std::vector<std::pair<int, int>>*> lcmapping({
            {Low, &lcpairs5},
            {Medium, &lcpairs7},
            {High, &lcpairs9},
            {VeryHigh, &lcpairs13},
        });
        
        static const std::map<Density, const std::vector<int>*> huemapping({
            {Low, &huetable6},
            {Medium, &huetable8},
            {High, &huetable12},
            {VeryHigh, &huetable24},
        });
        
        Geometry::Pointf selectioncoords;
        bool selected = false;
        
        //setup values
        lctbl = lcmapping.at(lcdensity);
        htbl = huemapping.at(huedensity);
        
        auto &huetable = *htbl;
        auto &lctable = *lctbl;
        
        auto sz = displaylayer.GetCalculatedSize();
        display.Resize(sz);
        display.ForAllValues([](auto &c) { c = 255; });
        
        sz.Width  -= huetable.size()*2 + 2;
        sz.Height -= lctable.size()*2  + 8 + (lctable.size()>8)*2; //extra space between grayscale and other colors
        
        float w = (float)sz.Width / huetable.size();
        float h = (float)sz.Height/ (lctable.size() + 2 + (lctable.size()>8));
        
        stride = {w + 2, h + 2};
        
        float x = 2, y = 2;
        
        auto drawrect = [&](ColorType color) {
            CGI::Rectangle(display, {x, y, w, h}, CGI::SolidFill<>(color));
            
            if(color == this->color) {
                selected = true;
                selectioncoords = {x, y};
            }
            
            x += w + 2;
        };
        
        //grayscale
        grayscaleoffset = {x, y};
        for(int c=0; c<huetable.size(); c++) {
            drawrect(Graphics::LChAf(100.f/(huetable.size()-1)*c, 0, 0));
        }
        y += h + 4;
        
        //color table
        x = 2;
        colortableoffset = {x, y};
        for(auto p : lctable) {
            x = 2;
            for(auto hue : huetable) {
                drawrect(Graphics::LChAf(p.first, p.second, hue));
            }
            
            y += h + 2;
        }
        
        x = 2;
        halftableoffset = {x, y};
        //additional low constrast colors
        if(lctable.size() > 8) {
            for(int hue=0; hue<huetable.size()/2; hue++) {
                drawrect(Graphics::LChAf(20, 10, huetable[hue*2]));
            }
            for(int hue=0; hue<huetable.size()/2; hue++) {
                drawrect(Graphics::LChAf(80, 15, huetable[hue*2]));
            }
            y += h + 2;
        }
        
        //compress very dark colors
        x = 2;
        for(int hue=0; hue<huetable.size()/2; hue++) {
            drawrect(Graphics::LChAf(8, 25, huetable[hue*2]));
        }
        for(int hue=0; hue<huetable.size()/2; hue++) {
            drawrect(Graphics::LChAf(5, 75, huetable[hue*2]));
        }
        y += h + 2;
        
        Geometry::PointList<Geometry::Pointf> selectionlines = {
            {  1.75f,   0.75f}, {w+2.25f,   0.75f},
            {w+3.25f,   1.75f}, {w+3.25f, h+2.25f},
            {w+2.25f, h+3.25f}, {  1.75f, h+3.25f},
            {  0.75f, h+2.25f}, {  0.75f,   1.75f},
            {  1.75f,   0.75f}
        };
        
        if(selected) {
            CGI::DrawLines(display, selectionlines + selectioncoords - Geometry::Pointf(2, 2), 1.5f, 
                           CGI::SolidFill<>(Graphics::Color::Black));
        }
        
        displaylayer.Clear();
        display.Prepare();
        display.Draw(displaylayer, 0, 0);
    }
    

    const std::vector<std::pair<int, int>> ColorPlane::lcpairs5 = {
        {50, 100}, {70, 80}, {95, 90}, 
    };
    
    const std::vector<std::pair<int, int>> ColorPlane::lcpairs7 = {
        {50, 100}, {80,134}, {70, 70}, {90, 95}, {40, 40}
    };
    
    const std::vector<std::pair<int, int>> ColorPlane::lcpairs9 = {
        {50,134}, {80,134}, {90, 95}, {70, 60}, {40, 70}, {40,100}, {40, 40}
    };
    
    const std::vector<std::pair<int, int>> ColorPlane::lcpairs13 = {
        { 50, 134}, { 80, 134}, { 80,  60}, { 90,  95}, { 60,  35}, 
        { 30,  40}, { 40,  60}, { 40, 100}, { 80,  30}, { 60,  50}
    };
    
    const std::vector<int> ColorPlane::huetable6 = {
        40, 60, 103, 136, 290, 330
    };
    
    const std::vector<int> ColorPlane::huetable8 = {
        40, 60, 80, 103, 136, 175, 290, 330
    };
    
    const std::vector<int> ColorPlane::huetable12 = {
        40, 60, 80, 103, 112, 136, 175, 200, 250, 290, 330, 0
    };
    
    const std::vector<int> ColorPlane::huetable24 = {
        40, 50, 60, 70, 80, 90, 103, 107, 112, 124, 136, 156, 168, 178, 188, 200, 250, 270, 290, 310, 330, 345, 0, 10
    };


} }
