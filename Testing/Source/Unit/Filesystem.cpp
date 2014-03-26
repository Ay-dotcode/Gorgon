#define CATCH_CONFIG_MAIN

#include <catch.h>

#undef CreateDirectory

#include <Source/Filesystem.h>

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


TEST_CASE( "Check if a file is a file", "[IsFile][Delete]") {
	std::ofstream testfile("testfile.txt");
	testfile.close();

	REQUIRE( fs::IsFile("testfile.txt") );

	REQUIRE( fs::Delete("testfile.txt") );

	REQUIRE_FALSE( fs::IsFile("testfile.txt") );
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


TEST_CASE( "Check if the current directory is writable", "[IsWritable]") {
	REQUIRE( fs::IsWritable(".") );
}

TEST_CASE( "Check if a file is writable", "[IsWritable]" ) {
	std::ofstream testfile("testfile.txt");
	testfile.close();

	REQUIRE( fs::IsWritable("testfile.txt") );

#ifdef LINUX
	system("chmod 000 testfile.txt");
#else
	SetFileAttributes("testfile.txt", 
		GetFileAttributes("testfile.txt") | FILE_ATTRIBUTE_READONLY);
#endif	

	REQUIRE_FALSE( fs::IsWritable("testfile.txt") );

#ifdef LINUX
	system("chmod 755 testfile.txt");
#else
	SetFileAttributes("testfile.txt", 
		GetFileAttributes("testfile.txt") & ~FILE_ATTRIBUTE_READONLY);
#endif
	
	fs::Delete("testfile.txt");

	REQUIRE_FALSE( fs::IsWritable("testfile.txt") );
}


TEST_CASE( "Check if a file is hidden", "[IsHidden]") {

	REQUIRE_FALSE( fs::IsHidden("testfile.txt") );

	std::ofstream testfile("testfile.txt");
	testfile.close();

	REQUIRE_FALSE( fs::IsHidden("testfile.txt") );

	fs::Delete("testfile.txt");

	testfile.open(".testfile");
	testfile.close();

#ifdef WIN32
	SetFileAttributes(".testfile", 
		GetFileAttributes("testfile.txt") | FILE_ATTRIBUTE_HIDDEN);
#endif	

	REQUIRE( fs::IsHidden(".testfile") );

#ifdef WIN32
	SetFileAttributes(".testfile", 
		GetFileAttributes("testfile.txt") & ~FILE_ATTRIBUTE_HIDDEN);
#endif	

	fs::Delete(".testfile");
} 


TEST_CASE( "Check if canonize throws", "[Canonize]" ) {
	REQUIRE_THROWS_AS( fs::Canonical("this/path/does/not/exists"), 
		fs::PathNotFoundError
	);

	try {
		fs::Canonical("this/path/does/not/exists");
	}
	catch(const fs::PathNotFoundError &err) {
		std::string s=err.what();
		REQUIRE( s.find("this/path/does/not/exists") != s.npos );
	}
}

TEST_CASE( "Check if canonize works", "[Canonize]" ) {
	std::ofstream testfile("testfile.txt");
	testfile<<"this is a test text"<<std::endl;
	testfile.close();

	auto filename=fs::Canonical("testfile.txt");

	std::cout<<"Make sure test path does not contain \\"<<std::endl;

	REQUIRE( filename.find_first_of('\\') == filename.npos );

	//canonize should always return a string that has at least
	//one /
	REQUIRE( filename.find_first_of('/') != filename.npos );

	//canonize should not leave / unless its a top level directory
	REQUIRE( fs::Canonical(".").back() != '/' );

	REQUIRE( fs::IsFile(filename) );

	std::ifstream testread(filename);
	std::string line;
	std::getline(testread, line);

	REQUIRE( line == "this is a test text" );

	fs::Delete("testfile.txt");
}

TEST_CASE( "Test if relative works", "[Relative]") {
	std::ofstream testfile("testfile.txt");
	testfile.close();

	fs::CreateDirectory("testdir");

	REQUIRE( fs::Relative("testfile.txt")=="testfile.txt" );

	REQUIRE( fs::Relative("testfile.txt", fs::Canonical("testdir")) == "../testfile.txt" );

	REQUIRE( fs::Relative(fs::Canonical("testfile.txt")) == "testfile.txt" );

	fs::Delete("testfile.txt");
	fs::Delete("testdir");
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


