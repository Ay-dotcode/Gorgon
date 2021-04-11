#pragma once

#include "Line.h"

namespace Gorgon { namespace CGI {
    
    template<int S_ = GORGON_DEFAULT_SUBDIVISIONS, class I_, class T_= float, class F1_ = SolidFill<>>
    void DrawBounds(I_ &target, const Geometry::basic_Bounds<T_> &bnds, 
                    float strokewidth = 1, 
                    F1_ stroke = SolidFill<>{Graphics::Color::Black}
    ) {
        Geometry::Pointf tl = bnds.TopLeft(), br = bnds.BottomRight();
        tl.X -= strokewidth/2;
        tl.Y -= strokewidth/2;
        br.X += strokewidth/2;
        br.Y += strokewidth/2;
        
        DrawLines(target, {tl, {br.X, tl.Y}, br, {tl.X, br.Y}, tl}, strokewidth, stroke);
    }
    
    template<int S_ = GORGON_DEFAULT_SUBDIVISIONS, class I_, class T_= float, class F1_ = SolidFill<>, class F2_ = SolidFill<>>
    void DrawBounds(I_ &target, const Geometry::basic_Bounds<T_> &bnds, 
                    F2_ fill,
                    float strokewidth, 
                    F1_ stroke = SolidFill<>{Graphics::Color::Black}
    ) {
        Geometry::Pointf tl = bnds.TopLeft(), br = bnds.BottomRight();
        
        Polyfill(target, {tl, {br.X, tl.Y}, br, {tl.X, br.Y}, tl}, fill);
        
        tl.X -= strokewidth/2;
        tl.Y -= strokewidth/2;
        br.X += strokewidth/2;
        br.Y += strokewidth/2;
        
        DrawLines(target, {tl, {br.X, tl.Y}, br, {tl.X, br.Y}, tl}, strokewidth, stroke);
    }
    
    template<int S_ = GORGON_DEFAULT_SUBDIVISIONS, class I_, class T_= float, class F2_ = SolidFill<>>
    void DrawBounds(I_ &target, const Geometry::basic_Bounds<T_> &bnds, 
                    F2_ fill
    ) {
        Geometry::Pointf tl = bnds.TopLeft(), br = bnds.BottomRight();
        
        Polyfill(target, {tl, {br.X, tl.Y}, br, {tl.X, br.Y}, tl}, fill);
    }
    
} }
