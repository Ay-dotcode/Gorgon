#include <Gorgon/Containers/Collection.h>
#include <Gorgon/Game/Map/TiledMap.h>
#include <Gorgon/Game/Parse/FillerSystem/Filler.h>
#include <Gorgon/Game/Renderer/Renderer.h>
#include <Gorgon/Game/World/World.h>
#include <Gorgon/Graphics/Animations.h>
#include <Gorgon/Graphics/Bitmap.h>
#include <Gorgon/Graphics/Layer.h>
#include <Gorgon/Scene.h>
#include <Gorgon/Struct.h>
#include <Gorgon/TMP.h>


Gorgon::Game::Parse::Filler::internal::any foo(Gorgon::Game::Parse::Filler::internal::any val) {
  return val;
}

using Gorgon::Game::Map::Tiled::Map;
using Gorgon::Game::Rendering::StandardTileRenderer;
 

int main() {
    
  	Gorgon::Containers::Collection<Gorgon::Graphics::RectangularAnimation> anm;

  	Gorgon::Game::Map::Tiled::Map map("fishbg.tmx");
 	std::cout << map.GetObjectGroup(1).previous_object_group_index << std::endl << map.GetObjectGroup(1).previous_layer_index;


    Gorgon::SceneManager manager; 
    Gorgon::Game::World<Gorgon::Game::Rendering::StandardTileRenderer<Map>> world(manager, StandardTileRenderer<Map>{Map{}, *new Gorgon::Graphics::Layer{}});
    StandardTileRenderer<Map> renderer(Map{}, *new Gorgon::Graphics::Layer{}); 
    

  return 0;
}
