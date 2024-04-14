#include <Gorgon/Game/Map/Tiled.h>

int main(void) {
  Gorgon::Game::Map::Tiled::TiledMap map("map.tmx");
  std::cout << map.tilewidth << std::endl;
  return 0;
}
