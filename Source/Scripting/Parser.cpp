#include "Parser.h"
#include "../Scripting.h"


namespace Gorgon {
	namespace Scripting {
		
		unsigned IntermediateParser::Parse(const std::string &input) {
			
			Instruction instruction;
			
			int ch=1;
			eatwhite(input, ch);
			
			if(input.size()<=ch) return 0;
			
			switch(input[ch]) {
				// comment
				case '#':
					return 0;
					
				case '.':
					storedfn(input, ch);
					return 1;
					
				case '-':
					fncall(input, ch);
					return 1;
					
				case '$':
					varassign(input, ch);
					return 1;
					
				default:
					throw ParseError({ParseError::UnexpectedToken, 0, 1, "Expected #, ., -, or $, found: "+input.substr(ch,1)});
			}
			
			eatwhite(input, ch);
			if(ch<input.size()) {
				throw ParseError({ParseError::UnexpectedToken, 0, 1, "Expected end of line, found: "+input.substr(ch,1)});
			}
		}
			
		void IntermediateParser::storedfn(const std::string& input, int &ch) {
			ch++;
			
			List.resize(List.size()+1);
			auto &inst=List.back();
			inst.Type=InstructionType::FunctionCall;
			inst.Store=parsetemporary(input, ch);
			
			eatwhite(input, ch);
			
			if(input.size()<=ch) {
				throw ParseError({ParseError::UnexpectedToken, 0, ch, "Expected =, end of string encountered."});
			}
			if(input[ch]!='=') {
				throw ParseError({ParseError::UnexpectedToken, 0, ch, "Expected =, found: "+input.substr(ch,1)});
			}
			ch++;
			
			eatwhite(input, ch);
			
			if(input.size()<=ch) {
				throw ParseError({ParseError::UnexpectedToken, 0, ch, "Expected -, end of string encountered."});
			}
			if(input[ch]!='-') {
				throw ParseError({ParseError::UnexpectedToken, 0, ch, "Expected -, found: "+input.substr(ch,1)});
			}
			ch++;
			
			inst.Name=parsevalue(input, ch);
			
			while(ch<input.size()) {				
				inst.Parameters.push_back(parsevalue(input, ch));
				
				eatwhite(input, ch);
			}
		}
		
		void IntermediateParser::fncall(const std::string& input, int &ch) {
			ch++;
			
			List.resize(List.size()+1);
			auto &inst=List.back();
			inst.Type=InstructionType::FunctionCall;
			inst.Store=0;
			
			eatwhite(input, ch);
			
			inst.Name=parsevalue(input, ch);
			
			while(ch<input.size()) {				
				inst.Parameters.push_back(parsevalue(input, ch));
				
				eatwhite(input, ch);
			}
		}			
		
		void IntermediateParser::varassign(const std::string& input, int &ch) {
			ch++;
			
			List.resize(List.size()+1);
			auto &inst=List.back();
			inst.Type=InstructionType::Assignment;
			
			eatwhite(input, ch);
			
			inst.Name.Type=ValueType::Literal;
			inst.Name.Literal={Integrals.Types["String"], extractquotes(input, ch)};
			
			inst.RHS=parsevalue(input, ch);
		}			
		
		void IntermediateParser::eatwhite(const std::string& input, int& ch) {
			for(;ch<input.size();ch++) {
				if(!std::isspace(input[ch])) return;
			}
		}
		
		Value IntermediateParser::parsevalue(const std::string& input, int& ch) {
			eatwhite(input, ch);
			if(input.size()<=ch) {
				throw ParseError({ParseError::UnexpectedToken, 0, ch, "Expected i, f, s, b, $, ., end of string encountered."});
			}
			if(input[ch]!='"') {
				throw ParseError({ParseError::UnexpectedToken, 0, ch, "Expected i, f, s, b, $, ., found: "+input.substr(ch,1)});
			}
			
			Value ret;
			
			switch(input[ch++]) {
				case '.':
					ret.Type  =ValueType::Temp;
					ret.Result=parsetemporary(input, ch);
					return ret;
					
				case '$':
					ret.Type  = ValueType::Variable;
					ret.Name  = extractquotes(input, ch);
					return ret;
					
				case 'c':
					ret.Type  = ValueType::Constant;
					ret.Name  = extractquotes(input, ch);
					return ret;
					
				case 'i':
					ret.Type    = ValueType::Literal;
					ret.Literal = {Integrals.Types["Int"], String::To<int>(extractquotes(input, ch))};
					return ret;
					
				case 'f':
					ret.Type    = ValueType::Literal;
					ret.Literal = {Integrals.Types["Float"], String::To<float>(extractquotes(input, ch))};
					return ret;
					
				case 'b':
					ret.Type    = ValueType::Literal;
					if(input.size()<=ch) {
						throw ParseError({ParseError::UnexpectedToken, 0, ch, "Expected 0 or 1, end of string encountered."});
					}
					if(input[ch]=='0') {
						ret.Literal = {Integrals.Types["Float"], false};
					}
					else if(input[ch]=='1') {
						ret.Literal = {Integrals.Types["Float"], true};
					}
					else {
						throw ParseError({ParseError::UnexpectedToken, 0, ch, "Expected 0 or 1, found: "+input.substr(ch,1)});
					}
					return ret;
					
				case 's':
					ret.Type    = ValueType::Literal;
					ret.Literal = {Integrals.Types["String"], extractquotes(input, ch)};
					return ret;
			}
		}
		
		std::string extractquotes(std::string input, int &ch) {
			if(input.size()<=ch) {
				throw ParseError({ParseError::UnexpectedToken, 0, ch, "Expected \", end of string encountered."});
			}
			if(input[ch]!='"') {
				throw ParseError({ParseError::UnexpectedToken, 0, ch, "Expected \", found: "+input.substr(ch,1)});
			}
			
			std::string ret="";
			
			int start=ch+1;
			
			bool escape=false;
			for(; ch<input.size(); ch++) {
				if(escape) {
					if(input[ch]=='"') {
						ret.push_back('"');
					}
					else if(input[ch]=='\\') {
						ret.push_back('\\');
					}
					else if(input[ch]=='\n') {
						ret.push_back('\n');
					}
					else {
						throw ParseError({ParseError::UnexpectedToken, 0, ch, "Invalid escape sequence: \\"+input.substr(ch,1)});
					}
				}
				else {
					if(input[ch]=='\\') {
						escape=true;
					}
					else if(input[ch]=='"') {
						break;
					}
					else {
						ret.push_back(input[ch]);
					}
				}
			}
			
			if(input.size()<=ch) {
				throw ParseError({ParseError::UnexpectedToken, 0, ch, "Closing \" not found."});
			}
			if(input[ch]!='"') {
				throw ParseError({ParseError::UnexpectedToken, 0, ch, "Expected \", found: "+input.substr(ch,1)});
			}
			
			ch++;
			return ret;
		}
		
		unsigned long parsetemporary(std::string input, int ch) {
			//TODO: Exception on not an int
			auto str=extractquotes(input, ch);
			auto ret=String::To<unsigned long>(str);
			if(ret==0) {
				throw std::runtime_error("Invalid temporary: "+str);
			}
			return ret;
		}
	}
}