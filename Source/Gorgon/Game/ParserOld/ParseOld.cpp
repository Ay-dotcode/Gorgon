//
// Created by theany on 10.02.2024.
//
#include "ParseOld.h"

#include <optional>
#include <regex>
#include <vector>


namespace Game::Parser {
    /* User literals */
    std::regex operator ""_reg(const char *str, size_t size) {
        return std::regex(str);
    }
    std::string operator ""_str(const char *str, size_t size) {
        return {str};
    }
    int operator ""_int(const char *str, size_t size){
        return std::stoi(str);
    }

    bool basic_parser::tagOpen(const std::string& str) { return str.find('<') != -1; }
    bool basic_parser::tagClose(const std::string& str)  { return str.find('>') != -1; }

    /* ------------ Implementation of Get class functions ------------ */

    // check if a given string is a legit numeric string
    bool basic_parser::Get::isNum(const std::string& str) {
        return std::all_of(str.begin(), str.end(), [](auto x) {
          return !(x < '0' or x > '9' and x != ' ');
        });
    }

    // search and get the text of given key
    std::string basic_parser::Get::get_text(const std::regex &needle, std::string haystack, const char &delim, const char &e) {
        if(const auto res = std::regex_search(haystack, needle); !res)
            return "";
        // Remove delim and the needle
        erase_if(haystack, [delim](const char &x) { return x == delim; });
        return haystack.substr(haystack.find(e) + 1);
    }

    // search and get the int value of given key
    int basic_parser::Get::get_int(const std::regex &needle, std::string haystack, const char &delim, const char &e) {
        const auto get = get_text(needle, std::move(haystack), delim, e);
        if(get.empty() or get == " ") {
            return -1;
        }
        if (!isNum(get)) {
            return -1;
        }
        return std::stoi(get);
    }

    std::string basic_parser::Get::get_text_no_key(const std::string &haystack, const char &delim, const char &e) {
        auto ret = haystack;
#ifdef DEBUG
#ifdef DOUT
        std::cout << ( ret.find(e) != -1 ? ret.substr(ret.find(e)) : "No char: " + e) << "\n";
#endif
#endif
        return ret;
    }

    /* */

    void basic_parser::isTagOpen(const std::string &str, bool &setter) {
        if(tagOpen(str) && !setter) {
            setter = true;
#ifdef DEBUG
#ifdef TAGOPENOUT
            std::cout << "Tag opened by: " << str << "!\n";
#endif
#endif
        }
        if(tagClose(str) && setter) {
            setter = false;
#ifdef DEBUG
#ifdef TAGCLOSEOUT
            std::cout << "Tag closed by: " << str << "!\n";
#endif
#endif
        }
    }

    bool basic_parser::isTagOpen(const std::string &str) {
        static bool setter = false;
        isTagOpen(str, setter);
        return setter;
    }

    std::optional<std::string> basic_parser::if_found_collect(const std::string &haystack, const std::string& key) {
        static bool found = false;
        if(haystack.find(key) != -1) {
            found = true;
        }
        if(!found)
            return std::nullopt;
        if(tagClose(haystack)) {
            found = false;
        }
        std::string temp = haystack; 
        std::erase_if(temp, [](char ch) {
            return ch == '<' or ch == '>';  
        });
        return temp;
    }


    template <class Ty>
    std::string basic_parser::parse(std::vector<std::pair<std::string, Ty>> SearchList) {
        return "";
    }
  
}
