#include <Source/Scripting.h>
#include <Source/Scripting/VirtualMachine.h>
#include <Source/Scripting/Embedding.h>
#include <thread>
#include <iostream>
#include "../../../Source/Scripting/VirtualMachine.h"
#include <Source/Scripting/Compilers.h>

using namespace Gorgon::Scripting;

const std::string source = "";
/*R"(
	a=4
	b=6
	if a>b
		echo(a)
	else
		echo(b)
	end
)";

R"(# $a = 4;

$"a" = i"4"
#ja"5"
."1" = fms"help" !"type"
fns"echo" s""
fns"echo" s"Type help" 
fns"echo" ."1"
fns"echo" s""

#."1"=fns"nullarr"
#fms"size" ."1"

#fns"return" i"4"

$"i" = i"1"
fkm"while"
."1" = fms"<" $"i" i"5"
fns"while" ."1"
	fns"echo" $"i"

	."1" = fms"+" $"i" i"1"
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

."1"=fms"{}" !"Point"
fms".x" ."1" i"5"
."2"=fms".x" ."1"
fns"echo" s"x value for point is: " ."2"


mms"[]" !"float" f"5" f"7"

# echo "3=pi ? " + (3=pi)
."1" = fms"=" i"3" !"pi"
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
fns"return"
)";*/

namespace Gorgon { namespace Scripting { namespace Compilers {
	extern bool showsvg__;
} } }

void togglesvg() {
	Compilers::showsvg__=!Compilers::showsvg__;
}

void testfill(std::string &s) {
	s+="abcdefg";
}

namespace Gorgon { namespace Geometry {
	extern Scripting::Library LibGeometry;
	void init_scripting();
	
} }

int main() {
	std::stringstream ss(source);
	StreamInput streaminput={std::cin, InputProvider::Programming};
	InputSource input={streaminput, ""};


	VirtualMachine vm;
	Gorgon::Geometry::init_scripting();
	vm.AddLibrary(Gorgon::Geometry::LibGeometry);
	
	Library mylib;
	
	auto mystr = new MappedValueType<std::string>("MyStr", "");

	auto StringToMyStrConvert=[&mystr](Data data) { 
		return Data{mystr, data.GetValue<std::string>()}; 
	};

	auto MyStrToStringConvert=[&mystr](Data data) {
		return Data{Types::String(), data.GetValue<std::string>()}; 
	};
	mystr->AddInheritance(Types::String(), StringToMyStrConvert, MyStrToStringConvert);

	mylib.AddFunctions({
		new MappedFunction("svg", "", nullptr, nullptr, ParameterList{}, 
						   MappedFunctions(togglesvg), MappedMethods(), KeywordTag),
					   
		new MappedFunction("testfill", "", nullptr, nullptr, 
			ParameterList{
				new Parameter("a", "", Types::String(), Gorgon::Scripting::ReferenceTag)
			}, MappedFunctions(testfill), MappedMethods()
		),
		
		new MappedFunction("gen", "", mystr, nullptr, ParameterList{},
						   MappedFunctions([]{ return std::string("abc"); }), MappedMethods()),
	});
	
	vm.AddLibrary(mylib);

	std::cout<<std::endl<<std::endl;
	Gorgon::Console::SetBold();
	Gorgon::Console::SetColor(Gorgon::Console::Yellow);
	std::cout<<"OUTPUT: "<<std::endl;
	Gorgon::Console::Reset();
	
	vm.Begin(input);
	//Compilers::Disassemble(input, std::cout);
	
	std::cout<<std::endl<<std::endl;
	

	while(vm.GetExecutionScopeCount()) {
		try {
			vm.Run();
		}
		catch(const Gorgon::Scripting::ParseError &ex) {
			
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
				if(linenr==ex.GetLine()) {
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
			std::cout<<"At line "<<ex.GetLine()<<":"<<ex.Char;
			Gorgon::Console::SetBold(false);
			Gorgon::Console::SetColor(Gorgon::Console::Red);
			std::cout<<": "<<ex.GetType();
			Gorgon::Console::SetColor(Gorgon::Console::Default);
			std::cout<<": "<<ex.GetMessage()<<std::endl;
			if(ex.GetDetails()!="") {
				std::cout<<" > "<<ex.GetDetails()<<std::endl;
			}
			
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

		}
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
