#include <Gorgon/Containers/Collection.h>
#include <Gorgon/Game/Map/TiledMap.h>
#include <Gorgon/Game/ObjectFinding/FindObject.h>
#include <Gorgon/Game/Parse/FillerSystem/Filler.h>
#include <Gorgon/Game/Pathfinding/AStar.h>
#include <Gorgon/Game/Renderer/Renderer.h>
#include <Gorgon/Game/World/World.h>
#include <Gorgon/Graphics/Animations.h>
#include <Gorgon/Graphics/Bitmap.h>
#include <Gorgon/Graphics/Layer.h>
#include <Gorgon/Scene.h>
#include <Gorgon/Struct.h>
#include <Gorgon/TMP.h>
#include <format>
 

int main() {
    Gorgon::Game::Map::Tiled::Map map{"fishbg.tmx"}; 
    auto objects = map.GetObjectGroup(0).GetObjects();
    Gorgon::Game::ObjectFinding::Tiled::FindObject finder; 
    auto results = finder.SearchArea(objects, {300, 150}, {400, 250}); 

    Gorgon::Game::Pathfinding::AStar::PathFinder path_finder; 
    path_finder.SetSize({map.tilewidth, map.tileheight}); 
    
    auto path = path_finder.FindPath({1, 1}, {results[0].x / map.tilewidth, results[0].y / map.tileheight});

    for(auto node : path) {
        std::cout << std::format("({}, {}), ", node.X, node.Y); 
    }
    return 0;
}
