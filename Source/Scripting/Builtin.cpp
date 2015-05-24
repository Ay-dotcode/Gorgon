#include "Embedding.h"
#include "Exceptions.h"
#include "Types/Array.h"
#include <math.h>

#ifdef _MSC_VER
	double exp10(double num) { return exp(num)/exp(2); }
#endif

namespace Gorgon {
	namespace Scripting {
		
		Library Integrals;
		Library Keywords;
		Library Reflection;
		
		namespace {
			void Print(std::vector<std::string> datav) {
				auto &out=VirtualMachine::Get().GetOutput();
				for(auto &data : datav) {
					out<<data;
				}
			}
			
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

			Data StringToData(const std::string &str) {
				return {Types::String(), str};
			}
			
			std::string CharToString(const char &c) {
				return std::string(1, c);
			}
			
			void static2(std::string name, Data value) {
				auto &vm=VirtualMachine::Get();
				
				auto &scope=vm.CurrentScopeInstance().GetScope();
				auto var=scope.GetVariable(name);
				
				// if static variable is being redefined, it will not be reassigned
				if(var) 
					return;
				
				//if this variable is already defined, throw error
				if(vm.CurrentScopeInstance().GetLocalVariable(name)) {
					throw AmbiguousSymbolException(name, SymbolType::Variable, "Variable is already declared non-static");
				}
				
				scope.SetVariable(name, value);
			}
			
			void static1(std::string name) {
				static2(name, Data::Invalid());
			}

		}

		Type *TypeType();
		void InitTypeType();

		Type *FunctionType();
		Type *ParameterType();
		
		Type *ArrayType();
		std::vector<Function*> ArrayFunctions();
		
		void init_builtin() {
			if(Integrals.Types.GetCount()) return;

			auto Variant = new MappedValueType<Data, String::From<Data>, StringToData>("Variant",
				"This type can contain any type.",
				Data::Invalid()
			);

			auto Int = new MappedValueType<int>( "Int", 
				"Integer data type. This type is binary saved/loaded as int32_t regardless of the platform. "
				"This data type does not support binary operator (bit shift, bitwise and/or). You should "
				"use *unsigned* for this purpose."
			);
			
			auto Float = new MappedValueType<float>( "Float",
				"Floating point data type. Gorgon library only supports systems that have 32bit floats."
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
			
			auto Char = new MappedValueType<char, CharToString>( "Char",
				"Represents single character. This variable type should not be used for binary operations."
			);
			
			auto String = new MappedValueType<std::string>( "String", 
				"Represents a string."
			);
			
				auto Bool = new MappedValueType<bool, &BoolToString, &StringToBool> ( "Bool",
					"Represents a truth statement. Can either be true or false."
			);
			
			
			Int->AddFunctions({

				new MappedOperator("+", "Adds two numbers together",
					Int, {
						MapOperator(
							[](int l, int r) { return l+r; }, 
							Int, Int
						),
						MapOperator(
							[](int l, double r) { return l+r; },
							Double, Double
						),
						MapOperator(
							[](int l, float r) { return l+r; },
							Float, Float
						),
					}
				),
				
				new MappedOperator("-", "Subtracts a number from this one",
					Int, {
						MapOperator(
							[](int l, int r) { return l-r; }, 
							Int, Int
						),
						MapOperator(
							[](int l, double r) { return l-r; },
							Double, Double
						),
						MapOperator(
							[](int l, float r) { return l-r; },
							Float, Float
						),
					}
				),
				
				new MappedOperator("*", "Multiplies two numbers together",
					Int, {
						MapOperator(
							[](int l, int r) { return l*r; }, 
							Int, Int
						),
						MapOperator(
							[](int l, double r) { return l*r; },
							Double, Double
						),
						MapOperator(
							[](int l, float r) { return l*r; },
							Float, Float
						),
					}
				),

				new MappedOperator( "/",
					"Divides this number to the given one. Division operation is performed in double type",
					Int, Double, Double, [](int l, double r) { return double(l)/r; }
				),
				
				new MappedOperator( "^",
					"Raises this number to the given power. All power operations are performed in double type",
					Int, Double, Double, [](int l, double r) { return pow(double(l), r); }
				),
				
				new MappedOperator( "mod",
					"Returns the remainder of the division of the left operand to the right operand.",
					Int, Int, Int, [](int l, int r) { 
						return l%r;
					}
				),
				
				new Function( "abs",
					"Returns the absolute value of this value",
					Int, {
						MapFunction(
							[](int v) {
								return abs(v);
							},
							Int,
							{}, ConstTag
						)
					}
				),
				
				MAP_COMPARE( =, ==, Int, int),
				MAP_COMPARE(>=, >=, Int, int),
				MAP_COMPARE(<=, <=, Int, int),
				MAP_COMPARE(!=, !=, Int, int),
				MAP_COMPARE( >, >,  Int, int),
				MAP_COMPARE( <, <,  Int, int),
			});
			
			Float->AddFunctions({
				new MappedOperator("+", "Adds two numbers together",
					Float, {
						MapOperator(
							[](float l, double r) { return l+r; },
							Double, Double
						),
						MapOperator(
							[](float l, float r) { return l+r; },
							Float, Float
						),
					}
				),
				
				new MappedOperator("-", "Subtracts a number from this one",
					Float, {
						MapOperator(
							[](float l, double r) { return l-r; },
							Double, Double
						),
						MapOperator(
							[](float l, float r) { return l-r; },
							Float, Float
						),
					}
				),
				
				new MappedOperator("*", "Multiplies two numbers together",
					Float, {
						MapOperator(
							[](float l, double r) { return l*r; },
							Double, Double
						),
						MapOperator(
							[](float l, float r) { return l*r; },
							Float, Float
						),
					}
				),

				new MappedOperator( "/",
					"Divides this number to the given one. Division operation is performed in double type",
					Float, Double, Double, [](float l, double r) { return double(l)/r; }
				),
				
				new MappedOperator( "^",
					"Raises this number to the given power. All power operations are performed in double type",
					Float, Double, Double, [](float l, double r) { return pow(double(l), r); }
				),
				
				new MappedOperator( "mod",
					"Returns the remainder of the division of the left operand to the right operand.",
					Float, Float, Float, [](float l, float r) -> float { 
						return fmod(l, r);
					}
				),
				
				new Function( "abs",
					"Returns the absolute value of this value",
					Float, {
						MapFunction(
							[](float v) {
								return v*(v<0?-1:1);
							},
							Float,
							{}, ConstTag
						)
					}
				),
				
				new Function( "floor",
					"Returns the largest integer number smaller than this one.", 
					Float, {
						MapFunction(
							[](float v) -> float {
								return floor(v);
							}, Float,
							{ },
							ConstTag
						)
					}
				),
				
				new Function( "ceil",
					"Returns the smallest integer number larger than this one.", 
					Float, {
						MapFunction(
							[](float v) -> float {
								return ceil(v);
							}, Float,
							{ },
							ConstTag
						)
					}
				),
				
				new Function( "round",
					"Returns the closest integer number to this one.", 
					Float, {
						MapFunction(
							[](float v) -> float {
								return round(v);
							}, Float,
							{ },
							ConstTag
						),
						MapFunction(
							[](float v, int digits) -> float {
								double exp=exp10(digits);
								return float(round(v*exp)/exp);
							}, Float,
							{ 
								Parameter( "digits",
									"The digits that will be kept after the decimal point.",
									Int
								)
							},
							ConstTag
						),
					}
				),
				
				MAP_COMPARE( =, ==, Float, float),
				MAP_COMPARE(>=, >=, Float, float),
				MAP_COMPARE(<=, <=, Float, float),
				MAP_COMPARE(!=, !=, Float, float),
				MAP_COMPARE( >, >,  Float, float),
				MAP_COMPARE( <, <,  Float, float),
			});
			
			Double->AddFunctions({
				new MappedOperator("+", "Adds two numbers together",
					Double, {
						MapOperator(
							[](double l, double r) { return l+r; },
							Double, Double
						),
					}
				),
				
				new MappedOperator("-", "Subtracts a number from this one",
					Double, {
						MapOperator(
							[](double l, double r) { return l-r; },
							Double, Double
						),
					}
				),
				
				new MappedOperator("*", "Multiplies two numbers together",
					Double, {
						MapOperator(
							[](double l, double r) { return l*r; },
							Double, Double
						),
					}
				),

				new MappedOperator( "/",
					"Divides this number to the given one. Division operation is performed in double type",
					Double, Double, Double, [](double l, double r) { return l/r; }
				),
				
				new MappedOperator( "^",
					"Raises this number to the given power. All power operations are performed in double type",
					Double, Double, Double, [](double l, double r) { return pow(l, r); }
				),
				
				new MappedOperator( "mod",
					"Returns the remainder of the division of the left operand to the right operand.",
					Double, Double, Double, [](double l, double r) { 
						return fmod(l, r);
					}
				),
				
				new Function( "abs",
					"Returns the absolute value of this value",
					Double, {
						MapFunction(
							[](double v) {
								return fabs(v);
							},
							Double,
							{}, ConstTag
						)
					}
				),
				
				new Function( "floor",
					"Returns the largest integer number smaller than this one.", 
					Double, {
						MapFunction(
							[](double v) -> double {
								return floor(v);
							}, Double,
							{ },
							ConstTag
						)
					}
				),
				
				new Function( "ceil",
					"Returns the smallest integer number larger than this one.", 
					Double, {
						MapFunction(
							[](double v) -> double {
								return ceil(v);
							}, Double,
							{ },
							ConstTag
						)
					}
				),
				
				new Function( "round",
					"Returns the closest integer number to this one.", 
					Double, {
						MapFunction(
							[](double v) {
								return round(v);
							}, Double,
							{ },
							ConstTag
						),
						MapFunction(
							[](double v, int digits) {
								double exp=exp10(digits);
								return round(v*exp)/exp;
							}, Double,
							{ 
								Parameter( "digits",
									"The digits that will be kept after the decimal point.",
									Int
								)
							},
							ConstTag
						),
					}
				),
				
				MAP_COMPARE( =, ==, Double, double),
				MAP_COMPARE(>=, >=, Double, double),
				MAP_COMPARE(<=, <=, Double, double),
				MAP_COMPARE(!=, !=, Double, double),
				MAP_COMPARE( >, >,  Double, double),
				MAP_COMPARE( <, <,  Double, double),
			});

			Unsigned->AddFunctions({
				new MappedOperator("+", "Adds two numbers together",
					Unsigned, {
						MapOperator(
							[](unsigned l, unsigned r) { return l+r; }, 
							Unsigned, Unsigned
						),
						MapOperator(
							[](unsigned l, double r) { return l+r; },
							Double, Double
						),
						MapOperator(
							[](unsigned l, float r) { return l+r; },
							Float, Float
						),
					}
				),
				
				new MappedOperator("-", "Subtracts a number from this one. This operation may cause overflow",
					Unsigned, {
						MapOperator(
							[](unsigned l, unsigned r) { return (int)l-(int)r; }, 
							Int, Unsigned
						),
						MapOperator(
							[](unsigned l, int r) { return (int)l-r; }, 
							Int, Int
						),
						MapOperator(
							[](unsigned l, double r) { return (double)l-r; },
							Double, Double
						),
						MapOperator(
							[](unsigned l, float r) { return (float)l-r; },
							Float, Float
						),
					}
				),
				
				new MappedOperator("*", "Multiplies two numbers together",
					Unsigned, {
						MapOperator(
							[](unsigned l, unsigned r) { return l*r; }, 
							Unsigned, Unsigned
						),
						MapOperator(
							[](unsigned l, int r) { return (int)l*r; }, 
							Int, Int
						),
						MapOperator(
							[](unsigned l, double r) { return (double)l*r; },
							Double, Double
						),
						MapOperator(
							[](unsigned l, float r) { return (float)l*r; },
							Float, Float
						),
					}
				),
				
				new MappedOperator( "band",
					"Performs a bitwise and operation",
					Unsigned, Unsigned, Unsigned, [](unsigned l, unsigned r) { return l&r; }
				),
				
				new MappedOperator( "bor",
					"Performs a bitwise or operation",
					Unsigned, Unsigned, Unsigned, [](unsigned l, unsigned r) { return l|r; }
				),
				
				new MappedOperator( "bxor",
					"Performs a bitwise xor operation",
					Unsigned, Unsigned, Unsigned, [](unsigned l, unsigned r) { return l^r; }
				),
				
				new MappedOperator( "bitset",
					"Makes the given bit 1",
					Unsigned, Unsigned, Unsigned, [](unsigned l, unsigned r) { return l|(1<<r); }
				),
				
				new MappedOperator( "bitunset",
					"Makes the given bit 0",
					Unsigned, Unsigned, Unsigned, [](unsigned l, unsigned r) { return l&(~(1<<r)); }
				),
				
				new MappedOperator( "bittest",
					"Checks if the given bit is 1",
					Unsigned, Bool, Unsigned, [](unsigned l, unsigned r) { return (l&(1<<r))!=0; }
				),
				
				new MappedOperator( "shl",
					"Shifts the bits of this number to left",
					Unsigned, Unsigned, Unsigned, [](unsigned l, unsigned r) { return l<<r; }
				),
				
				new MappedOperator( "shr",
					"Shifts the bits of this number to right",
					Unsigned, Unsigned, Unsigned, [](unsigned l, unsigned r) { return l>>r; }
				),
				
				new MappedOperator( "rol",
					"Rotates the bits of this number to left",
					Unsigned, Unsigned, Unsigned, [](unsigned l, unsigned r) { return (l<<r)|(l>>(sizeof(unsigned)*8-r)); } //gorgon works only on 8bit per byte systems
				),
				
				new MappedOperator( "ror",
					"Rotates the bits of this number to right",
					Unsigned, Unsigned, Unsigned, [](unsigned l, unsigned r) { return (l>>r)|(l<<(sizeof(unsigned)*8-r)); } //gorgon works only on 8bit per byte systems
				),

				new MappedOperator( "/",
					"Divides this number to the given one. Division operation is performed in double type",
					Unsigned, Double, Double, [](unsigned l, double r) { return double(l)/r; }
				),
				
				new MappedOperator( "^",
					"Raises this number to the given power. All power operations are performed in double type",
					Unsigned, Double, Double, [](unsigned l, double r) { return pow(double(l), r); }
				),
				
				new MappedOperator( "mod",
					"Returns the remainder of the division of the left operand to the right operand.",
					Unsigned, Unsigned, Unsigned, [](unsigned l, unsigned r) { 
						return l%r;
					}
				),
				
				new Function("inverse", "Inverts the bits", Unsigned, {
					MapFunction(
						[](unsigned val) { return ~val; },
						Unsigned, ParameterList(), ConstTag
					)
				}),
				
				new Function("binary", "Converts this number into a binary string", Unsigned, {
					MapFunction(
						[](unsigned val) -> std::string { 
							std::string ret(' ', sizeof(unsigned)*8);
							for(unsigned i=0;i<sizeof(unsigned)*8;i++) {
								ret[sizeof(unsigned)*8-i-1]=val&1 ? '1':'0';
								val=val>>1;
							}
							return ret; 
						},
						String, ParameterList(), ConstTag
					)
				}),
				
				MAP_COMPARE( =, ==, Unsigned, unsigned),
				MAP_COMPARE(>=, >=, Unsigned, unsigned),
				MAP_COMPARE(<=, <=, Unsigned, unsigned),
				MAP_COMPARE(!=, !=, Unsigned, unsigned),
				MAP_COMPARE( >, >,  Unsigned, unsigned),
				MAP_COMPARE( <, <,  Unsigned, unsigned),
			});
			
			Int->AddConstructors({
				MapTypecast<float, int>(Float, Int),
				MapTypecast<double, int>(Double, Int),
				MapTypecast<unsigned, int>(Unsigned, Int),
				MapTypecast<Gorgon::Byte, int>(Byte, Int)
			});
			
			Unsigned->AddConstructors({
				MapTypecast<float, unsigned>(Float, Unsigned),
				MapTypecast<double, unsigned>(Double, Unsigned),
				MapTypecast<int, unsigned>(Int, Unsigned),
				MapTypecast<Gorgon::Byte, unsigned>(Byte, Unsigned),
			});
			
			Float->AddConstructors({
				MapTypecast<unsigned, float>(Unsigned, Float),
				MapTypecast<double, float>(Double, Float),
				MapTypecast<int, float>(Int, Float),
				MapTypecast<Gorgon::Byte, float>(Byte, Float)
			});
			
			Double->AddConstructors({
				MapTypecast<float, double>(Float, Double),
				MapTypecast<unsigned, double>(Unsigned, Double),
				MapTypecast<int, double>(Int, Double),
				MapTypecast<Gorgon::Byte, double>(Byte, Double)
			});
			
			Byte->AddConstructors({
				MapTypecast<float, Gorgon::Byte>(Float, Byte),
				MapTypecast<double, Gorgon::Byte>(Double, Byte),
				MapTypecast<int, Gorgon::Byte>(Int, Byte),
				MapTypecast<unsigned, Gorgon::Byte>(Unsigned, Byte),
				MapTypecast<char, Gorgon::Byte>(Char, Byte)
			});
			
			Char->AddConstructors({
				MapTypecast<Gorgon::Byte, char>(Byte, Char)
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
				
				new Function("not", "Inverts boolean value", Bool, {
					MapFunction(
						[](bool val) { return !val; },
						Bool, ParameterList(), ConstTag
					)
				})
			});

			Bool->AddConstructors({
				MapTypecast<Gorgon::Byte, bool>(Byte, Bool),
				MapTypecast<unsigned, bool>(Unsigned, Bool),
				MapTypecast<int, bool>(Int, Bool),
			});
			
			String->AddFunctions({
				new Function("Length",
					"Returns the length of the string", String, 
				 {MapFunction(&std::string::length, Unsigned, {}, ConstTag)}
				),
				
				new Function("Substr",
					"Returns a part of the string", String, 
					{
						MapFunction(
							&std::string::substr, String, 
							{
								Parameter("Start", "Start of the substring, first element is at 0", Unsigned),
								Parameter("Length", "Length of the substring to be returned", Unsigned, 
										  Data(Unsigned, std::string::npos), OptionalTag)
							},
							ConstTag
						)
					}
				),
				
				new Function("Find",
					"Returns the position of a given string", String, 
					{
						MapFunction(
							(std::size_t(std::string::*)(const std::string &, std::size_t) const)&std::string::find, Unsigned, 
							{
								Parameter("Search", "The string to be searched", String),
								Parameter("Start", "Starting point of the search, "
									"if not specified search will start from the beginning", Unsigned, 
									Data(Unsigned, 0u), OptionalTag
 								),
							},
							ConstTag
						),
					}
				),
				
				new Function("ToLower",
					"Returns lowercase string", String,
					{
						MapFunction(&String::ToLower, String, { }, ConstTag)
					}
				),
				
				new Function("ToUpper",
					"Returns upper string", String,
					{
						MapFunction(&String::ToUpper, String, { }, ConstTag)
					}
				),
				
				
				new Function("Trim",
					"Trims start and the end of the string", String, 
					{
						MapFunction(
							[](const std::string &s) { 
								return String::Trim(s); 
							}, String, 
							{ 
							}, ConstTag
						)
					}
				),
				
				new Function("Extract",
					"Extracts the part of the string up to the given marker", String, 
					{
						MapFunction(
							(std::string(*)(std::string &, const std::string))String::Extract, String, 
							{
								Parameter("Marker", "String that will be searched.", String)
							}
						)
					}
				),
				
				new Function("Replace",
					"Replaces all instances of the given substring in this string with another string", String, 
					{
						MapFunction(
							(std::string(*)(const std::string &, const std::string &, const std::string &))&String::Replace, String,
							{
								Parameter("Search", "Search string to be replaced", String),
								Parameter("Replace", "String to replace, if not specified, "
									"empty string is assumed", String, Data(String, std::string("")), OptionalTag),
							},
							ConstTag
   						)
					}
				),
				
				new Function("[]",
					"Accesses individual characters of the string", String,
					{
						MapFunction(
							[](const std::string &str, unsigned index) {
								//...out of bounds error
								return str[index];
							}, Char,
							{
								Parameter("Index", "Index of the element to be retrieved", Unsigned)
							},
							ConstTag
						),
						MapFunction(
							[](std::string &str, unsigned index) -> char& {
								//...out of bounds error
								return str[index];
							}, Char,
							{
								Parameter("Index", "Index of the element to be retrieved", Unsigned)
							},
							ReferenceTag
						),
					}
				),
				
				new Function("[]=",
					"Changes individual characters of the string", String,
					{
						MapFunction(
							[](std::string &str, unsigned index, char c) {
								//...out of bounds error
								str[index]=c;
							}, nullptr,
							{
								Parameter("Index", "Index of the element to be retrieved", Unsigned),
								Parameter("Value", "Value to be assigned to the element", Char)
							}
						),						
					}
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
					Variant
				},
				FunctionList{},
				ConstantList {
					new Constant("Pi", "Contains the value of PI", Double,             Any(3.14159265358979)),
					new Constant("e",  "Contains the value of Euler's number", Double, Any(2.71828182845905)),
					new Constant("MAX_INT",  "Maximum value an integer can hold", Int, Any(std::numeric_limits<int>::max())),
					new Constant("MIN_INT",  "Minimum value an integer can hold", Int, Any(std::numeric_limits<int>::min())),
				}
			};
			
			Integrals.AddTypes({ArrayType()});
			Integrals.AddFunctions(ArrayFunctions());
			Integrals.AddFunctions({
				new Function("Print",
					"This function prints the given parameters to the screen without a new line at the end.", nullptr,
					{
						MapFunction(
							Print, nullptr, 
							{
								Parameter( "string",
									"The strings that will be printed.",
									String, OptionalTag
								)
							},
							StretchTag, RepeatTag
						)
					}
				),
				
				new Function("GetEnv",
					"Returns the environment variable with the given name", nullptr,
					{
						MapFunction(
							[](const std::string &n) -> std::string {
								char *v=getenv(n.c_str());
								if(v)
									return v;
								else
									return "";
							}, 
							Types::String(), 
							{
								Parameter( "string",
									"The name of the environment variable.",
									String
								)
							}
						)
					}
				),
				
				
				new Function("Read",
					"This function reads a value from the console.", nullptr,
					{
						MapFunction(
							[](const Type *type) -> Data {
								std::string line;
								std::getline(VirtualMachine::Get().GetInput(), line);
								
								return type->Parse(line);
							}, 
							Types::Variant(),
							{
								Parameter( "type",
									"The type of the input",
									TypeType(), Data(TypeType(), &Types::String(), true, true), 
									ConstTag, ReferenceTag, OptionalTag
								)
							}
						)
					}
				)
			});
			
			Keywords={"Keywords", "Function like keywords.",
				TypeList {},
				FunctionList {
					new Function("const",
						"Makes the given variable a constant", nullptr,
						{
							MapFunction(
								[](std::string varname) {
									auto var=VirtualMachine::Get().getvarref(varname);
									if(!var) {
										throw SymbolNotFoundException(varname, SymbolType::Variable);
									}
									var->MakeConstant();
								}, nullptr,
								{
									Parameter("Variable",
										"This is the variable to become constant",
										String, VariableTag
									)
								}
							)
						},
						KeywordTag
					),
					new Function("static",
						"Creates a static variable", nullptr, 
						{
							MapFunction(
								static1, nullptr,
								{
									Parameter(
										"Variable",
										"This is the variable to be declared",
										String, VariableTag
									)
								}
							),
							MapFunction(
								static2, nullptr,
								{
									Parameter(
										"Variable",
										"This is the variable to be declared",
										String, VariableTag
									),
									Parameter(
										"Value",
										"This is the variable to be declared",
										Variant
									)
								}
							)
						},
						KeywordTag
					),
					new Function("unset",
						"Unsets a given variable", nullptr,
						{
							MapFunction(
								[](std::string varname) {
									VirtualMachine::Get().UnsetVariable(varname);
								}, nullptr,
								{
									Parameter("Variable",
										"This is the variable to be unset",
										String, VariableTag
									)
								}
							)
						},
						KeywordTag
					),
				},
			};
			
			Reflection={"Reflection", "This library contains reflection objects",
				TypeList { TypeType(), FunctionType(), ParameterType() },
				FunctionList {
				new Function("Echo",
					"This function prints the given parameters to the screen with a newline following them.", nullptr,
					{
						MapFunction(
							Echo, nullptr, 
							{
								Parameter( "string",
									"The strings that will be printed.",
									String, OptionalTag
								)
							},
							StretchTag, RepeatTag
						)
					}, KeywordTag
				),
					new Function("TypeOf",
						"This function returns the type of the given variable.", nullptr, 
						{
							MapFunction(
								[](std::string variable) -> const Type* {
									return &VirtualMachine::Get().GetVariable(variable).GetType();
								}, TypeType(),
								{
									Parameter("Variable",
										"The variable to determine its type.",
										String, VariableTag
									)
								}, ReturnsConstTag
							)
							
						}
					),
					new Function("VarInfo",
						"This function returns information about a variable.", nullptr, 
						{
							MapFunction(
								[](std::string variable) {
									auto &vm=VirtualMachine::Get();
									auto var=vm.GetVariable(variable);
									vm.GetOutput()<<"Name : "<<var.GetName()<<std::endl;
									if(var.IsValid()) {
										vm.GetOutput()<<"Type : "<<var.GetType().GetName()<<std::endl;
										vm.GetOutput()<<"Ref  : "<<(var.IsReference() ? "yes" : "no")<<std::endl;
										vm.GetOutput()<<"Const: "<<(var.IsConstant()  ? "yes" : "no")<<std::endl;
										vm.GetOutput()<<"Value: "<<var.GetType().ToString(var)<<std::endl;
										if(var.IsReference()) {
											vm.GetOutput()<<"Ptr  : "<<var.GetData().Pointer()<<std::endl;
										}
									}
									else {
										vm.GetOutput()<<"Value: <Invalid>"<<std::endl;
									}
								}, nullptr,
								{
									Parameter("Variable",
										"The variable to determine its type.",
										String, VariableTag
									)
								}, ReturnsConstTag
							)
							
						}
					)
				}
			};
			
			InitTypeType();
		}
		
	}
}
