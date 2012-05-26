 #pragma once
#include <string.h>

namespace gge { namespace encoding { 

	std::string UriDecode(const std::string &str);
	std::string UriEncode(const std::string &str);
	
} }