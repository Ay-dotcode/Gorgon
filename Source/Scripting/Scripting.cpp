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

				try {
					parser->Compile(str);
				}
				catch(ParseError &err) {
					if(err.GetLine()<0)
						err.SetLine(pline-err.GetLine());
					
					throw err;
				}

				for(auto &inst : parser->List)
					lines.push_back({inst, pline});

				parser->List.clear();
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
