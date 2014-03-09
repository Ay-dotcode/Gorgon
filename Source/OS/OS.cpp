#include "OS.h"
#include <fstream>
#include "../Encoding/LZMA.h"
#include "GGEMain.h"



namespace gge { namespace os { namespace filesystem {
	//Slow!
	std::string LocateResource(std::string filename, std::string path, bool localonly) {
		using namespace gge::os;
		using namespace gge::os::filesystem;

		if(path!="") {
			if(IsFileExists(path+"/"+filename))
				return path+"/"+filename;

			if(IsFileExists("../"+path+"/"+filename))
				return "../"+path+"/"+filename;
		}

		if(IsFileExists(filename))
			return filename;

		if(IsFileExists("../"+filename))
			return "../"+filename;

		if(path!="") {
			if(IsFileExists(path+"/"+filename+".lzma")) {
				std::ifstream inputfile(path+"/"+filename+".lzma");
				std::ofstream outputfile(path+"/"+filename);

				gge::encoding::Lzma.Decode(inputfile,outputfile);

				return path+"/"+filename;
			}

			if(IsFileExists("../"+path+"/"+filename+".lzma")) {
				std::ifstream inputfile("../"+path+"/"+filename+".lzma");
				std::ofstream outputfile("../"+path+"/"+filename);

				gge::encoding::Lzma.Decode(inputfile,outputfile);

				return "../"+path+"/"+filename;
			}
		}

		if(IsFileExists(filename+".lzma")) {
			std::ifstream inputfile(filename+".lzma");
			std::string out;
			if(CreateDirectory(path)) {
				out=path+"/"+filename;
			}
			else {
				out=filename;
			}
			std::ofstream outputfile(out);

			gge::encoding::Lzma.Decode(inputfile,outputfile);

			return out;
		}

		if(IsFileExists("../"+filename+".lzma")) {
			std::ifstream inputfile("../"+filename+".lzma");
			std::string out;
			if(CreateDirectory(path)) {
				out="../"+path+"/"+filename;
			}
			else {
				out="../"+filename;
			}
			std::ofstream outputfile(out);

			gge::encoding::Lzma.Decode(inputfile,outputfile);

			return out;
		}

		if(!localonly) {
			std::vector<std::string> filenamelist;
			if(path!="") {
				filenamelist.push_back(user::GetDocumentsPath()+"/."+gge::Main.GetSystemName()+"/"+path+"/"+filename);
				filenamelist.push_back(GetAppDataPath()+"/."+gge::Main.GetSystemName()+"/"+path+"/"+filename);
				filenamelist.push_back(user::GetDocumentsPath()+"/"+gge::Main.GetSystemName()+"/"+path+"/"+filename);
				filenamelist.push_back(GetAppDataPath()+"/"+gge::Main.GetSystemName()+"/"+path+"/"+filename);
				filenamelist.push_back(user::GetDocumentsPath()+"/"+path+"/"+filename);
				filenamelist.push_back(GetAppDataPath()+"/"+path+"/"+filename);
			}
			filenamelist.push_back(user::GetDocumentsPath()+"/."+gge::Main.GetSystemName()+"/"+filename);
			filenamelist.push_back(user::GetDocumentsPath()+"/"+gge::Main.GetSystemName()+"/"+filename);

			for(auto it=filenamelist.begin();it!=filenamelist.end();++it) {
				if(IsFileExists(*it))
					return *it;
				if(IsFileExists(*it+".lzma")) {
					std::ifstream inputfile(*it+".lzma");
					std::ofstream outputfile(*it);

					gge::encoding::Lzma.Decode(inputfile,outputfile);

					return *it;
				}
			}
		}

		throw std::runtime_error("Cannot find file: "+filename);
	}
} } }
