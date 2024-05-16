#pragma once
#include <Gorgon/CGI.h>
#include <Gorgon/CGI/Line.h>
#include <Gorgon/Geometry/Line.h>
#include <Gorgon/Geometry/Point.h>
#include <Gorgon/Game/Map/TiledMap.h>
#include <Gorgon/Geometry/PointList.h>
#include <Gorgon/Geometry/Size.h>
#include <Gorgon/Graphics/Bitmap.h>
#include <Gorgon/Graphics/Color.h>
#include <Gorgon/Graphics/Layer.h>
#include <Gorgon/Utils/Assert.h>

#include <unistd.h>
#include <vector>


namespace Gorgon::Game::Pathfinding::AStar {

    using HeuristicFunction = std::function<unsigned int(Gorgon::Geometry::Point, Gorgon::Geometry::Point)>;
    using CoordinateList = Geometry::PointList<>;


    class Heuristic
    {
        static Gorgon::Geometry::Point GetHeuristic(Gorgon::Geometry::Point source_, Gorgon::Geometry::Point target_) {
            return{ abs(source_.X - target_.X),  abs(source_.Y - target_.Y) };
        }

        public:
        static uint manhattan(Gorgon::Geometry::Point source_, Gorgon::Geometry::Point target_) {
            auto delta = std::move(GetHeuristic(source_, target_));
            return static_cast<uint>(10 * (delta.Y + delta.Y));
        }

        static uint euclidean(Gorgon::Geometry::Point source_, Gorgon::Geometry::Point target_) {
            auto delta = std::move(GetHeuristic(source_, target_));
            return static_cast<uint>(10 * sqrt(pow(delta.X, 2) + pow(delta.X, 2)));
        }
        static uint octagonal(Gorgon::Geometry::Point source_, Gorgon::Geometry::Point target_) {
            auto delta = std::move(GetHeuristic(source_, target_));
            return 10 * (delta.X + delta.Y) + (-6) * std::min(delta.X, delta.Y);
        }
    };

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

    class Generator
    {
        bool DetectCollision(Gorgon::Geometry::Point coordinates_) {
            if (coordinates_.X < 0 || coordinates_.X >= worldSize.Width ||
                coordinates_.Y < 0 || coordinates_.Y >= worldSize.Height ||
                std::find(walls.begin(), walls.end(), coordinates_) != walls.end()) {
                return true;
            }
            return false;
        }

        TileNode* FindNodeOnList(NodeSet& nodes_, Gorgon::Geometry::Point coordinates_) {
            for (auto node : nodes_) {
                if (node->coordinates == coordinates_) {
                    return node;
                }
            }
            return nullptr;
        }

        void ReleaseNodes(NodeSet& nodes_) {
                for (auto it = nodes_.begin(); it != nodes_.end();) {
                    delete *it;
                    it = nodes_.erase(it);
                }
        }

    public:
        Generator() {
            SetDiagonalMovement(false);
            SetHeuristic(&AStar::Heuristic::manhattan);
            direction = {
                { 0, 1 }, { 1, 0 }, { 0, -1 }, { -1, 0 },
                { -1, -1 }, { 1, 1 }, { -1, 1 }, { 1, -1 }
            };
        }

        void SetSize(Gorgon::Geometry::Size layer_size_) {
            worldSize = layer_size_;
        }

        void SetDiagonalMovement(bool enable_) {
            directions = (enable_ ? 8 : 4);
        }
        
        void SetHeuristic(HeuristicFunction heuristic_) {
            heuristic = std::bind(heuristic_, std::placeholders::_1, std::placeholders::_2);
        }

        CoordinateList FindPath(Gorgon::Geometry::Point source_, Gorgon::Geometry::Point target_) {
            TileNode *current = nullptr;
            NodeSet openSet, closedSet;
            openSet.reserve(100);
            closedSet.reserve(100);
            openSet.push_back(new TileNode(source_));

            while (!openSet.empty()) {
                auto current_it = openSet.begin();
                current = *current_it;

                for (auto it = openSet.begin(); it != openSet.end(); it++) {
                    auto node = *it;
                    if (node->get_score() <= current->get_score()) {
                        current = node;
                        current_it = it;
                    }
                }

                if (current->coordinates == target_) {
                    break;
                }

                closedSet.push_back(current);
                openSet.erase(current_it);

                for (uint i = 0; i < directions; ++i) {
                    Gorgon::Geometry::Point newCoordinates(current->coordinates + direction[i]);
                    if (DetectCollision(newCoordinates) ||
                        FindNodeOnList(closedSet, newCoordinates)) {
                        continue;
                    }

                    uint totalCost = current->G + ((i < 4) ? 10 : 14);

                    TileNode *successor = FindNodeOnList(openSet, newCoordinates);
                    if (successor == nullptr) {
                        successor = new TileNode(newCoordinates, current);
                        successor->G = totalCost;
                        successor->H = heuristic(successor->coordinates, target_);
                        openSet.push_back(successor);
                    }
                    else if (totalCost < successor->G) {
                        successor->parent = current;
                        successor->G = totalCost;
                    }
                }
            }

            CoordinateList path;
            while (current != nullptr) {
                path.Push(current->coordinates);
                current = current->parent;
            }

            ReleaseNodes(openSet);
            ReleaseNodes(closedSet);

            return path;
        }

        void AddCollision(Gorgon::Geometry::Point coordinates_) {
            walls.Push(coordinates_);
        }

        void RemoveCollision(Gorgon::Geometry::Point coordinates_) {
            auto it = std::find(walls.begin(), walls.end(), coordinates_);
            if (it != walls.end()) {
                
                walls.Erase(it);
            }
        }
        void ClearCollisions() {
            walls.Clear();
        }

    private:
        HeuristicFunction heuristic;
        CoordinateList direction, walls;
        Gorgon::Geometry::Size worldSize;
        uint directions;
    };

}