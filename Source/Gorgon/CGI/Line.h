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
        
                                    // To ensure thickness is corrected for strictly inside
        Float w = settings.width / 2 + 1.0 / (S_ * 2);
        
        Geometry::Line<P_> prev;
        
        for(int i=0; i<p.Size()-1; i++) {
            Geometry::Line<P_> l = p.LineAt(i);
            
            auto v = (Geometry::Pointf(l.End) - Geometry::Pointf(l.Start)).Perpendicular().Normalize();
            
            if(i > 0) {
                auto prevv = (prev.End - prev.Start).Normalize();
                auto dotp = (prevv * v);
                
                auto avg = (prevv.Perpendicular() + v) / 2;
                
                if(dotp >= 0) {
                    points[1].Pop();
                    points[1].Push(l.Start - avg * w * (1 + avg.Y));
                }
                else {
                    points[1].Push(l.Start - v * w);
                }
                points[1].Push(l.End - v * w);
                
                if(dotp <= 0) {
                    points[0].Pop();
                    points[0].Push(l.Start + avg * w * (1 + dotp));
                }
                else {
                    points[0].Push(l.Start + v * w);
                }
                points[0].Push(l.End + v * w);
            }
            else {
                //points for left polygon
                points[0].Push(l.Start + v * w);
                points[0].Push(l.End + v * w);
                
                //points for right polygon
                points[1].Push(l.Start - v * w);
                points[1].Push(l.End - v * w);
            }
            
            prev = l;
        }
        
        //if closed, keep left/right polygons separate
        if(p[0] == p[p.Size()-1]) {
            points[0].Push(points[0][0]);
            points[1].Push(points[1][0]);
        }
        else { //if open join left/right polygons
            for(auto it=points[1].rbegin(); it!=points[1].rend(); ++it) {
                points[0].Push(*it);
            }
            points[0].Push(points[0][0]);
            points.pop_back();
        }
    
        Polyfill<S_, P_, F_>(target, points, stroke);
    }
    
} }
