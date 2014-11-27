#include "Parser.h"
#include "../Scripting.h"


namespace Gorgon {
	namespace Scripting {
		
		template<class ...P_>
		static int CheckInputFor(const std::string &input, int &ch, P_ ...args) {
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
		
		
		unsigned IntermediateParser::Parse(const std::string &input) {
			
			Instruction instruction;
			
			int ch=0;
			eatwhite(input, ch);
			
			if(input.size()<=ch) return 0;
			
			int ret=0;
			
			switch(CheckInputFor(input, ch, '#', '.', 'f', 'm', '$')) {
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
			
			switch(CheckInputFor(input, ch, '.', '$', '!', 'i', 'f', 'b', 's', 'a', 'n', 'd', 'u')) {
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
					if(input.size()<=ch) {
						throw ParseError({ParseError::UnexpectedToken, 0, ch, "Expected 0 or 1, end of string encountered."});
					}
					if(input[ch]=='0') {
						ret.Literal = {Types::Bool(), false};
					}
					else if(input[ch]=='1') {
						ret.Literal = {Types::Bool(), true};
					}
					else {
						throw ParseError({ParseError::UnexpectedToken, 0, ch, "Expected 0 or 1, found: "+input.substr(ch,1)});
					}
					ch++;
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
			//TODO: Exception on not an int
			auto str=extractquotes(input, ch);
			auto ret=String::To<Gorgon::Byte>(str);
			if(ret==0 && ret>255) {
				throw std::runtime_error("Invalid temporary: "+str);
			}
			return ret;
		}
	}
}