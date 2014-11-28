#include "Parser.h"

namespace Gorgon { namespace Scripting {

namespace internal {
struct node {
	std::string value;
	node *left;
	node *right;

	~node() {
		if(left) delete left;
		if(right) delete right;
	}
};
} // end of internal

using internal::node;

namespace {

	enum Tokype 

}

internal::node ProgrmmingParser::parse(const std::string &input) {
	int index = 0;

	node *root;

}

}} // end of namespaces scripting and gorgon

