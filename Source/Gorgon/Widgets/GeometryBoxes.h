#pragma once

#include "Inputbox.h"
#include "../Geometry/Point.h"
#include "../Geometry/Size.h"
#include "../Geometry/Bounds.h"
#include "../Geometry/Margin.h"

namespace Gorgon { namespace Widgets {
    
    /// An inputbox variant designed to edit points.
    using Pointbox = Inputbox<Geometry::Point>;
    
    /// An inputbox variant designed to edit points with floating point coordinates.
    using Pointfbox = Inputbox<Geometry::Pointf>;
    
    /// An inputbox variant designed to edit size data.
    using Sizebox = Inputbox<Geometry::Size>;
    
    /// An inputbox variant designed to edit bounds data.
    using Boundsbox = Inputbox<Geometry::Bounds>;
    
    /// An inputbox variant designed to edit margin data.
    using Marginbox = Inputbox<Geometry::Margin>;
    
} }
