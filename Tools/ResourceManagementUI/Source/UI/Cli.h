#pragma once
#include <string>
#include <vector>
#include "Gorgon/String.h"


namespace UI{

    struct CommandType{
        std::string name, val; 
    };
    
    class Cli{
    public:
        Cli(std::vector<std::string> &command);
        std::vector<UI::CommandType> GetMap()const;
        void Pop()const;
        void Push(const CommandType &val);
        void SetCommand(const std::vector<std::string> &command);
        std::vector<std::string> GetCommand();
        void Parse();
        const bool Empty();
        const bool HasFile();
        const bool HasQuit();
        const bool Find(const std::string name);
        const std::string GetFile();
    private:
        std::vector<std::string> commandline;
        std::vector<UI::CommandType> comMap;
        std::string files;
        bool quit = false;
    };
}
