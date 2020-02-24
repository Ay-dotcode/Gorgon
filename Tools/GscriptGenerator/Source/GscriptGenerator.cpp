#include <iostream>
#include <fstream>
#include <expat.h>
#include <string>
#include <cstring>
#include <memory>
#include <algorithm>
#include <vector>

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

struct Element{
  std::string name;
  std::string atrName;
  std::string value;
  std::string data;
  Element():name("Empty Name"),atrName("Empty atrName"),value("Empty value"),data("Empty Data"){};
  Element(std::string n,std::string a, std::string v):name(n),atrName(a),value(v),data("null"){};
  void addData(std::string data){this->data=data;};
};

std::string fileOut;
int carryOn = 1;
std::vector<Element> elements; 
std::string name;
std::string atr;
std::string atrVal;
std::string xmlData;
bool accept;
bool open ;


///////////////////////////////////////////////////////////////////////////////////////////////////////
static void addToFile(std::string data){
    std::ofstream file;
    file.open(fileOut, std::ios_base::app);
    file << data;
    file.close();
}

//XML start function/////////////////////////////////////////////////////////////////////////////////////
static void XMLCALL
start(void *data, const XML_Char *el, const XML_Char **attr){
    int i;
    //Reading in the attributes of the element
    if(strcmp(el,"innernamespace")==0){
        for (i = 0; attr[i]; i += 2){
            atr = attr[i];
            atrVal = attr[i+1];
            Element e =  *new Element(el,attr[i],attr[i+1]);
            elements.push_back(e);
            open = true;
        }
     
    }
    else if(strcmp(el,"highlight")==0){  
        Element e;
        if(strcmp(attr[0],"class")==0 && strcmp(attr[1],"keywordtype")==0)
            e =  *new Element(el,attr[0],attr[1]);

        else if(strcmp(attr[0],"class")==0 && strcmp(attr[1],"keywordflow")==0)
            e =  *new Element(el,attr[0],attr[1]);
            
        elements.push_back(e);
        open = true;
    }
    else if(strcmp(el,"ref")==0){   
        if(strcmp(attr[2],"kindref")==0 && strcmp(attr[3],"member")==0){
            Element e =  *new Element(el,attr[2],attr[3]);
            elements.push_back(e);
            open = true;
        }
        
    }
    else if(strcmp(el,"ref")==0){
        if(strcmp(attr[2],"kindref")==0 && strcmp(attr[3],"compound")==0){
            Element e =  *new Element(el,attr[2],attr[3]);
            elements.push_back(e);
            open = true;
        }
    }
    else if(strcmp(el,"compoundname")==0){
        Element e =  *new Element(el,"null","null");
        elements.push_back(e);
        open = true;
    }
    
    
}

//XML Data function//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void XMLCALL
fileData(void *data, const char* content, int len){
    
    std::string temp;
    std::string info;
    temp = content ;
    info = temp.substr(0,len);
    //xmlData = info;
    std::cout << "|=>"<< info << "<=|";
    std::string checkName;
    if(open == 1){
        elements.back().data = info;
        open = 0;
    }
}

//XML end function////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void XMLCALL 
end(void *data, const XML_Char *el){
    
    
}

//Main///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[]){
    
    if(argc != 2) {
        std::cout<<"Usage: "<<argv[0]<<" file"<<std::endl;
        return 1;
    }
    //File creation and initialization
    fileOut = argv[1];
    std::ofstream file;
    file.open(fileOut);
    std::string fileName = "Time";
    
    file << "//Some Random Information";
    //file << "\n#include \"Gorgon/Scripting/Embedding.h\"\n#include \"Gorgon/Scripting/Reflection.h\"\n#include \"Gorgon/Scripting.h\"\n\n";
    file.close();
    
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
    
    Element job;
    elements.push_back(job);
    
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
    std::cout<< "Finished Parsing!!\n";
    
    /////////////////////////////////Inside Cells///////////////////////////////////////////
    int count = 1;
    for(auto e : elements){
        std::cout << count++ << ")- "<< e.name << " -> " << e.atrName << " = " << e.value << " |DATA -> " << e.data << std::endl;
       
    }
    std::cout << "This is in cells\n\n";
    /////////////////////////////////End////////////////////////////////////////////////////
    
    
    
    ////////////////////////////////Creation of new Gscripting file/////////////////////////
    std::ofstream file1;
    file1.open(fileOut, std::ios::app);
    std::string t1 = "\n#include \"Gorgon/Scripting/Embedding.h\"\n#include \"Gorgon/Scripting/Reflection.h\"\n#include \"Gorgon/Scripting.h\"\n\n";
    bool initialized = false;
    for(auto e : elements){
        if(e.name == "compoundname"){
            e.data.resize(e.data.size()-2);
            t1 += "\n\n namespace Gorgon { namespace " + e.data + "{\n\t Scripting::Library Lib" + e.data + "(\"" + e.data + "\",\"Data types under " + e.data + " module and their member functions and operators\");\n\n\n" ;
            initialized = true;
        }
        
        if(initialized == true){
            t1 += "\tvoid init_scripting() { ";
            initialized = false;
        }
        
    }
    
    //t1 += "\n\n namespace Gorgon { namespace Time {\n\t  \n\t}\n} \n ";
    t1 += "\n\n\n\n\t}\n\n}  } \n ";
    //Scripting::Library LibTime(\"Time\",\"Data types under Time.h module and their member functions and operators\");
    file1 << t1;
    file1.close();
    
    ////////////////////////////////End/////////////////////////////////////////////////////
    
    return 0;
}
