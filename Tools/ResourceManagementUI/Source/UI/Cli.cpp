#include "Cli.h"

namespace UI{
    UI::Cli::Cli(std::vector<std::string> &command):commandline(command){
        Parse();
    }
    
    void UI::Cli::Push(const UI::CommandType& val)
    {
        comMap.push_back({});
        auto &ex = comMap.back();
        ex.name = val.name;
        ex.val = val.val;
    }
    
    std::vector<UI::CommandType> Cli::GetMap() const{
        return comMap;
    }
    
    const bool Cli::HasFile(){
        return files.empty() ? false : true;
    }
    
    const bool Cli::Find(const std::string name){
        for(auto &l : comMap)
            if(l.name == name)
                return true;
        return false;
    }
    
    const bool Cli::Empty(){
        return commandline.empty() ? true : false;
    }
    
    const bool Cli::HasQuit(){
        return quit;
    }
    
    const std::string Cli::GetFile(){
        return files;
    }
    
    void Cli::Parse(){
        if(!Empty()){
            CommandType type;
            std::string n;
            for(auto &l : commandline){
                if(l.find("=") != std::string::npos){
                    n = Gorgon::String::Extract(l, '=');
                    type.name = n;
                    type.val = l;
                    Push(type);
                }
                else{
                    if(l == "quit")
                        quit = true;
                    else
                        files = l;
                }
            }
        }
    }

}
