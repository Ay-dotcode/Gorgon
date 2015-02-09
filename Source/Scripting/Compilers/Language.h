#pragma once

#include <string>

#include "../../Scripting.h"


namespace Gorgon { namespace Scripting { namespace Compilers {
	
	/// Returns the precedence of the given operator. If the given string is not an operator
	/// returns -1
	int GetPrecedence(const std::string &op) {
		if(op=="") {
			return -1;
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
					return -1;
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
						return -1;
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
		
		return -1;
	}
	
} } }
