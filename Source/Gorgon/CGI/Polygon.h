#pragma once

#include "../Containers/Image.h"
#include "../CGI.h"
#include "../Geometry/PointList.h"
#include "../Geometry/Line.h"

#include <cmath>


namespace Gorgon { namespace CGI {
   
    /**
     * This function fills the given point list as a polygon. List is treated as closed
     * where last pixel connects to the first. 
     */
    template<class P_, class F_ = SolidFill<>>
    void Polyfill(Containers::Image &target, const Geometry::PointList<P_> &points, F_ fill = SolidFill<>{Graphics::Color::Black}) {
        using std::swap;
        
        if(points.Size() <= 1) return;
        
        auto yrange = std::minmax_element(points.begin(), points.end(), [](auto l, auto r) { return l.Y < r.Y; });
        
        auto ymin = yrange.first->Y;
        auto ymax = yrange.second->Y;
        
        if(ceil(ymin) > floor(ymax)) {
            return; //move to subpixel
        }
        
        int N = points.Size();
        
        for(int y = (int)floor(ymin); y<ceil(ymax); y++) {
            int off = 0;
            
            //trackback until the line that is not touching this scanline
            while(off > -N) {
                auto line = points.LineAt(off);
                
                if(line.MinY() < y+1 && line.MaxY() > y) {
                    off--;
                }
                else
                    break;
            }
            
            std::vector<std::pair<Float, Float>> xpoints;
            bool connected = false;
            int firstdir = 0, lastdir = 0;
            for(int i=off; i<N+off; i++) {
                auto line = points.LineAt(i);
                
                if(line.MinY() < y+1 && line.MaxY() > y) {
                    auto line = points.LineAt(i);
                    
                    auto slope = line.SlopeInv();
                    auto offset= line.OffsetInv();
                    
                    Float x1, x2;
                    
                    if(std::isinf(slope)) {
                        x1 = line.Start.X;
                        x2 = line.End.X;
                    }
                    else {
                        x1 = slope * y + offset;
                        x2 = slope * (y+1) + offset;
                    }
                    
                    if(x1 > x2)
                        swap(x1, x2);
                    
                    if(x1 < line.MinX())
                        x1 = line.MinX();
                    
                    if(x2 > line.MaxX())
                        x2 = line.MaxX();
                    
                    if(connected) {
                        auto &f = xpoints.back().first;
                        f = f > x1 ? x1 : f;
                        
                        auto &s = xpoints.back().second;
                        s = s < x2 ? x2 : s;
                    }
                    else {
                        firstdir = line.YDirection();
                        xpoints.push_back({x1, x2});
                    }
                    
                    lastdir = line.YDirection();
                    
                    connected = line.End.Y <= y + 1 && line.End.Y > y;
                }
                else {
                    if(connected && lastdir != firstdir) { //change in direction
                        xpoints.pop_back();
                    }
                    connected = false;
                }
            }
            
            if(connected && lastdir != firstdir) { //change in direction
                xpoints.pop_back();
            }
            
            std::sort(xpoints.begin(), xpoints.end(), [](auto l, auto r) { return l.first < r.first; });
            
            //join overlapping x sections
            for(int i=0; i<(int)xpoints.size()-1; i++) {
                if(xpoints[i].second >= xpoints[i+1].first) {
                    //join
                    xpoints[i+1].first = xpoints[i].first; //sorted
                    xpoints[i].second  = xpoints[i].second > xpoints[i+1].second ? xpoints[i].second : xpoints[i+1].second;
                    xpoints[i+1].second= xpoints[i].second;
                }
            }
            
            //fill
            for(int i=0; i<(int)xpoints.size()-1; i+=2) {
                int s = (int)ceil(xpoints[i].second);
                int e = (int)xpoints[i+1].first;
                if(s < e)
                    for(int x=s; x<e; x++) {
                        target.SetRGBAAt(x, y, fill(x, y, x, y, target.GetRGBAAt(x, y)));
                    }
            }
        }
    }
    
    /**
     * This function fills the given point list as a polygon. List is treated as closed
     * where last pixel connects to the first. 
     */
    template<class P_, class F_ = SolidFill<>>
    void Polyfill(Graphics::Bitmap &target, const Geometry::PointList<P_> &points, F_ fill = SolidFill<>{Graphics::Color::Black}) {
        if(target.HasData())
            Polyfill(target.GetData(), points, fill);
    }
} }
