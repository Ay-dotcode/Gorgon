 

#include <Gorgon/Filesystem.h>
#include <Gorgon/Filesystem/Iterator.h>

namespace FS = Gorgon::Filesystem;

#include <string>
#include <iostream>


int main() {
    
	for(FS::Iterator dir(".", "*.png"); dir.IsValid(); dir.Next())
        std::cout<<*dir<<std::endl;

	char ans;
	
	auto entries=Gorgon::Filesystem::EntryPoints();
	
	std::cout<<"List of entry points: "<<std::endl;
	for(auto e : entries) {
		std::cout<<e.Name<<(e.Writable ? "" : " (RO)")<<(e.Removable ? " (REM)" : "")<<": "<<e.Path<<std::endl;
	}
	std::cout<<"Are these correct (y/n)? ";
	std::cin>>ans;
	
	if(ans=='n') {
		std::cout<<"Gorgon::Filesystem::EntryPoints failed."<<std::endl;
		
		return 1;
	}
	
	std::cout<<"Please attach another external device and press enter to continue...";
	std::cin.ignore();
	std::cin.ignore();
	
	
	entries=Gorgon::Filesystem::EntryPoints();
	
	std::cout<<"List of entry points: "<<std::endl;
	for(auto e : entries) {
		std::cout<<e.Name<<(e.Writable ? "" : "(RO)")<<": "<<e.Path<<std::endl;
	}
	std::cout<<"Are these correct (y/n)? ";
	std::cin>>ans;
	
	if(ans=='n') {
		std::cout<<"Gorgon::Filesystem::EntryPoints failed."<<std::endl;
		
		return 1;
	}
	
	std::cout<<"Manual tests are completed."<<std::endl;
	
	return 0;
}

