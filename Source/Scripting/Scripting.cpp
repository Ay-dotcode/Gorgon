#include <map>

#include "../Scripting.h"
#include "Reflection.h"
#include "VirtualMachine.h"
#include "Embedding.h"

namespace Gorgon { namespace Scripting {
	
	const Instruction *InputSource::ReadInstruction(unsigned long line) {
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
					compiled=parser->Compile(str);
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

	InputSource::InputSource(InputProvider &provider, const std::string &name) : provider(provider), name(name) {
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

	
	MappedValueType<Data, String::From<Data>, GetVariableValue> Variant = {"Variant", 
		"This type can contain any type.",
		Data::Invalid()
	};
	
	int VariableScope::nextid=0;
	std::set<std::string, String::CaseInsensitiveLess> KeywordNames;
} }
