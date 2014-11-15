#include "../Embedding.h"
#include "../Data.h"
#include "../Types/Array.h"

namespace Gorgon {
	namespace Scripting {
		
		/// This function should be filled with the break handlers for breakable functions
		std::map<const Function *, std::function<void()>> BreakableFunctions;
		
		/// This function should be filled with the continue handlers for continuable functions
		std::map<const Function *, std::function<void()>> ContinuableFunctions;
		
		// to avoid name clashes
		namespace {
			struct ForScope {
				ForScope() { }

				ForScope(Array *array, SourceMarker marker, 
					const std::string &var) : array(array), arraydata(Types::Array(), array), marker(marker), var(var) {
				}

				ForScope(const Data &data) {
					*this=data.GetValue<ForScope>();
				}

				operator Data() {
					static auto type=MappedValueType<ForScope, String::From<ForScope>, ParseThrow<ForScope>>("ForScope", "");

					return Data(type, *this);
				}

				bool IsValid() {
					return array && array->GetSize()>elementindex;
				}

				SourceMarker marker;
				Array *array=nullptr;
				Data arraydata = Data::Invalid(); //to hold a reference
				int elementindex=0;
				std::string var;
			};

			std::ostream &operator<<(std::ostream &out, const ForScope &) {
				out<<"[[ForScope]]";

				return out;
			}

			Data ForFn(std::string var, Array *array) {
				auto &vm = VirtualMachine::Get();

				ForScope scope(array, vm.GetMarkerForNext(), var);

				if(scope.IsValid()) {
					vm.SetVariable(var, array->GetItemData(scope.elementindex++));
				}
				else {
					vm.StartSkipping();
				}

				return scope;
			}

			bool ForEnd(Data data) {
				auto &vm = VirtualMachine::Get();

				ForScope scope=data;

				if(vm.IsSkipping()) {
					vm.StopSkipping();
					return true;
				}
				else {
					if(scope.IsValid()) {
						vm.SetVariable(scope.var, scope.array->GetItemData(scope.elementindex++));
						vm.GetKeywordScope().SetData(scope);
						vm.Jump(scope.marker);
						
						return false;
					}
					else {
						return true;
					}
				}
			}
			
			void ForBreak() {
				auto &vm = VirtualMachine::Get();
				
				vm.StartSkipping();
			}
			
			void ForContinue() {
				auto &vm = VirtualMachine::Get();
				
				ForEnd(vm.GetKeywordScope().GetData());
			}
			
			void BreakFn() {
				auto &vm = VirtualMachine::Get();
				
				if(!vm.HasKeywordScope()) {
					throw FlowException("Break requires a keyword scope.");
				}
				
				auto &scopefn=vm.GetKeywordScope().GetFunction();
				std::function<void()> fntocall;
				
				for(auto &fn : BreakableFunctions) {
					if(fn.first==&scopefn) {
						fntocall=fn.second;
						break;
					}
				}
				
				if(!fntocall) {
					throw FlowException("Break cannot break out of the current keyword: "+scopefn.GetName());
				}
				
				fntocall();
			}
			
			void ContinueFn() {
				auto &vm = VirtualMachine::Get();
				
				if(!vm.HasKeywordScope()) {
					throw FlowException("Continue requires a keyword scope.");
				}
				
				auto &scopefn=vm.GetKeywordScope().GetFunction();
				std::function<void()> fntocall;
				
				for(auto &fn : ContinuableFunctions) {
					if(fn.first==&scopefn) {
						fntocall=fn.second;
						break;
					}
				}
				
				if(!fntocall) {
					throw FlowException("Continue cannot continue the current keyword: "+scopefn.GetName());
				}
				
				fntocall();
			}
			
		}

		
		Function *For() {
			static Function *fn=nullptr;			
			
			if(!fn) {
				fn=new Scripting::ScopedKeyword{"for",
					"This function allows iteration over a given array.",
					ParameterList{
						new Parameter{"Variable",
							"Variable that will be assigned to the value of the current element.",
							Types::String(), ReferenceTag, OutputTag
						},
						new Parameter{"Array",
							"Array to iterate over.",
							Types::Array()
						}
					}, 
					MappedFunctions(ForFn), ForEnd
				};
				
				BreakableFunctions.insert(std::make_pair(fn, ForBreak));
				ContinuableFunctions.insert(std::make_pair(fn, ForContinue));
			}
			
			return fn;
		}
		
		Function *Break() {
			static Function *fn=nullptr;
			
			if(!fn) {
				fn=new Scripting::MappedFunction{"break",
					"This function breaks from a supported keyword scope.",
					nullptr, nullptr, ParameterList(),
					MappedFunctions(BreakFn), MappedMethods(),
					KeywordTag
				};
			}
			
			return fn;
		}
		
		Function *Continue() {
			static Function *fn=nullptr;
			
			if(!fn) {
				fn=new Scripting::MappedFunction{"continue",
					"This function continues a loop. In \"for\", if exists, the next time will be processed. "
					"In while loop, the condition is checked again and if it is correct, the loop will be iterated "
					"again.",
					nullptr, nullptr, ParameterList(),
					MappedFunctions(ContinueFn), MappedMethods(),
					KeywordTag
				};
			}
			
			return fn;
		}
	}
}