#define CATCH_CONFIG_MAIN

#define WINDOWS_LEAN_AND_MEAN

#include <catch.h>
#include <Source/Geometry/Point.h>
#include <Source/Geometry/Size.h>
#include <Source/String.h>

#undef Rectangle

#pragma warning(disable:4305)
#pragma warning(disable:4244)

using namespace Gorgon::Geometry;

TEST_CASE( "Point constructors", "[Point]" ) {

	Point  p1(1.2, 2.2);
	Pointf p2(1.2, 2.2);
	Point  p3(p2);
	Pointf p4(p3);
	Point  p5;
	Pointf p6;
	Pointf p7;
	Point  p8=Point::CreateFrom(p1, 4, 0.785398f);   //45 degrees
	Pointf p9=Pointf::CreateFrom(p2, 4.1, 0.785398f); //45 degrees

	p5=p2;
	p6=p2;
	p7=p1;

	REQUIRE( p1.X == 1);
	REQUIRE( p1.Y == 2);

	REQUIRE( p2.X == Approx(1.2f) );
	REQUIRE( p2.Y == Approx(2.2f) );

	REQUIRE( p3.X == 1);
	REQUIRE( p3.Y == 2);

	REQUIRE( p4.X == Approx(1) );
	REQUIRE( p4.Y == Approx(2) );

	REQUIRE( p5.X == 1);
	REQUIRE( p5.Y == 2);

	REQUIRE( p6.X == Approx(1.2f) );
	REQUIRE( p6.Y == Approx(2.2f) );

	REQUIRE( p7.X == Approx(1) );
	REQUIRE( p7.Y == Approx(2) );

	REQUIRE( p8.X == 3);
	REQUIRE( p8.Y == 4);

	REQUIRE( p9.X == Approx(4.099138f) );
	REQUIRE( p9.Y == Approx(5.099138f) );

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

	REQUIRE( p1.Compare(p1) );
	REQUIRE( p1.Compare(p3) );
	REQUIRE_FALSE( p1.Compare(p5) );
	REQUIRE( p1 == p1 );
	REQUIRE_FALSE( p1 != p1 );

	REQUIRE( p2.Compare(p2) );
	REQUIRE( p2.Compare(p4) );
	REQUIRE_FALSE( p2.Compare(p6) );
	REQUIRE( p2 == p2 );
	REQUIRE_FALSE( p2 != p2 );
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
	REQUIRE( p6.X == Approx(6.7f) );
	REQUIRE( p6.Y == Approx(9.0f) );

	p6=p2-p4;
	REQUIRE( p6.X == Approx(-4.3f) );
	REQUIRE( p6.Y == Approx(-4.6f) );

	p6=p2*2;
	REQUIRE( p6.X == Approx(2.4f) );
	REQUIRE( p6.Y == Approx(4.4f) );

	p6=p2/2;
	REQUIRE( p6.X == Approx(0.6f) );
	REQUIRE( p6.Y == Approx(1.1f) );

	p6=p2*1.5;
	REQUIRE( p6.X == Approx(1.8f) );
	REQUIRE( p6.Y == Approx(3.3f) );

	p6=p2/0.5;
	REQUIRE( p6.X == Approx(2.4f) );
	REQUIRE( p6.Y == Approx(4.4f) );



	p5.Move(p1.X, p1.Y);
	REQUIRE( p5 == p1 );
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

	p6.Move(p2.X, p2.Y);
	REQUIRE( p6 == p2 );
	p6+=p4;
	REQUIRE( p6.X == Approx(6.7f) );
	REQUIRE( p6.Y == Approx(9.0f) );

	p6=p2;
	p6-=p4;
	REQUIRE( p6.X == Approx(-4.3f) );
	REQUIRE( p6.Y == Approx(-4.6f) );

	p6=p2;
	p6*=2;
	REQUIRE( p6.X == Approx(2.4f) );
	REQUIRE( p6.Y == Approx(4.4f) );

	p6=p2;
	p6/=2;
	REQUIRE( p6.X == Approx(0.6f) );
	REQUIRE( p6.Y == Approx(1.1f) );

	p6=p2;
	p6*=1.5;
	REQUIRE( p6.X == Approx(1.8f) );
	REQUIRE( p6.Y == Approx(3.3f) );

	p6=p2;
	p6/=0.5;
	REQUIRE( p6.X == Approx(2.4f) );
	REQUIRE( p6.Y == Approx(4.4f) );


	p5=-p1;
	REQUIRE( p5 == Point(-1,-2) );

	p6=-p2;
	REQUIRE( p6.X == Approx(-1.2f) );
	REQUIRE( p6.Y == Approx(-2.2f) );
}


TEST_CASE( "Point geometric info", "[Point]") {

	Point  p1(1, 2);
	Pointf p2(1.2f, 2.2f);
	Point  p3(5, 6);
	Pointf p4(5.5f, 6.8f);

	Point  p5;
	Pointf p6;

	REQUIRE( p1.Distance() == Approx(2.236068f) );
	REQUIRE( p2.Distance() == Approx(2.505992f) );

	REQUIRE( p1.Distance(p3) == Approx(5.656854f) );
	REQUIRE( p2.Distance(p4) == Approx(6.296824f) );

	REQUIRE( p1.Angle() == Approx(1.107149f) );
	REQUIRE( p2.Angle() == Approx(1.071450f) );

	REQUIRE( p1.Angle(p3) == Approx(-2.356195f) );
	REQUIRE( p2.Angle(p4) == Approx(-2.322499f) );

	REQUIRE( p1.Slope() == Approx(2.000000f) );
	REQUIRE( p2.Slope() == Approx(1.833333f) );

	REQUIRE( p1.Slope(p3) == Approx(1.000000f) );
	REQUIRE( p2.Slope(p4) == Approx(1.069767f) );
}

TEST_CASE( "Point <-> string", "[Point]") {
	
	Point  p1(1, 2);
	Pointf p2(1.2f, 2.2f);
	Point  p3;
	Pointf p4;

	std::stringstream ss;
	auto makestream=[&](std::string s) -> std::stringstream & {
		p3 = {0,0};
		p4 = {0,0};
		
		ss.str(s);
		ss.clear();
		
		return ss;
	};
	
	auto resetstream=[&] {
		p3 = {0,0};
		p4 = {0,0};
		
		ss.str("");
		ss.clear();
	};
	
	
	makestream("1,2")>>p3;
	REQUIRE( p1 == p3 );
	
	makestream("1.2,2.2")>>p3;
	REQUIRE( p1 == p3 );
	
	makestream("-1.2,-2.2")>>p3;
	REQUIRE( p1 == -p3 );
	
	makestream("(1,2)")>>p3;
	REQUIRE( p1 == p3 );
	
	makestream("(1.2,2.2)")>>p3;
	REQUIRE( p1 == p3 );
	
	makestream("(-1.2,-2.2)")>>p3;
	REQUIRE( p1 == -p3 );
	
	
	makestream("1, 2")>>p3;
	REQUIRE( p1 == p3 );
	
	makestream("1.2, 2.2")>>p3;
	REQUIRE( p1 == p3 );
	
	makestream("-1.2, -2.2")>>p3;
	REQUIRE( p1 == -p3 );
	
	makestream("(1, 2)")>>p3;
	REQUIRE( p1 == p3 );
	
	makestream("(1.2, 2.2)")>>p3;
	REQUIRE( p1 == p3 );
	
	makestream("(-1.2, -2.2)")>>p3;
	REQUIRE( p1 == -p3 );
	
	
	makestream("1,2")>>p3;
	REQUIRE( p1 == p3 );
	
	makestream("1.2,2.2")>>p3;
	REQUIRE( p1 == p3 );
	
	makestream("-1.2,-2.2")>>p3;
	REQUIRE( p1 == -p3 );
	
	
	makestream("(1,2) (1.2,2.2)")>>p3>>p4;
	REQUIRE( p1 == p3 );
	REQUIRE( p2 == p4 );
	
	makestream("(1.2,2.2) (1.2,2.2)")>>p3>>p4;
	REQUIRE( p1 == p3 );
	REQUIRE( p2 == p4 );
	
	makestream("(-1.2,-2.2) (-1.2,-2.2)")>>p3>>p4;
	REQUIRE( p1 == -p3 );
	REQUIRE( p2 == -p4 );

	
	resetstream();
	ss<<p1;
	REQUIRE( Gorgon::String::Replace(ss.str(), " ","") == "(1,2)" );

	resetstream();
	ss<<p2;
	REQUIRE( Gorgon::String::Replace(ss.str(), " ","") == "(1.2,2.2)" );
	
	resetstream();
	ss<<p1;
	makestream(ss.str())>>p3;
	REQUIRE( p1 == p3 );
	
	resetstream();
	ss<<p2;
	makestream(ss.str())>>p4;
	REQUIRE( p2 == p4 );
	
	REQUIRE( Gorgon::String::To<Point>("1,2") == p1 );
	REQUIRE( Gorgon::String::To<Point>("1.2,2.2") == p1 );
	REQUIRE( Gorgon::String::To<Pointf>("1.2,2.2") == p2 );
	
	REQUIRE( Gorgon::String::To<Point>("(1,2)") == p1 );
	REQUIRE( Gorgon::String::To<Point>("(1.2,2.2)") == p1 );
	REQUIRE( Gorgon::String::To<Pointf>("(1.2,2.2)") == p2 );
	
	REQUIRE( Gorgon::String::To<Point>("(1, 2)") == p1 );
	REQUIRE( Gorgon::String::To<Point>("(1.2, 2.2)") == p1 );
	REQUIRE( Gorgon::String::To<Pointf>("(1.2, 2.2)") == p2 );
}
