/* 

Okay I need to fix the problems but I donT wanna entirely start from zero or continue that code because 
I'm having a stupid undefined reference error and I don't know why. 

This is an XML Parser

*/


#ifndef PARSER_H
#define PARSER_H
#include <any>
#include <string>
#include <fstream>
#include <utility>
#include <vector>
#include <ostream>
#include <format>
#include "Parsed.h"
/* 

Start of Gorgon::Game namespace

*/



namespace Gorgon::Game::Parser::Experimental::Any {
    /* Holder class for null any to format */
    class any_null_type {};

    template<typename  Ty = any_null_type>
    class any_printable {
        private: 
            std::any a;
        public: 
            any_printable(Ty a) : a(a) {}
            any_printable(std::any a) : a(a) {}
            void operator=(const Ty &obj) {
                a = obj;
            }

            Ty Get() const{
                return std::any_cast<Ty>(a);
            }

            friend std::ostream& operator<<(std::ostream &os, const any_printable<Ty> &a) {
                os << std::format("{}", a.Get());
                return os;
            }
            bool has_value() const {
                return a.has_value();
            }
    };

}

template<class T, class CharT>
struct std::formatter<Gorgon::Game::Parser::Experimental::Any::any_printable<T>, CharT> : std::formatter<T, CharT> 
{
    template<class FormatContext>
    auto format(Gorgon::Game::Parser::Experimental::Any::any_printable<T> t, FormatContext& fc) const 
    {
        if(!t.has_value()) {
            return std::formatter<T, CharT>::format(Gorgon::Game::Parser::Experimental::Any::any_null_type {}, fc);
        }
        return std::formatter<T, CharT>::format(t.Get(), fc);
    }
};

template <>
struct std::formatter<Gorgon::Game::Parser::Experimental::Any::any_null_type> : std::formatter<std::string> {
    template<class FormatContext>
    auto format(Gorgon::Game::Parser::Experimental::Any::any_null_type, FormatContext& fc) const {
        return std::formatter<std::string>::format("Has no value.", fc);
    }
};

namespace Gorgon::Game::Parser {

    class basic_parser {
        public:
        /* This class won't allow empty initialization. */
        explicit basic_parser() = delete; 
        explicit basic_parser(const std::string &fileName);
        
        //Do not allow copying. Should be created from zero. Once instance usually more than enough
        explicit basic_parser(const basic_parser&) = delete;
        basic_parser& operator=(const basic_parser&) = delete;

        // Allow std::move 
        explicit basic_parser(basic_parser&&) = default;
        basic_parser& operator=(basic_parser&&) = default;
        ~basic_parser() {
            fs.close();
        }
        
        template<class Type>
        Parsed::Object parse(const std::vector<std::pair<std::string, Type>>);

        template<class SearchType>
        std::any find_text(const std::string &haystack, const std::string &needle);

        class RegexSearch {
            public: 
            static std::any find_text(const std::string &haystack, const std::string &needle);
        };

        class StringSearch {
            public: 
            static std::any find_text(const std::string &haystack, const std::string &needle);
        };
        

        private: 
        std::string fileName;
        std::fstream fs; 
    };



}



#endif // PARSER_H

