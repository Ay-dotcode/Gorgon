#include <iostream>
#include <fstream>
#include <Gorgon/Scripting/Compilers.h>
#include <Gorgon/Console.h>
#include <Gorgon/Scripting.h>

using namespace Gorgon;
using namespace Gorgon::Scripting;
using namespace Gorgon::Scripting::Compilers;


int main() {

	Programming parser;
	Initialize();
	

	//std::ofstream file("test.cpp");

	std::string str;
	
	int ind=0;
	
	while(true) {
		try {
			Gorgon::Console::SetColor(Gorgon::Console::White);
			Gorgon::Console::SetBold(false);
			std::getline(std::cin, str);
			std::cout<<std::endl;
			
			if(str=="!q") {
				parser.Finalize();
				exit(0);
			}

			parser.Compile(str, 1);
			for(; ind<parser.List.size(); ind++)  {
				std::cout<<Disassemble(&parser.List[ind])<<std::endl;
			}

			//testlexer(str, &file);

			Gorgon::Console::SetColor(Gorgon::Console::Black);
			Gorgon::Console::SetBold();
			std::cout << std::endl << std::endl << "---------------------------------" << std::endl;
		}
// 		catch(const ParseError &e) {
// 			Gorgon::Console::SetColor(Gorgon::Console::Red);
// 
// 			std::cout << e.What << std::endl;
// 
// 			//file << "REQUIRE_THROWS( parser.parse(\""<<str<<"\") );"<< std::endl;
// 		}
		catch(const char *e) {
			Gorgon::Console::SetColor(Gorgon::Console::Red);

			std::cout << e << std::endl;

			//file << "REQUIRE_THROWS( parser.parse(\""<<str<<"\") );"<< std::endl;
		}
	}
	

	return 0;
}
