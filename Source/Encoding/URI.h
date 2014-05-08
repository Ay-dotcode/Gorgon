 #pragma once

#include <string.h>

namespace Gorgon { namespace Encoding { 

	/// Decodes a given URI string.
	std::string UriDecode(const std::string &str);

	/// Encodes a given URI string.
	std::string UriEncode(const std::string &str);
	
} }
