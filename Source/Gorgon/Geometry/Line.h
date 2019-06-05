#pragma once

#include <vector>
#include <exception>

#include "Point.h"

namespace Gorgon { namespace Geometry {
    
    /**
     * This class represents a set of points. This class can be used for 
     * contours, line lists, polygons, or even point clouds. Some of the
     * functions may be meaningful for some of these uses.
     */
    template<class P_ = Pointf>
    class Line {
    public:
        
        ///Empty constructor
        Line() = default;
        
        Line(P_ start, P_ end) : Start(start), End(end) { }
        
        ///Returns the slope of the line. Assumes the line is 2D.
        Float Slope() const {
            return Float(End.Y - Start.Y) / (End.X - Start.X);
        }
        
        ///Returns the offset of the line. Assumes the line is 2D.
        Float Offset() const {
            return Start.Y - Slope() * Start.X;
        }
        
        //Needs a lot more operators and functions...
        
        ///Starting point of the line.
        P_ Start;
        
        ///Ending point of the line
        P_ End;
    };

    
    //non-member operations: translate, scale, rotate, etc...
    
} }
