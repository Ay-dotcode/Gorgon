#pragma once

#include "../Containers/Image.h"
#include "../CGI.h"
#include "../Geometry/PointList.h"
#include "../Geometry/Line.h"
#include "../Containers/Collection.h"

#include <cmath>


namespace Gorgon { namespace CGI {
   
    ///@cond internal
    namespace internal {
        struct vertexinfo {
            vertexinfo() = default;

            vertexinfo (Float first, Float second) : first(first), second(second) { }

            Float first, second;
            bool skip = false;
        };
        
        template<int S_, class PL_, class F_>
        void findpolylinestofill(const PL_ &pointlist, int ymin, int ymax, F_ fn) {
        using std::swap;
        
        for(int y = ymin; y<ymax; y++) {
            int nexty = y + 1;
            
            std::vector<vertexinfo> xpoints;
            
            for(const auto &points : pointlist) {
                int N = points.Size();
                
                if(N <= 1) continue;
            
                int off = 0;
                //trackback until the line that is not touching this scanline
                while(off > -N) {
                    auto line = points.LineAt(off);
                    
                    if(line.End.Y <= nexty && line.End.Y >= y) {//fix
                        off--;
                    }
                    else
                        break;
                }
            
            
                bool connected = false, wasconnected = false;
                int firstdir = 0, lastdir = 0;
                for(int i=off; i<N+off; i++) {
                    auto line = points.LineAt(i);
                    
                    if(line.MinY() < nexty && line.MaxY() > y) {
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
                            x2 = slope * nexty + offset;
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
                            if(wasconnected && lastdir != firstdir) { //change in direction
                                xpoints.back().skip= true;
                            }
                            
                            firstdir = line.YDirection();
                            xpoints.push_back({x1, x2});
                        }
                        
                        lastdir = line.YDirection();
                        
                        wasconnected = connected;
                        connected = line.End.Y <= nexty && line.End.Y >= y;
                    }
                    else {
                        if(wasconnected && lastdir != firstdir) { //change in direction
                            xpoints.back().skip= true;
                        }
                        connected = false;
                    }
                }
                
                if(wasconnected && lastdir != firstdir) { //change in direction
                    xpoints.back().skip= true;
                }
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
            fn(y, xpoints);
        }
    
        
    }
    }
    ///@endcond
    
    /**
     * This function fills the given point list as a polygon. List is treated as closed
     * where last pixel connects to the first. S_ is the number of subdivision for subpixel
     * accuracy. If subpixelonly is true, the segment where fully set pixels are determined
     * is skipped. When drawing very thin polygon such as lines, setting this parameters can
     * improve speed.
     */
    template<int S_ = 8, class P_= Geometry::Pointf, class F_ = SolidFill<>>
    void Polyfill(Containers::Image &target, const std::vector<Geometry::PointList<P_>> &points, F_ fill = SolidFill<>{Graphics::Color::Black}, bool subpixelonly = false) {
        
        if(points.size() < 1) return;
        
        int ymin = target.GetHeight() - 1;
        int ymax = 0;
        int xmin = target.GetWidth() - 1;
        
        for(const auto &p : points) {
            auto yrange = std::minmax_element(p.begin(), p.end(), [](auto l, auto r) { return l.Y < r.Y; });
            
            if(ymin > yrange.first->Y)
                ymin = yrange.first->Y;
            
            if(ymax < yrange.second->Y)
                ymax = yrange.second->Y;

            auto xrange = std::minmax_element(p.begin(), p.end(), [](auto l, auto r) { return l.X < r.X; });
            
            if(xmin > xrange.first->X)
                xmin = xrange.first->X;
        }
        
        Gorgon::FitInto(ymin, 0, target.GetHeight()-1);
        Gorgon::FitInto(ymax, 0, target.GetHeight());
        
        
        if(ceil(ymin) <= floor(ymax) && !subpixelonly) {
            internal::findpolylinestofill<1>(points, ymin, ymax, [&](float y, auto &xpoints) {
                for(int i=0; i<(int)xpoints.size()-1; i+=2) {
                    int s = (int)ceil(xpoints[i].second);
                    int e = (int)xpoints[i+1].first;
                    
                    Gorgon::FitInto(s, 0, target.GetWidth()-1);
                    Gorgon::FitInto(e, 0, target.GetWidth());
                    
                    if(xpoints[i].skip) {
                        i--; //only skip the first point and continue
                        continue;
                    }
                    else if(xpoints[i+1].skip) {
                        xpoints[i+1].skip = false;
                        i--; //fill until the start of the second point, next time start from the end of it
                    }
                    
                    if(s < e) {
                        for(int x=s; x<e; x++) {
                            target.SetRGBAAt(x, y, fill(x-xmin, y-ymin, x, y, target.GetRGBAAt(x, y)));
                            //save to speedup subpixel accuracy
                        }
                    }
                }
            });
        }
        
        //subpixel accurate filling
    }
    
    /**
     * This function fills the given point list as a polygon. List is treated as closed
     * where last pixel connects to the first. 
     */
    template<class P_, class F_ = SolidFill<>>
    void Polyfill(Graphics::Bitmap &target, const std::vector<Geometry::PointList<P_>> &points, F_ fill = SolidFill<>{Graphics::Color::Black}, bool subpixelonly = false) {
        if(target.HasData())
            Polyfill(target.GetData(), points, fill, subpixelonly);
    }
    
    /**
     * This function fills the given point list as a polygon. List is treated as closed
     * where last pixel connects to the first. S_ is the number of subdivision for subpixel
     * accuracy. If subpixelonly is true, the segment where fully set pixels are determined
     * is skipped. When drawing very thin polygon such as lines, setting this parameters can
     * improve speed.
     */
    template<int S_ = 8, class P_= Geometry::Pointf, class F_ = SolidFill<>>
    void Polyfill(Containers::Image &target, const Geometry::PointList<P_> &points, F_ fill = SolidFill<>{Graphics::Color::Black}, bool subpixelonly = false) {
        if(points.Size() <= 1) return;
        
        auto yrange = std::minmax_element(points.begin(), points.end(), [](auto l, auto r) { return l.Y < r.Y; });
        
        int ymin = (int)floor(yrange.first->Y);
        int ymax = (int)ceil(yrange.second->Y);
        
        Gorgon::FitInto(ymin, 0, target.GetHeight()-1);
        Gorgon::FitInto(ymax, 0, target.GetHeight());
        
        auto xrange = std::minmax_element(points.begin(), points.end(), [](auto l, auto r) { return l.X < r.X; });
        
        int xmin = (int)floor(xrange.first->X);
        
        if(ceil(ymin) <= floor(ymax) && !subpixelonly) {
            internal::findpolylinestofill<1>(Containers::Collection<const Geometry::PointList<P_>>({points}), ymin, ymax, [&](float y, auto &xpoints) {
                for(int i=0; i<(int)xpoints.size()-1; i+=2) {
                    int s = (int)ceil(xpoints[i].second);
                    int e = (int)xpoints[i+1].first;
                    
                    Gorgon::FitInto(s, 0, target.GetWidth()-1);
                    Gorgon::FitInto(e, 0, target.GetWidth());
                    
                    if(xpoints[i].skip) {
                        i--; //only skip the first point and continue
                        continue;
                    }
                    else if(xpoints[i+1].skip) {
                        xpoints[i+1].skip = false;
                        i--; //fill until the start of the second point, next time start from the end of it
                    }
                    
                    if(s < e) {
                        for(int x=s; x<e; x++) {
                            target.SetRGBAAt(x, y, fill(x-xmin, y-ymin, x, y, target.GetRGBAAt(x, y)));
                            //save to speedup subpixel accuracy
                        }
                    }
                }
            });
        }
    }
    
    /**
     * This function fills the given point list as a polygon. List is treated as closed
     * where last pixel connects to the first. 
     */
    template<class P_, class F_ = SolidFill<>>
    void Polyfill(Graphics::Bitmap &target, const Geometry::PointList<P_> &points, F_ fill = SolidFill<>{Graphics::Color::Black}, bool subpixelonly = false) {
        if(target.HasData())
            Polyfill(target.GetData(), points, fill, subpixelonly);
    }
    
} }
