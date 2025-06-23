#define CATCH_CONFIG_MAIN
#define WINDOWS_LEAN_AND_MEAN

#include <catch2/catch.hpp>

#include <Gorgon/String.h>
#include <Gorgon/String/Tokenizer.h>
#include <Gorgon/Types.h>

using namespace Gorgon;

/*TEST_CASE("Gorgon::String - To<T> Conversion") {
    REQUIRE(String::To<int>("42") == 42);
    REQUIRE(String::To<float>("3.14") == Approx(3.14f));
    REQUIRE(String::To<double>("2.718") == Approx(2.718));

    REQUIRE(String::To<int>(std::string("42")) == 42);
    REQUIRE(String::To<float>(std::string("3.14")) == Approx(3.14f));

    REQUIRE_THROWS(String::To<int>(""));
    REQUIRE_THROWS(String::To<float>("abc"));
    REQUIRE_THROWS(String::To<int>("1.2.3"));
}*/

TEST_CASE("Gorgon::String - From<T> Conversion") {
    REQUIRE(String::From(42) == "42");
    REQUIRE(String::From(3.14f).substr(0, 4) == "3.14");
    REQUIRE(String::From(true) == "1");
    REQUIRE(String::From(false) == "0");
}

TEST_CASE("Gorgon::String - Replace Function") {
    REQUIRE(String::Replace("hello world, world!", "world", "Earth") == "hello Earth, Earth!");
    REQUIRE(String::Replace("Hello World", "world", "Earth") == "Hello World");
    REQUIRE(String::Replace("aaaaa", "a", "ab") == "ababababab");
}

/*TEST_CASE("Gorgon::String - Case Conversion") {
    REQUIRE(String::ToUpper("çalışma") == "ÇALIŞMA");
    REQUIRE(String::ToLower("İSTANBUL") == "istanbul");
}*/

TEST_CASE("Gorgon::String - Extract Function") {
    std::string data = "this is a test text";
    REQUIRE(String::Extract(data, ' ') == "this");
    REQUIRE(data == "is a test text");

    data = "one,two,three";
    REQUIRE(String::Extract(data, ',') == "one");
    REQUIRE(String::Extract(data, ',') == "two");
    REQUIRE(String::Extract(data, ',') == "three");
}

TEST_CASE("Gorgon::String - Tokenizer Functionality") {
    std::string data = "this is a test";
    String::Tokenizer tokenizer(data, " ");
    std::vector<std::string> expected = {"this", "is", "a", "test"};

    size_t i = 0;
    for (; tokenizer.IsValid(); tokenizer.Next()) {
        REQUIRE(*tokenizer == expected[i]);
        ++i;
    }
}

TEST_CASE("Gorgon::String - FixLineEndings") {
    REQUIRE(String::FixLineEndings("--\x0d--\x0a--\x0d\x0a--") == "--\x0d\x0a--\x0d\x0a--\x0d\x0a--");
    REQUIRE(String::FixLineEndings("--\x0d--\x0a--\x0d\x0a--", String::LineEnding::LF) == "--\x0a--\x0a--\x0a--");
    REQUIRE(String::FixLineEndings("--\x0d--\x0a--\x0d\x0a--", String::LineEnding::CR) == "--\x0d--\x0d--\x0d--");
    REQUIRE(String::FixLineEndings("--\x0d--\x0a--\x0d\x0a--", String::LineEnding::None) == "--------");
}
