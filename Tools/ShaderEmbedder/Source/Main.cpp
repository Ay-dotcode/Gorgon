#include <iostream>
#include <string>
#include <vector>
#include <fstream>

int main(int argc, char* argv[]) {
	if(argc == 2 && std::string("help") == argv[1]) {
		std::cout<<"Usage: " << argv[0] << " outputfile inputfiles" << std::endl;
		std::cout<<"  Compiles input files into shader code strings in the output file. Filenames of the input files will be used for variable names. The name will be taken up to first point."<<std::endl;
		return 0;
	}
	if(argc < 3) {
		std::cerr << "This program requires at least two filenames." << std::endl;
		std::cout << "Usage: " << argv[0] << " outputfile inputfiles" << std::endl;
		return 1;
	}
	
	std::string outputfilename = argv[1];
	std::ofstream out(outputfilename);
	
	if(!out.is_open()) {
		std::cerr << "Cannot open output file \"" << outputfilename << "\" to write." << std::endl;
		return 2;
	}
	
	struct ifile {
		std::ifstream *stream;
		std::string name;
	};
	
	std::vector<ifile> inputfiles;
	for(int i = 2; i<argc; i++) {
		auto file = new std::ifstream(argv[i], std::ios::binary);
		if(!file->is_open()) {
			std::cerr << "Cannot open input file \"" << argv[i] << "\" for reading." << std::endl;
			
			return 3;
		}
		
		inputfiles.push_back({file, argv[i]});
	}
	
	for(auto file : inputfiles) {
		auto &inputfile = *file.stream;
		
		auto name = file.name;
		if(name.find_last_of('/') != name.npos) {
			name = name.substr(name.find_last_not_of('/'));
		}
		name = name.substr(0, name.find_first_of('.'));
		
		out << std::endl << "const std::string " << name << " = R\"thisisalongtoken(";
		out << inputfile.rdbuf();
		out << ")thisisalongtoken\";" << std::endl;
	}
	
	out.close();
	
	return 0;
}