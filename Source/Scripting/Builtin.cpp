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
			
			void Return1(Data data) {
				VirtualMachine::Get().Return(data);
			}
			
			void Return0() {
				VirtualMachine::Get().Return();
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

		}

		Type *TypeType();
		void InitTypeType();
		
		Type *ArrayType();
		std::vector<Function*> ArrayFunctions();
		
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
					[=](int l, Scripting::Data r) -> Scripting::Data { \
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
				
				intop(*, *, "Multiplies two numbers together", false),
				
				new MappedOperator( "/",
					"Divides this number to the given one. Division operation is performed in double type",
					Double, Int, Double, [](int l, double r) { return double(l)/r; }
				),
				
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
				MAP_COMPARE(!=, !=, Int, int),
				MAP_COMPARE( >, >,  Int, int),
				MAP_COMPARE( <, <,  Int, int),
			});

			Int->AddConstructors({
				Map_Typecast<float, int>("Integrals", Float, Int),
				Map_Typecast<double, int>("Integrals", Double, Int),
				Map_Typecast<unsigned, int>("Integrals", Unsigned, Int),
				Map_Typecast<Gorgon::Byte, int>("Integrals", Byte, Int)
			});
			
			Unsigned->AddConstructors({
				Map_Typecast<float, unsigned>("Integrals", Float, Unsigned),
				Map_Typecast<double, unsigned>("Integrals", Double, Unsigned),
				Map_Typecast<int, unsigned>("Integrals", Int, Unsigned),
				Map_Typecast<Gorgon::Byte, unsigned>("Integrals", Byte, Unsigned),
			});
			
			Float->AddConstructors({
				Map_Typecast<unsigned, float>("Integrals", Unsigned, Float),
				Map_Typecast<double, float>("Integrals", Double, Float),
				Map_Typecast<int, float>("Integrals", Int, Float),
				Map_Typecast<Gorgon::Byte, float>("Integrals", Byte, Float)
			});
			
			Double->AddConstructors({
				Map_Typecast<float, double>("Integrals", Float, Double),
				Map_Typecast<unsigned, double>("Integrals", Unsigned, Double),
				Map_Typecast<int, double>("Integrals", Int, Double),
				Map_Typecast<Gorgon::Byte, double>("Integrals", Byte, Double)
			});
			
			Byte->AddConstructors({
				Map_Typecast<float, Gorgon::Byte>("Integrals", Float, Byte),
				Map_Typecast<double, Gorgon::Byte>("Integrals", Double, Byte),
				Map_Typecast<int, Gorgon::Byte>("Integrals", Int, Byte),
				Map_Typecast<unsigned, Gorgon::Byte>("Integrals", Unsigned, Byte),
				Map_Typecast<char, Gorgon::Byte>("Integrals", Char, Byte)
			});
			
			Char->AddConstructors({
				Map_Typecast<Gorgon::Byte, char>("Integrals", Byte, Char)
			});
			
			Bool->AddFunctions({
				new MappedOperator("and", 
					"Conjunction of two boolean values",
					Bool, Bool, Bool, [](bool l, bool r) { return l && r; }
				),
				
				new MappedOperator("or", 
				   "Disjunction of two boolean values",
				   Bool, Bool, Bool, [](bool l, bool r) { return l || r; }
				),
				
				new MappedOperator("xor", 
				   "Exclusive or of two boolean values",
				   Bool, Bool, Bool, [](bool l, bool r) { return l != r; }
				),
				
				new MappedFunction("not",
					"Inverts boolean value",
					Bool, Bool, ParameterList(), 
					MappedFunctions([](bool val) { return !val; }),
					MappedMethods()
				)
			});
			
			Bool->AddConstructors({
				Map_Typecast<Gorgon::Byte, bool>("Integrals", Byte, Bool),
				Map_Typecast<unsigned, bool>("Integrals", Unsigned, Bool),
				Map_Typecast<int, bool>("Integrals", Int, Bool),
			});
			
			String->AddFunctions({
				new MappedFunction("Length",
					"Returns the length of the string", Unsigned, String, {}, 
					MappedFunctions(
						[](std::string str) -> unsigned { return (unsigned)str.length(); }
					), MappedMethods(),
					ConstTag
				),
				
				new MappedFunction("Substr",
					"Returns a part of the string", String, String, {
						new Parameter("Start", "Start of the substring, first element is at 0", Unsigned),
						new Parameter("Length", "Length of the substring, if not specified, "
							"all of the string after the start is taken", Unsigned, OptionalTag),
					}, 
					MappedFunctions(
						[](std::string str, unsigned start, unsigned len) -> std::string { 
							return str.substr(start, len); 
						},
 						[](std::string str, unsigned start) -> std::string { 
							return str.substr(start); 
						}
					), MappedMethods()
				),
				
				new MappedFunction("Find",
					"Returns the position of a given string", Unsigned, String, {
						new Parameter("Search", "The string to be searched", String),
						new Parameter("Start", "Starting point of the search, "
							"if not specified search will start from the beginning", Unsigned, OptionalTag),
					}, 
					MappedFunctions(
						[](std::string str, std::string src, unsigned pos) -> unsigned { return str.find(src, pos); },
						[](std::string str, std::string src) -> unsigned { return str.find(src); }
					), MappedMethods()
				),
				
				new MappedFunction("ToLower",
					"Returns lowercase string", String, String, {}, 
					MappedFunctions(
						[](std::string str) { return String::ToLower(str); }
					), MappedMethods()
				),
				
				new MappedFunction("ToUpper",
					"Returns lowercase string", String, String, {}, 
					MappedFunctions(
						[](std::string str) { return String::ToUpper(str); }
					), MappedMethods()
				),
				
				new MappedFunction("Trim",
					"Trims start and the end of the string", String, String, {}, 
					MappedFunctions(
						[](std::string str) { return String::Trim(str); }
					), MappedMethods()
				),
				
				new MappedFunction("Extract",
					"Extracts the part of the string up to the given marker", String, String, {
						new Parameter("Marker", "String that will be searched.", String)						
					}, 
					MappedFunctions(
						[](std::string &str, std::string marker) { return String::Extract(str, marker); }
					), MappedMethods()
				),
				
				new MappedFunction("Replace",
					"Replaces all instances of the given substring in this string with another string", String, String, {
						new Parameter("Search", "Search string to be replaced", String),
						new Parameter("Replace", "String to replace, if not specified, "
							"empty string is assumed", String, OptionalTag),
					}, 
					MappedFunctions(
						[](std::string str, std::string search, std::string replace) { 
							return String::Replace(str, search, replace); 
						},
						[](std::string str, std::string search) -> std::string { 
							return String::Replace(str, search, ""); 
						}
					), MappedMethods()
				),
				
				MAP_COMPARE( =,==, String, std::string),
				MAP_COMPARE(!=,!=, String, std::string),
				MAP_COMPARE( >, >, String, std::string),
				MAP_COMPARE( <, <, String, std::string),
				MAP_COMPARE(>=,>=, String, std::string),
				MAP_COMPARE(<=,<=, String, std::string),
				
				new MappedOperator("+",
					"String concatenation", String, String, String, 
					[](std::string left, std::string right) { return left+right; }
				),
				
				new MappedOperator("*", 
					"String duplication", String, String, Int,
					[](std::string l, int r) -> std::string { 
						std::string out; 
						for(int i=0;i<r;i++) out+=l;
						return out;
					}
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
			
			Integrals.AddTypes({ArrayType()});
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
					new MappedFunction("return",
						"Returns from the current function or terminates the execution if not in a "
						"function. If a value is supplied, it is assumed to be the return value of "
						"the function or execution",
						nullptr, nullptr,
						ParameterList {
							new Parameter(
								"Value",
								"This value is used as the return value of the function or execution",
								Variant, OptionalTag
							)
						},
						MappedFunctions(Return1, Return0), MappedMethods(),
						KeywordTag
					),
					new MappedFunction("const",
						"Makes the given variable a constant",
						nullptr, nullptr,
						ParameterList {
							new Parameter(
								"Variable",
								"This is the variable to become constant",
								String, VariableTag
							)
						},
						MappedFunctions([](std::string varname) {
							VirtualMachine::Get().GetVariable(varname).MakeConstant();
						}), MappedMethods(),
						KeywordTag
					)
				},
			};
			
			Reflection={"Reflection", "This library contains reflection objects",
				TypeList { TypeType() },
				FunctionList {
					new MappedFunction("TypeOf",
						"This function returns the type of the given variable.",
						TypeType(), nullptr,
						ParameterList {
							new Parameter(
								"Variable",
								"The variable to determine its type.",
								String, VariableTag
							)
						},
						MappedFunctions([](std::string variable) {
							return &VirtualMachine::Get().GetVariable(variable).GetType();
						}), MappedMethods()
					)
				}
			};
			
			InitTypeType();
		}
		
	}
}
