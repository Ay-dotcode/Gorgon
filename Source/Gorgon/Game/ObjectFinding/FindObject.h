#pragma once

#include "ObjectFinding.h"
#include <Gorgon/Game/Map/TiledMap.h>
#include <Gorgon/Geometry/Point.h>
#include <Gorgon/Geometry/Size.h>
#include <utility>

namespace Gorgon::Game::ObjectFinding::Tiled {
    class FindObject : public base_objectfinder<Map::Tiled::Object>{
        public: 
        ObjectList<Map::Tiled::Object> SearchArea(std::vector<Map::Tiled::Object> obj_list, Geometry::Point top_left, Geometry::Point bottom_right) override {
            ObjectList<Map::Tiled::Object> ret; 
            for(auto object : obj_list) {
                if(object.x > top_left.X and object.x < bottom_right.X and object.y > top_left.Y and object.y < bottom_right.Y) {
                    ret.push_back(object); 
                }
            }
            return ret; 
        }

        ObjectList<Map::Tiled::Object> SearchTileGid(std::vector<Map::Tiled::Object> obj_list, int gid) {
            ObjectList<Map::Tiled::Object> ret; 
            for(auto object : obj_list) {
                if(object.gid == gid) {
                    ret.push_back(object); 
                }
            }
            return ret; 
        }

        ObjectList<Map::Tiled::Object> SearchTileId(std::vector<Map::Tiled::Object> obj_list, int id) {
            ObjectList<Map::Tiled::Object> ret; 
            for(auto object : obj_list) {
                if(object.id == id) {
                    ret.push_back(object); 
                }
            }
            return ret; 
        }

        ObjectList<Map::Tiled::Object> SearchBySize(std::vector<Map::Tiled::Object> obj_list, Geometry::Size size, bool(*compare)(Geometry::Size a, Geometry::Size b) = [](Geometry::Size a, Geometry::Size b) { 
            return a == b; 
            }) {
            ObjectList<Map::Tiled::Object> ret; 
            for(auto object : obj_list) {
                if(compare({object.width, object.height}, size)) {
                    ret.push_back(object); 
                }
            }
            return ret; 
        }

        ObjectList<Map::Tiled::Object> SearchBetweenSize(std::vector<Map::Tiled::Object> obj_list, Geometry::Size low, Geometry::Size high) {
            ObjectList<Map::Tiled::Object> ret; 
            auto lower = [&](Geometry::Size low, Geometry::Size high) {
                return low.Area() < high.Area(); 
            }; 
            if(not lower(low, high)) {
                std::swap(low, high); 
            }
            for(auto object : obj_list) {
                Geometry::Size obj_size = {object.width, object.height}; 
                if(lower(low, obj_size) and lower(obj_size, high)) {
                    ret.push_back(object); 
                }
            }
            return ret; 
        }
    }; 
}