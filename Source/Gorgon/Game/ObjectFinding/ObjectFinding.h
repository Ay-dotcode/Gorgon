#pragma once 
#include <Gorgon/Geometry/Size.h>
#include <Gorgon/Geometry/PointList.h>
#include <Gorgon/Geometry/Point.h>
#include <vector>

namespace Gorgon::Game::ObjectFinding {
    template<typename T_>
    using ObjectList = std::vector<T_>;
    using CoordinateList = Geometry::PointList<>;

    template<typename T_>
    class base_objectfinder {
        public: 
        base_objectfinder() = default; 
        virtual ObjectList<T_> SearchArea(std::vector<T_>, Geometry::Point, Geometry::Point) = 0; 
    };
}