#include "Parser.h"
#include "../Scripting.h"
#include <locale>
#include "../Enum.h"
#include <sstream>
#include <vector>
#include <string>
#include <memory>

#include "../Console.h"

namespace Gorgon { namespace Scripting {

using namespace internal;

namespace {

struct Token {
	enum Type {
		Float = 1, Int = 2, Bool = 3, String = 4,
		Identifier, Operator, LeftP, RightP, LeftSqrP,
		RightSqrP, LeftCrlyP, RightCrlyP, Seperator,
		Membership, Namespace, EoS, EqualSign, None,
	};

	Token(const std::string &repr = "", Type type = None): repr(repr), type(type) {}

	virtual std::string getrepr() const { return repr; }
	Type gettype() const { return type; }
	
	bool operator == (Type type) const { return type==this->type; }
	
	bool operator != (Type type) const { return type!=this->type; }

	std::string repr;
	Type type;
};

template<class ...P_>
int checkforinput(const std::string &input, int &ch, P_ ...args) {
	char allowed[] = {args...};
	int elements = sizeof...(args);

	auto errstr = [&] ()-> std::string {
		std::string ret = "Expected ";
		for(int i = 0; i < elements; i++) {
			if(i != 0) {
				ret.push_back('/');
			}
			ret.push_back(allowed[i]);
		}

		return ret;
	};

	if(input.length() <= ch) {
		throw ParseError({ParseError::UnexpectedToken, 0, ch, errstr() + ", end of string encountered."});
	}

	char c = input[ch++];

	for(int i = 0; i < sizeof...(args); i++) {
		if(allowed[i] == c) {
			return i;
		}
	}

	ch--;
	throw ParseError({ParseError::UnexpectedToken, 0, ch, errstr() + ", found: " + input.substr(ch, 1)});
}

std::string extractquotes(const std::string &input, int &ch) {
	std::string ret = "";

	int quotes = checkforinput(input, ch, '\'', '"') + 1;

	int start = ch;

	bool escape = false;
	int escapenum = 0;
	Gorgon::Byte num;
	for(; ch < input.size(); ch++) {
		char c = input[ch];
		if(escape) {
			if(c == '"') {
				ret.push_back('"');
			}
			else if(c == '\\') {
				ret.push_back('\\');
			}
			else if(c == 'n') {
				ret.push_back('\n');
			}
			else if(c >= '0' && c <= '9') {
				escapenum++;
				num = (num << 4) + (c - '0');
			}
			else if(c >= 'a' && c <= 'f') {
				escapenum++;
				num = (num << 4) + (c - 'a' + 10);
			}
			else {
				throw ParseError({ParseError::UnexpectedToken, 0, ch, "Invalid escape sequence: \\" + input.substr(ch, 1)});
			}

			if(escapenum != 1) {
				escape = false;
			}
			if(escapenum == 2) {
				ret.push_back((char)num);
				escapenum = 0;
			}
		}
		else {
			if(c == '\\') {
				escape = true;
				num = 0;
			}
			else if(c == '"' && quotes == 2) {
				break;
			}
			else if(c == '\'' && quotes == 1) {
				break;
			}
			else {
				ret.push_back(c);
			}
		}
	}

	checkforinput(input, ch, quotes == 2 ? '"' : '\'');

	return ret;
}

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
	if(op == ")") {
		return 8;
	}
	else if(op == "*" || op == "/" || op == "%") {
		return 7;
	}
	else if(op == "+" || op == "-") {
		return 6;
	}
	else if(op == "<" || op == "<=" || op == ">" || op == ">=") {
		return 5;
	}
	else if(op == "==" || op == "!=") {
		return 4;
	}
	else if(op == "&") {
		return 3;
	}
	else if(op == "|") {
		return 2;
	}
	else if(op == "(") {
		return 1;
	}
	else if(op == "=") {
		return 0;
	}
	else {
		throw ParseError{ParseError::UnexpectedToken, 0, '~', "Unexpected operator"};
	}
}

Token consumenexttoken(const std::string &input, int &index) {
	while(index < input.length() && isspace(input[index])) index++;

	if(index >= input.length()) return Token{";", Token::EoS};

	std::string acc = "";


	char c = input[index++];
	switch(c) {
		case '"' :
		case '\'':
			return Token{extractquotes(input, --index), Token::String};

		case '(': return Token{"(", Token::Operator};
		case ')': return Token{")", Token::Operator};
		case '[': return Token{"[", Token::LeftSqrP};
		case ']': return Token{"]", Token::RightSqrP};
		case '{': return Token{"{", Token::LeftCrlyP};
		case '}': return Token{"}", Token::RightCrlyP};
		case '.': return Token{".", Token::Membership};
		case ':': return Token{":", Token::Namespace};
		case ',': return Token{",", Token::Seperator};
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
	else if(c == '-') {
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
		throw ParseError{ParseError::UnexpectedToken, 0, c, "Invalid character"};
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
					throw ParseError{ParseError::UnexpectedToken, 0, c, "Unexpected character"};
				}
				acc.push_back(c);
				flt = true;
			}
			else if(isbreaker(c)) {
				return Token{acc, flt ? Token::Float : Token::Int};
			}
			else {
				throw ParseError{ParseError::UnexpectedToken, 0, c, "Unknown character"};
			}
		}
		else if(isdigit(c)) {
			if(opornumber) {
				numeric = true;
			}
			else if(oporequals) {
				return Token{acc, Token::EqualSign};
			}

			acc.push_back(c);
		}
		else if(op || opornumber || oporequals) {
			if(isoperator(c)) {
				acc.push_back(c);
			}
			else {
				return Token{acc, Token::Operator};
			}
			op = true;
			opornumber = false;
			oporequals = false;
		}
		else if(oporequals) {
			return Token{acc, Token::EqualSign};
		}
		else if(isalpha(c) || c == '_' || c == ':') {
			acc.push_back(c);
		}
		else if(isbreaker(c)) {

			if(acc == "true" || acc == "false") {
				return Token{acc, Token::Bool};
			}

			return Token{acc, /*var ? Token::Variable : */Token::Identifier};
		}
		else {
			throw ParseError{ParseError::UnexpectedToken, 0, c, "Unknown character"};
		}
	}
}

Token peeknexttoken(const std::string &input, int index) {
	return consumenexttoken(input, index);
}

DefineEnumStringsCM(Token, Type,
	{Token::Float, "Float"}, {Token::Int, "Int"}, {Token::Bool, "Bool"}, {Token::String, "String"},
	{Token::Identifier, "Identifier"}, {Token::Operator, "Operator"}, {Token::LeftP, "LeftP"},
	{Token::RightP, "RightP"},{Token::LeftSqrP, "LeftSqrP"},{Token::RightSqrP, "RightSqrP"},
	{Token::LeftCrlyP, "LeftCrlyP"}, {Token::RightCrlyP, "RightCrlyP"}, {Token::Seperator, "Seperator"}, 
	{Token::Membership, "Membership"}, {Token::Namespace, "Namespace"}, {Token::EoS, "EoS"}, {Token::None, "None"}
);

} // end of unnamed namespace

namespace internal {
	struct node {
		node() {}
		node(const Token data): data(data) {}

		Token data;
		std::vector<node*> leaves;
		
		int startlocation;

		~node() {
			for(std::size_t i = 0; i < leaves.size(); i++) {
				if(leaves[i]) delete leaves[i];
			}
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
			for(int i = 0; i < index; i++) { std::cout << "'"; }
			std::cout << std::endl << std::endl;

			i++;
		}
		if(cases) {
			(*cases) << "\t\tREQUIRE(tokens[" << i << "].type == " << String::From(token.type) << ");" << std::endl;
			(*cases) << "\t}" << std::endl << std::endl;
		}
	}

} // end of internal

node *parseexpression(const std::string &input, int &index) {
	Token token;
	
	int par=0;
	
	while(1) {
		token=consumenexttoken(input, index);
		
		
	}
}

node *ProgrammingParser::parse(const std::string &input) {
	int index = 0;
	node *root = new node;
	
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


}} // end of namespaces scripting and gorgon

