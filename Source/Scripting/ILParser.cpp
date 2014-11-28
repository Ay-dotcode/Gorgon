#include "Parser.h"
#include "../Scripting.h"


namespace Gorgon {
	namespace Scripting {
		
		/**
		 * @page GScript
		 * @subpage ILParser
		 */
		
		/**
		 * @page ILParser Intermediate Language
		 * 
		 * Gorgon Script Intermediate Language (IL) is designed for debugging and disassembling. It is human readable
		 * representation of the underlying data structure. It is not designed to be back compatible. Therefore,
		 * it is not adviced to do any serious programming with this language. However, it is very usable to see
		 * dialect compiler outputs to check for any errors. Its one to one representation makes it very easy to 
		 * convert from/to internal data structure.
		 * 
		 * @section Structure Structure
		 * Every instruction is in a single line. The structure of IL is very rigid and the rules are enforced.
		 * \# character at the *beginning* of the line causes parser to ignore that line. Empty lines are ignored.
		 * Additionally, spaces at the beginning and the end of the line are ignored. Tabs are also counted as space.
		 * Following is a simple assignment that performs `$var = 3 + $i`
		 * @code
		 * #$var = 3 + $i
		 * ."1"   = fms"+" i"3" $"i"
		 * $"var" = ."1"
		 * @endcode
		 * 
		 * Spaces are not significant in terms of parameter separation, so the second parameter could start right after
		 * the previous one without any spaces in between.
		 * 
		 * @section Value_Types Value Types
		 * %Value types are directly derived from Gorgon::Scripting::ValueType. However, literal types are also denoted.
		 * Every value type has a unique character that identifies it. A double quotes should follow this character, 
		 * spaces are not allowed.
		 * 
		 * 
		 * @subsection VT_Variable Variable
		 * Variable is denoted with $ symbol. It should be followed by variable name in double quotes. Following is an example
		 * for a variable value type:
		 * @code
		 * $"var"
		 * @endcode
		 * 
		 * @subsection VT_Constant Constant
		 * %Constant is denoted with ! symbol. It should be followed by constant name in double quotes. %Constant can be qualified
		 * with namespace (using column for separator). Following is the constant Pi in the library `Integrals`:
		 * @code
		 * !"Integrals:Pi"
		 * @endcode
		 * 
		 * 
		 * @subsection VT_Identifier Identifier
		 * Since compilation step cannot always differentiate between language constructs, VM accepts identifier type and resolves it
		 * on the fly. However, if it is possible, specifying the type is recommended as it will work faster. Identifiers are marked 
		 * with "?" character.
		 * 
		 * 
		 * @subsection VT_Temporary Temporaries
		 * Temporaries are necessary to break compound statements. There are 255 temporaries in a typical GScript Virtual Machine.
		 * Their index starts from 1. Temporary symbol is dot. and like other value constructs, temporary index should be in quotes.
		 * These values can be reused as soon as the old one is no longer needed. A temporary can be used multiple times and will
		 * not be unset automatically. This might cause delayed destruction of reference counted objects. Therefore, compilers are
		 * should set an instructor to unset temporaries.
		 * 
		 * 
		 * @subsection VT_Literal Literals
		 * Every literal has a different symbol. Double quoted textual value of the literal should follow the symbol. Currently,
		 * literals are parsed without syntax checking. This *might* be fixed in the future. Following is the list of literal
		 * symbols that are currently supported:
		 * - i : Integer
		 * - u : Unsigned
		 * - f : Float
		 * - d : Double
		 * - c : Char
		 * - n : Byte
		 * - s : %String
		 * 
		 * 
		 * @section Instruction_Types Instruction Types
		 * IL contains three basic instruction types. Other instruction types that exists in Gorgon::Scripting::InstructionType
		 * are variants of these basic types.
		 * 
		 * 
		 * @subsection Assignment Assignment
		 * Assignment instructions starts with a variable value and can uniquely be identified from the first $ sign. After
		 * variable value (see \ref VT_Variable), there should be an equal sign followed by the value to be assigned. This
		 * value could be represented with any value type denoted in \ref Value_Types. Assignment to a function result is not 
		 * possible without using a temporary. Following is an example for assignment:
		 * @code
		 * $"start" = i"4"
		 * @endcode
		 * 
		 * 
		 * @subsection Call Function/Method call
		 * A function call is denoted by "f" symbol, while "m" is method call. These symbols are followed by either "n" or "m",
		 * "n" meaning namespace function/method where as "m" is member. Please note that GScript supports argument dependent 
		 * lookup over types. This means that even if a function/method is marked as namespace and it does not exists, it is looked
		 * from the first argument. This system is consistent with the notion that "this" object is passed as the first parameter
		 * to member functions/methods. A value should follow to indicate the function/method identifier. This could be either
		 * a string literal, constant (same as string literal) or variable. If it is a variable and its type is string, that function
		 * is searched and called, if not, the variable type's () function is called. 
		 * 
		 * After the function identifier, parameters of the function is supplied. Each parameter is a value. Values do not need
		 * spaces in between, however, it is possible to separate them with spaces. 
		 * 
		 * 
		 * The following line calls echo function as `echo("Result: ", 3+4);`
		 * ."1" = fns"+" i"3" i"4"
		 * fns"echo" s"Result" ."1"
		 * 
		 * 
		 * @subsection KeywordMarker Keyword marker
		 * A keyword should be placed before any calculations for a keyword is made. It is required to mark the start of the keyword
		 * call, as this point could be used later on as a jump target. A keyword marker starts with "fkm" character series and is
		 * followed by the name of the keyword in double quotes.
		 * 
		 * 
		 * @subsection RemoveTemp Remove temporary
		 * Remove temporary instruction is used to unset temporaries so that if they contain a reference object, it can be
		 * destructed. The format for this instruction is an x followed by temporary index in quotes.
		 */
		
		
		
		namespace {
			template<class ...P_>
			int CheckInputFor(const std::string &input, int &ch, P_ ...args) {
				char allowed[]={args...};
				int elements=sizeof...(args);

				auto errstr=[&] ()-> std::string {
					std::string ret="Expected ";
					for(int i=0;i<elements;i++) {
						if(i!=0) {
							ret.push_back('/');
						}
						ret.push_back(allowed[i]);
					}
					
					return ret;
				};
				
				if(input.length()<=ch) {
					throw ParseError({ParseError::UnexpectedToken, 0, ch, errstr()+", end of string encountered."});
				}
				
				char c=input[ch++];
				
				for(int i=0;i<sizeof...(args);i++) {
					if(allowed[i]==c) {
						return i;
					}
				}
				
				ch--;
				throw ParseError({ParseError::UnexpectedToken, 0, ch, errstr()+", found: "+input.substr(ch,1)});
			}
		}
		
		unsigned IntermediateParser::Parse(const std::string &input) {
			
			Instruction instruction;
			
			int ch=0;
			eatwhite(input, ch);
			
			if(input.size()<=ch) return 0;
			
			int ret=0;
			
			switch(CheckInputFor(input, ch, '#', '.', 'f', 'm', '$', 'x')) {
				case 0:
					return 0;
					
				case 1:
					storedfn(input, ch);
					ret=1;
					break;

				case 2:
				case 3:
					fncall(input, --ch);
					ret=1;
					break;

				case 4:
					varassign(input, ch);
					ret=1;
					break;
					
					
				case 5:
					List.resize(List.size()+1);
					auto &inst=List.back();
					inst.Type  = InstructionType::RemoveTemp;
					inst.Store = parsetemporary(input, ch);
					ret=1;
					break;
			}
			
			eatwhite(input, ch);
			if(ch<input.size()) {
				throw ParseError({ParseError::UnexpectedToken, 0, 1, "Expected end of line, found: "+input.substr(ch,1)});
			}
			
			return ret;
		}
		
		void IntermediateParser::storedfn(const std::string& input, int &ch) {
			auto temp=parsetemporary(input, ch);
			
			eatwhite(input, ch);			
			CheckInputFor(input, ch, '=');
			eatwhite(input, ch);
			
			fncall(input, ch, false);

			auto &inst=List.back();
			inst.Store=temp;
		}
		
		void IntermediateParser::fncall(const std::string& input, int &ch, bool allowmethod) {
			List.resize(List.size()+1);
			auto &inst=List.back();
			
			bool ismethod=CheckInputFor(input, ch, 'f', (allowmethod ? 'm' : '\0'))==1;
			
			if(ismethod) {
				bool ismember=CheckInputFor(input, ch, 'n', 'm');
				
				inst.Type=ismember ? InstructionType::MemberMethodCall : InstructionType::MethodCall;
			}
			else {
				switch(CheckInputFor(input, ch, 'm', 'n', (allowmethod ? 'k': '\0'))) {
				case 0:
					inst.Type=InstructionType::MemberFunctionCall;
					break;
				case 1:
					inst.Type=InstructionType::FunctionCall;
					break;
				case 2:
					inst.Type=InstructionType::Mark;
					CheckInputFor(input, ch, 'm');
					inst.Name.Name=extractquotes(input, ch);
					return;
				}
			}
			
			inst.Store=0;
			
			eatwhite(input, ch);
			
			inst.Name=parsevalue(input, ch);
			
			while(ch<input.size()) {				
				inst.Parameters.push_back(parsevalue(input, ch));
				eatwhite(input, ch);
			}
		}			
		
		void IntermediateParser::varassign(const std::string& input, int &ch) {
			List.resize(List.size()+1);
			auto &inst=List.back();
			inst.Type=InstructionType::Assignment;
			
			inst.Name.Type=ValueType::Variable;
			inst.Name.Name=extractquotes(input, ch);
			
			eatwhite(input, ch);
			
			CheckInputFor(input, ch, '=');
			
			inst.RHS=parsevalue(input, ch);
		}
		
		void IntermediateParser::eatwhite(const std::string& input, int& ch) {
			for(;ch<input.size();ch++) {
				if(input[ch]!=' ' && input[ch]!='\t') return;
			}
		}
		
		Value IntermediateParser::parsevalue(const std::string& input, int& ch) {
			eatwhite(input, ch);

			Value ret;
			
			switch(CheckInputFor(input, ch, '.', '$', '!', 'i', 'f', 'b', 's', 'c', 'B', 'd', 'u')) {
				case 0:
					ret.Type  =ValueType::Temp;
					ret.Result=parsetemporary(input, ch);
					return ret;
					
				case 1:
					ret.Type  = ValueType::Variable;
					ret.Name  = extractquotes(input, ch);
					return ret;
					
				case 2:
					ret.Type  = ValueType::Constant;
					ret.Name  = extractquotes(input, ch);
					return ret;
					
				case 3:
					ret.Type    = ValueType::Literal;
					ret.Literal = {Types::Int(), String::To<int>(extractquotes(input, ch))};
					return ret;
					
				case 4:
					ret.Type    = ValueType::Literal;
					ret.Literal = {Types::Float(), String::To<float>(extractquotes(input, ch))};
					return ret;
					
				case 5:
					ret.Type    = ValueType::Literal;
					CheckInputFor(input, ch, '"');
					ret.Literal = {Types::Bool(), CheckInputFor(input, ch, '0', '1')};
					CheckInputFor(input, ch, '"');
					
					return ret;
					
				case 6:
					ret.Type    = ValueType::Literal;
					ret.Literal = {Types::String(), extractquotes(input, ch)};
					return ret;
					
				case 7:
					ret.Type    = ValueType::Literal;
					ret.Literal = {Types::Char(), (char)String::To<int>(extractquotes(input, ch))};
					return ret;
					
				case 8:
					ret.Type    = ValueType::Literal;
					ret.Literal = {Types::Byte(), (Gorgon::Byte)String::To<int>(extractquotes(input, ch))};
					return ret;
					
				case 9:
					ret.Type    = ValueType::Literal;
					ret.Literal = {Types::Double(), String::To<double>(extractquotes(input, ch))};
					return ret;
					
				case 10:
					ret.Type    = ValueType::Literal;
					ret.Literal = {Types::Unsigned(), String::To<unsigned>(extractquotes(input, ch))};
					return ret;
			}
			
			throw 0;
		}
		
		std::string IntermediateParser::extractquotes(const std::string &input, int &ch) {
			std::string ret="";
			
			CheckInputFor(input, ch, '"');
			
			int start=ch;
			
			bool escape=false;
			int escapenum=0;
			Gorgon::Byte num;
			for(; ch<input.size(); ch++) {
				char c=input[ch];
				if(escape) {
					if(c=='"') {
						ret.push_back('"');
					}
					else if(c=='\\') {
						ret.push_back('\\');
					}
					else if(c=='n') {
						ret.push_back('\n');
					}
					else if(c>='0' && c<='9') {
						escapenum++;
						num=num<<4+(c-'0');
					}
					else if(c>='a' && c<='f') {
						escapenum++;
						num=num<<4+(c-'a'+10);
					}
					else {
						throw ParseError({ParseError::UnexpectedToken, 0, ch, "Invalid escape sequence: \\"+input.substr(ch,1)});
					}
					
					if(escapenum!=1) {
						escape=false;
					}
					if(escapenum==2) {
						ret.push_back((char)num);
					}
				}
				else {
					if(c=='\\') {
						escape=true;
					}
					else if(c=='"') {
						break;
					}
					else {
						ret.push_back(c);
					}
				}
			}
			
			CheckInputFor(input, ch, '"');
			
			return ret;
		}
		
		unsigned long IntermediateParser::parsetemporary(const std::string &input, int &ch) {
			auto str=extractquotes(input, ch);
			auto ret=String::To<Gorgon::Byte>(str);
			if(ret==0 && ret>255) {
				throw std::runtime_error("Invalid temporary: "+str);
			}
			return ret;
		}
	}
}
