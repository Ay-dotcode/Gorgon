#define CATCH_CONFIG_MAIN

#include <catch.h>

#include <Filesystem.h>

#include <iostream>
#include <string>
#include <stdlib.h>





TEST_CASE( "Save - Saving file", "[Save]") {
	Gorgon::Filesystem::Save("test.txt", "This is a test\n");
	std::ifstream file("test.txt", std::ios::binary);
	
	REQUIRE( file.is_open() );
	
	std::string s;
	std::getline(file, s);
	
	file.seekg(0, std::ios::end);
	
	REQUIRE( s == "This is a test" );
	REQUIRE( (int)file.tellg() == 15 );
	
	remove("test.txt");
	
	file.close();	
}


TEST_CASE( "Save - Saving binary file", "[Save]") {
	std::string teststring="This is a test\n";
	teststring.push_back(0); //testing binary

	Gorgon::Filesystem::Save("test.bin", teststring);
	std::ifstream file("test.bin", std::ios::binary);
	
	file.seekg(0, std::ios::end);
	
	REQUIRE( (int)file.tellg() == teststring.length() );
	
	remove("test.bin");
	
	file.close();	
}


TEST_CASE( "Save - Overwrite", "[Save]") {
	std::string teststring="This is a test\n";
	Gorgon::Filesystem::Save("test.txt", teststring);
	
	teststring="t";
	Gorgon::Filesystem::Save("test.txt", teststring);
	
	std::ifstream file("test.txt", std::ios::binary);
	file.seekg(0, std::ios::end);
	REQUIRE( (int)file.tellg() == 1 );
	
	remove("test.txt");
}




TEST_CASE( "Load - Loading file", "[Load]") {	
	std::string teststring="This is a test\n";
	Gorgon::Filesystem::Save("test.txt", teststring);	
	
	REQUIRE( Gorgon::Filesystem::Load("test.txt")==teststring );
	
	remove("test.txt");
}


TEST_CASE( "Load - Loading binary file", "[Load]") {	
	std::string teststring="This is a test\n";
	teststring.push_back(0);
	Gorgon::Filesystem::Save("test.bin", teststring);	
	
	REQUIRE( Gorgon::Filesystem::Load("test.bin")==teststring );
	
	remove("test.bin");
}


TEST_CASE( "Load - PathNotFoundError", "[Load]") {	
	REQUIRE_THROWS_AS( 
		Gorgon::Filesystem::Load("thisfiledoesnotexists/iamsureofit.neverever"), 
		Gorgon::Filesystem::PathNotFoundError 
	);
}


