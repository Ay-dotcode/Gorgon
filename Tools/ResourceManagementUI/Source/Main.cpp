#include <iostream>
#include <Gorgon/Main.h>
#include <Gorgon/String.h>
#include "UI/App.h"


//Main execution function of Resource Management UI
int main(int argc, char *argv[]){
    
    Gorgon::Initialize("ResourceManagementUI");
    
    int fs = 14;
    
    UI::App app({550,350}, fs, "DejaVuSans.ttf", "Gorgon Resource Manager");

    
    while(true){
        
        Gorgon::NextFrame();
        
    }
    
    return 0;
}
