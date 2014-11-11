#include "../Embedding.h"
#include "../Data.h"
#include "../Types/Array.h"

namespace Gorgon {
	namespace Scripting {

		// to avoid name clashes
		namespace {
			struct WhileScope {
				WhileScope() { }

				WhileScope(SourceMarker marker) : marker(marker) {
				}

				WhileScope(const Data &data) {
					*this=data.GetValue<WhileScope>();
				}
				
				static Type &selftype() {
					static auto type=MappedValueType<WhileScope, String::From<WhileScope>, ParseThrow<WhileScope>>("WhileScope", "");
					
					return type;
				}

				operator Data() {

					return Data(selftype(), *this);
				}

				SourceMarker marker;
				bool loop = false;
			};
			

			std::ostream &operator<<(std::ostream &out, const WhileScope &) {
				out<<"[[WhileScope]]";

				return out;
			}
			
			Data WhileFn(bool cond) {
				auto &vm = VirtualMachine::Get();
				WhileScope scope;
				bool reusing=false;
				
				if(vm.HasKeywordScope() && vm.GetKeywordScope().GetData().GetType()==WhileScope::selftype()) {
					scope=vm.GetKeywordScope().GetData();
					if(!scope.loop) {
						scope={};
					}
					else {
						reusing=true;
					}
				}
				
				if(!scope.marker.IsValid()) {
					scope={vm.GetMarkerForKeywordStart()};
				}
				
				if(!cond) {
					vm.StartSkipping();
				}
				
				if(reusing) {
					scope.loop=false;
					vm.GetKeywordScope().SetData(scope);
					
					return Data::Invalid();
				}
				else {
					return scope;
				}
			}

			bool WhileEnd(Data data) {
				auto &vm = VirtualMachine::Get();

				WhileScope scope=data;

				if(vm.IsSkipping()) {
					vm.StopSkipping();
					
					return true;
				}
				else {
					scope.loop=true;
					vm.GetKeywordScope().SetData(scope);
					vm.Jump(scope.marker);
					
					return false;
				}
			}
		}
		
		Function *While() {
			static Function *fn=new Scripting::ScopedKeyword{"while",
				"This function allows condition controlled loops.",
				ParameterList{
					new Parameter{"Condition",
						"Condition that controls whether the loop would occur.",
						Types::Bool()
					}
				}, 
				MappedFunctions(WhileFn), WhileEnd
			};
			return fn;
		}
	}
}
