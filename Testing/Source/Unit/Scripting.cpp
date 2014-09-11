#define CATCH_CONFIG_MAIN

#define WINDOWS_LEAN_AND_MEAN

#include <catch.h>

#include <Source/Scripting/Reflection.h>


using namespace Gorgon::Scripting;
using namespace Gorgon;


TEST_CASE("Basic scripting", "[firsttest]") {
	
	Type mytype;
	
	
	Parameter param1("name", mytype, "heeelp", OutputTag);
	
	REQUIRE(param1.IsReference());
	
	REQUIRE(param1.IsOutput());
	
	REQUIRE_FALSE(param1.IsInput());
	
	
	Parameter param2("name2", mytype, "heeelp", OptionalTag);
	
	REQUIRE_FALSE(param2.IsReference());
	
	REQUIRE(param2.IsOptional());
	
	
	Parameter param3("name2", mytype, "heeelp", {1, 2, 3}, OptionalTag);
	
	REQUIRE_FALSE(param3.IsReference());
	
	REQUIRE(param3.IsOptional());
	
	
	Parameter param4("name2", mytype, "heeelp",{}, {OptionalTag});
	
	REQUIRE_FALSE(param4.IsReference());
	
	REQUIRE(param4.IsOptional());
	
	
}
