#include <Gorgon/Containers/Collection.h>
#include <Gorgon/Game/Map/TiledMap.h>
#include <Gorgon/Game/Parse/FillerSystem/Filler.h>
#include <Gorgon/Game/Renderer/Renderer.h>
#include <Gorgon/Graphics/Animations.h>
#include <Gorgon/Graphics/Bitmap.h>
#include <Gorgon/Struct.h>
#include <Gorgon/TMP.h>

Gorgon::Game::Parse::Filler::internal::any foo(Gorgon::Game::Parse::Filler::internal::any val) {
    return val;
}


int main() {

    Gorgon::Containers::Collection<Gorgon::Graphics::RectangularAnimation> anm; 
    
    Gorgon::Game::Map::Tiled::Map map("fishbg.tmx"); 
    std::cout << map.GetObjectGroup(0).name << std::endl << map.GetObjectGroup(0).previous_layer_index; 

    return 0; 
}
