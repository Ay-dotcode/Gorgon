#include "Scope.h"

namespace Gorgon { namespace Scripting {
	
	const Instruction *Scope::ReadInstruction(unsigned long line) {
		if(line<lines.size()) {
			return &lines[line].instruction;
		}
		else {
			if(!provider) return nullptr;
			
			while(line>=lines.size()) {
				std::string str;
				
				if(!provider->ReadLine(str, true)) {
					parser->Finalize();
					
					return nullptr;
				}
				pline++;
				
				int compiled=0;
				try {
					compiled=parser->Compile(str, pline);
				}
				catch(Exception &err) {
					if(!err.IsLineSet())
						err.SetLine(pline+err.GetLine());
					if(err.GetSourcename()=="") {
						err.SetSourcename(GetName());
					}
					throw;
				}
				
				for(unsigned i=parser->List.size()-compiled;i<parser->List.size();i++) {
					lines.push_back({parser->List[i], pline});
				}
				
				parser->List.erase(parser->List.end()-compiled, parser->List.end());
			}
			
			return &lines[line].instruction;
		}
	}
	
	Scope::Scope(InputProvider &provider, const std::string &name) : provider(&provider), name(name) {
		switch(provider.GetDialect()) {
			case InputProvider::Intermediate:
				parser=new Compilers::Intermediate(this);
				break;
			case InputProvider::Programming:
				parser=new Compilers::Programming(this);
				break;
			default:
				Utils::ASSERT_FALSE("Unknown dialect");
		}
	}
	
	Scope::Scope(Scope& parent, const std::string& name) : parent(&parent), name(name) {
		
	}

	
	std::shared_ptr<ScopeInstance> Scope::Instantiate() {
		auto inst=new ScopeInstance(*this, nullptr);
		
		inst->parent=nullptr;
		instances.Add(inst);
		
		return std::shared_ptr<ScopeInstance>(inst);
	}
	
	std::shared_ptr<ScopeInstance> Scope::Instantiate(ScopeInstance &parent) {
		auto inst=Instantiate();
		inst->parent=&parent;
		
		return inst;
	}
} }
