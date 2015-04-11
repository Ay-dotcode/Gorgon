#include <locale>
#include <sstream>
#include <vector>
#include <string>
#include <memory>
#include <fstream>

#include "../../Enum.h"
#include "../../Scripting.h"
#include "../Compilers.h"
#include "AST.h"
#include "Language.h"
#include "Utils.h"

#ifndef _NDEBUG
#	include "../../Console.h"
#endif

#pragma warning(disable:4018)


///@cond INTERNAL

namespace Gorgon { namespace Scripting { namespace Compilers {
	bool showsvg__=false;
	namespace {

		struct Token {
			enum Type {
				Int,
				Unsigned,
				Float,
				Double,
				Char,
				Byte,
				String,
				Bool,
				Identifier,
				Operator,
				LeftP,
				RightP,
				LeftSqrP,
				RightSqrP,
				LeftCrlyP,
				RightCrlyP,
				Seperator,
				Membership,
				Namespace,
				EoS,
				EqualSign,
				None,
			};

			Token(const std::string &repr, Type type, int start): repr(repr), type(type), start(start) {}
			
			Token() : type(None), start(-1) { }


			bool operator == (Type type) const {
				return type==this->type;
			}

			bool operator != (Type type) const {
				return type!=this->type;
			}
			
			const Data GetLiteralValue() {
				switch(type) {
					case Int:
						return Data(Types::Int(), String::To<int>(repr));
					case Unsigned:
						return Data(Types::Unsigned(), String::To<unsigned>(repr));
					case Float:
						return Data(Types::Float(), String::To<float>(repr));
					case Double:
						return Data(Types::Double(), String::To<double>(repr));
					case Char:
						return Data(Types::Char(), String::To<char>(repr));
					case Byte:
						return Data(Types::Byte(), String::To<Gorgon::Byte>(repr));
					case String:
						return Data(Types::String(), repr);
					case Bool:
						if(repr=="true")
							return Data(Types::Bool(), true);
						else 
							return Data(Types::Bool(), String::To<bool>(repr));
					default:
						Utils::ASSERT_FALSE("This token does not represent a literal");
				}
			}
			
			bool isliteral() {
				switch(type) {
					case Int:
					case Unsigned:
					case Float:
					case Double:
					case Bool:
					case String:
					case Char:
					case Byte:
						return true;
					default:
						return false;
				}
			}

			std::string repr;
			Type type;
			int start;
		};

		bool isbreaker(char c) {
			
			if(isspace(c) || c == 0) return true;

			switch(c) {
			case '@':
			case '#':
			case '+':
			case '-':
			case '*':
			case '/':
			case '<':
			case '^':
			case '>':
			case '&':
			case '|':
			case '=':
			case '!':
			case '\\':
			case '?':
			case '%':
			case '(':
			case ')':
			case '[':
			case ']':
			case '{':
			case '}':
			case '.':
			case '$':
			case ',':
			case '\n':
				return true;
			default:
				return false;
			}
		}
		
		bool isoperator(char c) {
			switch(c) {
				case '+':
				case '-':
				case '*':
				case '/':
				case '^':
				case '<':
				case '>':
				case '&':
				case '|':
				case '=':
				case '!':
				case '\\':
					return true;
				default:
					return false;
			}
		}
		
		Token consumenexttoken(const std::string &input, int &index, bool expectop=false) {
			while(index < input.length() && isspace(input[index])) index++;
			
			int start=index;

			if(index >= input.length()) return Token {";", Token::EoS, start};

			std::string acc = "";

			
			char c = input[index++];
			switch(c) {
			case '"' :
			case '\'': {
				auto s=ExtractQuotes(input, --index);
				std::string literalmarker;
				while(input.length()>index && !isbreaker(input[index])) {
					literalmarker.push_back(input[index++]);
				}
				if(literalmarker=="" || literalmarker=="s") {
					return Token {s, Token::String, start};
				}
				else if(literalmarker=="c") {
					if(s.length()!=1) {
						throw ParseError(ExceptionType::InvalidLiteral, "Literal 'c' is only valid for single character", index);
					}
					return Token {String::From(s[0]), Token::Char, start};
				}
				else {
					throw ParseError(ExceptionType::InvalidLiteral, "Unknown string literal: "+literalmarker, index);
				}
			}
			case '(':
				return Token {"(", Token::LeftP, start};
			case ')':
				return Token {")", Token::RightP, start};
			case '[':
				return Token {"[", Token::LeftSqrP, start};
			case ']':
				return Token {"]", Token::RightSqrP, start};
			case '{':
				return Token {"{", Token::LeftCrlyP, start};
			case '}':
				return Token {"}", Token::RightCrlyP, start};
			case '.':
				return Token {".", Token::Membership, start};
			case ':':
				return Token {":", Token::Namespace, start};
			case ',':
				return Token {",", Token::Seperator, start};
			}

			bool opornumber = false;
			bool oporequals=false;
			bool numeric = false;
			bool flt = false; //float
			bool op = false;
			bool var = false;

			acc.push_back(c);
			if(isdigit(c)) {
				numeric = true;
			}
			else if(c == '-' && !expectop) {
				opornumber = true;
			}
			else if(c== '=') {
				oporequals=true;
			}
			else if(isoperator(c)) {
				op = true;
			}
			else if(!isalpha(c)) {
				throw ParseError{ExceptionType::UnexpectedToken, "Invalid character.", index};
			}

			bool literalpart=false;
			std::string literal;
			for(; index<input.length() + 1; index++) {
				char c;
				if(input.length()>index)
					c = input[index];
				else
					c = 0;

				if(numeric) {
					if(isalpha(c) || literalpart) {
						if(isbreaker(c)) {
							if(literal=="i") {
								return Token {acc, Token::Int, start};
							}
							else if(literal=="u") {
								return Token {acc, Token::Unsigned, start};
							}
							else if(literal=="f") {
								return Token {acc, Token::Float, start};
							}
							else if(literal=="d") {
								return Token {acc, Token::Double, start};
							}
							else if(literal=="c") {
								return Token {acc, Token::Char, start};
							}
							else if(literal=="n") {
								return Token {acc, Token::Byte, start};
							}
							else if(literal=="b") {
								return Token {acc, Token::Char, start};
							}
							else if(literal=="s") {
								return Token {acc, Token::String, start};
							}
							else {
								throw ParseError(ExceptionType::InvalidLiteral, "Unknown numeric literal: "+literal, index);
							}
						}
						
						literal+=c;
						literalpart=true;
					}
					else if(isdigit(c)) {
						acc.push_back(c);
					}
					else if(c == '.') {
						if(flt) {
							throw ParseError{ExceptionType::UnexpectedToken, "Unexpected character", index};
						}
						acc.push_back(c);
						flt = true;
					}
					else if(isbreaker(c)) {
						return Token {acc, flt ? Token::Float : Token::Int, start};
					}
					else {
						throw ParseError{ExceptionType::UnexpectedToken, "Unknown character.", index};
					}
				}
				else if(isdigit(c) && !op) {
					if(opornumber) {
						numeric = true;
					}
					else if(oporequals) {
						return Token {acc, !expectop ? Token::EqualSign : Token::Operator, start};
					}

					acc.push_back(c);
				}
				else if(op || opornumber || oporequals) {
					if(isoperator(c)) {
						acc.push_back(c);
					}
					else {
						return Token {acc, 
							(
								oporequals || 
								(acc.back()=='=' && acc.size()==2 && acc.front()!='>' && acc.front()!='!' && acc.front()!='=' && acc.front()!='<')
							) && 
							!expectop
							? Token::EqualSign : Token::Operator, start};
					}
					op = true;
					opornumber = false;
					oporequals = false;
				}
				else if(oporequals) {
					return Token {acc, Token::EqualSign, start};
				}
				else if(isalpha(c) || c == '_' || c == ':') {
					acc.push_back(c);
				}
				else if(isbreaker(c)) {

					if(acc == "true" || acc == "false") {
						return Token {acc, Token::Bool, start};
					}

					return Token {acc, /*var ? Token::Variable : */Token::Identifier, start};
				}
				else {
					throw ParseError{ExceptionType::UnexpectedToken, "Unknown character.", index};
				}
			}
			
			throw ParseError{ExceptionType::UnexpectedToken, "Invalid token.", index};
		}

		Token peeknexttoken(const std::string &input, int index) {
			return consumenexttoken(input, index);
		}

		// For easier debugging
		DefineEnumStringsCM(Token, Type,
			{Token::Float, "Float"}, {Token::Int, "Int"}, {Token::Bool, "Bool"}, {Token::String, "String"},
			{Token::Identifier, "Identifier"}, {Token::Operator, "Operator"}, {Token::LeftP, "LeftP"},
			{Token::RightP, "RightP"},{Token::LeftSqrP, "LeftSqrP"},{Token::RightSqrP, "RightSqrP"},
			{Token::LeftCrlyP, "LeftCrlyP"}, {Token::RightCrlyP, "RightCrlyP"}, {Token::Seperator, "Seperator"},
			{Token::Membership, "Membership"}, {Token::Namespace, "Namespace"}, {Token::EoS, "EoS"}, {Token::None, "None"}
		);
		
		void testlexer(const std::string &input, std::ostream *cases) {
			int index = 0;
			int i = 0;
			Token token;
			if(cases) {
				(*cases) << "\t{" << std::endl
				<< "\t\tauto tokens=parser.parse(\"" << input << "\");" << std::endl << std::endl;
			}
			while((token = consumenexttoken(input, index)).type != Token::EoS) {
				if(cases) {
					(*cases) << "\t\tREQUIRE(tokens[" << i << "].type == "    << String::From(token.type)<< ");" << std::endl;
					(*cases) << "\t\tREQUIRE(tokens[" << i << "].value == \"" << String::Replace(token.repr, "\"", "\\\"") << "\");" << std::endl << std::endl;
				}
				
#ifndef _NDEBUG
				Gorgon::Console::SetColor(Gorgon::Console::Yellow);
				Gorgon::Console::SetBold();
				std::cout << String::From(token.type) << ": ";
				Gorgon::Console::SetBold(false);
				std::cout << token.repr << std::endl;
				std::cout << input << std::endl;
				for(int j = 0; j < index; j++) {
					std::cout << "'";
				}
				std::cout << std::endl << std::endl;
#endif
				
				i++;
			}
			if(cases) {
				(*cases) << "\t\tREQUIRE(tokens[" << i << "].type == " << String::From(token.type) << ");" << std::endl;
				(*cases) << "\t}" << std::endl << std::endl;
			}
		}
	}	
	
	ASTNode *NewNode(ASTNode::NodeType type, Token token) {
		ASTNode *node = new ASTNode(type);
		
		node->Start=token.start;
		
		if(token.isliteral())
			node->LiteralValue=token.GetLiteralValue();
		else
			node->Text=token.repr;
		
		return node;
	}
	
	ASTNode *parseexpression(const std::string &input, int &index);
	
	
	//Parses a list of expressions separated by comma
	Containers::Collection<ASTNode> parseexpressions(const std::string &input, int &index) {
		
		Token token = peeknexttoken(input, index);
		Containers::Collection<ASTNode> ret;
		
		if(token==Token::RightP || token==Token::RightCrlyP || token==Token::RightSqrP) {
			return ret;
		}
		
		while(true) {
			ret.Push(parseexpression(input, index));
			
			token=consumenexttoken(input, index);
			
			if(token!=Token::Seperator) {
				index=token.start;
				
				return ret;
			}
		}
	}
	
	//Parses a single term
	ASTNode *parseterm(const std::string &input, int &index) {
		Token token=consumenexttoken(input, index);
		
		// first token should be identifier
		if(token.type == Token::Identifier) {
			; //ok go on
		}
		else if(token==Token::LeftCrlyP) { //or lone object constructor
			auto nw = NewNode(ASTNode::Construct, token);
			
			auto exprs=parseexpressions(input, index);
			
			token=consumenexttoken(input, index);
			
			if(token!=Token::RightCrlyP) {
				delete nw;
				throw ParseError{ExceptionType::UnexpectedToken, "Expected expression list.", index};
			}
			
			nw->Leaves.Push(NewNode(ASTNode::Empty,{}));
			for(auto &expr : exprs)
				nw->Leaves.Push(expr);
			
			return nw;
		}
		else {
			throw ParseError{ExceptionType::UnexpectedToken, "Expected identifier.", index};
		}
		
		
		
		//start with first
		auto root=NewNode(ASTNode::Identifier, token);
		
		while(1) {
			token=consumenexttoken(input, index);

			//membership has a single following identifier
			if(token==Token::Membership) {
				auto nw = NewNode(ASTNode::Member, token);
				token=consumenexttoken(input, index);
				
				if(token!=Token::Identifier) {
					delete root;
					delete nw;
					
					throw ParseError{ExceptionType::UnexpectedToken, "Expected identifier.", index};
				}
				
				nw->Leaves.Push(root);
				nw->Leaves.Push(NewNode(ASTNode::Identifier, token));
				root=nw;
			}
			else if(token==Token::LeftCrlyP) { //constructor
				auto nw = NewNode(ASTNode::Construct, token);
				
				auto exprs=parseexpressions(input, index);
				
				token=consumenexttoken(input, index);
				
				if(token!=Token::RightCrlyP) {
					delete root;
					delete nw;
					throw ParseError{ExceptionType::UnexpectedToken, "Expected expression list.", index};
				}
				
				nw->Leaves.Push(root);
				for(auto &expr : exprs)
					nw->Leaves.Push(expr);
				
				root=nw;
			}
			else if(token==Token::LeftSqrP) { //[] contains an expression
				auto nw = NewNode(ASTNode::Index, token);
				
				auto exprs=parseexpressions(input, index);
				
				token=consumenexttoken(input, index);
				
				if(token!=Token::RightSqrP) {
					delete root;
					delete nw;
					throw ParseError{ExceptionType::UnexpectedToken, "Expected expression list.", index};
				}
				
				nw->Leaves.Push(root);
				for(auto &expr : exprs)
					nw->Leaves.Push(expr);
				
				root=nw;
			}
			else {
				//anything unknown will be rolled back and we will exit
				index=token.start;
				
				break;
			}
		}
		
		return root;
	}

	ASTNode *parseexpression(const std::string &input, int &index) {
		Token token;

		int par=0;
		bool nextisop=false;
		
		struct opnode {
			ASTNode *op;
			Token data;
			int precedence;
		};
		
		std::vector<opnode> opstack;
		Containers::Collection<ASTNode> outputstack;
		
		
		auto printtrees = [&]() {
			for(auto &t : outputstack) {
				PrintAST(t);
				std::cout<<std::endl;
			}
			
			for(auto &op : opstack) {
				std::cout<<op.op->Text<<" ";
			}
			
			std::cout<<std::endl;
			std::cout<<std::string('*', 20);
			std::cout<<std::endl;
		};
		
		//pops an operator from the opstack and joins last two elements in the
		//output stack using it
		auto popoff = [&] {
			if(outputstack.GetSize()<2) {
				throw ParseError{ExceptionType::UnexpectedToken, "Missing operand.", index};
			}
			ASTNode *second  = &outputstack.Pop();
			ASTNode *first = &outputstack.Pop();
			
			ASTNode *op=opstack.back().op;
			opstack.pop_back();
			
			op->Leaves.Add(first);
			op->Leaves.Add(second);
			
			outputstack.Push(op);
		};
		
		int parcount=0;
		int infunction=0;

		//this algorithm terminates on the first token that cannot be processed
		//and will not throw if there is no error up to that point.
		while(true) {
			token=consumenexttoken(input, index, nextisop);
			
			//printtrees();
			
		reevalute:
			//if an operator is expected
			if(nextisop) {
				//next will not be an operator unless some specific case occurs
				nextisop=false;

				//token is an operator (can be identified as identifier as well)
				if(token==Token::Operator || token==Token::Identifier) {
					int precedence=0;
					if(token==Token::Identifier) {
						try {
							precedence=GetPrecedence(token.repr);
						}
						catch(...) {
							index=token.start;
							break;
						}
					}
					else {
						//find precedence, this also validates the operator
						precedence=GetPrecedence(token.repr);
					}
					
					//create the ASTNode
					token.type=Token::Operator;					
					ASTNode *op=NewNode(ASTNode::Operator, token);
					
					//process higher precedence operators
					while(opstack.size() && opstack.back().precedence <= precedence) {
						popoff();
					}
					
					//add to the op stack
					opstack.push_back({op, token, precedence});
				}
				else if(token==Token::Seperator && infunction) { //comma
					//process until to the start of the function call. if there are previous
					//parameters, they would have already been processed.
					while(opstack.size() && opstack.back().data!=Token::LeftP) {
						popoff();
					}
					if(opstack.size()==0) throw ParseError{ExceptionType::UnexpectedToken, "Unexpected token.", ',', index};
				}
				else if(token==Token::RightP && parcount) { //if no parenthesis is expected, just exit.
					//pop until the start of the parenthesis
					while(opstack.back().data!=Token::LeftP) {
						popoff();
					}
					
					//get rid of the starting parenthesis as it is not required.
					auto n=opstack.back();
					opstack.pop_back();
					delete n.op;
					
					//if these parenthesis are a part of a function call
					if(infunction) {
						infunction--;
						//collect parameters
						Containers::Collection<ASTNode> params;
						
						//if there are multiple function calls, the ones that are processed will have their token type
						//set to none
						while(outputstack.Last()->Type!=ASTNode::FunctionCall || outputstack.Last()->Text!="!!!!!") {
							params.Push(&outputstack.Pop());
						}
						
						outputstack.Last()->Text="";
						
						//place parameters into the function call
						for(auto it=params.Last();it.IsValid();--it) {
							outputstack.Last()->Leaves.Push(*it);
						}
					}
					
					parcount--;
					nextisop=true;
				}
				else { //unknown token, just break from the loop, this could be a possible termination token
					index=token.start;
					break;
				}
			}
			else { //expecting an operand
				//next will be an operator unless there is specific situation
				nextisop=true;

				//literals are used as is
				if(token.isliteral()) {
					outputstack.Push(NewNode(ASTNode::Literal, token));
				}
				else if(token==Token::Identifier || token==Token::LeftCrlyP) { //either variable/constant or a function call
					index=token.start; //roll back so parseterm could do the parsing
					auto term = parseterm(input, index);
// 					auto nw = NewNode(ASTNode::Term, {"", Token::None, token.start});
// 					nw->leaves.Push(term);
// 					term=nw;
					
					if(peeknexttoken(input, index)==Token::LeftP) { //function call
						infunction++;
						//function call ASTNode
						//!ugly
						auto fn=NewNode(ASTNode::FunctionCall, {"!!!!!", Token::EoS, token.start}); //EoS marks active function call
						fn->Leaves.Push(term);
						outputstack.Push(fn);
						
						nextisop=false;
					}
					else { //variable/constant
						outputstack.Push(term);
					}
				}
				else if(token==Token::LeftP) {
					opstack.push_back({NewNode(ASTNode::Operator, token), token, 25});
					parcount++;
					nextisop=false;
				}
				else if(token==Token::RightP) {
					goto reevalute;
				}
				else {
					throw ParseError{ExceptionType::UnexpectedToken, "Unexpected token.", index};
				}
			}
		}

		if(parcount) throw ParseError{ExceptionType::MismatchedParenthesis, "Mismatched parentheses.", index};
		
		while(opstack.size()) {
			popoff();
		}
		
		if(outputstack.GetSize()!=1) throw ParseError{ExceptionType::UnexpectedToken, "Invalid expression.", index};
		
		//printtree(outputstack[0]);
		
		return &outputstack[0];
	}
	
	void fixconstructors(ASTNode *tree, ASTNode *parent=nullptr) {
		if(tree->Type==ASTNode::Construct) {
			if(tree->Leaves[0].Type==ASTNode::Empty) {
				if(!parent || parent->Type!=ASTNode::Index) {
					throw std::runtime_error("Unexpected empty constructor.");
				}
				
				tree->Leaves.Delete(*tree->Leaves.begin());
				tree->Leaves.Insert(parent->Leaves[0].Duplicate(),0);
			}
		}
		
		for(auto &ASTNode : tree->Leaves) {
			fixconstructors(&ASTNode, tree);
		}
	}
	
	static const std::set<std::string, String::CaseInsensitiveLess> internalkeywords={
		"if", "for", "elseif", "else", "while", "continue", "break", "end", "static"
	};
	
	ASTNode *parse(const std::string &input) {
		int index = 0;
		ASTNode *root = nullptr;

		try {
			Token token=consumenexttoken(input, index);

			auto assignment = [&](ASTNode *term, std::string keyword) {
				if(!term) {
					term=parseterm(input, index);
					
					token=consumenexttoken(input, index);
					
					if(token!=Token::EqualSign) {					
						throw ParseError{ExceptionType::UnexpectedToken, keyword+" should be followed by an assignment.", index};
					}
				}
				
				auto expr=parseexpression(input, index);
				
				if(token.repr.size()!=1) {
					ASSERT(token.repr.size()==2, "Assignment operator size problem.");
					
					//check if this really is an operator
					GetPrecedence(token.repr.substr(0,1));
					
					ASTNode *compound=NewNode(ASTNode::Operator, {token.repr.substr(0,1), Token::Operator, token.start});
					
					token.repr=token.repr.substr(1);
					
					compound->Leaves.Push(term->Duplicate());
					compound->Leaves.Push(expr);
					expr=compound;
				}
				
				root=NewNode(ASTNode::Assignment, token);
				root->Leaves.Push(term);
				root->Leaves.Push(expr);
				
				if( (token=consumenexttoken(input, index)) !=Token::EoS) {
					throw ParseError{ExceptionType::UnexpectedToken, "Expected End of String.", index};
				}
			};
			
			if(token == Token::EoS) return nullptr;

			if(token!=Token::Identifier) {
				throw ParseError{ExceptionType::UnexpectedToken, "Expected identifier, found: "+token.repr, index};
			}
			
			if(KeywordNames.count(token.repr) || internalkeywords.count(token.repr)) { //keyword
				root=NewNode(ASTNode::Keyword, token);
				
				if(String::ToLower(token.repr)=="for") {
					auto tokenvar=consumenexttoken(input, index);
					if(tokenvar!=Token::Identifier) {
						throw ParseError{ExceptionType::UnexpectedToken, "Expected identifier, found: "+token.repr, index};
					}
					root->Leaves.Push(NewNode(ASTNode::Variable, tokenvar));
					
					auto tokenin=consumenexttoken(input, index);					
					if(String::ToLower(tokenin.repr)!="in") {
						throw ParseError{ExceptionType::UnexpectedToken, "Expected `in`, found: "+tokenin.repr, index};
					}
					
					auto expr=parseexpression(input, index);
					root->Leaves.Push(expr);
				}
				else if(String::ToLower(token.repr)=="const" || String::ToLower(token.repr)=="static") { //this is actually an assignment
					auto name=String::ToLower(token.repr);
					assignment(nullptr, name);
					auto proot=root;
					root=new ASTNode(ASTNode::Keyword);
					root->Text=name;
					root->Leaves.Push(proot);
				}
				else {
					token=peeknexttoken(input, index);
					
					while(token!=Token::EoS) {
						auto expr=parseexpression(input, index);
						root->Leaves.Push(expr);
						
						token=peeknexttoken(input, index);
					}
				}
			}
			else {
				index=0; //parse from start
				auto term=parseterm(input, index);
				
				token=consumenexttoken(input, index);
				
				if(token==Token::EqualSign) {//assignment
					assignment(term, "");
				}
				else if(token==Token::LeftP) { //function
					index=0; //let expression do the parsing
					root=parseexpression(input, index);
					
					if( (token=consumenexttoken(input, index)) !=Token::EoS) {
						throw ParseError{ExceptionType::UnexpectedToken, "Expected End of String.", index};
					}
				}
				else {
					throw ParseError{ExceptionType::UnexpectedToken, "Invalid expression.", index};
				}
			}
			
			fixconstructors(root);
		}
		catch(...) {
			delete root;
			throw;
		}
		
		//PrintAST(*root);
		//std::cout<<std::endl;
		
		
		return root;
	}

	//extracts a line from input string
	void extractline(std::string &input, std::string &prepared, std::vector<unsigned> &linestarts) {
		int inquotes=0, escape=0;
		struct parenthesis {
			unsigned long location;
			char type;
		};
		std::vector<parenthesis> parens;
		
		unsigned len=input.length();
		int cutfrom=-1;
		int clearafter=-1;

		
		auto transform = [&](int ch, int &oline, int &och) {
			for(int line=0;line<linestarts.size();line++) {
				if(linestarts[line]>ch) {
					oline=line-1;
					och=ch-linestarts[line-1];
					
					return;
				}
			}
			
			oline=linestarts.size()-1;
			och=ch-linestarts.back();
		};
		
		for(unsigned i=0;i<len;i++) {
			char c=input[i];
			
			if(inquotes) {
				if(escape==1) {
					if(isdigit(c)) {
						escape=2;
					}
					else {
						escape=0;
					}
				}
				else if(escape==2) {
					escape=0;
				}
				else if(c=='\\') {
					escape=1;
				}
				else if(inquotes==1 && c=='\'') {
					inquotes=0;
				}
				else if(inquotes==2 && c=='"') {
					inquotes=0;
				}
			}
			else if(c=='\'') {
				inquotes=1;
			}
			else if(c=='"') {
				inquotes=2;
			}
			else if(c=='#') { //simply skip to the end of the line
				clearafter=i;
				break;
			}
			else if(c==';') { // the line will end
				int cline, pline, cchar, pchar;
				
				if(parens.size()) {
					transform(i, cline, cchar);
					transform(parens.back().location, pline, pchar);
					
					input="";
					if(cline!=pline) {
						throw ParseError{ExceptionType::MismatchedParenthesis, 
							"`" + std::string(1, parens.back().type) + "` at character " +
							String::From(pchar + 1) + " " + String::From(cline - pline) + " lines above " +
							" is not closed.",
							cchar, -cline
						};
					}
					else {
						throw ParseError{ExceptionType::MismatchedParenthesis,
							"`" + std::string(1, parens.back().type) + "` at character " +
								String::From(pchar + 1) +
								" is not closed.",
							cchar, -cline
										
						};
					}
				}
				
				if(c==';') {
					cutfrom=i;
				}
				break;
			}
			else if(c=='(' || c=='[' || c=='{') {
				parens.push_back({i, c});
			}
			else if(c==')' || c==']' || c=='}') {
				bool error=
					parens.size()==0 ||
					(c==')' && parens.back().type!='(') ||
					(c==']' && parens.back().type!='[') ||
					(c=='}' && parens.back().type!='{')	;
				
				if(error) {
					int cline, cchar;
					transform(i, cline, cchar);
					
					throw ParseError{ExceptionType::UnexpectedToken, "`"+std::string(1, c)+"` is unexpected",
						cchar, -cline		
					};
				}
				else {
					parens.pop_back();
				}
			}
		}
		
		if(cutfrom==-1) {
			if(clearafter!=-1) {
				input=input.substr(0, clearafter);
			}
			
			//everything is fine, line ends at the very end
			if(inquotes==0 && parens.size()==0) {
				using std::swap;
				swap(prepared, input);
				linestarts.clear();
			}
			else {
				return;
			}
		}
		else {
			prepared=input.substr(0, cutfrom);
			input=input.substr(cutfrom+1);
			
			auto last=linestarts.back();
			linestarts.clear();
			linestarts.push_back(cutfrom-last);
		}
	}
	
	unsigned Programming::Compile(const std::string &input, unsigned long pline) {
		//If necessary split the line or request more input
		if(left.size()) {
			left.push_back('\n');
		}
		linestarts.push_back(left.size());

		left.append(input);
		
		int elements=waiting;
		waiting=0;
		
 		while(left.length()) { //parse until we run out of data
			std::string process;
			
			extractline(left, process, linestarts);
			
			auto ret=parse(process);
			//ASTToSVG(input, *ret, {}, true);
			///... fix line and char start of ast tree
			
			if(ret) {
				try {
					elements+=compiler.Compile(ret);
				}
				catch(...) {
					delete ret;
				}
			}
			else {
				break;
			}
			
			if(showsvg__) {
				std::vector<std::string> strlines;
				for(auto it=List.end()-elements;it!=List.end();++it) {
					strlines.push_back(Disassemble(&(*it)));
				}
				
				ASTToSVG(input, *ret, strlines, true);
			}
			
		}
		
		if(!compiler.IsReady()) {
			waiting=elements;
			elements=0;
		}
		
		return elements;
	}
	
	void Programming::Finalize() {
		left.push_back(';');
		Compile("", -1);
	}

} } }
//@endcond