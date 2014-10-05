#include <Source/Scripting.h>
#include <Source/Scripting/VirtualMachine.h>
#include <thread>
#include <iostream>

using namespace Gorgon::Scripting;

const std::string source = R"(
	$"a" = i"4"
	#+s"if"
	."1" = -s"=" $"a" i"4"
	#-s"if" ."1"
	-s"Echo" s"Working..."
	-s"Echo" ."1"
	#+s"end"
	#-s"end"
)";

int main() {
	
	Initialize();
	
	std::stringstream ss(source);
	StreamInput streaminput={ss, InputProvider::Intermediate};
	InputSource input={streaminput, ""};


	VirtualMachine vm;

	vm.Start(input);

	//std::cout<<vm.GetVariable("a")<<std::endl;

	return 0;
}
