#define CATCH_CONFIG_MAIN

#define WINDOWS_LEAN_AND_MEAN

#include <catch.h>

#include <Source/Scripting/Reflection.h>


using namespace Gorgon::Scripting;
using namespace Gorgon;


TEST_CASE("Basic scripting", "[firsttest]") {
	
	Type mytype;
	
	
	const Parameter param1("name", "heeelp", mytype, OutputTag);
	
	REQUIRE(param1.GetName() == "name");
	REQUIRE(param1.GetHelp() == "heeelp");
	REQUIRE(&param1.GetType() == &mytype);
	
	REQUIRE(param1.IsReference());
	
	REQUIRE(param1.IsOutput());
	
	REQUIRE_FALSE(param1.IsInput());
	
	
	const Parameter param2("name2", "heeelp", mytype, OptionalTag);
	
	REQUIRE_FALSE(param2.IsReference());
	
	REQUIRE(param2.IsOptional());
	
	
	const Parameter param3("name2", "heeelp", mytype,  {1, 2, 3}, OptionalTag);
	
	REQUIRE_FALSE(param3.IsReference());
	
	REQUIRE(param3.IsOptional());
	
	
	const Parameter param4("name2", "heeelp", mytype, {}, {OptionalTag});
	
	REQUIRE_FALSE(param4.IsReference());
	
	REQUIRE(param4.IsOptional());
	
	const Function fn1 { "TestFn",
		"This is a test function bla bla bla",
		{
			new Parameter{ "Name",
				"This parameter is bla bla",
				mytype, OutputTag
			},
			new Parameter{ "Second",
				"This. ....",
				mytype, OptionalTag
			}
		},
		//returns nothing
		MethodTag, StretchTag
	};
	
	REQUIRE(fn1.GetName()=="TestFn");
	REQUIRE(fn1.GetHelp()=="This is a test function bla bla bla");
	REQUIRE(fn1.Parameters.GetCount()==2);
	REQUIRE(fn1.Parameters[0].GetName()=="Name");
	REQUIRE_FALSE(fn1.HasReturnType());
	REQUIRE(fn1.HasMethod());
	REQUIRE(fn1.StretchLast());
	REQUIRE_FALSE(fn1.IsKeyword());
}
