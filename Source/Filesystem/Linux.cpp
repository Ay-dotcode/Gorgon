#include "../Filesystem.h"

#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>

namespace Gorgon { namespace Filesystem {
	
	bool CreateDirectory(const std::string &path) {
		auto pos=path.length();

		pos=path.find_last_of('/');

		if(pos!=path.npos && !IsDirectory(path.substr(0,pos)))
			CreateDirectory(path.substr(0,pos));

		mkdir(path.c_str(), 0755);

		return IsDirectory(path);
	}
	
	bool IsDirectory(const std::string &path) {
		if ( access(path.c_str(), 0) )
			return false;

		struct stat status;

		stat( path.c_str(), &status );

		if(status.st_mode & S_IFDIR)
			return true;
		else
			return false;
	}
	
	bool IsFile(const std::string &path) {
		if ( access(path.c_str(), 0) )
			return false;

		struct stat status;

		stat( path.c_str(), &status );

		if(status.st_mode & S_IFREG)
			return true;
		else
			return false;
	}
	
	bool IsWritable(const std::string &path) {
		return access(path.c_str(), W_OK)==-1;
	}
	
	bool IsHidden(const std::string &f) {
		if(f=="") return false;

		std::string file=f;
		if(file[file.length()-1]=='/') {
			file=file.substr(0,file.length()-1);
		}

		if(file[file.length()-1]=='~') return true;
		
		if(file.find_first_of('/', 0)==file.npos) {
			return file[0]=='.';
		}
		else {
			file=file.substr(file.find_last_of('/', -1)+1);
			return file[0]=='.';
		}

		return false;
	}
	
	std::string Canonize(const std::string &path) {
		char *newpath;
		newpath=realpath(path.c_str(), nullptr);
		if(newpath==nullptr) {
			throw std::runtime_error("Cannot canonize the give path: "+path);
		}
		std::string ret=newpath;
		std::free(newpath);
		return ret;
	}
	
	bool ChangeDirectory(const std::string &path) {
		return chdir(path.c_str())==0;
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
				if(IsDirectory(path) && dir.back()!=path) {
					dir.push_back(path);

					// list its contents into open list
					DirectoryIterator it(path);
					for(;it.IsValid(); it.Next()) {
						if(*it!="." && *it!="..") {
							open.push(path + "/" + *it);
						}
					}
				}
				else {
					// if this is the directory to be erased
					if(dir.back()==path) dir.pop_back();
					open.pop();
					
					if(remove(path)!=0) return false;
				}
			}
			
			return true;
		}
		else {
			return remove(path.c_str())==0;
		}
	}

	
	std::string CurrentDirectory() {
		char path[1024];
		
		getcwd(path, 1024);
		
		return path;
	}
	
	
} }