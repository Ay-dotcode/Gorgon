#include <Gorgon/Game/Parse/FillerSystem/Filler.h>
#include <Gorgon/Struct.h>
#include <Gorgon/Game/Map/Tiled.h>

int main() {
    Gorgon::Game::Map::Tiled::TiledMap map("map.tmx"); 
    for(const auto& row : map.GetLayerMapData()[2]) {
        for(const auto& col : row) {
            std::cout << std::format("{},", col.inner.first_attribute().value()); 
        }
        std::cout << "\n";
    }


    return 0; 
}