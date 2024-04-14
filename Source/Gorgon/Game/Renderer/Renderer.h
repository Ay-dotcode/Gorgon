/*

Renderer.h
Generic rendering system. 
Provides function, not a class. 
It's ODR violation but still the Renderer function will be here. 

*/
#pragma once
#ifndef RENDERER_H
#define RENDERER_H
#include <Gorgon/Game/Map/Tiled.h>
#include <Gorgon/Graphics/Bitmap.h>


namespace Gorgon::Game::Rendering {

    // Generic renderer for tiled maps.
    void generic_isometric_renderer(std::vector<Graphics::Bitmap> bitmapList, Map::Tiled::TiledMap map) {
        int x = 0, y = 0, i = 0; 
        for(const auto &layer : map.GetLayerMapData()) {
            for(const auto& row : layer) {
                for(const auto& tile : row) {
                    if(tile.firstgid == "0") {
                        continue;
                    }
                }
            }
        }
    } 

}
#endif // RENDERER_H