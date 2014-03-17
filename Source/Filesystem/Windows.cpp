#include "../Filesystem.h"

#include <windows.h>

#undef CreateDirectory

namespace Gorgon { namespace Filesystem {

	bool CreateDirectory(const std::string &name) {
		auto pos=name.length();

		pos=name.find_last_of("\\/",std::string::npos);
		if(pos!=std::string::npos) {
			if(!IsDirectoryExists(name.substr(0,pos)))
				CreateDirectory(name.substr(0,pos));
		}

		CreateDirectoryA(name.c_str(), NULL);

		return IsDirectoryExists(name);
	}

	bool IsDirectory(const std::string &path) {
		if ( _access(path.c_str(), 0) )
			return false;

		struct stat status;

		stat( path.c_str(), &status );

		if(status.st_mode & S_IFDIR)
			return true;
		else
			return false;
	}

	bool IsFile(const std::string &path) {
		if ( _access(path.c_str(), 0) )
			return false;

		struct stat status;

		stat( path.c_str(), &status );

		if(status.st_mode & S_IFREG)
			return true;
		else
			return false;
	}
	
	bool IsWritable(const std::string &path) {
		return _access(path.c_str(), 2)!=-1;
	}

	bool IsHidden(const std::string &path) {
		unsigned long attr=GetFileAttributes(path.c_str());

		return (attr&FILE_ATTRIBUTE_HIDDEN)!=0 || (attr&FILE_ATTRIBUTE_SYSTEM)!=0;
	}

	std::string Canonize(const std::string &path) {
		char newpath[1024];
		GetFullPathName(path.c_str(), 1024, newpath, NULL);
		std::string ret(newpath);	
		for(auto &c : ret) if(c=='\\') c='/';
		
		return ret;
	}

	bool ChangeDirectory(const std::string &path) {
		return _chdir(path.c_str())==0;
	}
	
	std::string CurrentDirectory() {
		char path[1024];
		GetCurrentDirectory(MAX_PATH,pwd);
		
		std::string ret(path);
		for(auto &c : ret) if(c=='\\') c='/';
		
		return ret;
	}
	
	bool Copy(const std::string &source, const std::string &target) {
		SHFILEOPSTRUCT s = { };
		s.hwnd = NULL;
		s.wFunc = FO_COPY;
		s.fFlags = FOF_SILENT | FOF_NOCONFIRMMKDIR | FOF_MULTIDESTFILES | FOF_NOERRORUI | FOF_NOCONFIRMATION;
		s.pTo = target.append('\0').c_str();
		s.pFrom = source.append('\0').c_str();
		return SHFileOperation(&s)==0;
	}
	
	bool Move(const std::string &source, const std::string &target) {
		return MoveFile(source.c_str(), target.c_str())!=0;
	}
	
} }

