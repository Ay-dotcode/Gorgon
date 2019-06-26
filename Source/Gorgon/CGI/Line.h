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
		Geometry::Pointf prevoff;

		//first point is special
		{
			Geometry::Line<P_> l = p.LineAt(0);
			auto off = (Geometry::Pointf(l.End) - Geometry::Pointf(l.Start)).Perpendicular().Normalize() * w;

			//if closed the first two points will be added last
			if(p.Front() != p.Back()) {
				//point for left polygon
				points[0].Push(l.Start + off);

				//point for right polygon
				points[1].Push(l.Start - off);
			}

			//point for left polygon
			points[0].Push(l.End + off);

			//point for right polygon
			points[1].Push(l.End - off);

			prev = l;
			prevoff = off;
		}

        
        for(int i=1; i<p.Size()-1; i++) {
            Geometry::Line<P_> l = p.LineAt(i);
            
            auto off = (Geometry::Pointf(l.End) - Geometry::Pointf(l.Start)).Perpendicular().Normalize() * w;
            
			auto prevv = (prev.End - prev.Start).Normalize();
			auto dotp = (prevv * off);

			auto r = l.End - l.Start;
			auto s = prev.End - prev.Start;

			if(dotp >= 0) {
				auto p = l.Start - off;
				auto q = prev.Start - prevoff;

				auto intersect = q + s * ((q - p).CrossProduct(r) / r.CrossProduct(s));

				points[1].Pop();
				points[1].Push(intersect);
			}
			else {
				points[1].Push(l.Start - off);
			}
			points[1].Push(l.End - off);

			if(dotp <= 0) {
				auto p = l.Start + off;
				auto q = prev.Start + prevoff;

				auto intersect = q + s * ((q - p).CrossProduct(r) / r.CrossProduct(s));

				points[0].Pop();
				points[0].Push(intersect);
			}
			else {
				points[0].Push(l.Start + off);
			}
			points[0].Push(l.End + off);
            
            prev = l;
			prevoff = off;
        }
        
        //if closed, keep left/right polygons separate
        if(p.Front() == p.Back()) {
			//add start points of first line by checking the angle with the last line
			Geometry::Line<P_> l = p.LineAt(0);

			auto off = (Geometry::Pointf(l.End) - Geometry::Pointf(l.Start)).Perpendicular().Normalize() * w;

			auto prevv = (prev.End - prev.Start).Normalize();
			auto dotp = (prevv * off);

			auto r = l.End - l.Start;
			auto s = prev.End - prev.Start;

			if(dotp >= 0) {
				auto p = l.Start - off;
				auto q = prev.Start - prevoff;

				auto intersect = q + s * ((q - p).CrossProduct(r) / r.CrossProduct(s));

				points[1].Pop();
				points[1].Push(intersect);
			}
			else {
				points[1].Push(l.Start - off);
			}

			if(dotp <= 0) {
				auto p = l.Start + off;
				auto q = prev.Start + prevoff;

				auto intersect = q + s * ((q - p).CrossProduct(r) / r.CrossProduct(s));

				points[0].Pop();
				points[0].Push(intersect);
			}
			else {
				points[0].Push(l.Start + off);
			}

			points[0].Push(points[0][0]);
            points[1].Push(points[1][0]);
            std::reverse(points[1].begin(), points[1].end());
        }
        else { //if open join left/right polygons
            for(auto it=points[1].rbegin(); it!=points[1].rend(); ++it) {
                points[0].Push(*it);
            }
            points[0].Push(points[0][0]);
            points.pop_back();
        }
    
        Polyfill<S_, 0, P_, F_>(target, points, stroke);
    }
    
} }
