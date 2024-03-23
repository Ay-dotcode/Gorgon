#include "Parser/Parser.h"
#include <iostream>
#include <format>
#include <utility>
#include <vector>

using namespace Gorgon::Game::Parser::Experimental::Any;
using namespace Gorgon::Game::Parser; 



int main(int argc, char **argv) {
  nullable_type<int> a {5}; 
  std::cout << std::format("{}", a) << std::endl; 
  basic_parser parser("map.tmx"); 
  parser.parse({{"<tileset", {"name"}}});
  return 0;
}
