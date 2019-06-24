#pragma once

#include "Polygon.h"

namespace Gorgon { namespace CGI {
    
    struct StrokeSettings {
        StrokeSettings(Float width = 1.0f) : width(width) { 
        }
        
        Float width;
        
    };
    
    
    /**
     * Draw a point list as a list of lines. Currently cannot properly
     * draw closed lines.
     */
    template<int S_ = 8, class P_= Geometry::Pointf, class F_ = SolidFill<>>
    void DrawLines(Containers::Image &target, const Geometry::PointList<P_> &p, StrokeSettings settings = 1.0, F_ stroke = SolidFill<>{Graphics::Color::Black}) {
        Geometry::PointList<P_> points;
        Geometry::PointList<P_> ret;
        
        Float w = settings.width / 2;
        
        for(int i=0; i<p.Size()-1; i++) {
            Geometry::Line<P_> l = p.LineAt(i);
            
            auto v = (l.End - l.Start).Perpendicular().Normalize();
            points.Push(l.Start + v * w);
            points.Push(l.End + v * w);
            ret.Push(l.Start - v * w);
            ret.Push(l.End - v * w);
        }
        
        for(auto it=ret.rbegin(); it!=ret.rend(); ++it) {
            points.Push(*it);
        }
        points.Push(points[0]);
        
        for(auto &p : points) {
            std::cout<<p.X<<","<<p.Y<<" ";
        }
        std::cout<<std::endl;
    
        Polyfill<S_, P_, F_>(target, points, stroke);
    }
    
} }
