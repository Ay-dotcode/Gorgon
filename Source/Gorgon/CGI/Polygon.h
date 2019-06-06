#pragma once

#include "../Containers/Image.h"
#include "../CGI.h"
#include "../Geometry/PointList.h"


namespace Gorgon { namespace CGI {
   
    /**
     * This function fills the given point list as a polygon. List is treated as closed
     * where last pixel connects to the first. 
     */
    template<class P_, class F_ = SolidFill<>>
    void Polyfill(Containers::Image &target, const Geometry::PointList<P_> &points, F_ fill = SolidFill<>{Graphics::Color::Black}) {
        if(points.Size() <= 1) return;
        
        for(auto p : points)
            target.SetRGBAAt(p, fill(0,0,0,0,0));
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
