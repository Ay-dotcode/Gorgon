#include <iostream>
#include <fstream>
#include <expat.h>
#include <string>
#include <cstring>
#include <memory>
#include <algorithm>
#include <vector>
//#include <cstdlib>

#ifdef XML_LARGE_SIZE
# if defined(XML_USE_MSC_EXTENSIONS) && _MSC_VER < 1400
#  define XML_FMT_INT_MOD "I64"
# else
#  define XML_FMT_INT_MOD "ll"
# endif
#else
# define XML_FMT_INT_MOD "l"
#endif

#ifdef XML_UNICODE_WCHAR_T
# define XML_FMT_STR "ls"
#else
# define XML_FMT_STR "s"
#endif

int Depth;
std::string fileOut;
int carryOn = 1;
std::vector<std::string> cell; 
std::string xmlData;
bool accept;



static void addToFile(std::string data){
    std::ofstream file;
    file.open(fileOut, std::ios_base::app);
    file << data;
    file.close();
}

//XML start function
static void XMLCALL
start(void *data, const XML_Char *el, const XML_Char **attr){
    
    int i;
    
    //Reading in the attributes of the element
    for (i = 0; attr[i]; i += 2){
        cell.push_back(attr[i]);
        cell.push_back(attr[i+1]);
    }
    
    Depth++;
    
}

//XML Data function
static void XMLCALL
fileData(void *data, const char* content, int len){
    
    std::string temp;
    std::string info;
        
    temp = content ;
    info = temp.substr(0,len);
    xmlData = info;
    
}

//XML end function
static void XMLCALL 
end(void *data, const XML_Char *el){
    
    int i;
    for (i = 0; i < Depth; i++){
        addToFile(" ");
    }
    std::string info = xmlData;
    
    if(strcmp(el,"compoundname")==0)
        addToFile( "\n#include \"" + info + "\"\n");
    
    if(strcmp(el,"innernamespace")==0)
        addToFile("namespace " + info + " {");
    
    
    Depth--;
}

//Main
int main(int argc, char* argv[]){
    
    if(argc != 2) {
        std::cout<<"Usage: "<<argv[0]<<" file"<<std::endl;
        return 1;
    }
    //File creation and initialization
    fileOut = argv[1];
    std::ofstream file;
    file.open(fileOut);
    //std::string fileName = "Time";
    
    file << "//Some Random Information";
    file << "\n#include \"Gorgon/Scripting/Embedding.h\"\n#include \"Gorgon/Scripting/Reflection.h\"\n#include \"Gorgon/Scripting.h\"\n\n";
    
    
    //Start of parsing
    std::cout<< "Starting...\n";
    XML_Parser p = XML_ParserCreate(NULL);
    std::cout<< "Parser Created" << std::endl;;
    if (!p){
        std::cout<< "Couldn't allocate memory for parser\n";
        exit(-1);
    }
    
    XML_SetElementHandler(p, start, end);
    XML_SetCharacterDataHandler(p, fileData);
    
    while(carryOn){
        int done;
        int check;
        
        std::ifstream f1;
        f1.open("/home/luca/gorgon-game-engine/Docs/xml/_time_8h.xml");
        
        f1.seekg(0, std::ios::end);
        size_t len = f1.tellg();
        f1.seekg(0, std::ios::beg);
        char Buff[len];
        std::cout<< "len added..."<< len << std::endl;
//         if(len > sizeof (Buff))
//             len = sizeof (Buff);
        std::cout<< "len changed?..."<< len << std::endl;
        f1.read(Buff,len);
            
//          std::cout<< Buff<< std::endl;
        
        if(f1.fail()){
            std::cout<< "Read Error!\n";
            exit(-1);
        }
        
        done = f1.eof();
        std::cout<< "Done set..."<< done << std::endl;
        
        check = XML_Parse(p, Buff, len, done);
        std::cout<< "check set..."<< check << std::endl;
        if(check == XML_STATUS_ERROR){
            std::cout<< "Status Error... " << check << std::endl;
            exit(1);
        }
        std::cout<< "Passed if..."<< std::endl;
        if(done)
            break;
        std::cout<< "Passed break..."<< std::endl;
        carryOn = 0;
    }
    
    XML_ParserFree(p);
    
    std::ofstream file1;
    file1.open(fileOut, std::ios_base::app);
    file1 << " \n\t}\n}";
    //file1 << "\n\n namespace Gorgon { namespace Time {\n\tScripting::Library LibTime(\"Time\",\"Data types under Time.h module and their member functions and operators\");  \n\t}\n} \n ";
    file1.close();
    
    std::cout<< "Finished Parsing!!\n";
    return 0;
}
