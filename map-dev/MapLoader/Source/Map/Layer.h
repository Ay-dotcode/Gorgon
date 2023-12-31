#pragma once


#include <Gorgon/Geometry/Size.h>
#include <Gorgon/Resource/Image.h>
#include <vector>
template<class Tile_Type, class Object> 
class Layer_Base {
    protected:
    std::vector<Tile_Type> tiles; 
    std::vector<Object>    objects;
    Gorgon::Geometry::Size size;



}; 

template<class Tile> 
class Tile_Base {
    protected:
    Tile tile;

};