#include "Parser.h"
#include "../Scripting.h"
#include <locale>
#include "../Enum.h"
#include "../Console.h"

namespace Gorgon { namespace Scripting {

using namespace internal;

namespace {

enum TokenType {
	EndOfString,
	Identifier,
	IntLiteral,
	StringLiteral,
	FloatLiteral,
	BooleanLiteral,
	VariableIdentifier,
	Operator,
	LPar, RPar,
	LSq, RSq,
	LCur, RCur,
	Comma,
	Dot,
	NSOp,
	EqualSign,
	None,
};

DefineEnumStrings(TokenType,
	{EndOfString, "EndOfString"}, {Identifier, "Identifier"}, {IntLiteral, "IntLiteral"},
	{StringLiteral, "StringLiteral"}, {FloatLiteral, "FloatLiteral"}, {BooleanLiteral, "BooleanLiteral"},
	{VariableIdentifier, "VariableIdentifier"}, {Operator, "Operator"}, {LPar, "LPar"},
	{RPar, "RPar"}, {LSq, "LSq"}, {RSq, "RSq"}, {LCur, "LCur"}, {RCur, "RCur"}, {Comma, "Comma"},
	{Dot, "Dot"}, {NSOp, "NSOp"}, {EqualSign, "EqualSign"}, {None, "None"}
);

struct Token {
	Token(TokenType type = None, std::string value = ""): Type(type), Value(value) {
	}
	TokenType Type;
	std::string Value;
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


Token getnexttoken(const std::string &input, int &index) {
	while(index < input.length() && isspace(input[index])) index++;

	if(index >= input.length()) return EndOfString;

	std::string acc = "";


	char c = input[index++];
	switch(c) {
	case '"':
	case '\'':
		return Token(StringLiteral, extractquotes(input, --index));
	case '(': return LPar;
	case ')': return RPar;
	case '[': return LSq;
	case ']': return RSq;
	case '{': return LCur;
	case '}': return RCur;
	case '=': return EqualSign;
	case '.': return Dot;
	case ':': return NSOp;
	case ',': return Comma;
	}

	bool opornumber = false;
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
	else if(!isalpha(c)) {
		throw ParseError({ParseError::UnexpectedToken, 0, c, "Invalid char" + input.substr(c, 1)});
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
					ParseError({ParseError::UnexpectedToken, 0, c, "Unexpected character" + input.substr(c, 1)});
				}
				acc.push_back(c);
				flt = true;
			}
			else if(isbreaker(c)) {
				return{flt ? FloatLiteral : IntLiteral, acc};
			}
			else {
				ParseError({ParseError::UnexpectedToken, 0, c, "Unknown character" + input.substr(c, 1)});
			}
		}
		else if(isdigit(c)) {
			if(opornumber) {
				numeric = true;
			}

			acc.push_back(c);
		}
		else if(op || opornumber) {
			if(isoperator(c)) {
				acc.push_back(c);
			}
			else {
				return{Operator, acc};
			}
			op = true;
			opornumber = false;
		}
		else if(isalpha(c) || c == '_' || c == ':') {
			acc.push_back(c);
		}
		else if(isbreaker(c)) {

			if(acc == "true" || acc == "false") {
				return{BooleanLiteral, acc};
			}

			return{var ? VariableIdentifier : Identifier, acc};
		}
	}
}

Token peeknexttoken(const std::string &input, int index) {
	return getnexttoken(input, index);
}

} // end of unnamed namespace




namespace internal {
	struct node {
		node(const Token &token = {}, node *l = nullptr, node *r = nullptr, bool b = false):
			value(token), left(l), right(r), binary(b) {}

		Token value;
		node *left;
		node *right;	

		bool binary;

		~node() {
			if(left) delete left;
			if(right) delete right;
		}
	};

	void testlexer(const std::string &input, std::ostream *cases) {
		int index = 0;
		int i = 0;
		Token token;
		if(cases) {
			(*cases) << "	{" << std::endl
				<< "		auto tokens=parser.parse(\"" << input << "\");" << std::endl << std::endl;
		}
		while((token = getnexttoken(input, index)).Type != EndOfString) {
			if(cases) {
				(*cases) << "		REQUIRE(tokens[" << i << "].Type == " << String::From(token.Type) << ");" << std::endl;
				(*cases) << "		REQUIRE(tokens[" << i << "].Value == \"" << String::Replace(token.Value, "\"", "\\\"") << "\");" << std::endl << std::endl;
			}
			Gorgon::Console::SetColor(Gorgon::Console::Yellow);
			Gorgon::Console::SetBold();
			std::cout << String::From(token.Type) << ": ";
			Gorgon::Console::SetBold(false);
			std::cout << token.Value << std::endl;
			std::cout << input << std::endl;
			for(int i = 0; i < index; i++) { std::cout << "'"; }
			std::cout << std::endl << std::endl;

			i++;
		}
		if(cases) {
			(*cases) << "		REQUIRE(tokens[" << i << "].Type == " << String::From(token.Type) << ");" << std::endl;
			(*cases) << "	}" << std::endl << std::endl;
		}
	}

} // end of internal


internal::node *ProgrammingParser::parse(const std::string &input) {
	int index = 0;
	Token token = getnexttoken(input, index);

	if(token.Type == EndOfString) {
		throw ParseError({ParseError::UnexpectedToken, 0, input[index], "Unexpected end of string"});
	}

	internal::node *root = new node;

	if(token.Type == VariableIdentifier) {
		root = assignment(input, 0);
	}
	else if(token.Type == Identifier) {
		Token next = peeknexttoken(input, index);

		if(next.Type == LPar) {
			root = fncall(input, 0);
		}
		else if(next.Type == EqualSign) { // variable without $ at the beginning
			root = assignment(input, 0);
		}
		else {
			root = keyword(input, 0);
		}
	}

	token = getnexttoken(input, index);

	if(token.Type != EndOfString) {
		throw ParseError({ParseError::UnexpectedToken, 0, input[index], "Expected end of input"});
	}
}


node *ProgrammingParser::assignment(const std::string &input, int index) {
	node *assignment = new node, *lhs = new node, *rhs = new node;
	assignment->left = lhs;
	assignment->right = rhs;

	Token token;

	token = getnexttoken(input, index);
	lhs->value = token;

	token = getnexttoken(input, index);
	if(token.Type != EqualSign) {
		throw ParseError({ParseError::UnexpectedToken, 0, input[index], "Expected assignment operator"});
	}
	assignment->value = token;

	rhs = expression(input, index);


	return assignment;
}

}} // end of namespaces scripting and gorgon

