#pragma once

#include <sys/stat.h> 
#include <string> 

namespace gge { namespace filesystem {
	inline bool isPathExists(const std::string Filename) {
		struct stat inf;

		if(Filename[Filename.length()-1]=='\\')
			return stat(Filename.substr(0, Filename.length()-1).c_str(), &inf)==0;
		else
			return stat(Filename.c_str(), &inf)==0;
	}
} }
