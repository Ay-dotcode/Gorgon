#define CATCH_CONFIG_MAIN

#define WINDOWS_LEAN_AND_MEAN

#include <catch.h>

#include <Source/String.h>
#include <Source/String/Tokenizer.h>
#include <Source/Types.h>

using namespace Gorgon;

TEST_CASE( "To type conversion", "[To]") {
	
	REQUIRE( String::To<int>("5") == 5 );
	REQUIRE( String::To<int>("-105") == -105 );
	REQUIRE( String::To<float>("5.5") == Approx(5.5f) );
	REQUIRE( String::To<double>("5.512345345454") == Approx(5.512345345454) );
	REQUIRE( String::To<float>("-55.5") == Approx(-55.5f) );
	REQUIRE( String::To<float>("5.543e2") == Approx(554.3f) );
	REQUIRE( String::To<short int>("5.512345345454") == 5 );
	REQUIRE( String::To<unsigned>("12983") == 12983 );


	REQUIRE( String::To<int>(std::string("5")) == 5 );
	REQUIRE( String::To<int>(std::string("-105")) == -105 );
	REQUIRE( String::To<float>(std::string("5.5")) == Approx(5.5f) );
	REQUIRE( String::To<double>(std::string("5.512345345454")) == Approx(5.512345345454) );
	REQUIRE( String::To<float>(std::string("-55.5")) == Approx(-55.5f) );
	REQUIRE( String::To<float>(std::string("5.543e2")) == Approx(554.3f) );
	REQUIRE( String::To<short int>(std::string("5.512345345454")) == 5 );
	REQUIRE( String::To<unsigned>(std::string("12983")) == 12983 );
	
}

TEST_CASE( "Replace", "[Replace]") {
	
	REQUIRE( String::Replace("this is a test", "is", "was") == "thwas was a test" );
	REQUIRE( String::Replace("was a test", "is", "was") == "was a test" );
	REQUIRE( String::Replace("loong to short", "loong", "long") == "long to short" );
	REQUIRE( String::Replace("", "a", "b") == "" );
	REQUIRE( String::Replace("this is a test", "", "a") == "this is a test" ); 
	REQUIRE( String::Replace("this is a test", "is", "is") == "this is a test" ); 
	REQUIRE( String::Replace("this is a test", "is", "") == "th  a test" ); 
	REQUIRE( String::Replace("aaaaa", "a", "ab") == "ababababab" );
}

TEST_CASE( "Trim", "[Trim]") {
	
	REQUIRE( String::Trim("  aa  ") == "aa" );
	REQUIRE( String::Trim("aa bb cc") == "aa bb cc" );
	REQUIRE( String::Trim("  \naa bb \t\ncc\n") == "aa bb \t\ncc" );
	REQUIRE( String::Trim("  \tasdf\n\r") == "asdf" );
	REQUIRE( String::Trim("aAé!'^+%&/()=?") == "aAé!'^+%&/()=?" );
	REQUIRE( String::Trim("aaabbaacc", "ac") == "bb" );
	REQUIRE( String::Trim("  aa  ", "bb") == "  aa  " );
	REQUIRE( String::Trim("b  aa  b", "bb") == "  aa  " );
	REQUIRE( String::Trim("aa  ") == "aa" );
	REQUIRE( String::Trim("  aa") == "aa" );
	
	REQUIRE( String::TrimStart("  aa  ") == "aa  " );
	REQUIRE( String::TrimStart("aa bb cc") == "aa bb cc" );
	REQUIRE( String::TrimStart("  \naa bb \t\ncc\n") == "aa bb \t\ncc\n" );
	REQUIRE( String::TrimStart("  \tasdf\n\r") == "asdf\n\r" );
	REQUIRE( String::TrimStart("aAé!'^+%&/()=?") == "aAé!'^+%&/()=?" );
	REQUIRE( String::TrimStart("ccaaabbaacc", "ac") == "bbaacc" );
	REQUIRE( String::TrimStart("  aa  ", "bb") == "  aa  " );
	REQUIRE( String::TrimStart("b  aa  b", "bb") == "  aa  b" );
	REQUIRE( String::TrimStart("aa  ") == "aa  " );
	REQUIRE( String::TrimStart("  aa") == "aa" );
	
	REQUIRE( String::TrimEnd("  aa  ") == "  aa" );
	REQUIRE( String::TrimEnd("aa bb cc") == "aa bb cc" );
	REQUIRE( String::TrimEnd("  \naa bb \t\ncc\n") == "  \naa bb \t\ncc" );
	REQUIRE( String::TrimEnd("  \tasdf\n\r") == "  \tasdf" );
	REQUIRE( String::TrimEnd("aAé!'^+%&/()=?") == "aAé!'^+%&/()=?" );
	REQUIRE( String::TrimEnd("aaacbbaacc", "ac") == "aaacbb" );
	REQUIRE( String::TrimEnd("  aa  ", "bb") == "  aa  " );
	REQUIRE( String::TrimEnd("b  aa  b", "bb") == "b  aa  " );
	REQUIRE( String::TrimEnd("aa  ") == "aa" );
	REQUIRE( String::TrimEnd("  aa") == "  aa" );
}

TEST_CASE( "Case transform", "[ToUpper][ToLower]" ) {
	REQUIRE( String::ToUpper("abc") == "ABC" );
	REQUIRE( String::ToUpper("aBc") == "ABC" );
	REQUIRE( String::ToUpper(".!aBc rr") == ".!ABC RR" );
	REQUIRE( String::ToUpper("!'^+%&") == "!'^+%&" );
	REQUIRE( String::ToUpper("") == "" );
	
	REQUIRE( String::ToLower("ABC") == "abc" );
	REQUIRE( String::ToLower("aBc") == "abc" );
	REQUIRE( String::ToLower(".!aBc rr") == ".!abc rr" );
	REQUIRE( String::ToLower("!'^+%&") == "!'^+%&" );
	REQUIRE( String::ToLower("") == "" );
}

TEST_CASE( "Conversion from", "[From]" ) {
	REQUIRE( String::From(char(15)) == "15" );
	REQUIRE( String::From(char(-15)) == "-15" );
	REQUIRE( String::From(Gorgon::Byte(200)) == "200" );
	REQUIRE( String::From(1500) == "1500" );
	REQUIRE( String::From(-1500) == "-1500" );
	REQUIRE( String::From(1500u) == "1500" );
	REQUIRE( String::From(1500l) == "1500" );
	REQUIRE( String::From(1500ul) == "1500" );
	REQUIRE( String::From("asdf") == "asdf" );
	REQUIRE( String::From(std::string("asdf")) == "asdf" );
	REQUIRE( String::TrimEnd(String::From(4.56f), "0") == "4.56" );
	REQUIRE( String::TrimEnd(String::From(4.56 ), "0") == "4.56" );
}

TEST_CASE( "Extraction", "[Extract][ExtractOutsideQuotes]" ) {
	std::string data="this is a test text";
	
	REQUIRE( String::Extract(data, ' ') == "this" );
	REQUIRE( data == "is a test text" );
	
	REQUIRE( String::Extract(data, ' ') == "is" );
	REQUIRE( data == "a test text" );
	
	REQUIRE( String::Extract(data, ' ') == "a" );
	REQUIRE( data == "test text" );
	
	REQUIRE( String::Extract(data, ' ') == "test" );
	REQUIRE( data == "text" );
	
	REQUIRE( String::Extract(data, ' ') == "text" );
	REQUIRE( data == "" );

	REQUIRE( String::Extract(data, ' ') == "" );
	REQUIRE( data == "" );
	

	data="this \"is a test\" text";
	
	REQUIRE( String::Extract(data, ' ') == "this" );
	REQUIRE( data == "\"is a test\" text" );
	
	REQUIRE( String::Extract(data, ' ') == "\"is" );
	REQUIRE( data == "a test\" text" );
	
	REQUIRE( String::Extract(data, ' ') == "a" );
	REQUIRE( data == "test\" text" );
	
	REQUIRE( String::Extract(data, ' ') == "test\"" );
	REQUIRE( data == "text" );
	
	REQUIRE( String::Extract(data, ' ') == "text" );
	REQUIRE( data == "" );

	REQUIRE( String::Extract(data, ' ') == "" );
	REQUIRE( data == "" );

	
	data="this \"is a test\" text";
	
	REQUIRE( String::ExtractOutsideQuotes(data, ' ') == "this" );
	REQUIRE( data == "\"is a test\" text" );
	
	REQUIRE( String::ExtractOutsideQuotes(data, ' ') == "\"is a test\"" );
	REQUIRE( data == "text" );
	
	REQUIRE( String::ExtractOutsideQuotes(data, ' ') == "text" );
	REQUIRE( data == "" );

	REQUIRE( String::ExtractOutsideQuotes(data, ' ') == "" );
	REQUIRE( data == "" );
	

	data="this \"is a' test\" text";
	
	REQUIRE( String::ExtractOutsideQuotes(data, ' ') == "this" );
	REQUIRE( data == "\"is a' test\" text" );
	
	REQUIRE( String::ExtractOutsideQuotes(data, ' ') == "\"is a' test\"" );
	REQUIRE( data == "text" );
	
	REQUIRE( String::ExtractOutsideQuotes(data, ' ') == "text" );
	REQUIRE( data == "" );

	REQUIRE( String::ExtractOutsideQuotes(data, ' ') == "" );
	REQUIRE( data == "" );
	
	
	data="this 'is a test' text";
	
	REQUIRE( String::ExtractOutsideQuotes(data, ' ') == "this" );
	REQUIRE( data == "'is a test' text" );
	
	REQUIRE( String::ExtractOutsideQuotes(data, ' ') == "'is a test'" );
	REQUIRE( data == "text" );
	
	REQUIRE( String::ExtractOutsideQuotes(data, ' ') == "text" );
	REQUIRE( data == "" );

	REQUIRE( String::ExtractOutsideQuotes(data, ' ') == "" );
	REQUIRE( data == "" );
	
	
	data="this 'is a\" test' text";
	
	REQUIRE( String::ExtractOutsideQuotes(data, ' ') == "this" );
	REQUIRE( data == "'is a\" test' text" );
	
	REQUIRE( String::ExtractOutsideQuotes(data, ' ') == "'is a\" test'" );
	REQUIRE( data == "text" );
	
	REQUIRE( String::ExtractOutsideQuotes(data, ' ') == "text" );
	REQUIRE( data == "" );

	REQUIRE( String::ExtractOutsideQuotes(data, ' ') == "" );
	REQUIRE( data == "" );
	

	data="this 'is a\" test' text";
	
	REQUIRE( String::ExtractOutsideQuotes(data, ' ', String::QuoteType::Double) == "this" );
	REQUIRE( data == "'is a\" test' text" );
	
	REQUIRE( String::ExtractOutsideQuotes(data, ' ', String::QuoteType::Double) == "'is" );
	REQUIRE( data == "a\" test' text" );
	
	REQUIRE( String::ExtractOutsideQuotes(data, ' ', String::QuoteType::Double) == "a\" test' text" );
	REQUIRE( data == "" );

	REQUIRE( String::ExtractOutsideQuotes(data, ' ', String::QuoteType::Double) == "" );
	REQUIRE( data == "" );
	

	data="this \"is a' test\" text";
	
	REQUIRE( String::ExtractOutsideQuotes(data, ' ', String::QuoteType::Single) == "this" );
	REQUIRE( data == "\"is a' test\" text" );
	
	REQUIRE( String::ExtractOutsideQuotes(data, ' ', String::QuoteType::Single) == "\"is" );
	REQUIRE( data == "a' test\" text" );
	
	REQUIRE( String::ExtractOutsideQuotes(data, ' ', String::QuoteType::Single) == "a' test\" text" );
	REQUIRE( data == "" );

	REQUIRE( String::ExtractOutsideQuotes(data, ' ', String::QuoteType::Single) == "" );
	REQUIRE( data == "" );
}


TEST_CASE( "Tokenizer", "[Tokenizer]" ) {
	
	std::string data="this is a test text.aaa";
	String::Tokenizer t{data, " "};
	std::vector<std::string> texts{"this", "is", "a", "test", "text.aaa"};
	
	int i=0;
	for(; t.IsValid(); t.Next()) {
		REQUIRE( *t == texts[i] );
		i++;
	}
	
	t={data, " "};
	
	i=0;
	for(; t != t.end(); ++t) {
		REQUIRE( *t == texts[i] );
		i++;
	}
	
	t={data, " ."};
	texts.back()="text";
	texts.push_back("aaa");
	
	i=0; 
	for(; t.IsValid(); t.Next()) {
		REQUIRE( *t == texts[i] );
		i++;
	}
}
