#define CATCH_CONFIG_MAIN
#include <catch.h>

#include <Source/Containers/Hashmap.h>
#include <stdint.h>
#include <vector>
#include <algorithm>

using namespace Gorgon::Containers;
using std::string;


class A {
public:
	A() {
		acnt++;
	}
	
	~A() { acnt--; }
	
	A(const A&)=delete;
	
	A& operator =(const A&)=delete;
	
	int a = 0;

	static int acnt;
};

int A::acnt=0;

TEST_CASE("Hashmap", "[Hashmap]") {
	Hashmap<string, A> amap;
	
	A a1, a2;
	
	A &a3 = *new A();
	
	amap.Add("one", a1);
	amap.Add("two", a2);
	amap.Add("three", a3);
	
	REQUIRE(amap.GetCount() == 3);
	
	amap.Remove("two");
	
	REQUIRE(amap.GetCount() == 2);
	
	REQUIRE(A::acnt == 3);
	
	amap.Delete("three");
	
	REQUIRE(amap.GetCount() == 1);
	REQUIRE(A::acnt == 2);
	
	amap.RemoveAll();
	
	REQUIRE(A::acnt == 2);
	REQUIRE(amap.GetCount() == 0);
}

TEST_CASE("Hashmap iterators", "[Hashmap]") {
	Hashmap<string, A> amap;
	
	A a1, a2;
	
	A &a3 = *new A();
	
	amap.Add("one", a1);
	amap.Add("two", a2);
	amap.Add("three", a3); // this should be second item as "three"<"two"

	int i=0;
	for(auto it : amap) {
		switch(i) {
			case 0:
				REQUIRE( it.first=="one" );
				REQUIRE( &it.second == &a1 );
				break;
			case 1:
				REQUIRE( it.first=="three" );
				REQUIRE( &it.second == &a3 );
				(amap.begin()+1).SetItem(a1);
				REQUIRE( &(amap.begin()+1).Current().second==&a1 );
				break;
			case 2:
				REQUIRE( it.first=="two" );
				REQUIRE( &it.second == &a2 );
		}
		i++;
	}
}
