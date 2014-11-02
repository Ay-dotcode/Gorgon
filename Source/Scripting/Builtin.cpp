#include "Embedding.h"
#include "Exceptions.h"
#include "Types/Array.h"

namespace Gorgon {
	namespace Scripting {
		
		Library Integrals;
		Library Keywords;
		Library Reflection;
		
		namespace {
			void Echo(std::vector<std::string> datav) {
				auto &out=VirtualMachine::Get().GetOutput();
				for(auto &data : datav) {
					out<<data;
				}
				out<<std::endl;
			}

			std::string BoolToString(const bool &val) {
				return val ? "true" : "false";
			}

			bool StringToBool(const std::string &str) {
				if(str=="true") {
					return true;
				}
				if(str=="false") {
					return true;
				}
				return String::To<bool>(str);
			}

			void End(std::string validate="") {
				auto &vm=VirtualMachine::Get();
				if(!vm.HasKeywordScope()) {
					throw FlowException("end without any keyword");
				}

				if(validate!="") {
					if(String::ToLower(vm.GetKeywordScope().GetFunction().GetName())!=String::ToLower(validate)) {
						throw FlowException("end does not match with the correct keyword", 
							"Current scope is "+vm.GetKeywordScope().GetFunction().GetName()+
							" while given keyword for end is "+validate);
					}
				}

				if(vm.SkippingDepth()) {
					vm.ReduceSkipping();
				}
				else {
					if(vm.GetKeywordScope().CallEnd()) {
						vm.PopKeywordScope();
					}
				}
			}

		}

		Function *If();
		Function *ElseIf();
		Function *Else();
		
		Type *TypeType();
		void InitTypeType();
		
		Type *ArrayType();
		std::initializer_list<Function*> ArrayFunctions();
		
		void init_builtin() {
			if(Integrals.Types.GetCount()) return;
			
			auto Int = new MappedValueType<int>( "Int", 
				"Integer data type. This type is binary saved/loaded as int32_t regardless of the platform. "
				"This data type does not support binary operator (bit shift, bitwise and/or). You should "
				"use *unsigned* for this purpose."
			);
			
			auto Float = new MappedValueType<float>( "Float",
				"Floating point data type. Gorgon library only supports system that has 32bit floats."
			);
			
			auto Double = new MappedValueType<double>( "Double",
				"Double precision floating point data type. Gorgon library only supports system that has 64bit floats."
			);
			
			auto Unsigned = new MappedValueType<unsigned>( "Unsigned",
				"Unsigned integer. Supports bit operations."
			);
			
			auto Byte = new MappedValueType<Gorgon::Byte>( "Byte",
				"Represents a single byte in a memory type. This is a binary data type. *Char* should be "
				"used to represent a character in a string."
			);
			
			auto Char = new MappedValueType<char>( "Char",
				"Represents single character. This variable type should not be used for binary operations."
			);
			
			auto String = new MappedValueType<std::string>( "String", 
				"Represents a string."
			);
			
			auto Bool = new MappedValueType<bool, &BoolToString, &StringToBool> ( "Bool",
				"Represents a truth statement. Can either be true or false."
			);
			
#define intop(op, name, help, ch) \
				new MappedOperator( #name, \
					#help, Variant, Int, Variant, \
					[&](int l, Scripting::Data r) -> Scripting::Data { \
						if(r.GetType()==Int) \
							return {Int, l op r.GetValue<int>()}; \
						else if(ch && r.GetType()==Char) \
							return {Int, l op r.GetValue<char>()}; \
						else if(r.GetType()==Byte) \
							return {Int, l op r.GetValue<Gorgon::Byte>()}; \
						else if(r.GetType()==Unsigned) \
							return {Int, l op r.GetValue<unsigned>()}; \
						else if(r.GetType()==Float) \
							return Float->Functions["+"].Call(false, {{Float, float(l)}}); \
						else if(r.GetType()==Double) \
							return Double->Functions["+"].Call(false, {{Double, double(l)}}); \
						else \
							throw SymbolNotFoundException( \
								#name, SymbolType::Function, \
								"Cannot find "#name" operator between int and "+r.GetType().GetName()); \
					} \
				) \
			
			Int->AddFunctions({
				intop(+, +, "Adds two numbers together", true),
				
				intop(-, -, "Subtracts the given number", true),
				
				new MappedOperator( "/",
					"Divides this number to the given one. Division operation is performed in double type",
					Double, Int, Double, [](int l, double r) { return double(l)/r; }
				),
				
				intop(*, *, "Multiplies two numbers together", false),
				
				new MappedOperator( "^",
					"Raises this number to the given power. All power operations are performed in double type",
					Double, Int, Double, [](int l, double r) { return pow(double(l), r); }
				),
				
				new MappedOperator( "mod",
					"Returns the remainder of the division of the left operand to the right operand.",
					Int, Int, Int, [](int l, int r) { 
						return l>=0 ? l%r : (l%r)+r;
					}
				),
				
				MAP_COMPARE( =, ==, Int, int),
				MAP_COMPARE(>=, >=, Int, int),
				MAP_COMPARE(<=, <=, Int, int),
				MAP_COMPARE(<>, !=, Int, int),
				MAP_COMPARE( >, >,  Int, int),
				MAP_COMPARE( <, <,  Int, int),
			});

			Int->AddConstructors({
				Map_Typecast<float, int>("Integrals", Float, Int),
				Map_Typecast<double, int>("Integrals", Double, Int),
				Map_Typecast<unsigned, int>("Integrals", Unsigned, Int),
				Map_Typecast<Gorgon::Byte, int>("Integrals", Byte, Int)
			});
			
			String->AddFunctions({
				new MappedFunction("Length",
					"Returns the length of the string", Unsigned, String, {}, 
					MappedFunctions(
						[](std::string str) -> unsigned { return (unsigned)str.length(); }
					), MappedMethods()
				),
				
				MAP_COMPARE( =, ==, String, std::string),

				new MappedOperator("+",
					"String concatenation", String, String, String, 
					[](std::string left, std::string right) { return left+right; }
				)
			});
			
			Integrals={"Integral", "Integral types and functions", 
				TypeList {
					Bool,
					Int,
					Float,
					Double,
					Unsigned,
					Byte,
					Char,
					String,
					&Variant
				},
				FunctionList{},
				ConstantList {
					new Constant("Pi", "Contains the value of PI", {Double, 3.14159265358979}),
				}
			};
			
			Integrals.AddFunctions(ArrayFunctions());
			Integrals.AddFunctions({
				new MappedFunction("Echo",
					"This function prints the given parameters to the screen.",
					nullptr, nullptr, ParameterList {
						new Parameter( "string",
							"The strings that will be printed.",
							String
						)
					},
					MappedFunctions(Echo), MappedMethods(),
					StretchTag, RepeatTag
				)
			});
			
			Keywords={"Keywords", "Standard keywords like if and for.",
				TypeList {},
				FunctionList {
					If(), ElseIf(), Else(), 
					new MappedFunction("end", 
						"Ends the current scope",
						nullptr, nullptr,
						ParameterList {
							new Parameter(
								"Keyword",
								"This function accepts a keyword name for scope validation.",
								String, OptionalTag
							)
						},
						MappedFunctions(&End, []() { End(); }), MappedMethods(), 
						KeywordTag, NeverSkipTag
					)
				},
			};
			
			Reflection={"Reflection", "This library contains reflection objects",
				TypeList { TypeType() },
				FunctionList { }
			};
			
			InitTypeType();
		}
		
	}
}
