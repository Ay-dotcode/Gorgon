#include "../Embedding.h"
#include "../Data.h"
#include "../Types/Array.h"

namespace Gorgon {
	namespace Scripting {

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

		}

		
		Function *For() {
			static Function *fn=new Scripting::ScopedKeyword{"for",
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
			return fn;
		}
	}
}