#include "Parser.h"
#include "Exceptions.h"
#include <regex>
#include <string>


Gorgon::Game::Parser::basic_parser::basic_parser(const std::string &fileName) : fileName(fileName) {
  fs.open(fileName, std::ios::in);
}

Gorgon::Game::Parser::Parsed::Object Gorgon::Game::Parser::basic_parser::parse(const std::vector<std::pair<std::string, std::vector<std::string>>> SearchList) {
    if(!fs.is_open()) {
        throw Game::Parser::Exception::FileOpenException("File could not be opened.");
    }
    
    std::string line; 

    while(fs >> line) {
        // Check if the line is a comment
        if(line[0] == '#') {
            continue;
        }

        // Check if the line is empty
        if(line.empty()) {
            continue;
        }

    }


    return Parsed::Object();
}

template<class SearchType>
Gorgon::Game::Parser::Experimental::Any::nullable_type<std::string> Gorgon::Game::Parser::basic_parser::find_text(const std::string &haystack, const std::string &needle) {
    return SearchType::find_text(haystack, needle);
}

Gorgon::Game::Parser::Experimental::Any::nullable_type<std::string> Gorgon::Game::Parser::basic_parser::RegexSearch::find_text(const std::string &haystack, const std::string &needle) {
    if(std::regex_search(haystack, std::regex(needle))) {
        return Experimental::Any::nullable_type(haystack);
    }
    return {};
}

Gorgon::Game::Parser::Experimental::Any::nullable_type<std::string> Gorgon::Game::Parser::basic_parser::StringSearch::find_text(const std::string &haystack, const std::string &needle) {
    if(haystack.find(needle) != std::string::npos) {
        return Experimental::Any::nullable_type(haystack);
    }
    return {};
}