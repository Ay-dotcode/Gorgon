#pragma once 

#include <exception>
#include <string>
namespace Gorgon::Game::Exception {
    class bad_parameter : public std::exception {
        private: 
        std::string msg;
        public:
        bad_parameter(const char * msg) : msg(msg) {}
        bad_parameter(std::string msg) : msg(msg) {} 
        bad_parameter() : msg("Bad parameter") {};

        const char * what() {
            return msg.c_str(); 
        }
    }; 

    class not_implemented : public std::exception {
        private: 
        std::string msg;
        public:
        not_implemented(const char * msg) : msg(msg) {}
        not_implemented(std::string msg) : msg(msg) {} 
        not_implemented() : msg("Not implemented.") {};

        const char * what() {
            return msg.c_str(); 
        }
    };

    class not_prepared : public std::exception {
        private: 
        std::string msg;
        public:
        not_prepared(const char * msg) : msg(msg) {}
        not_prepared(std::string msg) : msg(msg) {} 
        not_prepared() : msg("Not implemented.") {};

        const char * what() {
            return msg.c_str(); 
        }
    };

    class not_ready : public std::exception {
        private: 
        std::string msg;
        public:
        not_ready(const char * msg) : msg(msg) {}
        not_ready(std::string msg) : msg(msg) {} 
        not_ready() : msg("Not implemented.") {};

        const char * what() {
            return msg.c_str(); 
        }
    };

    class ParseFailed : public std::exception {
        std::string msg; 
        public: 
        ParseFailed(const char * msg = "Parse failed!") : msg(msg) {}
        const char * what() {
            return msg.c_str();
        }
    };

    
}; 