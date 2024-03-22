#include "Parser.h"
#include "Exceptions.h"
#include <regex>
#include <format>

// Automatically call formattable objects







Gorgon::Game::Parser::basic_parser::basic_parser(const std::string &fileName) : fileName(fileName) {
  fs.open(fileName, std::ios::in);
}

template<class Type>
Gorgon::Game::Parser::Parsed::Object Gorgon::Game::Parser::basic_parser::parse(const std::vector<std::pair<std::string, Type>> SearchList) {
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
std::any Gorgon::Game::Parser::basic_parser::find_text(const std::string &haystack, const std::string &needle) {
    return SearchType::find_text(haystack, needle);
}

std::any Gorgon::Game::Parser::basic_parser::RegexSearch::find_text(const std::string &haystack, const std::string &needle) {
    if(std::regex_search(haystack, std::regex(needle))) {
        return std::any(haystack);
    }
    return std::any();
}

std::any Gorgon::Game::Parser::basic_parser::StringSearch::find_text(const std::string &haystack, const std::string &needle) {
    if(haystack.find(needle) != std::string::npos) {
        return std::any(haystack);
    }
    return std::any();
}