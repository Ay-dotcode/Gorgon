#include "Parser/Parse.h"
#include <iostream>
#include <string>
#include <vector>

std::string operator""_print(const char *str, size_t size) {
  std::cout << std::format("{}", str) << std::endl;
  return {str};
}

int main(int argc, char **argv) {
  using Game::Parser::Experimental::Console::Out::echo;

  Game::Parser::basic_parser parser("map.tmx");
  try {
      parser.Parse<std::string>({{"tileset", {".*", "height"}}});
  }
  catch(Game::Parser::Exception::UnacceptableTypeException Ex) {
      std::cout << std::format("Error: {}", Ex.what()) << std::endl;
  }
  

  return 0;
}
