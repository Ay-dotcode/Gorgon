#define CATCH_CONFIG_MAIN

#define WINDOWS_LEAN_AND_MEAN

#include <catch.h>

#include <Source/Scripting/Embedding.h>
#include <Source/TMP.h>
#include "Source/Geometry/Point.h"


using namespace Gorgon::Scripting;
using namespace Gorgon;
using Gorgon::Geometry::Point;

int checktestfn=0;

void TestFn(int a, int b=1) {
	checktestfn+=a*b;
}

void TestFn_1(int a) {
	TestFn(a);
}

class A {
public:
	A() { }
	explicit A(const std::string &) {}
	
	int a() {
		return 42;
	}
	
	int b(float a) {
		return 42*a;
	}
	
	explicit operator std::string() const {
		return "A";
	}
	
	int bb;
};


TEST_CASE("Basic scripting", "[firsttest]") {
	
	Scripting::Initialize();
	VirtualMachine vm;
	vm.Activate();
	auto myfloattype=new MappedValueType<float>("myfloattype", "test type");
	auto myvaluetype = new MappedValueType<A>("myvaluetype", "test type");
	auto myreftype=new MappedReferenceType<A>("myreftype", "test type");
	auto mypointtype = MappedValueType<Point>("mypointtype", "test type");

	mypointtype.AddDataMembers({
		new MappedData<Point, int>(&Point::X, "x", "field storing location on x coordinate", Integrals.Types["Int"]),
		new MappedData<Point, int>(&Point::Y, "y", "field storing location on y coordinate", Integrals.Types["Int"])
	});
	Data pointdatatest = { mypointtype, Any(Point{1, 1}) };
	
	REQUIRE(pointdatatest.GetValue<Point>().X == 1);
	REQUIRE(pointdatatest.GetValue<Point>().Y == 1);

	int testval=0;
	myvaluetype->AddDataMembers({
		new MappedData  <A, int>(&A::bb, "bb", "bb is bla bla", Integrals.Types["Int"]),
		new DataAccessor<A, int>([](const A &a) { return 5; }, [&testval](A &a, int b) {testval=b;}, "cc", "dgfsf", Integrals.Types["Int"])
	});
	
	Data datatest = { myvaluetype, Any(A()) };
	myvaluetype->DataMembers["bb"].Set(datatest, Data(Integrals.Types["Int"], Any(4)));
	
	
	REQUIRE(datatest.GetValue<A>().bb == 4);	
	REQUIRE(myvaluetype->DataMembers["bb"].Get(datatest).GetValue<int>() == 4);
	
	
	myvaluetype->DataMembers["cc"].Set(datatest, Data(Integrals.Types["Int"], Any(4)));
	REQUIRE(testval == 4);
	REQUIRE(myvaluetype->DataMembers["cc"].Get(datatest).GetValue<int>() == 5);
	
	
	
	myreftype->AddDataMembers({
		new MappedData  <A*, int>(&A::bb, "bb", "bb is bla bla", Integrals.Types["Int"]),
		new DataAccessor<A*, int>([](const A &a) { return 6; }, [&testval](A &a, int b) {testval=b;}, "cc", "dgfsf", Integrals.Types["Int"])
	});
	
	Data datareftest = { myreftype, Any(new A()) };
	myreftype->DataMembers["bb"].Set(datareftest, Data(Integrals.Types["Int"], Any(4)));
	
	
	REQUIRE(datareftest.GetValue<A*>()->bb == 4);	
	REQUIRE(myreftype->DataMembers["bb"].Get(datareftest).GetValue<int>() == 4);
	
	myreftype->DataMembers["cc"].Set(datareftest, Data(Integrals.Types["Int"], Any(6)));
	REQUIRE(testval == 6);
	REQUIRE(myreftype->DataMembers["cc"].Get(datareftest).GetValue<int>() == 6);
	
	
	
	
	//const Parameter param1("name", "heeelp", Integrals.Types["Int"], OutputTag);
	//
	//REQUIRE(param1.GetName() == "name");
	//REQUIRE(param1.GetHelp() == "heeelp");
	//REQUIRE(&param1.GetType() == Integrals.Types["Int"]);
	//
	//REQUIRE(param1.IsReference());
	//
	//REQUIRE(param1.IsOutput());
	//
	//REQUIRE_FALSE(param1.IsInput());
	//
	//
	//const Parameter param2("name2", "heeelp", Integrals.Types["Int"], OptionalTag);
	//
	//REQUIRE_FALSE(param2.IsReference());
	//
	//REQUIRE(param2.IsOptional());
	//
	//
	//const Parameter param3("name2", "heeelp", Integrals.Types["Int"],  {Any(1), Any(2), Any(3)}, OptionalTag);
	//
	//REQUIRE_FALSE(param3.IsReference());
	//
	//REQUIRE(param3.IsOptional());
	//
	//
	//const Parameter param4("name2", "heeelp", Integrals.Types["Int"], {}, {OptionalTag});
	//
	//REQUIRE_FALSE(param4.IsReference());
	//
	//REQUIRE(param4.IsOptional());
	
	const MappedFunction fn1{ "TestFn",
		"This is a test function bla bla bla",
		nullptr, //return type
		nullptr, // not a member function
		ParameterList{
			new Parameter{ "Name",
				"This parameter is bla bla",
				Integrals.Types["Int"], OptionalTag
			},
			new Parameter{ "Second",
				"This is the multiplier",
				Integrals.Types["Int"], OptionalTag
			}
		},
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
	
	std::vector<Data> v={{Integrals.Types["Int"], 5}, {Integrals.Types["Int"], 2}};
	fn1.Call(false, v);
	
	REQUIRE(checktestfn == 10);
	
	
	std::vector<Data> v2={{Integrals.Types["Int"], 5}};
	fn1.Call(false, v2);
	
	REQUIRE(checktestfn == 15);

	fn1.Call(true, {});
	REQUIRE(checktestfn == 16);
	
	
	MappedFunction fn2{ "TestFn2",
		"This is a test function bla bla bla",
		Integrals.Types["Int"], //return type
		nullptr, // memberof
		ParameterList{
			new Parameter{ "Name",
				"This parameter is bla bla",
				myreftype
			}
		},
		MappedFunctions(&A::a), MappedMethods()
	};
	
	REQUIRE( fn2.Call(false, { {myreftype, new A()} }).GetValue<int>() == 42 );
	
	MappedFunction fn3{ "TestFn3",
		"This is a test function bla bla bla",
		Integrals.Types["Int"], //return type
		nullptr, // member of
		ParameterList{
			new Parameter{ "Name",
				"This parameter is bla bla",
				myvaluetype
			}
		},
		MappedFunctions(&A::a), MappedMethods()
	};
	
	REQUIRE( fn3.Call(false, {{myvaluetype, A()}}).GetValue<int>() == 42 );
	
	
	MappedFunction fn4{ "TestFn4",
		"This is a test function bla bla bla",
		Integrals.Types["Int"], //return type
		myvaluetype, //member of
		ParameterList{
			new Parameter{ "Name",
				"This parameter is bla bla",
				myfloattype
			}
		},
		MappedFunctions(&A::b), MappedMethods()
	};
	
	myvaluetype->AddFunctions({&fn4});
	
	REQUIRE( fn4.Call(false, { {myvaluetype, A()}, {myfloattype, 2.0f} }).GetValue<int>() == 84 );
	
	
	MappedFunction fn5{ "TestFn5",
		"This is a test function bla bla bla",
		Integrals.Types["Int"], //return type
		myreftype, //member of
		ParameterList{
			new Parameter{ "Name",
				"This parameter is bla bla",
				myfloattype
			}
		},
		MappedFunctions(&A::b), MappedMethods()
	};
	
	myreftype->AddFunctions({&fn5});
	
	REQUIRE( fn5.Call(false, { {myreftype, new A()}, {myfloattype, 1.0f} }).GetValue<int>() == 42 );
	REQUIRE_THROWS( fn5.Call(false, { {myvaluetype, A()}, {myfloattype, 1.0f} }).GetValue<int>());
	REQUIRE_THROWS( fn5.Call(false, { {myvaluetype, A()}, {Integrals.Types["Int"], 1} }).GetValue<int>());
	;
	
	VirtualMachine vm;
	REQUIRE( vm.FindConstant("Pi").GetData().GetValue<double>() == Approx(3.1416f) );
}
