#define CATCH_CONFIG_MAIN

#include <catch.h>

#undef CreateDirectory

#include <Filesystem.h>

#include <iostream>
#include <string>
#include <stdlib.h>
#include <cstring>

namespace fs=Gorgon::Filesystem;


TEST_CASE( "PathNotFoundError", "[PathNotFoundError]" ) {
	try {
		throw fs::PathNotFoundError("Not found text");
	}
	catch(const fs::PathNotFoundError &err) {
		REQUIRE( std::strcmp(err.what(), "Not found text") == 0 ) ;
	}

	try {
		throw fs::PathNotFoundError("Not found text");
	}
	catch(const std::runtime_error &err) {
		REQUIRE( std::strcmp(err.what(), "Not found text") == 0 ) ;
	}

	try {
		throw fs::PathNotFoundError("Not found text");
	}
	catch(const std::exception &err) {
		REQUIRE( std::strcmp(err.what(), "Not found text") == 0 ) ;
	}
}



TEST_CASE( "Create directory", "[CreateDirectory][IsDirectory][IsFile][Delete]" ) {
	REQUIRE( fs::CreateDirectory("thisistest") );

	REQUIRE( fs::IsDirectory("thisistest") );
	REQUIRE_FALSE( fs::IsFile("thisistest") );

	REQUIRE( fs::Delete("thisistest") );

	REQUIRE_FALSE( fs::IsDirectory("thisistest") );
	REQUIRE_FALSE( fs::IsFile("thisistest") );
}

TEST_CASE( "Create nested directory", "[CreateDirectory][IsDirectory][Delete]") {
	REQUIRE( fs::CreateDirectory("this/is/test") );

	REQUIRE( fs::IsDirectory("this/is/test") );

	fs::Delete("this");

	REQUIRE_FALSE( fs::IsDirectory("this/is/test") );
	REQUIRE_FALSE( fs::IsDirectory("this/is") );
	REQUIRE_FALSE( fs::IsDirectory("this") );
}


TEST_CASE( "Check file is not a directory", "[IsDirectory][Delete]") {
	std::ofstream testfile("testfile.txt");
	testfile.close();

	REQUIRE_FALSE( fs::IsDirectory("testfile.txt") );

	REQUIRE( fs::Delete("testfile.txt") );
}


TEST_CASE( "Check if a file is a file", "[IsFile]") {
	std::ofstream testfile("testfile.txt");
	testfile.close();

	REQUIRE( fs::IsFile("testfile.txt") );

	fs::Delete("testfile.txt");
}


TEST_CASE( "Check file exists", "[IsExists]") {
	std::ofstream testfile("testfile.txt");
	testfile.close();

	REQUIRE( fs::IsExists("testfile.txt") );

	fs::Delete("testfile.txt");

	REQUIRE_FALSE( fs::IsExists("testfile.txt") );
}

TEST_CASE( "Check directory exists", "[IsExists]") {
	fs::CreateDirectory("test");

	REQUIRE( fs::IsExists("test") );

	fs::Delete("test");

	REQUIRE_FALSE( fs::IsExists("test") );
}


TEST_CASE( "Save - Saving file", "[Save]") {
	fs::Save("test.txt", "This is a test\n");
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

	fs::Save("test.bin", teststring);
	std::ifstream file("test.bin", std::ios::binary);
	
	file.seekg(0, std::ios::end);
	
	REQUIRE( (int)file.tellg() == teststring.length() );
	
	remove("test.bin");
	
	file.close();	
}


TEST_CASE( "Save - Overwrite", "[Save]") {
	std::string teststring="This is a test\n";
	fs::Save("test.txt", teststring);
	
	teststring="t";
	fs::Save("test.txt", teststring);
	
	std::ifstream file("test.txt", std::ios::binary);
	file.seekg(0, std::ios::end);
	REQUIRE( (int)file.tellg() == 1 );
	
	remove("test.txt");
}




TEST_CASE( "Load - Loading file", "[Save][Load]") {	
	std::string teststring="This is a test\n";
	fs::Save("test.txt", teststring);	
	
	REQUIRE( fs::Load("test.txt")==teststring );
	
	remove("test.txt");
}


TEST_CASE( "Load - Loading binary file", "[Save][Load]") {	
	std::string teststring="This is a test\n";
	teststring.push_back(0);
	fs::Save("test.bin", teststring);	
	
	REQUIRE( fs::Load("test.bin")==teststring );
	
	remove("test.bin");
}


TEST_CASE( "Load - PathNotFoundError", "[Load][PathNotFoundError") {	
	REQUIRE_THROWS_AS( 
		fs::Load("thisfiledoesnotexists/iamsureofit.neverever"), 
		fs::PathNotFoundError 
	);
}


