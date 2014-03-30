#define CATCH_CONFIG_MAIN

#define WINDOWS_LEAN_AND_MEAN

#include <catch.h>
#include <Source/Geometry/Point.h>

#undef Rectangle

using namespace Gorgon::Geometry;

TEST_CASE( "Point constructors", "[Point]" ) {

	Point  p1(1.2, 2.2);
	Pointf p2(1.2, 2.2);
	Point  p3(p2);
	Pointf p4(p3);
	Point  p5;
	Pointf p6;
	Pointf p7;

	p5=p2;
	p6=p2;
	p7=p1;

	REQUIRE( p1.X == 1);
	REQUIRE( p1.Y == 2);

	REQUIRE( p2.X == 1.2f);
	REQUIRE( p2.Y == 2.2f);

	REQUIRE( p3.X == 1);
	REQUIRE( p3.Y == 2);

	REQUIRE( p4.X == 1);
	REQUIRE( p4.Y == 2);

	REQUIRE( p5.X == 1);
	REQUIRE( p5.Y == 2);

	REQUIRE( p6.X == 1.2f);
	REQUIRE( p6.Y == 2.2f);

	REQUIRE( p7.X == 1);
	REQUIRE( p7.Y == 2);
}

TEST_CASE( "Point comparison", "[Point]") {
	Point  p1(1, 2);
	Pointf p2(1.2f, 2.2f);
	Point  p3(1, 2);
	Pointf p4(1.2f, 2.2f);
	Point  p5(3, 4);
	Pointf p6(3.3f, 7.1f);

	REQUIRE( p1 == p3 );
	REQUIRE( p2 == p4 );

	REQUIRE_FALSE( p1 == p5 );
	REQUIRE_FALSE( p2 == p6 );

	REQUIRE_FALSE( p1 != p3 );
	REQUIRE_FALSE( p2 != p4 );

	REQUIRE( p1 != p5 );
	REQUIRE( p2 != p6 );
}

TEST_CASE( "Point arithmetic", "[Point]" ) {

	Point  p1(1, 2);
	Pointf p2(1.2f, 2.2f);
	Point  p3(5, 6);
	Pointf p4(5.5f, 6.8f);

	Point  p5;
	Pointf p6;

	p5=p1+p3;
	REQUIRE( p5 == Point(6,8) );

	p5=p1-p3;
	REQUIRE( p5 == Point(-4,-4) );

	p5=p1*2;
	REQUIRE( p5 == Point(2,4) );

	p5=p1/2;
	REQUIRE( p5 == Point(0,1) );

	p5=p1*1.5;
	REQUIRE( p5 == Point(1,3) );

	p5=p1/0.5;
	REQUIRE( p5 == Point(2,4) );

	p6=p2+p4;
	REQUIRE( p6 == Pointf(6.7f,9.0f) );

	p6=p2-p4;
	REQUIRE( Approx(p6.Distance(Pointf(-4.3f,-4.6f))) == 0 );

	p6=p2*2;
	REQUIRE( p6 == Pointf(2.4f,4.4f) );

	p6=p2/2;
	REQUIRE( p6 == Pointf(0.6f,1.1f) );

	p6=p2*1.5;
	REQUIRE( Approx(p6.Distance(Pointf(1.8f,3.3f))) == 0 );

	p6=p2/0.5;
	REQUIRE( p6 == Pointf(2.4f,4.4f) );



	p5=p1;
	p5+=p3;
	REQUIRE( p5 == Point(6,8) );

	p5=p1;
	p5-=p3;
	REQUIRE( p5 == Point(-4,-4) );

	p5=p1;
	p5*=2;
	REQUIRE( p5 == Point(2,4) );

	p5=p1;
	p5/=2;
	REQUIRE( p5 == Point(0,1) );

	p5=p1;
	p5*=1.5;
	REQUIRE( p5 == Point(1,3) );

	p5=p1;
	p5/=0.5;
	REQUIRE( p5 == Point(2,4) );

	p6=p2;
	p6+=p4;
	REQUIRE( p6 == Pointf(6.7f,9.0f) );

	p6=p2;
	p6-=p4;
	REQUIRE( Approx(p6.Distance(Pointf(-4.3f,-4.6f))) == 0 );

	p6=p2;
	p6*=2;
	REQUIRE( p6 == Pointf(2.4f,4.4f) );

	p6=p2;
	p6/=2;
	REQUIRE( p6 == Pointf(0.6f,1.1f) );

	p6=p2;
	p6*=1.5;
	REQUIRE( Approx(p6.Distance(Pointf(1.8f,3.3f))) == 0 );

	p6=p2;
	p6/=0.5;
	REQUIRE( p6 == Pointf(2.4f,4.4f) );


	p5=-p1;
	REQUIRE( p5 == Point(-1,-2) );

	p6=-p2;
	REQUIRE( p6 == Pointf(-1.2,-2.2) );
}


