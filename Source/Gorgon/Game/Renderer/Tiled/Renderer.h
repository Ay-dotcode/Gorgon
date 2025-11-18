#pragma once
#include <Gorgon/Game/Map/TiledMap.h>
#include "../Renderer.h"

namespace Gorgon::Game::Rendering::Tiled {
    template<class Derived>
    using base_renderer = base_tile_renderer<Map::Tiled::Map, Derived>;
    using StandardRenderer = StandardTileRenderer<Map::Tiled::Map>; 
    using IsometricRenderer = IsometricTileRenderer<Map::Tiled::Map>;  
}