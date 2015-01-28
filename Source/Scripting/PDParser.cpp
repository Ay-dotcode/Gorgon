#include "Parser.h"
#include "ParserUtils.h"
#include "../Scripting.h"
#include <locale>
#include "../Enum.h"
#include <sstream>
#include <vector>
#include <string>
#include <memory>
#include <fstream>

#include "../Console.h"
#include <../../../../../../../Program Files (x86)/Microsoft Visual Studio 11.0/VC/include/locale>

///@cond INTERNAL

namespace Gorgon { namespace Scripting {

	namespace {

		struct Token {
			enum Type {
				Int,
				Float,
				Bool,
				String,
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
				ASSERT(isliteral(), "This token does not represent a literal");
				
				switch(type) {
					case Int:
						return Data(Types::Int(), String::To<int>(repr));
					case Float:
						return Data(Types::Float(), String::To<float>(repr));
					case Bool:
						return Data(Types::Bool(), String::To<bool>(repr));
					case String:
						return Data(Types::String(), repr);
					default:
						throw ParseError{ParseError::UnexpectedToken, "Invalid literal type."};
				}
			}
			
			bool isliteral() {
				switch(type) {
					case Int:
					case Float:
					case Bool:
					case String:
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
		
		int getprecedence(const std::string &op) {
			if(op=="") {
				throw ParseError{ParseError::UnexpectedToken, "Invalid operator."};
			}
			
			if(op.length()==1) {
				switch(op[0]) {
				case '^':
					return 4;
					
				case '*':
				case '/':
					return 6;
					
				case '+':
				case '-':
					return 9;
					
				case '<':
				case '>':
				case '=':
					return 12;
					
				case '&':
					return 13;
					
				case '|':
					return 14;
					
				default:
					throw ParseError{ParseError::UnexpectedToken, "Invalid operator.", op.front()};
				}
			}
			else if(op.length()==2) {
				if(op[1]=='=') {
					switch(op[0]) {
					case '>':
					case '<':
					case '!':
					case '=':
						return 10;
						
					default:
						throw ParseError{ParseError::UnexpectedToken, "Invalid operator.", op.front()};
					}
				}
			}
				
			if(op=="<<" || op==">>" || op=="bitleft" || op=="bitright") {
				return 5;
			}
			else if(op=="bitand" || op=="bitor" || op=="bitxor") {
				return 7;
			}
			else if(op=="bitset" || op=="contains" || op=="in" || op=="notin" || op=="union" || op=="intersect") {
				return 10;
			}
			else if(op=="mod") {
				return 6;
			}
			else if(op=="and") {
				return 13;
			}
			else if(op=="or") {
				return 14;
			}
			
			throw ParseError({ParseError::UnexpectedToken, "Invalid operator."});
		}

		Token consumenexttoken(const std::string &input, int &index, bool expectop=false) {
			while(index < input.length() && isspace(input[index])) index++;
			
			int start=index;

			if(index >= input.length()) return Token {";", Token::EoS, start};

			std::string acc = "";

			
			char c = input[index++];
			switch(c) {
			case '"' :
			case '\'':
				return Token {ExtractQuotes(input, --index), Token::String, start};

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
				throw ParseError{ParseError::UnexpectedToken, "Invalid character.", c, index};
			}

			for(; index<input.length() + 1; index++) {
				char c;
				if(input.length()>index)
					c = input[index];
				else
					c = 0;

				if(numeric) {
					if(isdigit(c)) {
						acc.push_back(c);
					}
					else if(c == '.') {
						if(flt) {
							throw ParseError{ParseError::UnexpectedToken, "Unexpected character", c, index};
						}
						acc.push_back(c);
						flt = true;
					}
					else if(isbreaker(c)) {
						return Token {acc, flt ? Token::Float : Token::Int, start};
					}
					else {
						throw ParseError{ParseError::UnexpectedToken, "Unknown character.", c, index};
					}
				}
				else if(isdigit(c) && !op) {
					if(opornumber) {
						numeric = true;
					}
					else if(oporequals) {
						return Token {acc, Token::EqualSign, start};
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
					throw ParseError{ParseError::UnexpectedToken, "Unknown character.", c, index};
				}
			}
			
			throw ParseError{ParseError::UnexpectedToken, "Invalid token."};
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

		struct node {
			enum Type {
				Literal,
				FunctionCall,
				MethodCall,
				Member,
				Identifier,
				Operator,
				Term,
				Index,
				Construct,
				Keyword,
				Assignment,
				Empty
			} type;
			
			//node() : data("", Token::None, 0) {}
			node(Type type, const Token data): data(data), type(type) {}
			
			node *duplicate() {
				node *newnode=new node(type, data);
				newnode->leaves=leaves.Duplicate();
				
				return newnode;
			}

			Token data;
			Containers::Collection<node> leaves;

			~node() {
				leaves.Destroy();
			}

		};

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
				Gorgon::Console::SetColor(Gorgon::Console::Yellow);
				Gorgon::Console::SetBold();
				std::cout << String::From(token.type) << ": ";
				Gorgon::Console::SetBold(false);
				std::cout << token.repr << std::endl;
				std::cout << input << std::endl;
				for(int i = 0; i < index; i++) {
					std::cout << "'";
				}
				std::cout << std::endl << std::endl;

				i++;
			}
			if(cases) {
				(*cases) << "\t\tREQUIRE(tokens[" << i << "].type == " << String::From(token.type) << ");" << std::endl;
				(*cases) << "\t}" << std::endl << std::endl;
			}
		}
	}
	
	node *parseexpression(const std::string &input, int &index);
	
	Containers::Collection<node> parseexpressions(const std::string &input, int &index) {
		
		Token token = peeknexttoken(input, index);
		Containers::Collection<node> ret;
		
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
	
	node *parseterm(const std::string &input, int &index) {
		Token token=consumenexttoken(input, index);
		
		// first token should be identifier
		if(token.type == Token::Identifier) {
			; //ok go on
		}
		else if(token==Token::LeftCrlyP) { //or lone object constructor
			auto nw = new node(node::Construct, token);
			
			auto exprs=parseexpressions(input, index);
			
			token=consumenexttoken(input, index);
			
			if(token!=Token::RightCrlyP) {
				delete nw;
				throw ParseError{ParseError::UnexpectedToken, "Expected expression list."};
			}
			
			nw->leaves.Push(new node(node::Empty,{}));
			for(auto &expr : exprs)
				nw->leaves.Push(expr);
			
			return nw;
		}
		else {
			throw ParseError{ParseError::UnexpectedToken, "Expected identifier."};
		}
		
		
		
		//start with first
		auto root=new node(node::Identifier, token);
		
		while(1) {
			token=consumenexttoken(input, index);

			//membership has a single following identifier
			if(token==Token::Membership) {
				auto nw = new node(node::Member, token);
				token=consumenexttoken(input, index);
				
				if(token!=Token::Identifier) {
					delete root;
					delete nw;
					
					throw ParseError{ParseError::UnexpectedToken, "Expected identifier."};
				}
				
				nw->leaves.Push(root);
				nw->leaves.Push(new node(node::Identifier, token));
				root=nw;
			}
			else if(token==Token::LeftCrlyP) {
				auto nw = new node(node::Construct, token);
				
				auto exprs=parseexpressions(input, index);
				
				token=consumenexttoken(input, index);
				
				if(token!=Token::RightCrlyP) {
					delete root;
					delete nw;
					throw ParseError{ParseError::UnexpectedToken, "Expected expression list."};
				}
				
				nw->leaves.Push(root);
				for(auto &expr : exprs)
					nw->leaves.Push(expr);
				
				root=nw;
			}
			else if(token==Token::LeftSqrP) { //[] contains an expression
				auto nw = new node(node::Index, token);
				
				auto exprs=parseexpressions(input, index);
				
				token=consumenexttoken(input, index);
				
				if(token!=Token::RightSqrP) {
					delete root;
					delete nw;
					throw ParseError{ParseError::UnexpectedToken, "Expected expression list."};
				}
				
				nw->leaves.Push(root);
				for(auto &expr : exprs)
					nw->leaves.Push(expr);
				
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
	
	std::string dottree(node &tree, int &ind) {
		std::string type;
		std::string detail;
		std::string ret;
		
		switch(tree.type) {
		case node::Assignment:
			type="Asgn";
			break;
		case node::Construct:
			type="Const";
			break;
		case node::Empty:
			type="Empt";
			break;
		case node::FunctionCall:
			type="Func";
			break;
		case node::MethodCall:
			type="Func";
			break;
		case node::Identifier:
			type="Iden";
			detail=tree.data.repr;
			break;
		case node::Index:
			type="Indx";
			break;
		case node::Keyword:
			type="Kywd";
			detail=tree.data.repr;
			break;
		case node::Literal:
			type="Ltrl";
			detail=tree.data.repr+" ("+String::From(tree.data.type)+")";
			break;
		case node::Member:
			type="Mmbr";
			break;
		case node::Operator:
			type="Oper";
			detail=tree.data.repr;
			break;
		case node::Term:
			type="Term";
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
		
		for(auto &chld : tree.leaves) {
			ret+=String::From(myid)+"->"+String::From(ind)+";\n";
			
			ret+=dottree(chld, ind);			
		}
		
		return ret;
	}
	
	void dottree(const std::string &line, node &tree, const std::vector<std::string> &last) {
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
		OS::Open("temp.dot.svg");
	}
	
	void printtree(node &tree) {
		if(tree.type==node::FunctionCall) {
			std::cout<<"( ";
		}
		
		if(tree.type==node::Construct) {
			std::cout<<"{ ";
		}
		if(tree.type==node::Index) {
			std::cout<<"[ ";
		}

		int i=0;
		for(auto &n : tree.leaves) {
			
			if((tree.type==node::FunctionCall || tree.type==node::Construct || tree.type==node::Index) && i++) {
				std::cout<<", ";
			}

			printtree(n);
		}
		
		if(tree.type==node::FunctionCall) {
			std::cout<<")[CALL] ";
		}
		else if(tree.type==node::Construct) {
			std::cout<<"} ";
		}
		else if(tree.type==node::Index) {
			std::cout<<"] ";
		}
		else {
			std::cout<<tree.data.repr<<" ";
		}
	};

	node *parseexpression(const std::string &input, int &index) {
		Token token;

		int par=0;
		bool nextisop=false;
		
		struct opnode {
			node *op;
			int precedence;
		};
		
		std::vector<opnode> opstack;
		Containers::Collection<node> outputstack;
		
		
		auto printtrees = [&]() {
			for(auto &t : outputstack) {
				printtree(t);
				std::cout<<std::endl;
			}
			
			for(auto &op : opstack) {
				std::cout<<op.op->data.repr<<" ";
			}
			
			std::cout<<std::endl;
			std::cout<<std::string('*', 20);
			std::cout<<std::endl;
		};
		
		//pops an operator from the opstack and joins last two elements in the
		//output stack using it
		auto popoff = [&] {
			if(outputstack.GetSize()<2) {
				throw ParseError{ParseError::UnexpectedToken, "Missing operand."};
			}
			node *second  = &outputstack.Pop();
			node *first = &outputstack.Pop();
			
			node *op=opstack.back().op;
			opstack.pop_back();
			
			op->leaves.Add(first);
			op->leaves.Add(second);
			
			outputstack.Push(op);
		};
		
		int parcount=0;
		int infunction=0;

		//this algorithm terminates on the first token that cannot be processed
		//and will not throw if there is no error up to that point.
		while(true) {
			token=consumenexttoken(input, index, nextisop);
			
			//printtrees();
			
			//if an operator is expected
			if(nextisop) {
				//next will not be an operator unless some specific case occurs
				nextisop=false;

				//token is an operator (can be identified as identifier as well)
				if(token==Token::Operator || token==Token::Identifier) {
					int precedence=0;
					if(token==Token::Identifier) {
						try {
							precedence=getprecedence(token.repr);
						}
						catch(...) {
							index=token.start;
							break;
						}
					}
					else {
						//find precedence, this also validates the operator
						precedence=getprecedence(token.repr);
					}
					
					//create the node
					token.type=Token::Operator;					
					node *op=new node(node::Operator, token);
					
					//process higher precedence operators
					while(opstack.size() && opstack.back().precedence <= precedence) {
						popoff();
					}
					
					//add to the op stack
					opstack.push_back({op, precedence});
				}
				else if(token==Token::Seperator && infunction) { //comma
					//process until to the start of the function call. if there are previous
					//parameters, they would have already been processed.
					while(opstack.size() && opstack.back().op->data!=Token::LeftP) {
						popoff();
					}
					if(opstack.size()==0) throw ParseError{ParseError::UnexpectedToken, "Unexpected token.", ','};
				}
				else if(token==Token::RightP && parcount) { //if no parenthesis is expected, just exit.
					//pop until the start of the parenthesis
					while(opstack.back().op->data!=Token::LeftP) {
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
						Containers::Collection<node> params;
						
						//if there are multiple function calls, the ones that are processed will have their token type
						//set to none
						while(outputstack.Last()->type!=node::FunctionCall || outputstack.Last()->data.type!=Token::EoS) {
							params.Push(&outputstack.Pop());
						}
						
						outputstack.Last()->data.type=Token::None;
						
						//place parameters into the function call
						for(auto it=params.Last();it.IsValid();--it) {
							outputstack.Last()->leaves.Push(*it);
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
					outputstack.Push(new node(node::Literal, token));
				}
				else if(token==Token::Identifier || token==Token::LeftCrlyP) { //either variable/constant or a function call
					index=token.start; //roll back so parseterm could do the parsing
					auto term = parseterm(input, index);
// 					auto nw = new node(node::Term, {"", Token::None, token.start});
// 					nw->leaves.Push(term);
// 					term=nw;
					
					if(peeknexttoken(input, index)==Token::LeftP) { //function call
						infunction++;
						//function call node
						auto fn=new node(node::FunctionCall, {"", Token::EoS, token.start}); //EoS marks active function call
						fn->leaves.Push(term);
						outputstack.Push(fn);
						
						nextisop=false;
					}
					else { //variable/constant
						outputstack.Push(term);
					}
				}
				else if(token==Token::LeftP) {
					opstack.push_back({new node(node::Operator, token), 25});
					parcount++;
					nextisop=false;
				}
				else {
					throw ParseError{ParseError::UnexpectedToken, "Unexpected token."};
				}
			}
		}

		if(parcount) throw ParseError{ParseError::MismatchedParenthesis, "Mismatched parentheses."};
		
		while(opstack.size()) {
			popoff();
		}
		
		if(outputstack.GetSize()!=1) throw ParseError{ParseError::UnexpectedToken, "Invalid expression."};
		
		//printtree(outputstack[0]);
		
		return &outputstack[0];
	}
	
	void fixconstructors(node *tree, node *parent=nullptr) {
		if(tree->type==node::Construct) {
			if(tree->leaves[0].type==node::Empty) {
				if(parent->type!=node::Index) {
					throw ParseError{ParseError::UnexpectedToken, "Unexpected empty constructor."};
				}
				
				tree->leaves.Delete(*tree->leaves.begin());
				tree->leaves.Insert(parent->leaves[0].duplicate(),0);
			}
		}
		
		for(auto &node : tree->leaves) {
			fixconstructors(&node, tree);
		}
	}
	
	node *parse(const std::string &input) {
		int index = 0;
		node *root = nullptr;

		Token token=consumenexttoken(input, index);

		if(token == Token::EoS) return nullptr;

		if(token!=Token::Identifier) {
			throw ParseError{ParseError::UnexpectedToken, "Expected identifier."};
		}
		
		if(KeywordNames.count(token.repr)) { //keyword
			root=new node(node::Keyword, token);
			
			token=peeknexttoken(input, index);
			
			while(token!=Token::EoS) {
				auto expr=parseexpression(input, index);
				root->leaves.Push(expr);
				
				token=peeknexttoken(input, index);
			}
		}
		else {
			index=0; //parse from start
			auto term=parseterm(input, index);
			
			token=consumenexttoken(input, index);
			
			if(token==Token::EqualSign) {
				auto expr=parseexpression(input, index);
				
				if(token.repr.size()!=1) {
					ASSERT(token.repr.size()==2, "Assignment operator size problem.");
					
					//check if this really is an operator
					getprecedence(token.repr.substr(0,1));
					
					node *compound=new node(node::Operator, {token.repr.substr(0,1), Token::Operator, token.start});
					
					token.repr=token.repr.substr(1);
					
					compound->leaves.Push(term->duplicate());
					compound->leaves.Push(expr);
					expr=compound;
				}
				
				root=new node(node::Assignment, token);
				root->leaves.Push(term);
				root->leaves.Push(expr);
				
				if( (token=consumenexttoken(input, index)) !=Token::EoS) {
					throw ParseError{ParseError::UnexpectedToken, "Expected End of String."};
				}
			}
			else if(token==Token::LeftP) { //function
				index=0; //let expression do the parsing
				root=parseexpression(input, index);
				
				if( (token=consumenexttoken(input, index)) !=Token::EoS) {
					throw ParseError{ParseError::UnexpectedToken, "Expected End of String."};
				}
			}
			else {
				throw ParseError{ParseError::UnexpectedToken, "Invalid expression."};
			}
		}
		
		fixconstructors(root);
		
		printtree(*root);
		std::cout<<std::endl;
		
		
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
						throw ParseError{ParseError::MismatchedParenthesis, 
									     "`" + std::string(1, parens.back().type) + "` at character " +
											String::From(pchar + 1) + " " + String::From(cline - pline) + " lines above " +
											" is not closed.",
										 cchar, -cline
						};
					}
					else {
						throw ParseError{ParseError::MismatchedParenthesis,
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
					
					throw ParseError{ParseError::UnexpectedToken, "`"+std::string(1, c)+"` is unexpected",
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
	
	//generate output works only with functioncalls, it can be used to suppress result saving
	Value compilevalue(node &tree, std::vector<Instruction> &list, Byte &tempind, bool generateoutput=true) {
		if(tree.type==node::Operator) { //arithmetic
			ASSERT(tree.leaves.GetSize()==2, "Operators require two operands");
			
			Instruction inst;
			inst.Type=InstructionType::FunctionCall;
			inst.Name.Type=ValueType::Literal;
			inst.Name.Literal=Data(Types::String(), tree.data.repr);
			inst.Parameters.push_back(compilevalue(tree.leaves[0], list, tempind));
			inst.Parameters.push_back(compilevalue(tree.leaves[1], list, tempind));
			inst.Store=tempind;
			
			list.push_back(inst);
			
			Value v;
			v.Type=ValueType::Temp;
			v.Result=tempind++;
			
			return v;
		}
		
		if(tree.type==node::Literal) {
			Value v;
			v.Type=ValueType::Literal;
			v.Literal=tree.data.GetLiteralValue();
			
			return v;
		}
		
		if(tree.type==node::Identifier) {
			Value v;
			v.Type=ValueType::Identifier;
			v.Name=tree.data.repr;
			
			return v;
		}
		
		if(tree.type==node::FunctionCall || tree.type==node::MethodCall) {
			ASSERT(tree.leaves.GetSize()>0, "Function name is missing");
			
			Instruction inst;
			inst.Type=(tree.type==node::MethodCall ? InstructionType::MethodCall : InstructionType::FunctionCall);
			inst.Name.Type=ValueType::Literal;
			inst.Name.Literal=Data(Types::String(), tree.leaves[0].data.repr);
			
			for(int i=1;i<tree.leaves.GetCount();i++) {
				inst.Parameters.push_back(compilevalue(tree.leaves[i], list, tempind));
			}
			
			inst.Store=generateoutput ? tempind : 0;
			
			list.push_back(inst);
			
			Value v;
			v.Type=ValueType::Temp;
			v.Result=generateoutput ? tempind++ : 0;
			
			return v;
		}
		
		if(tree.type==node::Construct) {
			ASSERT(tree.leaves.GetSize()>0, "Object name is missing");
			
			Instruction inst;
			inst.Type=InstructionType::FunctionCall;
			inst.Name.Type=ValueType::Literal;
			inst.Name.Literal=Data(Types::String(), std::string("{}"));
			
			for(int i=0;i<tree.leaves.GetCount();i++) {
				inst.Parameters.push_back(compilevalue(tree.leaves[i], list, tempind));
			}
			
			inst.Store=tempind;
			
			list.push_back(inst);
			
			Value v;
			v.Type=ValueType::Temp;
			v.Result=tempind++;
			
			return v;
		}
		
		if(tree.type==node::Index) {
			ASSERT(tree.leaves.GetSize()>0, "Object name is missing");
			
			Instruction inst;
			inst.Type=InstructionType::FunctionCall;
			inst.Name.Type=ValueType::Literal;
			inst.Name.Literal=Data(Types::String(), std::string("[]"));
			
			for(int i=0;i<tree.leaves.GetCount();i++) {
				inst.Parameters.push_back(compilevalue(tree.leaves[i], list, tempind));
			}
			
			inst.Store=tempind;
			
			list.push_back(inst);
			
			Value v;
			v.Type=ValueType::Temp;
			v.Result=tempind++;
			
			return v;
		}
		
		Utils::NotImplemented();
	}
	
	//only adds to the list
	unsigned compile(node *tree, std::vector<Instruction> &list) {
		auto sz=list.size();
		
		Byte tempind=1;
		if(tree->type==node::Assignment) {
			Instruction inst;
			inst.Type=InstructionType::Assignment;
			if(tree->leaves[0].type==node::Identifier) {
				inst.Name.Type=ValueType::Variable;
				inst.Name.Name=tree->leaves[0].data.repr;
			}
			else if(tree->leaves[0].type==node::Member) {
				//tree->leaves[0].leaves[0]
			}
			
			inst.RHS=compilevalue(tree->leaves[1], list, tempind);
			
			list.push_back(inst);
		}
		else if(tree->type==node::FunctionCall || tree->type==node::MethodCall) {
			auto v=compilevalue(*tree, list, tempind, false);
		}
		else if(tree->type==node::Keyword) {
			Instruction inst1, inst2;
			inst1.Type=InstructionType::Mark;
			inst1.Name.Name=tree->data.repr;
			list.push_back(inst1);
			
			inst2.Type=InstructionType::FunctionCall;
			inst2.Name.Type=ValueType::Literal;
			inst2.Name.Literal=Data(Types::String(), tree->data.repr);
			inst2.Store=0;
			for(auto &p : tree->leaves) {
				inst2.Parameters.push_back(compilevalue(p, list, tempind));
			}
			list.push_back(inst2);
		}
		
		return list.size()-sz;
	}
	
	unsigned ProgrammingParser::Parse(const std::string &input) {
		//If necessary split the line or request more input
		if(left.size()) {
			left.push_back('\n');
		}
		linestarts.push_back(left.size());

		left.append(input);
		
		int elements=0;
		
		while(left.length()) { //parse until we run out of data
			std::string process;
			
			extractline(left, process, linestarts);
			
			auto ret=parse(process);
			
			if(ret) {
				try {
					elements+=compile(ret, List);
				}
				catch(...) {
					delete ret;
				}
			}
			else {
				break;
			}

			std::vector<std::string> strlines;
			for(auto it=List.end()-elements;it!=List.end();++it) {
				strlines.push_back(Disassemble(&(*it)));
			}
			
			dottree(input, *ret, strlines);
			
		}
		
		return elements;
	}
	
	void ProgrammingParser::Finalize() {
		left.push_back(';');
		Parse("");
	}

} }
//@endcond