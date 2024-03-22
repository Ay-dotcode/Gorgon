#include "Parser/Parser.h"
#include <iostream>
#include <format>

using namespace Gorgon::Game::Parser::Experimental::Any;


int main(int argc, char **argv) {
  any_printable a {std::any()}; 
  std::cout << std::format("{}", a) << std::endl; 
  return 0;
}
