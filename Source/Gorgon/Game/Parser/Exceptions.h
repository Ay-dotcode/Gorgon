#ifndef GORGON_PARSER_EXCEPTIONS_H
#define GORGON_PARSER_EXCEPTIONS_H
#include <exception>
#include <string>

// This is an exception class for unacceptable types.
namespace Gorgon::Game::Parser::Exception {
    class UnacceptableTypeException : std::exception {
        std::string msg;
        public: 
        UnacceptableTypeException() : msg("Unacceptable type given.") {}
        UnacceptableTypeException(std::string msg) : msg(msg) {}
        std::string what() { return msg; }
    }; 

    class FileOpenException : std::exception {
        std::string msg;
        public: 
        FileOpenException() : msg("File could not be opened.") {}
        FileOpenException(std::string msg) : msg(msg) {}
        std::string what() { return msg; }
    };

    class FileReadException : std::exception {
        std::string msg;
        public: 
        FileReadException() : msg("File could not be read.") {}
        FileReadException(std::string msg) : msg(msg) {}
        std::string what() { return msg; }
    };

    class BadFormatException : std::exception {
        std::string msg;
        public: 
        BadFormatException() : msg("Bad format.") {}
        BadFormatException(std::string msg) : msg(msg) {}
        std::string what() { return msg; }
    };
}

#endif