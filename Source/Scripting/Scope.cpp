#include "Scope.h"

namespace Gorgon { namespace Scripting {
	
	const Instruction *Scope::ReadInstruction(unsigned long line) {
		if(line<lines.size()) {
			return &lines[line].instruction;
		}
		else {
			while(line>=lines.size()) {
				std::string str;
				
				if(!provider.ReadLine(str, true)) {
					parser->Finalize();
					
					return nullptr;
				}
				pline++;
				
				int compiled=0;
				try {
					compiled=parser->Compile(str, pline);
				}
				catch(ParseError &err) {
					if(err.GetLine()<0)
						err.SetLine(pline-err.GetLine());
					
					throw err;
				}
				
				for(int i=parser->List.size()-compiled;i<parser->List.size();i++) {
					lines.push_back({parser->List[i], pline});
				}
				
				parser->List.erase(parser->List.end()-compiled, parser->List.end());
			}
			
			return &lines[line].instruction;
		}
	}
	
	Scope::Scope(InputProvider &provider, const std::string &name) : provider(provider), name(name) {
		switch(provider.GetDialect()) {
			case InputProvider::Intermediate:
				parser=new Compilers::Intermediate();
				break;
			case InputProvider::Programming:
				parser=new Compilers::Programming();
				break;
			default:
				Utils::ASSERT_FALSE("Unknown dialect");
		}
	}
		
	int ScopeInstance::nextid=0;
		
} }