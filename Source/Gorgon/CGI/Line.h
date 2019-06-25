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
        if(p.Size() == 0) return;
        
        std::vector<Geometry::PointList<P_>> points;
        points.push_back({});
        points.push_back({});
        
        Float w = settings.width / 2;
        
        for(int i=0; i<p.Size()-1; i++) {
            Geometry::Line<P_> l = p.LineAt(i);
            
            auto v = (l.End - l.Start).Perpendicular().Normalize();
            points[0].Push(l.Start + v * w);
            points[0].Push(l.End + v * w);
            points[1].Push(l.Start - v * w);
            points[1].Push(l.End - v * w);
        }
        
        if(p[0] == p[p.Size()-1]) {
            points[0].Push(points[0][0]);
            points[1].Push(points[1][0]);
        }
        else {
            for(auto it=points[1].rbegin(); it!=points[1].rend(); ++it) {
                points[0].Push(*it);
            }
            points[0].Push(points[0][0]);
            points.pop_back();
        }
    
        Polyfill<S_, P_, F_>(target, points, stroke);
    }
    
} }
