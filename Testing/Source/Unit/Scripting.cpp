#define CATCH_CONFIG_MAIN

#define WINDOWS_LEAN_AND_MEAN

#include <catch.h>

#include <Source/Scripting/Embedding.h>
#include <Source/TMP.h>


using namespace Gorgon::Scripting;
using namespace Gorgon;

int checktestfn=0;

void TestFn(int a, int b=1) {
	checktestfn+=a*b;
}

void TestFn_1(int a) {
	TestFn(a);
}

class A {
public:
	int a() {
		return 42;
	}
	
	int b(float a) {
		return 42*a;
	}
};


TEST_CASE("Basic scripting", "[firsttest]") {
	
	auto mytype=new Type("mytype", "test type", Any(0));
	auto myfloattype=new Type("myfloattype", "test type", Any(0.0f));
	auto myvaluetype = new Type("myvaluetype", "test type", Any(A()));
	auto myreftype=new Type("myreftype", "test type", Any((A*)nullptr), ReferenceTag);
	
	
	const Parameter param1("name", "heeelp", mytype, OutputTag);
	
	REQUIRE(param1.GetName() == "name");
	REQUIRE(param1.GetHelp() == "heeelp");
	REQUIRE(&param1.GetType() == mytype);
	
	REQUIRE(param1.IsReference());
	
	REQUIRE(param1.IsOutput());
	
	REQUIRE_FALSE(param1.IsInput());
	
	
	const Parameter param2("name2", "heeelp", mytype, OptionalTag);
	
	REQUIRE_FALSE(param2.IsReference());
	
	REQUIRE(param2.IsOptional());
	
	
	const Parameter param3("name2", "heeelp", mytype,  {Any(1), Any(2), Any(3)}, OptionalTag);
	
	REQUIRE_FALSE(param3.IsReference());
	
	REQUIRE(param3.IsOptional());
	
	
	const Parameter param4("name2", "heeelp", mytype, {}, {OptionalTag});
	
	REQUIRE_FALSE(param4.IsReference());
	
	REQUIRE(param4.IsOptional());
	
	const MappedFunction fn1{ "TestFn",
		"This is a test function bla bla bla",
		nullptr, //return type
		ParameterList{
			new Parameter{ "Name",
				"This parameter is bla bla",
				mytype, OptionalTag
			},
			new Parameter{ "Second",
				"This is the multiplier",
				mytype, OptionalTag
			}
		},
		nullptr, // not a member function
		MappedFunctions(&TestFn, &TestFn_1, []{TestFn(1);}), MappedMethods(TestFn, TestFn_1, []{TestFn(1);}),
		StretchTag, MethodTag
	};
	
	REQUIRE(fn1.GetName()=="TestFn");
	REQUIRE(fn1.GetHelp()=="This is a test function bla bla bla");
	REQUIRE(fn1.Parameters.GetCount()==2);
	REQUIRE(fn1.Parameters[0].GetName()=="Name");
	REQUIRE_FALSE(fn1.HasReturnType());
	//REQUIRE_FALSE(fn1.HasMethod());
	REQUIRE(fn1.StretchLast());
	REQUIRE_FALSE(fn1.IsKeyword());
	
	std::vector<Data> v={{mytype, 5}, {mytype, 2}};
	fn1.Call(false, v);
	
	REQUIRE(checktestfn == 10);
	
	
	std::vector<Data> v2={{mytype, 5}};
	fn1.Call(false, v2);
	
	REQUIRE(checktestfn == 15);

	fn1.Call(true, {});
	REQUIRE(checktestfn == 16);
	
	
	MappedFunction fn2{ "TestFn2",
		"This is a test function bla bla bla",
		mytype, //return type
		ParameterList{
			new Parameter{ "Name",
				"This parameter is bla bla",
				myreftype
			}
		},
		nullptr, // memberof
		MappedFunctions(&A::a), MappedMethods()
	};
	
	REQUIRE( fn2.Call(false, { {myreftype, new A()} }).GetValue<int>() == 42 );
	
	MappedFunction fn3{ "TestFn3",
		"This is a test function bla bla bla",
		mytype, //return type
		ParameterList{
			new Parameter{ "Name",
				"This parameter is bla bla",
				myvaluetype
			}
		},
		nullptr, // member of
		MappedFunctions(&A::a), MappedMethods()
	};
	
	REQUIRE( fn3.Call(false, {{myvaluetype, A()}}).GetValue<int>() == 42 );
	
	
	MappedFunction fn4{ "TestFn4",
		"This is a test function bla bla bla",
		mytype, //return type
		ParameterList{
			new Parameter{ "Name",
				"This parameter is bla bla",
				myfloattype
			}
		},
		myvaluetype, //member of
		MappedFunctions(&A::b), MappedMethods()
	};
	
	myvaluetype->AddFunctions({&fn4});
	
	REQUIRE( fn4.Call(false, { {mytype, A()}, {myfloattype, 2.0f} }).GetValue<int>() == 84 );
	
	
	MappedFunction fn5{ "TestFn5",
		"This is a test function bla bla bla",
		mytype, //return type
		ParameterList{
			new Parameter{ "Name",
				"This parameter is bla bla",
				myfloattype
			}
		},
		myreftype, //member of
		MappedFunctions(&A::b), MappedMethods()
	};
	
	myreftype->AddFunctions({&fn5});
	
	REQUIRE( fn5.Call(false, { {mytype, new A()}, {myfloattype, 1.0f} }).GetValue<int>() == 42 );
	REQUIRE_THROWS( fn5.Call(false, { {mytype, A()}, {myfloattype, 1.0f} }).GetValue<int>());
	REQUIRE_THROWS( fn5.Call(false, { {mytype, A()}, {mytype, 1} }).GetValue<int>());
}
