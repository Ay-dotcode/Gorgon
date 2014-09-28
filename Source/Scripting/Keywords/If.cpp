#include "../Embedding.h"

namespace Gorgon { namespace Scripting {
	Function *If();
	Function *Else();
	// to avoid name clashes
	namespace {
		
		struct IfScope {
			IfScope() { }
			
			IfScope(const Data &data) {
				*this=data.GetValue<IfScope>();
			}
			
			bool passedcorrect = false;
			bool skipping = false;
			
			operator Data() {
				static auto type=MappedValueType<IfScope, String::From<IfScope>, ParseThrow<IfScope>>("IfScope", "");
				
				return Data(type, *this);
			}
		};
		
		std::ostream &operator<<(std::ostream &out, const IfScope &) {
			out<<"[[IfScope]]";
		}
		
		Data IfFn(bool expression) {
			auto &vm = VirtualMachine::Get();
			
			IfScope scope;
			
			if(expression) {
				vm.StartSkipping();
				scope.skipping=true;
			}
			else {
				scope.passedcorrect=true;
			}
			
			return scope;
		}
		
		bool IfEnd(Data data) {
			auto &vm = VirtualMachine::Get();
			
			IfScope scope=data;
			
			if(scope.skipping) {
				vm.StopSkipping();
			}
			
			return true;
		}
		
		void ElseFn() {
			auto &vm = VirtualMachine::Get();
			auto &current = vm.GetKeywordScope();
			
			if(current.GetFunction()!=If()) {
				throw UnexpectedKeywordException("else", "else keyword requires an if scope. "
					"There might be a missing or extra 'end'");
			}
			
			IfScope scope=current.GetData();
			
			if(scope.passedcorrect) {
				vm.StartSkipping();
			}
			else {
				vm.StopSkipping();
			}
		}
	}
	
	Function *If() {
		static Function *fn=new ScopedKeyword{"if",
			"If the given condition is false, this method skips until the matching end statement.",
			ParameterList{
				new Parameter("Condition", 
					"Condition for the if statement. If true the contents will be executed.",
					Integrals.Types["Bool"]
				)
			}, 
			MappedFunctions(IfFn), IfEnd
		};
		return fn;
	}
	
	Function *Else() {
		static Function *fn=new MappedFunction{"else",
			"This method skips until the end statement if the previously given condition "
			"is true. Should be paired with an if statement to work.",
			nullptr, nullptr,
			ParameterList{ },
			MappedFunctions(ElseFn), MappedMethods(), KeywordTag, NeverSkipTag
		};
		
		return fn;
	}
	
	
} }
