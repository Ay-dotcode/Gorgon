#pragma once 
#include <Gorgon/Geometry/Size.h>
#include <Gorgon/Geometry/PointList.h>
#include <Gorgon/Geometry/Point.h>
#ifdef WIN32
using uint = unsigned int;
#endif
namespace Gorgon::Game::Pathfinding {
    struct TileNode
    {
        uint G, H;
        Geometry::Point coordinates;
        TileNode *parent;

        TileNode(Geometry::Point coord_, TileNode *parent_ = nullptr) : parent(parent_), G(0), H(0), coordinates(coord_) {}
        uint get_score() {
            return G + H;
        }
    };
    using NodeSet = std::vector<TileNode*>;
    using CoordinateList = Geometry::PointList<>;


    class base_pathfinder {
        public: 
        base_pathfinder() = default; 
        virtual void SetSize(Gorgon::Geometry::Size) = 0; 
        virtual CoordinateList FindPath(Gorgon::Geometry::Point, Gorgon::Geometry::Point) = 0; 
        virtual void SetDiagonalMovement(bool) = 0; 
        virtual void AddBlock(Gorgon::Geometry::Point) = 0;
        virtual void RemoveBlock(Gorgon::Geometry::Point) = 0; 
        virtual void ClearBlocks() = 0; 
    };
 }