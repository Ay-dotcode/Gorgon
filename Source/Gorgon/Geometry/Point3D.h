#pragma once

#include <iostream>
#include "Point.h"

namespace Gorgon { namespace Geometry {
    
    template<class T_>
    class basic_Point3D {
    public:
			/// Default constructor, does not zero initialize point.
        basic_Point3D() { }

        /// Filling constructor
        basic_Point3D(const T_ &X, const T_ &Y, const T_ &Z) : X(X), Y(Y), Z(Z) { }

        /// Filling constructor
        basic_Point3D(const basic_Point<T_> &point, const T_ &Z = T_()) : X(point.X), Y(point.Y), Z(Z) { }
        
        
        
        T_ X;
        T_ Y;
        T_ Z;
    };

    /// Allows streaming of point. A point will be printed inside parenthesis with
    /// a comma separating X and Y values.
    template <class T_>
    std::ostream &operator << (std::ostream &out, const basic_Point3D<T_> &point) {
        out<<"("<<point.X<<", "<<point.Y<<", "<<point.Z<<")";

        return out;
    }
    
    using Point3Df = basic_Point3D<float>;
    
} }
