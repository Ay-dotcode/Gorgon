#include <iostream>
#include <Gorgon/Main.h>
#include <Gorgon/String.h>
#include <Gorgon/OS.h>

#include "UI/App.h"

#ifdef LINUX
#include <unistd.h>
#include <wait.h>

namespace Gorgon { namespace OS {
    bool Start(const std::string &name, std::streambuf *&buf, const std::vector<std::string> &args);
} }
#endif

//Main execution function of Resource Management UI
int main(int argc, char *argv[]){
    int fs = 12;
    std::string fontName = "";
    
    Gorgon::Initialize("ResourceManagementUI");
    
    //This will set up default fonts on a linux system if the provided font is not available
    std::streambuf *buf;
    Gorgon::OS::Start("fc-match", buf, {"-v", "sans"});
    system("pwd");
    
    //Initializes the UI
    UI::App app({550,350}, fs, "Gorgon Resource Manager");

    //Continues to diplay the UI until it is closed by the user
    while(true){
        
        Gorgon::NextFrame();
        
    }
    
    return 0;
}
