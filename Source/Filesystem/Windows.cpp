#include "../Filesystem.h"

#include <cstdio>
#include <iostream>
#include <direct.h>
#include <sys/stat.h>
#include <io.h>

#include <windows.h>
#include <shlobj.h>
#include "Iterator.h"

#undef CreateDirectory

namespace Gorgon { namespace Filesystem {

	bool CreateDirectory(const std::string &name) {
		auto pos=name.length();

		pos=name.find_last_of("\\/",std::string::npos);
		if(pos!=std::string::npos) {
			if(!IsDirectory(name.substr(0,pos)))
				CreateDirectory(name.substr(0,pos));
		}

		CreateDirectoryA(name.c_str(), NULL);

		return IsDirectory(name);
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
	
	bool IsExists(const std::string &path) {
		return _access(path.c_str(), 0)!=-1;
	}
	
	bool IsWritable(const std::string &path) {
		return _access(path.c_str(), 2)!=-1;
	}

	bool IsHidden(const std::string &path) {
		unsigned long attr=GetFileAttributes(path.c_str());
		
		if(attr==INVALID_FILE_ATTRIBUTES) return false;

		return (attr&FILE_ATTRIBUTE_HIDDEN)!=0 || (attr&FILE_ATTRIBUTE_SYSTEM)!=0;
	}
	
	void fixwinslashes(std::string &s) {
		for(auto &c : s) if(c=='\\') c='/';
	}

	std::string Canonical(const std::string &path) {
		if(!IsExists(path)) {
			throw PathNotFoundError("Cannot canonicalize the given path: "+path);
		}

		char newpath[1024];
		if(GetFullPathName(path.c_str(), 1024, newpath, NULL) == 0) {
			throw PathNotFoundError("Cannot canonicalize the given path: "+path);
		}

		std::string ret(newpath);
		fixwinslashes(ret);
		
		return ret;
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
					if(dir.back()==path) {
						dir.pop_back();
						if(RemoveDirectory(path.c_str())==0) return false;
					}
					else {
						if(remove(path.c_str())!=0) return false;
					}
					open.pop_back();
				}
			}

			return true;
		}
		else {
			return remove(path.c_str())==0;
		}
	}

	bool ChangeDirectory(const std::string &path) {
		return _chdir(path.c_str())==0;
	}
	
	std::string CurrentDirectory() {
		char path[1024];
		GetCurrentDirectory(MAX_PATH, path);
		
		std::string ret(path);
		fixwinslashes(ret);
		
		return ret;
	}
	
	bool Copy(const std::string &source, const std::string &target) {
		SHFILEOPSTRUCT s = { };
		s.hwnd = NULL;
		s.wFunc = FO_COPY;
		s.fFlags = FOF_SILENT | FOF_NOCONFIRMMKDIR | FOF_MULTIDESTFILES | FOF_NOERRORUI | FOF_NOCONFIRMATION;
		
		std::string src=source;
		std::string trg=target;
		
		src.push_back(0);
		trg.push_back(0);
		
		s.pTo = trg.c_str();
		s.pFrom = src.c_str();
		return SHFileOperation(&s)==0;
	}
	
	bool Move(const std::string &source, const std::string &target) {
		return MoveFile(source.c_str(), target.c_str())!=0;
	}

	std::string ApplicationDirectory() {
		HMODULE hModule = GetModuleHandle(NULL);
		char path[MAX_PATH];
		GetModuleFileName(hModule, path, MAX_PATH);

		return GetDirectory(path);
	}
	
	std::vector<EntryPoint> EntryPoints() {
		char drvs[512], name[128];
		char *drives=drvs;
		unsigned long serial, flags;

		unsigned result=GetLogicalDriveStrings(512, drives);

		if(result==0) return std::vector<EntryPoint>();

		std::vector<EntryPoint> entries;
		
		EntryPoint e;
		
		CHAR my_documents[MAX_PATH];
		my_documents[0]=0;

		SHGetFolderPath(NULL, CSIDL_PROFILE, NULL, SHGFP_TYPE_CURRENT, my_documents);
		
		e.Path=my_documents;
		e.Name="Home";
		e.Readable=true;
		e.Writable=IsWritable(e.Path);		
		fixwinslashes(e.Path);
		entries.push_back(e);
		
		while(*drives) {
			
			e.Path=drives;
			if(e.Path.back()!='\\') e.Path.push_back('/');
			fixwinslashes(e.Path);
			name[0]=0;
			if(GetVolumeInformation(drives, name, 128, &serial, NULL, &flags, NULL, 0)) {
				e.Name=name;
				if(e.Name.empty())
					e.Name=e.Path;
				e.Writable=!(flags&FILE_READ_ONLY_VOLUME);
				e.Readable=true;
				entries.push_back(e);
			}
			drives+=std::strlen(drives)+1;
		}

		return entries;
	}
	
	namespace internal {
		class iterator_data {
		public:
			iterator_data() : data(new WIN32_FIND_DATAA) { }
			~iterator_data() {
				FindClose(search_handle);
				delete data;
			}
			
			WIN32_FIND_DATAA *data;
			HANDLE search_handle;
			std::string pattern;
		};
	}
	
	Iterator::Iterator(const std::string &directory, const std::string &pattern) : 
	data(new internal::iterator_data), basedir(directory) {
		std::string src=directory;
		if(src[src.length()-1]!='\\') src+="\\";
		src+=pattern;
		
		data->search_handle=FindFirstFileA(src.c_str(), data->data);
		data->pattern=pattern;
		
		if(data->search_handle==INVALID_HANDLE_VALUE) {
			Destroy();
			throw PathNotFoundError("Cannot open directory for reading");
		}
		
		current=data->data->cFileName;
		if(current=="." || current=="..") Next();
	}
	
	Iterator::Iterator(const Iterator &other) {
		if(!other.data) {
			data=nullptr;
			return;
		}
		
		data=new internal::iterator_data;
		
		std::string src=other.basedir;
		if(src[src.length()-1]!='\\') src+="\\";
		src+=other.data->pattern;
		data->pattern=other.data->pattern;
		
		data->search_handle=FindFirstFileA(src.c_str(), data->data);
		
		if(data->search_handle==INVALID_HANDLE_VALUE) {
			Destroy();
			throw PathNotFoundError("Cannot open directory for reading");
		}
		
		basedir=other.basedir;
		current=other.current;
		
		//move to the other's position
		while(data->data->cFileName!=current) {
			if(FindNextFileA(data->search_handle, data->data) == FALSE) {
				Destroy();
				break;
			}
		}
	}
	
	void Iterator::Destroy() {
		delete data;
		data=nullptr;
		current="";
	}
	
	
	bool Iterator::Next() {
#ifndef NDEBUG
		if(!data || !data->data || data->search_handle==INVALID_HANDLE_VALUE) {
			throw std::runtime_error("Invalid iterator");
		}
#endif

		if (FindNextFileA (data->search_handle, data->data) == FALSE) {
			Destroy();
			return false;
		}
		else {
			current=data->data->cFileName;
		}
		
		if(current=="." || current=="..") return Next();
		
		return true;
	}
	
} }
