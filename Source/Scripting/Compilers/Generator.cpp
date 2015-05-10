#include "../Scope.h"
#include "../Instruction.h"
#include "../../Scripting.h"

namespace Gorgon { namespace Scripting { namespace Compilers {
	
	std::string disassemblevalue(const Value &value) {
		switch(value.Type) {
			case ValueType::Constant:
				return "!\""+value.Name+"\"";
			case ValueType::Literal: 
			{
				const auto &type=value.Literal.GetType();
				if(type==Types::Bool()) {
					return  std::string("b\"")+(value.Literal.GetValue<bool>() ? "1" : "0") +"\"";
				}
				else if(type==Types::String()) {
					return "s\""+value.Literal.GetValue<std::string>() + "\"";
				}
				else if(type==Types::Int()) {
					return "i\""+ String::From(value.Literal.GetValue<int>()) + "\"";
				}
				else if(type==Types::Float()) {
					return "f\""+ String::From(value.Literal.GetValue<float>()) + "\"";
				}
				else if(type==Types::Double()) {
					return "d\""+ String::From(value.Literal.GetValue<double>()) + "\"";
				}
				else if(type==Types::Char()) {
					return "c\""+ String::From(value.Literal.GetValue<char>()) + "\"";
				}
				else if(type==Types::Byte()) {
					return "n\""+ String::From(value.Literal.GetValue<Byte>()) + "\"";
				}
				else if(type==Types::Unsigned()) {
					return "u\""+ String::From(value.Literal.GetValue<unsigned>()) + "\"";
				}
				else {
					throw std::runtime_error("Unknown literal type");
				}
			}
			case ValueType::Temp:
				return ".\""+ String::From(value.Result) +"\"";
				
			case ValueType::Variable:
				return "$\""+value.Name+"\"";
				
			case ValueType::Identifier:
				return "?\""+value.Name+"\"";
				
			default:
				throw std::runtime_error("Unknown value type");
		}
	}
	
	std::string Disassemble(const Instruction *instruction) {
		if(instruction==nullptr) return "";
		
		const char *fnmark=nullptr;
		
		switch(instruction->Type) {
		case InstructionType::Assignment:
			if(instruction->Reference) 
				return std::string("$\"")+instruction->Name.Name+"\" : "+disassemblevalue(instruction->RHS);
			else
				return std::string("$\"")+instruction->Name.Name+"\" = "+disassemblevalue(instruction->RHS);
			
		case InstructionType::FunctionCall:
			fnmark="fn";
		case InstructionType::MemberFunctionCall:
			if(!fnmark) fnmark="fm";
		case InstructionType::MethodCall:
			if(!fnmark) fnmark="mn";
		case InstructionType::MemberMethodCall:
		{
			if(!fnmark) fnmark="mm";
			
			std::string ret;
			if(instruction->Store) {
				ret=std::string(".\"")+String::From(instruction->Store)+"\" = " + fnmark + disassemblevalue(instruction->Name);
			}
			else {
				ret=fnmark + disassemblevalue(instruction->Name);
			}
			
			for(auto &param : instruction->Parameters) {
				ret+=" "+disassemblevalue(param);
			}
			
			return ret;
		}
		
		case InstructionType::SaveToTemp:
			if(instruction->Reference) 
				return ".\""+String::From(instruction->Store)+"\" : " + disassemblevalue(instruction->RHS);
			else
				return ".\""+String::From(instruction->Store)+"\" = " + disassemblevalue(instruction->RHS);
			
		case InstructionType::RemoveTemp:
			return "x\""+String::From(instruction->Store)+"\"";
			
			
		case InstructionType::Jump:
			return "ja\"" + String::From(instruction->JumpOffset) + "\"";
			
		case InstructionType::JumpTrue:
			return "jt\"" + String::From(instruction->JumpOffset) + "\"" + disassemblevalue(instruction->RHS);
			
		case InstructionType::JumpFalse:
			return "jf\"" + String::From(instruction->JumpOffset) + "\"" + disassemblevalue(instruction->RHS);
			
		}
		
		return "";
	}

	void Disassemble(Scope &scope, std::ostream &out)  {
		unsigned long line=0;
		while(auto inst=scope.ReadInstruction(line++)) {
			out<<Disassemble(inst)<<std::endl;
		}
	}

} } }