#pragma once

#include "../Types.h"
#include "../Graphics/Bitmap.h"
#include "../Graphics/Color.h"
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
            int wind = false;
        };
        
        template<class PL_, class F_, class T_>
        void findpolylinestofill(const PL_ &pointlist, T_ ymin, T_ ymax, F_ fn, T_ step = 1, T_ cover = 1) {
        using std::swap;
        
        for(T_ y = ymin; y<ymax; y += step) {
            T_ nexty = y + cover;
            
            std::vector<vertexinfo> xpoints;
            
            for(const auto &points : pointlist) {
                int N = (int)points.GetSize();
                
                if(N <= 1) continue;
            
                int off = 0;
                //trackback until the line that is not touching this scanline
                while(off > -N) {
                    auto line = points.GetLine(off);
                    
                    if(line.End.Y < nexty && line.End.Y > y) {
                        off--;
                    }
                    if(line.Start.Y < nexty && line.Start.Y > y) {
                        off--;
                    }
                    else
                        break;
                }
                
                if(off == -N)
                    continue;
            
            
                bool connected = false, wasconnected = false;
                int firstdir = 0, lastdir = 0;
                for(int i=off; i<N+off; i++) {
                    auto line = points.GetLine(i);
                    
                    if(line.MinY() < nexty && line.MaxY() > y) {
                        auto line = points.GetLine(i);
                        
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
                            xpoints.back().wind = firstdir;
                        }
                        
                        lastdir = line.YDirection();
                        
                        wasconnected = connected;
                        connected = line.End.Y <= nexty && line.End.Y >= y;
                    }
                    else {
                        if(wasconnected && lastdir != firstdir) { //change in direction
                            xpoints.back().skip= true;
                            //xpoints.back().wind = 0;
                        }
                        connected = false;
                    }
                }
                
                if(wasconnected && lastdir != firstdir) { //change in direction
                    xpoints.back().skip= true;
                    //xpoints.back().wind = 0;
                }
            }
            
            std::sort(xpoints.begin(), xpoints.end(), [](auto l, auto r) { return l.second < r.second; });
            
            //join overlapping x sections
            /*for(int i=0; i<(int)xpoints.size()-1; i++) {
                if(xpoints[i].second >= xpoints[i+1].first) {
                    //join
                    xpoints[i+1].first = xpoints[i].first; //sorted
                    xpoints[i].second  = xpoints[i].second > xpoints[i+1].second ? xpoints[i].second : xpoints[i+1].second;
                    xpoints[i+1].second= xpoints[i].second;
                }
            }*/
            
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
     * improve speed. S_ should be a power of two for this algorithm to work properly. W_ is
     * winding, 1 is odd, 0 is non-zero.
     */
    template<int S_ = 8, int W_ = 1, class P_= Geometry::Pointf, class F_ = SolidFill<>>
    void Polyfill(Containers::Image &target, const std::vector<Geometry::PointList<P_>> &p, F_ fill = SolidFill<>{Graphics::Color::Black}) {
        if(p.size() < 1) return;
        
        std::vector<Geometry::PointList<P_>> p2;
        const std::vector<Geometry::PointList<P_>> *pp;
        
        if(S_ > 1) {
            for(auto &pl : p) {
                p2.push_back(pl.Duplicate());
            }
            pp = &p2;
        }
        else {
            pp = &p;
        }
        
        const std::vector<Geometry::PointList<P_>> &points = *pp;
        
        
        Float ymin = Float(target.GetHeight()*S_ - 1);
        Float ymax = 0;
        int xmin = Float(target.GetWidth()*S_ - 1);
        int xmax = 0;
        bool found = false;
        
        for(const auto &p : points) {
            if(p.GetSize() <= 0) continue;
            
            auto yrange = std::minmax_element(p.begin(), p.end(), [](auto l, auto r) { return l.Y < r.Y; });
            
            if(ymin > yrange.first->Y)
                ymin = yrange.first->Y;
            
            if(ymax < yrange.second->Y)
                ymax = yrange.second->Y;

            auto xrange = std::minmax_element(p.begin(), p.end(), [](auto l, auto r) { return l.X < r.X; });
            
            if(xmin > xrange.first->X)
                xmin = (int)xrange.first->X;
            
            if(xmax < xrange.second->X)
                xmax = (int)xrange.second->X;
            
            found = true;
        }
        
        if(!found) return;
        
        ymax++; //ensuring the last line is not skipped due to <
        
        Gorgon::FitInto<Float>(ymin, 0.f, (Float)target.GetHeight()-1);
        Gorgon::FitInto<Float>(ymax, 0.f, (Float)target.GetHeight());
        
        
        Gorgon::FitInto(xmin, 0, target.GetWidth()-1);
        Gorgon::FitInto(xmax, 0, target.GetWidth()-1);
        
        if(ceil(ymin) > floor(ymax)) return;
        
        if(S_ > 1) { //subpixel
            for(auto &pl : p2)
                pl *= S_;
            
            Float cy = -1;
            
            int ew = xmax-xmin+1; //effective width
            std::vector<int> cnts(ew); //line buffer for counting
            int yminint = (int)floor(ymin*S_);
            
            float a = 1.f / (S_ * S_);
            
            internal::findpolylinestofill(points, yminint, (int)ceil(ymax*S_), [&](float y, auto &xpoints) {
                if(int(cy) != int(y/S_)) {
                    if(y != yminint) { //transfer
                        for(int x=0; x<ew; x++) {
                            if(cnts[x]) {
                                Graphics::RGBA prevcol = target.GetRGBAAt(int(x+xmin), (int)cy);
                                Graphics::RGBA col = fill((int)x, int(cy-yminint), int(x + xmin), (int)cy, prevcol);
                                int targeta = (int)round(a * cnts[x] * col.A);
                                FitInto(targeta, 0, 255);
                                
                                col.A = targeta;
                                
                                prevcol.Blend(col);
                                
                                
                                if(cnts[x] == 0 && col.A > 0)
                                    std::cout<<x<<std::endl;
                                
                                target.SetRGBAAt(x + xmin, cy,  prevcol);
                            }
                        }
                    }
                    
                    //reset
                    memset(&cnts[0], 0, cnts.size()*sizeof(int));
                    
                    cy = int(y/S_);
                }
                
                int wind = 0;
                for(int i=0; i<(int)xpoints.size()-1; i++) {
                    if(xpoints[i].skip) 
                        continue;
                    
                    wind += xpoints[i].wind;
                    
                    if(wind == 0) continue;
                    
                    Float s = ceil(xpoints[i].second)/S_;
                    std::sort(xpoints.begin() + i + 1, xpoints.end(), [](auto l, auto r) { return l.first < r.first; });
                    
                    while(true) {
                        i++;
                        
                        if(i >= xpoints.size())
                            break;
                        
                        wind += xpoints[i].wind;
                        
                        if(W_ == 0 ? (wind == 0) : (wind%2==0)) {
                            Float e = floor(xpoints[i].first)/S_;
                            
                            FitInto<Float>(s, xmin, xmax+1);
                            FitInto<Float>(e, xmin, xmax+1);
                            if(s < e) {
                                for(Float x=s; x<e; x+=1.f/S_) {
                                    cnts[(int)x-xmin]++;
                                }
                            }
                            
                            if(xpoints[i].skip) {
                                xpoints[i].skip = false;
                                xpoints[i].wind *= -1;
                                i--;
                            }
                            
                            break;
                        }
                        
                        if(xpoints[i].skip) {
                            xpoints[i].skip = false;
                            xpoints[i].wind *= -1;
                            i--;
                        }
                    }
                    
                    if(xpoints.begin() + i == xpoints.end()) continue;
                    
                    std::sort(xpoints.begin() + i + 1, xpoints.end(), [](auto l, auto r) { return l.second < r.second; });
                }
            });
        }
        else { //integer
            internal::findpolylinestofill(points, (int)floor(ymin), (int)ceil(ymax), [&](float y, auto &xpoints) {
                int wind = 0;
                for(int i=0; i<(int)xpoints.size()-1; i++) {
                    if(xpoints[i].skip) 
                        continue;
                    
                    wind += xpoints[i].wind;
                    
                    if(wind == 0) continue;
                    
                    int s = (int)ceil(xpoints[i].second);
                    std::sort(xpoints.begin() + i + 1, xpoints.end(), [](auto l, auto r) { return l.first < r.first; });
                    
                    while(true) {
                        i++;
                        
                        if(i >= xpoints.size())
                            break;
                        
                        wind += xpoints[i].wind;
                        
                        if(W_ == 0 ? (wind == 0) : (wind%2 == 0)) {
                            int e = (int)floor(xpoints[i].first);
                            
                            FitInto(s, 0, target.GetWidth());
                            FitInto(e, 0, target.GetWidth());
                            if(s < e) {
                                for(int x=s; x<e; x++) {
                                    target.SetRGBAAt(x, y, fill(x-xmin, y-ymin, x, y, target.GetRGBAAt(x, y)));
                                }
                            }
                            
                            if(xpoints[i].skip) {
                                xpoints[i].skip = false;
                                xpoints[i].wind *= -1;
                                i--;
                            }
                            
                            break;
                        }
                            
                        if(xpoints[i].skip) {
                            xpoints[i].skip = false;
                            xpoints[i].wind *= -1;
                            i--;
                        }
                    }
                    
                    if(xpoints.begin() + i == xpoints.end()) continue;
                    
                    std::sort(xpoints.begin() + i + 1, xpoints.end(), [](auto l, auto r) { return l.second < r.second; });
                }
            });
        }
    }
    
    /**
     * This function fills the given point list as a polygon. List is treated as closed
     * where last pixel connects to the first. 
     */
    template<int S_ = 8, int W_ = 1, class P_, class F_ = SolidFill<>>
    void Polyfill(Graphics::Bitmap &target, const std::vector<Geometry::PointList<P_>> &points, F_ fill = SolidFill<>{Graphics::Color::Black}) {
        if(target.HasData())
            Polyfill<S_, W_, P_, F_>(target.GetData(), points, fill);
    }
    
    /**
     * This function fills the given point list as a polygon. List is treated as closed
     * where last pixel connects to the first. S_ is the number of subdivision for subpixel
     * accuracy.
     */
    template<int S_ = 8, int W_ = 1, class P_= Geometry::Pointf, class F_ = SolidFill<>>
    void Polyfill(Containers::Image &target, const Geometry::PointList<P_> &p, F_ fill = SolidFill<>{Graphics::Color::Black}) {
        if(p.GetSize() <= 1) return;

        Geometry::PointList<P_> p2;
        const Geometry::PointList<P_> *pp;
        
        if(S_ > 1) {
            p2 = p.Duplicate();
            pp = &p2;
        }
        else {
            pp = &p;
        }
        const Geometry::PointList<P_> &points = *pp;
        
        auto yrange = std::minmax_element(points.begin(), points.end(), [](auto l, auto r) { return l.Y < r.Y; });
        
        Float ymin = yrange.first->Y;
        Float ymax = yrange.second->Y;
        
        ymax++;
        
        Gorgon::FitInto<Float>(ymin, 0, target.GetHeight()-1);
        Gorgon::FitInto<Float>(ymax, 0, target.GetHeight());
        
        auto xrange = std::minmax_element(points.begin(), points.end(), [](auto l, auto r) { return l.X < r.X; });
        
        int xmin = xrange.first->X;
        int xmax = xrange.second->X;
        
        Gorgon::FitInto(xmin, 0, target.GetWidth()-1);
        Gorgon::FitInto(xmax, 0, target.GetWidth()-1);
        
        if(ceil(ymin) > floor(ymax)) return;
        
        if(S_ > 1) { //subpixel
            p2 *= S_;
            
            Float cy = -1;
            
            int ew = xmax-xmin+1; //effective width
            std::vector<int> cnts(ew); //line buffer for counting
            int yminint = (int)floor(ymin*S_);
            
            float a = 1.f / (S_ * S_);
            
            internal::findpolylinestofill(Containers::Collection<const Geometry::PointList<P_>>({points}), yminint, (int)ceil(ymax*S_), [&](float y, auto &xpoints) {
                if(int(cy) != int(y/S_)) {
                    if(y != yminint) { //transfer
                        for(int x=0; x<ew; x++) {
                            if(cnts[x]) {
                                Graphics::RGBA prevcol = target.GetRGBAAt(x+xmin, cy);
                                Graphics::RGBA col = fill(x, cy-yminint, x + xmin, cy, prevcol);
                                int targeta = (int)round(a * cnts[x] * col.A);
                                FitInto(targeta, 0, 255);
                                
                                col.A = targeta;
                                
                                prevcol.Blend(col);
                                
                                
                                if(cnts[x] == 0 && col.A > 0)
                                    std::cout<<x<<std::endl;
                                
                                target.SetRGBAAt(x + xmin, cy,  prevcol);
                            }
                        }
                    }
                    
                    //reset
                    memset(&cnts[0], 0, cnts.size()*sizeof(int));
                    
                    cy = int(y/S_);
                }
                

                int wind = 0;
                for(int i=0; i<(int)xpoints.size()-1; i++) {
                    if(xpoints[i].skip) 
                        continue;
                    
                    wind += xpoints[i].wind;
                    
                    if(wind == 0) continue;
                    
                    Float s = ceil(xpoints[i].second)/S_;
                    std::sort(xpoints.begin() + i + 1, xpoints.end(), [](auto l, auto r) { return l.first < r.first; });
                    
                    while(true) {
                        i++;
                        
                        if(i >= xpoints.size())
                            break;
                        
                        wind += xpoints[i].wind;
                        
                        if(W_ == 0 ? (wind == 0) : (wind%2==0)) {
                            Float e = floor(xpoints[i].first)/S_;
                            
                            FitInto<Float>(s, xmin, xmax+1);
                            FitInto<Float>(e, xmin, xmax+1);
                            if(s < e) {
                                for(Float x=s; x<e; x+=1.f/S_) {
                                    cnts[(int)x-xmin]++;
                                }
                            }
                            
                            if(xpoints[i].skip) {
                                xpoints[i].skip = false;
                                xpoints[i].wind *= -1;
                                i--;
                            }
                            
                            break;
                        }
                        
                        if(xpoints[i].skip) {
                            xpoints[i].skip = false;
                            xpoints[i].wind *= -1;
                            i--;
                        }
                    }
                    
                    if(xpoints.begin() + i == xpoints.end()) continue;
                    
                    std::sort(xpoints.begin() + i + 1, xpoints.end(), [](auto l, auto r) { return l.second < r.second; });
                }
            });
        }
        else {
            internal::findpolylinestofill(Containers::Collection<const Geometry::PointList<P_>>({points}), ymin, ymax, [&](float y, auto &xpoints) {
                int wind = 0;
                for(int i=0; i<(int)xpoints.size()-1; i++) {
                    if(xpoints[i].skip) 
                        continue;
                    
                    wind += xpoints[i].wind;
                    
                    if(wind == 0) continue;
                    
                    int s = (int)ceil(xpoints[i].second);
                    std::sort(xpoints.begin() + i + 1, xpoints.end(), [](auto l, auto r) { return l.first < r.first; });
                    
                    while(true) {
                        i++;
                        
                        if(i >= xpoints.size())
                            break;
                        
                        wind += xpoints[i].wind;
                        
                        if(W_ == 0 ? (wind == 0) : (wind%2 == 0)) {
                            int e = (int)floor(xpoints[i].first);
                            
                            FitInto(s, 0, target.GetWidth());
                            FitInto(e, 0, target.GetWidth());
                            if(s < e) {
                                for(int x=s; x<e; x++) {
                                    target.SetRGBAAt(x, y, fill(x-xmin, y-ymin, x, y, target.GetRGBAAt(x, y)));
                                }
                            }
                            
                            if(xpoints[i].skip) {
                                xpoints[i].skip = false;
                                xpoints[i].wind *= -1;
                                i--;
                            }
                            
                            break;
                        }
                        
                        if(xpoints[i].skip) {
                            xpoints[i].skip = false;
                            xpoints[i].wind *= -1;
                            i--;
                        }
                    }
                    
                    if(xpoints.begin() + i == xpoints.end()) continue;
                    
                    std::sort(xpoints.begin() + i + 1, xpoints.end(), [](auto l, auto r) { return l.second < r.second; });
                }
            });
        }
    }
    
    /**
     * This function fills the given point list as a polygon. List is treated as closed
     * where last pixel connects to the first. 
     */
    template<int S_ = 8, class P_, class F_ = SolidFill<>>
    void Polyfill(Graphics::Bitmap &target, const Geometry::PointList<P_> &points, F_ fill = SolidFill<>{Graphics::Color::Black}) {
        if(target.HasData())
            Polyfill<S_>(target.GetData(), points, fill);
    }
    
} }
