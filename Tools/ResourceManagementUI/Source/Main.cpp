#include <iostream>
#include <Gorgon/Main.h>
#include <Gorgon/String.h>
#include <Gorgon/OS.h>

#include <Gorgon/Filesystem.h>

#include "UI/App.h"

#include <filesystem>

#ifdef LINUX
#include <unistd.h>
#include <wait.h>


namespace Gorgon { namespace OS {
    bool Start(const std::string &name, std::streambuf *&buf, const std::vector<std::string> &args);
} }
#endif

std::vector<std::string> PrepareArguments(int argc, char *argv[]) {
    std::string line;
    std::vector<std::string> commandline;
    for (int i = 1; i < argc; i++) {
        line += argv[i];
        if (line.find("=") != std::string::npos) {
            if (line.at(0) == '-')
                line.erase(0, 1);
            commandline.push_back(line);
            line.clear();
        }
        else if (line.find("-") == std::string::npos) {
            commandline.push_back(line);
            line.clear();
        }
        else {
            auto n = Gorgon::String::ToLower(line);
            if (n == "-quit") {
                line.erase(0, 1);
                commandline.push_back(line);
                line.clear();
            }
        
        }
    }
    return commandline;
}

//Main execution function of Resource Management UI
int main(int argc, char *argv[]){
    int fs = 12;
    std::string fontName = "";
    std::vector<std::string> commandline;
    if (argc > 0) 
        commandline = PrepareArguments(argc, argv);
    
    Gorgon::Initialize("ResourceManagementUI");
    //This will set up default fonts on a linux system if the provided font is not available
    std::streambuf *buf;
    Gorgon::OS::Start("fc-match", buf, {"-v", "sans"});
    system("pwd");
    
    std::string fp = Gorgon::Filesystem::CurrentDirectory();
    
    //Initializes the UI
    UI::App app({850,440}, fs, "Gorgon Resource Manager", commandline, fp);

    //Continues to diplay the UI until it is closed by the user
    while(true){
        
        Gorgon::NextFrame();
        
    }
    
    return 0;
}
