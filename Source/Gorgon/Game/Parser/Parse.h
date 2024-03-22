#pragma once
#include <cstddef>
#include <exception>
#include <format>
#include <fstream>
#include <iostream>
#include <map>
#include <optional>
#include <ostream>
#include <regex>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <variant>
#include <vector>

namespace Game::Parser::Parsed {
class Object {
public:
  /* This is a default object that'll carry parsed xml objects. */
  std::map<std::string, std::variant<std::string, int, Object>> map;

  template <class Type> Type &Get(const std::string &key) {
    return std::get<Type>(map[key]);
  }
  void Add(const std::string &key,
           const std::variant<std::string, int, Object> &val) {
    map[key] = val;
  }
};
} // namespace Game::Parser::Parsed

namespace Game::Parser::Experimental::Console::Out {
class Echo_Class {
  // this is an interface class for cout
public:
  template <class T> Echo_Class &operator()(const T &o) {
    std::cout << std::format("{}", o) << std::endl;
    return *this;
  }
  template <class T> Echo_Class &operator[](const T &o) {
    std::cout << std::format("{}", o) << std::endl;
    return *this;
  }
  template <class T> Echo_Class &print(const T &o) {
    std::cout << std::format("{}", o) << std::endl;
    return *this;
  }
};
static Echo_Class echo;
} // namespace Game::Parser::Experimental::Console::Out

namespace Game::Parser::Exception {
class UnacceptableTypeException : std::exception {
  std::string msg;

public:
  UnacceptableTypeException() : msg("Unacceptable type given.") {}
  UnacceptableTypeException(std::string msg) : msg(msg) {}
  std::string what() { return msg; }
};
} // namespace Game::Parser::Exception

namespace Game::Parser {
/* These are for ease of use. */
std::regex operator""_reg(const char *str, size_t size);
// auto <var_name> = "string"; set's it as const char *, it's undesired
// situation. So _str converts it into string.
constexpr std::string operator""_str(const char *str, size_t size);
// c string to integer conversion in an easy step.
constexpr int operator""_int(const char *str, size_t size);
template <class Key, class Val>
constexpr std::map<Key, Val> make_map(const Key &key, const Val &val) {
  return {key, val};
}

class basic_parser {
  std::string fileName;
  std::map<std::string, Parsed::Object> elements;
  std::fstream fs;
  Parsed::Object parsedResult;

  static bool tagOpen(const std::string &str);
  static bool tagClose(const std::string &str);
  static bool isTagOpen(const std::string &str);
  static void isTagOpen(const std::string &str, bool &setter);
  static std::optional<std::string>
  if_found_collect(const std::string &haystack, const std::string &key);

  class Get {
    static bool isNum(const std::string &str);

  public:
    // Get text contains the key.
    constexpr static std::string get_text(const std::regex &needle,
                                          std::string haystack,
                                          const char &delim = '\"',
                                          const char &e = '=');
    // Get integer
    constexpr static int get_int(const std::regex &needle, std::string haystack,
                                 const char &delim = '\"', const char &e = '=');
    // Get text without a key, (finds a key and a value and returns it).
    constexpr static std::string get_text_no_key(const std::string &haystack,
                                                 const char &delim = '\"',
                                                 const char &e = '=');
  };


  template <class Ty>
  std::string parse(std::map<std::string, std::vector<Ty>> SearchList);

public:
  basic_parser() = delete;
  explicit basic_parser(const std::string &file_name)
      : fileName(file_name), fs(file_name) {}



  template <class Ty>
  std::string Parse(const std::map<std::string, std::vector<Ty>> &SearchList) {
    return this->parse<Ty>(SearchList);
  };
};
} // namespace Game::Parser
