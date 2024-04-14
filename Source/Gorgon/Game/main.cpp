#include <Gorgon/Game/Parse/FillerSystem/Filler.h>
#include <Gorgon/Game/Renderer/Renderer.h>
#include <Gorgon/Graphics/Bitmap.h>
#include <Gorgon/Struct.h>
#include <Gorgon/Game/Map/Tiled.h>
#include <Gorgon/TMP.h>



struct Test {
    public: 
    int foo = 5; 
    float bar = 3.14;
    DefineStructMembers(Test, foo, bar);  
}; 

int main() {
    Test test;


    Gorgon::Game::Map::Tiled::TiledMap map("map.tmx");
    
    return 0; 
}
