#include <Source/Scripting.h>
#include <Source/Scripting/VirtualMachine.h>
#include <thread>
#include <iostream>

using namespace Gorgon::Scripting;

const std::string source = R"(# $a = 4;
$"a" = i"4"

# echo "3=pi ? " + (3=pi)
."1" = -s"=" i"3" c"pi"
."2" = *s"+" s"3=pi ? " ."1"
-s"echo" ."2"

# if $a = 4
+ "if"
."1" = *s"=" $"a" i"4"
-s"if" ."1"
	# echo("working")
	/s"echo" s"Working..."

	# Echo $a=5
	# normally cannot be generated
	-s"Echo" ."1"

	%s"=" $"a" i"5"

	# Echo pi
	-s"Echo" c"pi"

# else
+ "else"
-s"else"
	
	# echo("Really working")
	-s"echo" s"Really working..."

# end
+ "end"
-s"end"

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
	catch(const ParseError &ex) {

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
	catch(const std::exception &ex) {
		Gorgon::Console::SetColor(Gorgon::Console::Red);
		Gorgon::Console::SetBold();
		std::cout<<ex.what()<<std::endl;
		Gorgon::Console::Reset();
		std::cout<<std::endl<<std::endl;
		return 3;
	}

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
	Gorgon::Console::Reset();

	//std::cout<<vm.GetVariable("a")<<std::endl;

	return 0;
}
