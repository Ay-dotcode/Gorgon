#include "../Filesystem.h"
#include "Iterator.h"

#include <fstream>

namespace Gorgon { namespace Filesystem {
	
	std::string startupdir;
	
	void Initialize() {
		startupdir=CurrentDirectory();
	}
	
	std::string StartupDirectory() {
		return startupdir;
	}
	
	std::string ApplicationDirectory() {
		std::string path=Canonize("/proc/self/exe");
		
		return GetDirectory(path);
	}
	
	bool Delete(const std::string &path) {
		if(IsDirectory(path)) {
			std::vector<std::string> open, dir;
			open.push_back(path);
			
			// while we still have more to delete
			while(open.size()) {
				// get the path to delete
				std::string path=open.back();
				
				// if the path is a directory and its
				// contents are not considered
				if(IsDirectory(path) && (!dir.size() || dir.back()!=path)) {
					dir.push_back(path);

					// list its contents into open list
					Iterator it(path);
					for(;it.IsValid(); it.Next()) {
						if(*it!="." && *it!="..") {
							open.push_back(path + "/" + *it);
						}
					}
				}
				else {
					// if this is the directory to be erased
					if(dir.back()==path) dir.pop_back();
					open.pop_back();
					
					if(remove(path.c_str())!=0) return false;
				}
			}
			
			return true;
		}
		else {
			return remove(path.c_str())==0;
		}
	}
	
	bool Copy(const std::vector<std::string> &source, const std::string &target) {
		for(auto &s : source) {
			if(!Copy(s, target)) return false;
		}
		
		return true;
	}
	
	bool Save(const std::string &filename, const std::string &data, bool append) {
		std::ofstream file(filename, (append ? std::ios::binary | std::ios::app : std::ios::binary));
		
		if(!file.is_open()) return false;
		
		file<<data;
		
		if(!file) return false;
		
		file.close();
		
		return true;
	}
	
	std::string Load(const std::string &filename) {
		std::ifstream file(filename, std::ios::binary);
		
		if(!file.is_open()) throw PathNotFoundError("Cannot find the file: "+filename);
		
		file.seekg(0, std::ios::end);
		auto size=file.tellg();
		file.seekg(0, std::ios::beg);
		
		if(!file) throw std::runtime_error("Cannot read the file: "+filename);
		
		std::string ret;
		ret.reserve(size);
		
		ret.assign( (std::istreambuf_iterator<char>(file) ),
					(std::istreambuf_iterator<char>()    ) );
		
		if(file.fail()) throw std::runtime_error("Cannot read the file: "+filename);
		
		return ret;
	}
	
	
	
} }
