#include <iostream>
#include <fstream>

int main(int argc, char* argv[]){
    
    if(argc != 2) {
        std::cout<<"Usage: "<<argv[0]<<" file"<<std::endl;
        return 1;
    }
    
    std::ofstream file;
    file.open(argv[1]);
    
    file << "#include <iostream>\n\n using namespace std;\n\nint main(int argc, char* argv[])\n{\n\tcout << \"Gscript Generator\"; \n\nreturn 0;\n} ";
    
    file.close();
    return 0;
    
}
