#include <Source/Scripting.h>
#include <Source/Scripting/Parser.h>
#include <thread>
#include <iostream>

using namespace Gorgon::Scripting;

int main() {
	
	Initialize();
	
	IntermediateParser parser;
	try {
		parser.Parse(R"($"a"=i"4")");
		parser.Parse(R"(."1"=-s"fn" i"4")");
		parser.Parse(R"(."2"=-s"fn" ."1")");
		parser.Parse(R"(-s"fn" ."1")");
		parser.Parse(R"($"b"=."1")");
	}
	catch(const ParseError &err) {
		std::cout<<err.Code<<": "<<err.What<<" at "<<err.Char<<std::endl;
	}
	
	Instruction inst = parser.List.back();

	std::cout << (int)inst.Type << ": " << inst.Name.Literal << ": " << inst.RHS.Result << std::endl;

	inst = parser.List[3];

	std::cout << (int)inst.Type << ": " << inst.Name.Literal << ": " << inst.Parameters[0].Result << std::endl;

	inst = parser.List[1];

	std::cout << (int)inst.Type << ": " << inst.Name.Literal << ": " << inst.Store << ": " << inst.Parameters[0].Literal << std::endl;

	return 0;
}
