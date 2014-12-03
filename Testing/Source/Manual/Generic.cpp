#include <Source/Scripting.h>
#include <Source/Scripting/VirtualMachine.h>
#include <thread>
#include <iostream>
#include "../../../Source/Scripting/VirtualMachine.h"

using namespace Gorgon::Scripting;

const std::string source = R"(# $a = 4;
$"a" = i"4"

."1" = fns"help" !"type"
fns"echo" s""
fns"echo" s"Type help" 
fns"echo" ."1"
fns"echo" s""

#."1"=fns"nullarr"
#fms"size" ."1"

#fns"return" i"4"

$"i" = i"1"
fkm"while"
."1" = fns"<" $"i" i"5"
fns"while" ."1"
	fns"echo" $"i"

	."1" = fns"+" $"i" i"1"
	$"i" = ."1"
	
	#fns"break"
fkm"end"
fns"end"

."1" = fns"range" i"0" i"5"
fms"push" ."1" i"9"
fms"resize" ."1" i"10"
fns"echo" ."1"

fkm"for"
fns"for" $"i" ."1"
	fns"echo" $"i"
fkm"end"
fns"end"

mms"[]" !"float" f"5" f"7"

# echo "3=pi ? " + (3=pi)
."1" = fns"=" i"3" !"pi"
."2" = fms"+" s"3=pi ? " ."1"
fns"echo" s"The result is " ."2"
x"1"
x"2"

# if $a = 4
fkm"if"
."1" = fms"=" $"a" i"4"
fns"if" ."1"
	# echo("working")
	mns"echo" s"Working..."

	# Echo $a=5
	# normally cannot be generated
	fns"Echo" ."1"

	mms"=" $"a" i"5"

	# Echo pi
	fns"Echo" !"pi"

# else
fkm"else"
fns"else"
	
	# echo("Really working")
	fns"echo" s"Really working..."

# end
fkm"end"
fns"end"

)";

int main() {
	Initialize();

	
	std::stringstream ss(source);
	StreamInput streaminput={ss, InputProvider::Intermediate};
	InputSource input={streaminput, ""};


	VirtualMachine vm;

	std::cout<<std::endl<<std::endl;
	Gorgon::Console::SetBold();
	Gorgon::Console::SetColor(Gorgon::Console::Yellow);
	std::cout<<"OUTPUT: "<<std::endl;
	Gorgon::Console::Reset();

	try {
		vm.Start(input);
	}
	catch(const Exception &ex) {

		std::cout<<std::endl<<std::endl;
		Gorgon::Console::SetBold();
		Gorgon::Console::SetColor(Gorgon::Console::Yellow);
		std::cout<<"SOURCE CODE: "<<std::endl;
		Gorgon::Console::Reset();

		std::stringstream ss2(source);
		std::string line;
		int linenr=0;
		while(std::getline(ss2, line)) {
			linenr++;
			if(linenr==ex.GetLine())
				Gorgon::Console::SetColor(Gorgon::Console::Red);

			Gorgon::Console::SetBold();
			std::cout<<std::setw(3)<<linenr<<" ";
			Gorgon::Console::SetBold(false);
			std::cout<<line<<std::endl;
			Gorgon::Console::Reset();
		}
		std::cout<<std::endl<<std::endl;


		std::cout<<std::endl<<std::endl;
		Gorgon::Console::SetBold();
		Gorgon::Console::SetColor(Gorgon::Console::Yellow);
		std::cout<<"ERROR: "<<std::endl;
		Gorgon::Console::Reset();

		Gorgon::Console::SetBold();
		std::cout<<"At line "<<ex.GetLine();
		Gorgon::Console::SetBold(false);
		Gorgon::Console::SetColor(Gorgon::Console::Red);
		std::cout<<": "<<ex.GetType();
		Gorgon::Console::SetColor(Gorgon::Console::Default);
		std::cout<<": "<<ex.GetMessage()<<std::endl;
		if(ex.GetDetails()!="") {
			std::cout<<" > "<<ex.GetDetails()<<std::endl;
		}

		return 1;
	}
	catch(const Gorgon::Scripting::ParseError &ex) {

		std::stringstream ss2(source);
		std::string line;
		int linenr=0;
		while(std::getline(ss2, line)) {
			linenr++;
			if(linenr==ex.Line)
				Gorgon::Console::SetColor(Gorgon::Console::Red);

			Gorgon::Console::SetBold();
			std::cout<<std::setw(3)<<linenr<<" ";
			Gorgon::Console::SetBold(false);
			std::cout<<line<<std::endl;
			Gorgon::Console::Reset();
			if(linenr==ex.Line) {
				for(int i=0; i<ex.Char+4; i++) {
					std::cout<<" ";
				}
				std::cout<<"^"<<std::endl;
			}
		}
		std::cout<<std::endl<<std::endl;


		std::cout<<std::endl<<std::endl;
		Gorgon::Console::SetBold();
		Gorgon::Console::SetColor(Gorgon::Console::Yellow);
		std::cout<<"ERROR: "<<std::endl;
		Gorgon::Console::Reset();

		Gorgon::Console::SetBold();
		std::cout<<"At line "<<ex.Line<<":"<<ex.Char;
		Gorgon::Console::SetBold(false);
		Gorgon::Console::SetColor(Gorgon::Console::Red);
		std::cout<<": "<<ex.Code;
		Gorgon::Console::SetColor(Gorgon::Console::Default);
		std::cout<<": "<<ex.What<<std::endl;

		return 2;
	}
	//catch(const std::exception &ex) {
	//	Gorgon::Console::SetColor(Gorgon::Console::Red);
	//	Gorgon::Console::SetBold();
	//	std::cout<<ex.what()<<std::endl;
	//	Gorgon::Console::Reset();
	//	std::cout<<std::endl<<std::endl;
	//	return 3;
	//}

	std::cout<<std::endl<<std::endl;
	Gorgon::Console::SetBold();
	Gorgon::Console::SetColor(Gorgon::Console::Yellow);
	std::cout<<"SOURCE CODE: "<<std::endl;
	Gorgon::Console::Reset();

	std::stringstream ss2(source);
	std::string line;
	int linenr=0;
	while(std::getline(ss2, line)) {
		linenr++;
		Gorgon::Console::SetBold();
		std::cout<<std::setw(3)<<linenr<<" ";
		Gorgon::Console::SetBold(false);
		std::cout<<line<<std::endl;
		Gorgon::Console::Reset();
	}
	std::cout<<std::endl<<std::endl;

	std::cout<<std::endl<<std::endl;
	Gorgon::Console::SetColor(Gorgon::Console::Green);
	std::cout<<"Execution successful."<<std::endl<<std::endl;
	
	Data ret=vm.GetReturnValue();
	if(ret.IsValid()) {
		std::cout<<"Return value: "<<ret<<std::endl;
	}
	Gorgon::Console::Reset();

	//std::cout<<vm.GetVariable("a")<<std::endl;

	return 0;
}
