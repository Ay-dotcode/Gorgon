#include <string>
#include <fstream>

#include "AST.h"
#include "../VirtualMachine.h"
#include "../../Scripting.h"

namespace Gorgon { namespace Scripting { namespace Compilers {
	
	std::string dottree(ASTNode &tree, int &ind) {
		std::string type;
		std::string detail;
		std::string ret;
		
		switch(tree.Type) {
			case ASTNode::Assignment:
				type="Asgn";
				break;
			case ASTNode::Construct:
				type="Const";
				break;
			case ASTNode::Empty:
				type="Empt";
				break;
			case ASTNode::FunctionCall:
				type="Func";
				break;
			case ASTNode::MethodCall:
				type="Func";
				break;
			case ASTNode::Identifier:
				type="Iden";
				detail=tree.Text;
				break;
			case ASTNode::Index:
				type="Indx";
				break;
			case ASTNode::Keyword:
				type="Kywd";
				detail=tree.Text;
				break;
			case ASTNode::Literal:
				type="Ltrl";
				detail=tree.LiteralValue.ToString()+" ("+tree.LiteralValue.GetType().GetName()+")";
				break;
			case ASTNode::Member:
				type="Mmbr";
				break;
			case ASTNode::Operator:
				type="Oper";
				detail=tree.Text;
				break;
			default:
				type="Unknown";
				break;
		}
		
		int myid=ind;
		
		if(detail.length()) {
			detail=String::Replace(String::Replace(String::Replace(detail, "\"", "\\\""),"|", "\\|")," ","\\ ");
			ret=String::From(ind++)+"[shape=record, label=\"{"+type+"|"+detail+"}\"];\n";
		}
		else {
			ret=String::From(ind++)+"[shape=rectangle, label=\""+type+"\"];\n";
		}
		
		for(auto &chld : tree.Leaves) {
			ret+=String::From(myid)+"->"+String::From(ind)+";\n";
			
			ret+=dottree(chld, ind);			
		}
		
		return ret;
	}
	
	void ASTToSVG(const std::string &line, ASTNode &tree, const std::vector<std::string> &last, bool show) {
		std::string dot = "digraph { 0[shape=rectangle, label=\""+String::Replace(line, "\"", "\\\"")+"\"];\n";
		
		int n=1;
		dot+=dottree(tree, n);
		
		dot+="0 -> 1;\n";
		if(last.size()) {
			dot+="{rank=sink; last[shape=record, label=\"{";
		}
		int i=0;
		for(auto &str : last) {
			if(i++) {
				dot+="|";
			}
			dot+=String::Replace(String::Replace(String::Replace(String::Replace(String::Replace(str, "\"", "\\\""),"|", "\\|")," ","\\ "), "{", "\\{"),"}", "\\}");
		}
		if(last.size()) {
			dot+="}\"];}";
		}
		dot+="}";
		
		{
			std::ofstream temp("temp.dot");
			temp<<dot;
		}
		
		OS::Start("dot", {"temp.dot", "-Tsvg", "temp.dot", "-O"});
		if(show)
			OS::Open("temp.dot.svg");
	}
	
	void PrintAST(ASTNode &tree) {
		if(tree.Type==ASTNode::FunctionCall) {
			std::cout<<"( ";
		}
		
		if(tree.Type==ASTNode::Construct) {
			std::cout<<"{ ";
		}
		if(tree.Type==ASTNode::Index) {
			std::cout<<"[ ";
		}
		
		int i=0;
		for(auto &n : tree.Leaves) {
			
			if((tree.Type==ASTNode::FunctionCall || tree.Type==ASTNode::Construct || tree.Type==ASTNode::Index) && i++) {
				std::cout<<", ";
			}
			
			PrintAST(n);
		}
		
		if(tree.Type==ASTNode::FunctionCall) {
			std::cout<<")[CALL] ";
		}
		else if(tree.Type==ASTNode::Construct) {
			std::cout<<"} ";
		}
		else if(tree.Type==ASTNode::Index) {
			std::cout<<"] ";
		}
		else {
			std::cout<<tree.Text<<" ";
		}
	};
	
	//generate output works only with functioncalls, it can be used to suppress result saving
	Value compilevalue(ASTNode &tree, std::vector<Instruction> &list, Byte &tempind, bool generateoutput=true, std::vector<Instruction> *writeback=nullptr) {
		if(tree.Type==ASTNode::Operator) { //arithmetic
			ASSERT(tree.Leaves.GetSize()==2, "Operators require two operands");
			
			Instruction inst;
			inst.Type=InstructionType::MemberFunctionCall;
			inst.Name.Type=ValueType::Literal;
			inst.Name.Literal=Data(Types::String(), tree.Text);
			inst.Parameters.push_back(compilevalue(tree.Leaves[0], list, tempind));
			inst.Parameters.push_back(compilevalue(tree.Leaves[1], list, tempind));
			inst.Store=tempind;
			
			list.push_back(inst);
			
			Value v;
			v.Type=ValueType::Temp;
			v.Result=tempind++;
			
			return v;
		}
		
		//simple literal value, use as is
		if(tree.Type==ASTNode::Literal) {
			Value v;
			v.Type=ValueType::Literal;
			v.Literal=tree.LiteralValue;
			
			return v;
		}
		
		//identifier value, use as is
		if(tree.Type==ASTNode::Identifier) {
			Value v;
			v.Type=ValueType::Identifier;
			v.Name=tree.Text;
			
			return v;
		}
		
		//variable, use as is
		if(tree.Type==ASTNode::Variable) {
			Value v;
			v.Type=ValueType::Variable;
			v.Name=tree.Text;
			
			return v;
		}
		
		//constant, use as is
		if(tree.Type==ASTNode::Constant) {
			Value v;
			v.Type=ValueType::Constant;
			v.Name=tree.Text;
			
			return v;
		}
		
		//function call
		if(tree.Type==ASTNode::FunctionCall || tree.Type==ASTNode::MethodCall) {
			ASSERT(tree.Leaves.GetSize()>0, "Function name is missing");
			
			Instruction inst;
			//writebacks are the instructions necessary to write back composition nodes in an object.
			std::vector<Instruction> writebacks;
			
			//regular function call
			if(tree.Leaves[0].Type==ASTNode::Identifier) {
				inst.Name.Type=ValueType::Identifier;
				inst.Name.Name=tree.Leaves[0].Text;
				
				inst.Type=(tree.Type==ASTNode::MethodCall ? InstructionType::MethodCall : InstructionType::FunctionCall);
			}
			//member function call
			else if(tree.Leaves[0].Type==ASTNode::Member) {
				ASSERT(tree.Leaves[0].Leaves.GetCount()==2, "Membership nodes should have two children");
				
				//extract function name, it is the right most child in membership tree
				inst.Name.SetStringLiteral(tree.Leaves[0].Leaves[1].Text);
				
				//first parameter is the this pointer, compile that too. This enables cascading. Writeback is necessary
				//to save composition node objects after calling the function as the function might modify that object
				inst.Parameters.push_back(compilevalue(tree.Leaves[0].Leaves[0], list, tempind, true, &writebacks));
				
				inst.Type=(tree.Type==ASTNode::MethodCall ? InstructionType::MemberMethodCall : InstructionType::MemberFunctionCall);
			}
			
			//parameters
			for(int i=1;i<tree.Leaves.GetCount();i++) {
				inst.Parameters.push_back(compilevalue(tree.Leaves[i], list, tempind));
			}
			
			//depending on whether this should generate output
			inst.Store=generateoutput ? tempind : 0;
			
			//add function call
			list.push_back(inst);
			
			//add writebacks after the function call, so that execution will first retrieve
			//the object, call the function and write it back
			for(auto inst=writebacks.rbegin(); inst!=writebacks.rend(); ++inst) {
				list.push_back(*inst);
			}
			
			//prepare the value to be used. 
			Value v;
			v.Type=ValueType::Temp;
			v.Result=generateoutput ? tempind++ : 0;
			
			return v;
		}
		
		//generate construction code
		if(tree.Type==ASTNode::Construct) {
			ASSERT(tree.Leaves.GetSize()>0, "Object name is missing");
			
			Instruction inst;
			//construction is a member function of the type object
			inst.Type=InstructionType::MemberFunctionCall;
			inst.Name.SetStringLiteral("{}");
			
			//parameters
			for(int i=0;i<tree.Leaves.GetCount();i++) {
				inst.Parameters.push_back(compilevalue(tree.Leaves[i], list, tempind));
			}
			
			inst.Store=tempind;
			
			list.push_back(inst);
			
			Value v;
			v.Type=ValueType::Temp;
			v.Result=tempind++;
			
			return v;
		}

		//index access or array construction, a proper compiler should known types to generate
		//much better code. As we do know anything about whats going on we should generate
		//[] and []= code together if writebacks are requested. This node must generate optional writebacks.
		//!TODO add writeback
		if(tree.Type==ASTNode::Index) {
			ASSERT(tree.Leaves.GetSize()>0, "Object name is missing");
			
			Instruction inst;
			inst.Type=InstructionType::MemberFunctionCall;
			inst.Name.SetStringLiteral("[]");
			
			//parameters of index operator
			for(int i=0;i<tree.Leaves.GetCount();i++) {
				inst.Parameters.push_back(compilevalue(tree.Leaves[i], list, tempind));
			}
			
			inst.Store=tempind;
			
			list.push_back(inst);
			
			//if writeback is requested, instructions to check if the given item is an array like, if so
			//instructions to writeback the member should be used.
// 			if(writeback) {
// 				Instruction writebackinst;
// 				
// 				writebackinst.Type=InstructionType::MemberFunctionCall;
// 				writebackinst.Name="[]=";
// 				writebackinst.Parameters.push_back(accessval);
// 				writebackinst.Parameters.push_back(v);
// 				writebackinst.Store=0;
// 				
// 				writeback->push_back(writebackinst);
// 			}
			
			//return the result as a temporary
			Value v;
			v.Type=ValueType::Temp;
			v.Result=tempind++;
			
			return v;
		}
		//Membership nodes accesses composition nodes and writeback is required in some cases
		if(tree.Type==ASTNode::Member) { 
			ASSERT(tree.Leaves.GetCount()==2, "Membership nodes should have two children");
			
			Instruction inst;
			inst.Type=InstructionType::MemberFunctionCall;
			
			//Member access can only be performed with names
			inst.Name.SetStringLiteral("."+tree.Leaves[1].Text);
			
			//compile left tree, it may have more memberships or indexed access
			auto accessval=compilevalue(tree.Leaves[0], list, tempind, true, writeback);
			inst.Parameters.push_back(accessval);
			inst.Store=tempind;
			
			list.push_back(inst);
			
			Value v;
			v.Type=ValueType::Temp;
			v.Result=tempind++;
			
			//if requested prepare write back. Const markings and symbol table based compilation
			//will reduce the amount of writebacks
			if(writeback) {
				Instruction writebackinst;
				
				//same as access
				writebackinst.Type=InstructionType::MemberFunctionCall;
				writebackinst.Name=inst.Name;
				writebackinst.Parameters.push_back(accessval);
				writebackinst.Parameters.push_back(v);
				writebackinst.Store=0;
				
				writeback->push_back(writebackinst);
			}
			
			return v;
		}
		
		Utils::ASSERT_FALSE("Unknown AST Node");
	}
	
	unsigned ASTCompiler::Compile(ASTNode *tree) {
		auto sz=list.size();
		
		//temporaries start from 1
		Byte tempind=1;

		std::vector<Instruction> writebacks;
		
		//Assignment operation
		if(tree->Type==ASTNode::Assignment) {
			ASSERT(tree->Leaves.GetCount()==2, "Assignment requires two parameters");
			
			Instruction inst;
			inst.Type=InstructionType::Assignment;
			
			inst.RHS=compilevalue(tree->Leaves[1], list, tempind);
			
			if(tree->Leaves[0].Type==ASTNode::Identifier || tree->Leaves[0].Type==ASTNode::Variable) {
				inst.Name.Type=ValueType::Variable;
				inst.Name.Name=tree->Leaves[0].Text;
			}
			else if(tree->Leaves[0].Type==ASTNode::Member) {
				ASSERT(tree->Leaves[0].Leaves.GetCount()==2, "Membership nodes should have two children");
				
				inst.Name.SetStringLiteral("."+ tree->Leaves[0].Leaves[1].Text);
				inst.Store=0;
				inst.Parameters.push_back(compilevalue(tree->Leaves[0].Leaves[0], list, tempind, true, &writebacks));
				inst.Parameters.push_back(compilevalue(tree->Leaves[1], list, tempind));
				inst.Type=InstructionType::MemberFunctionCall;
			}
			else if(tree->Leaves[0].Type==ASTNode::Index) {
				ASSERT(tree->Leaves[0].Leaves.GetCount()==1, "Missing object");
				
				inst.Name.SetStringLiteral("[]=");
				inst.Store=0;
				inst.Parameters.push_back(compilevalue(tree->Leaves[0].Leaves[0], list, tempind, true, &writebacks));
				for(int i=1; i<tree->Leaves.GetSize(); i++) {
					inst.Parameters.push_back(compilevalue(tree->Leaves[i], list, tempind));
				}
				inst.Type=InstructionType::MemberFunctionCall;
			}
			else {
				ASSERT(false, "Assignment can only be performed to variable, membership and indexing nodes");
			}
			
			list.push_back(inst);
			
			//save any writebacks
			for(auto inst=writebacks.rbegin(); inst!=writebacks.rend(); ++inst) {
				list.push_back(*inst);
			}
		}
		
		//Simple function or method call, difer the compilation to compile value. Result of the function call
		//will be ignored
		else if(tree->Type==ASTNode::FunctionCall || tree->Type==ASTNode::MethodCall) {
			auto v=compilevalue(*tree, list, tempind, false);
		}
		
		//Keyword call
		else if(tree->Type==ASTNode::Keyword) {
			if(compilekeyword(tree, tempind)) {
				//nothing to do
			}
			else {
				Instruction  inst;
				
				//Call the keyword as a function
				inst.Type=InstructionType::FunctionCall;
				inst.Name.SetStringLiteral(tree->Text);
				
				//keywords do not have return values
				inst.Store=0;
				
				//parameters
				for(auto &p : tree->Leaves) {
					inst.Parameters.push_back(compilevalue(p, list, tempind));
				}
				list.push_back(inst);
			}
		}
		else {
			ASSERT(false, "Unknown top level AST Node");
		}
		
		//release used temporaries
		for(int i=1;i<tempind;i++) {
			Instruction inst;
			inst.Type=InstructionType::RemoveTemp;
			inst.Store=i;
			
			list.push_back(inst);
		}
		
		return list.size()-sz;
	}
	
	bool ASTCompiler::compilekeyword(ASTNode *tree, Byte &tempind) {
		//fully compile if keyword
		if(String::ToLower(tree->Text)=="if") {
			if(tree->Leaves.GetCount()==0) {
				throw MissingParameterException("condition", 0, "Bool", "Condition for If keyword is not specified");
			}
			else if(tree->Leaves.GetCount()>1) {
				throw TooManyParametersException(tree->Leaves.GetCount(), 1, "If keyword requires only a single condition");
			}
			
			Instruction jf;
			jf.Type=InstructionType::JumpFalse;
			jf.RHS=compilevalue(tree->Leaves[0], list, tempind);
			
			scopes.push_back(scope{scope::ifkeyword, (int)list.size()});
			
			list.push_back(jf);
			waitingcount++;
			
			return true;
		}
		else if(String::ToLower(tree->Text)=="else") {
			if(scopes.size()==0 || scopes.back().type!=scope::ifkeyword) {
				throw FlowException("Else without If");
			}
			if(scopes.back().elsepassed) {
				throw FlowException("Multiple Else for a single If");
			}
			
			Instruction ja;
			ja.Type=InstructionType::Jump;
			list.push_back(ja);
			
			list[scopes.back().indices.back()].JumpOffset=list.size()-scopes.back().indices.back();
			scopes.back().indices.back()=list.size()-1;
			scopes.back().elsepassed=true;
			
			return true;
		}
		else if(String::ToLower(tree->Text)=="elseif") {
			if(scopes.size()==0 || scopes.back().type!=scope::ifkeyword) {
				throw FlowException("ElseIf without If");
			}
			if(scopes.back().elsepassed) {
				throw FlowException("ElseIf statements must be before Else statement");
			}
			if(tree->Leaves.GetCount()==0) {
				throw MissingParameterException("condition", 0, "Bool", "Condition for ElseIf keyword is not specified");
			}
			else if(tree->Leaves.GetCount()>1) {
				throw TooManyParametersException(tree->Leaves.GetCount(), 1, "ElseIf keyword requires only a single condition");
			}
			
			Instruction ja;
			ja.Type=InstructionType::Jump;
			list.push_back(ja);
			
			list[scopes.back().indices.back()].JumpOffset=list.size()-scopes.back().indices.back();
			scopes.back().indices.back()=list.size()-1;
			
			Instruction jf;
			jf.Type=InstructionType::JumpFalse;
			jf.RHS=compilevalue(tree->Leaves[0], list, tempind);
			
			scopes.back().indices.push_back(list.size());
			list.push_back(jf);				
			
			return true;
		}
		else if(String::ToLower(tree->Text)=="while") {
			if(tree->Leaves.GetCount()==0) {
				throw MissingParameterException("condition", 0, "Bool", "Condition for While keyword is not specified");
			}
			else if(tree->Leaves.GetCount()>1) {
				throw TooManyParametersException(tree->Leaves.GetCount(), 1, "While keyword requires only a single condition");
			}
			
			scopes.push_back(scope{scope::whilekeyword, (int)list.size()});
			
			Instruction jf;
			jf.Type=InstructionType::JumpFalse;
			jf.RHS=compilevalue(tree->Leaves[0], list, tempind);
			
			scopes.back().indices.push_back((int)list.size());
			
			list.push_back(jf);
			waitingcount++;
			
			return true;
		}
		else if(String::ToLower(tree->Text)=="break") {
			scope *supported=nullptr;
			for(auto it=scopes.rbegin(); it!=scopes.rend(); ++it) {
				if(it->type==scope::whilekeyword) {
					supported=&*it;
					break;
				}
			}
			
			if(supported==nullptr) {
				throw FlowException("Break without a breakable keyword scope");
			}
			else if(tree->Leaves.GetCount()>0) {
				throw TooManyParametersException(tree->Leaves.GetCount(), 1, "Break keyword requires no parameters");
			}
			
			supported->indices.push_back((int)list.size());
			
			Instruction ja;
			ja.Type=InstructionType::Jump;
			list.push_back(ja);
			
			return true;
		}
		else if(String::ToLower(tree->Text)=="continue") {
			scope *supported=nullptr;
			for(auto it=scopes.rbegin(); it!=scopes.rend(); ++it) {
				if(it->type==scope::whilekeyword) {
					supported=&*it;
					break;
				}
			}
			
			if(supported==nullptr) {
				throw FlowException("Continue without a supported keyword scope");
			}
			else if(tree->Leaves.GetCount()>0) {
				throw TooManyParametersException(tree->Leaves.GetCount(), 1, "Continue keyword requires no parameters");
			}
			
			supported->indices2.push_back((int)list.size());
			
			Instruction ja;
			ja.Type=InstructionType::Jump;
			list.push_back(ja);
			
			return true;
		}
		else if(String::ToLower(tree->Text)=="const") {
			ASSERT(tree->Leaves.GetSize()==1, "const keyword requires a single parameter");
			ASSERT(tree->Leaves[0].Leaves.GetSize()==2, "Assignment requires two parameters");
			ASSERT(tree->Leaves[0].Leaves[0].Type==ASTNode::Identifier || 
				   tree->Leaves[0].Leaves[0].Type==ASTNode::Variable,
				   "const can only be applied to simple variables"
			);
			
			Compile(&tree->Leaves[0]);
			
			Instruction inst;
			inst.Type=InstructionType::FunctionCall;
			inst.Name.SetStringLiteral("const");
			Value v;
			v.SetVariable(tree->Leaves[0].Leaves[0].Text);
			inst.Parameters.push_back(v);
			list.push_back(inst);
			
			return true;
		}
		else if(String::ToLower(tree->Text)=="static") {
			ASSERT(tree->Leaves.GetSize()==1, "static keyword requires a single parameter");
			ASSERT(tree->Leaves[0].Leaves.GetSize()==2, "Assignment requires two parameters");
			ASSERT(tree->Leaves[0].Leaves[0].Type==ASTNode::Identifier || 
				   tree->Leaves[0].Leaves[0].Type==ASTNode::Variable,
				   "static can only be applied to simple variables"
			);
			
			Instruction inst;
			inst.Type=InstructionType::FunctionCall;
			inst.Name.SetStringLiteral("static");
			inst.Store=0;
			Value v;
			v.SetVariable(tree->Leaves[0].Leaves[0].Text);
			inst.Parameters.push_back(v);
			inst.Parameters.push_back(compilevalue(tree->Leaves[0].Leaves[1], list, tempind));
			list.push_back(inst);
			
			return true;
		}
		else if(String::ToLower(tree->Text)=="end") {
			if(scopes.size()==0) {
				throw FlowException("`End` without a keyword scope");
			}
			
			if(tree->Leaves.GetCount()==1) {
				if(String::ToLower(tree->Leaves[0].Text)!=scope::keywordnames[scopes.back().type]) {
					throw FlowException("`End` does not match with the correct keyword", 
										"Current scope is "+scope::keywordnames[scopes.back().type]+
										" while given keyword for end is "+tree->Leaves[0].Text);
				}
			}
			else if(tree->Leaves.GetCount()!=0) {
				throw TooManyParametersException(tree->Leaves.GetCount(), 1, "`End` keyword has only one optional parameter.");
			}
			
			switch(scopes.back().type) {
				case scope::ifkeyword:
					for(auto &index : scopes.back().indices) {
						list[index].JumpOffset=list.size()-index;
					}
					break;
				
				case scope::whilekeyword:					
					auto start=scopes.back().indices.front();
					
					Instruction ja;
					ja.Type=InstructionType::Jump;
					ja.JumpOffset=start-list.size();
					list.push_back(ja);
					
					auto elm=scopes.back().indices.size();
					for(unsigned i=1; i<elm; i++) {
						auto index=scopes.back().indices[i];
						list[index].JumpOffset=list.size()-index;
					}
					for(auto &index : scopes.back().indices2) {
						list[index].JumpOffset=start-index;
					}
					
					break;
			}
			
			scopes.pop_back();
			waitingcount--;
			
			return true;
		}
		
		return false;
	}
	
	const std::string ASTCompiler::scope::keywordnames[] = {"none", "if", "while"};
	
} } }
