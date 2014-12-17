#include "Parser.h"
#include "ParserUtils.h"
#include "../Scripting.h"
#include <locale>
#include "../Enum.h"
#include <sstream>
#include <vector>
#include <string>
#include <memory>

#include "../Console.h"

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

			virtual std::string getrepr() const {
				return repr;
			}
			Type gettype() const {
				return type;
			}

			bool operator == (Type type) const {
				return type==this->type;
			}

			bool operator != (Type type) const {
				return type!=this->type;
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
				throw "Invalid operator";
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
				case '|':
					return 13;
					
				default:
					throw "Invalid operator";
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
						throw "Invalid operator";
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
			
			throw "Invalid operator";
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
			else if(isoperator(c)) {
				op = true;
			}
			else if(c == '$') {
				var = true;
				acc.clear();
			}
			else if(c== '=') {
				oporequals=true;
			}
			else if(!isalpha(c)) {
				throw ParseError {ParseError::UnexpectedToken, 0, index, "Invalid character"};
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
							throw ParseError {ParseError::UnexpectedToken, 0, index, "Unexpected character"};
						}
						acc.push_back(c);
						flt = true;
					}
					else if(isbreaker(c)) {
						return Token {acc, flt ? Token::Float : Token::Int, start};
					}
					else {
						throw ParseError {ParseError::UnexpectedToken, 0, index, "Unknown character"};
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
						return Token {acc, Token::Operator, start};
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
					throw ParseError {ParseError::UnexpectedToken, 0, index, "Unknown character"};
				}
			}
			
			throw "Invalid token";
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
				Member,
				Identifier,
				Operator
			} type;
			
			//node() : data("", Token::None, 0) {}
			node(Type type, const Token data): data(data), type(type) {}

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
		
		std::function<void(node&)> printtree = [&](node &tree) {
			for(auto &n : tree.leaves) {
				printtree(n);
			}
			
			if(tree.type==node::FunctionCall) {
				std::cout<<"[CALL] ";
			}
			else {
				std::cout<<tree.data.repr<<" ";
			}
		};
		
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
		
		auto popoff = [&] {
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

		while(true) {
			token=consumenexttoken(input, index, true);
			
			printtrees();
			
			if(nextisop) {
				nextisop=false;

				if(token==Token::Operator || token==Token::Identifier) {
					int precedence=getprecedence(token.repr);
					
					token.type=Token::Operator;
					
					node *op=new node(node::Operator, token);
					
					while(opstack.size() && opstack.back().precedence < precedence) {
						popoff();
					}
					
					opstack.push_back({op, precedence});
				}
				else if(token==Token::Seperator) {
					while(opstack.back().op->data!=Token::LeftP) {
						popoff();
					}
				}
				else if(token==Token::RightP && parcount) {
					while(opstack.back().op->data!=Token::LeftP) {
						popoff();
					}
					
					auto n=opstack.back();
					opstack.pop_back();
					delete n.op;
					
					if(infunction--) {
						Containers::Collection<node> params;
						
						while(outputstack.Last()->type!=node::FunctionCall || outputstack.Last()->data.type!=Token::EoS) {
							params.Push(&outputstack.Pop());
						}
						
						outputstack.Last()->data.type=Token::None;
						for(auto &p : params) {
							outputstack.Last()->leaves.Push(p);
						}
					}
					
					parcount--;
					nextisop=true;
				}
				else {
					index=token.start;
					break;
				}
			}
			else {
				nextisop=true;

				if(token.isliteral()) {
					outputstack.Push(new node(node::Literal, token));
				}
				else if(token==Token::Identifier) { //either variable/constant or a function call
					if(peeknexttoken(input, index)==Token::LeftP) { //function call
						infunction++;
						//parcount++;
						auto fn=new node(node::FunctionCall, {"", Token::EoS, token.start});
						fn->leaves.Push(new node(node::Identifier, token));
						outputstack.Push(fn);

						//opstack.push_back({new node(node::Operator, token), 25});
						
						nextisop=false;
					}
					else {
						outputstack.Push(new node(node::Identifier, token));
					}
				}
				else if(token==Token::LeftP) {
					opstack.push_back({new node(node::Operator, token), 25});
					parcount++;
					nextisop=false;
				}
				else {
					throw "Unexpected token";
				}
			}
		}
		
		while(opstack.size()) {
			popoff();
		}
		
		if(outputstack.GetSize()!=1) throw "Invalid expression";
		
		printtree(outputstack[0]);
		
		return &outputstack[0];
	}
	
	void ProgrammingParser::parseexpr(const std::string &input) {
		int index=0;
		parseexpression(input, index);
	}

	node *parse(const std::string &input) {
		int index = 0;
		node *root = nullptr;

		Token token=consumenexttoken(input, index);

		if(token == Token::EoS) return nullptr;

		if(token.type != Token::Identifier) {
			throw "Should be identifier";
		}

		bool canbekeyword=true;
		while(1) {
			token=consumenexttoken(input, index);

			if(token==Token::Membership) {
				//cont
				token=consumenexttoken(input, index);
				if(token!=Token::Identifier) {
					throw "Should be identifier";
				}
				canbekeyword=false;
			}
			else if(token==Token::LeftSqrP) {
				//cont
				//parseexpression(input, index);
				token=consumenexttoken(input, index);
				if(token!=Token::RightSqrP) {
					throw "should be expression";
				}
				canbekeyword=false;
			}
			else if(token==Token::EqualSign) {
				//assignment
				//parseexpression(input, index);
				token=consumenexttoken(input, index);
				if(token != Token::EoS) {
					throw "string should end";
				}

				break;
			}
			else if(token==Token::LeftP) {
				//exprarry=parseexpressionarray(input, index);
				token=consumenexttoken(input, index);
				if(token!=Token::RightP) {
					throw "should be expression";
				}

				token=peeknexttoken(input, index);
				if(token != Token::EoS) {
					if(!canbekeyword) {
						throw "unexpected something";
					}

					//if(exprarry.leaves.size()>1) throw "hata"

					while(token!=Token::EoS) {
						//parseexpression(input, index);
						token=peeknexttoken(input, index);
					}
				}
			}
			else {
				if(!canbekeyword) {
					throw "unexpected something";
				}

				token=peeknexttoken(input, index);
				while(token!=Token::EoS) {
					//parseexpression(input, index);
					token=peeknexttoken(input, index);
				}
			}
		}


		return root;
	}


} }

