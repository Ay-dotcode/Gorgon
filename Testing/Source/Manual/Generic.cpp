#include <Source/Scripting.h>
#include <Source/Scripting/VirtualMachine.h>
#include <thread>
#include <iostream>

using namespace Gorgon::Scripting;

const std::string source = R"(
	$"a" = i"4"
	#+s"if"
	."1" = -s"=" $"a" i"5"
	-s"if" ."1"
	-s"echo" s"Working..."
	-s"Echo" ."1"
	-s"Echo" c"pi"
	-s"else"
	-s"echo" s"Really working..."
	#+s"end"
	-s"end"
)";

int main() {
	
	Initialize();
	
	std::stringstream ss(source);
	StreamInput streaminput={ss, InputProvider::Intermediate};
	InputSource input={streaminput, ""};


	VirtualMachine vm;

	try {
		vm.Start(input);
	}
	catch(const Exception &ex) {
		std::cout<<"At line "<<ex.GetLine()<<": "<<ex.GetType()<<": "<<ex.GetMessage()<<std::endl;
		if(ex.GetDetails()!="") {
			std::cout<<" > "<<ex.GetDetails()<<std::endl;
		}
	}

	//std::cout<<vm.GetVariable("a")<<std::endl;

	return 0;
}
