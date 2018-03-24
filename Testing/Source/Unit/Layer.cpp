#define CATCH_CONFIG_MAIN
#include <catch.h>

#undef TEST

#include <Gorgon/Main.h>
#include <Gorgon/Layer.h>
#include <Gorgon/Window.h>
#include <Gorgon/Graphics/Layer.h>

using namespace Gorgon;
using namespace Gorgon::Geometry;

TEST_CASE("Layer base", "[Layer]") {

	//Initialize("Unittestlayer");


	Layer l1;

	l1.Resize(200, 300);

	REQUIRE(l1.GetWidth() == 200);

	REQUIRE(l1.GetHeight() == 300);

	REQUIRE(l1.GetSize() == Size(200, 300));

	REQUIRE(l1.GetLeft() == 0);

	REQUIRE(l1.GetTop() == 0);

	REQUIRE(l1.GetLocation() == Point(0, 0));

	REQUIRE(l1.GetEffectiveBounds() == Bounds(0, 0, 200, 300));



	Layer l2;

	REQUIRE(l1.Children.GetCount() == 0);

	l1.Add(l2);

	REQUIRE(l1.Children.GetCount() == 1);

	REQUIRE(l2.GetSize() == Size(0, 0));

	REQUIRE(l2.GetEffectiveBounds() == Bounds(0, 0, 200, 300));

	l1.Move(10, 15);

	REQUIRE(l1.GetEffectiveBounds() == Bounds(10, 15, 210, 315));

	REQUIRE(l2.GetEffectiveBounds() == Bounds(0, 0, 200, 300));

	l2.Move(20, 25);

	REQUIRE(l2.GetEffectiveBounds() == Bounds(20, 25, 200, 300));

	l2.Resize(40, 50);

	REQUIRE(l2.GetEffectiveBounds() == Bounds(20, 25, 60, 75));

	REQUIRE(l2.GetEffectiveBounds().GetSize() == Size(40, 50));



	Layer l3;

	l1.Add(l3);

	l3.Resize(60, 70);

	REQUIRE(l3.GetEffectiveBounds() == Bounds(0, 0, 60, 70));

	l3.Move(30, 35);

	REQUIRE(l3.GetEffectiveBounds() == Bounds(30, 35, 90, 105));

	REQUIRE(l3.GetOrder() == 1);
}

TEST_CASE("Graphics layer", "[Layer]") {

	//Initialize("Unittestlayer");


	Graphics::Layer l1;

	l1.Resize(200, 300);

	REQUIRE(l1.GetWidth() == 200);

	REQUIRE(l1.GetHeight() == 300);

	REQUIRE(l1.GetSize() == Size(200, 300));

	REQUIRE(l1.GetTargetSize() == Size(200, 300));

	REQUIRE(l1.GetLeft() == 0);

	REQUIRE(l1.GetTop() == 0);

	REQUIRE(l1.GetLocation() == Point(0, 0));

	REQUIRE(l1.GetEffectiveBounds() == Bounds(0, 0, 200, 300));



	Graphics::Layer l2;

	REQUIRE(l1.Children.GetCount() == 0);

	l1.Add(l2);

	REQUIRE(l1.Children.GetCount() == 1);

	REQUIRE(l2.GetSize() == Size(0, 0));

	REQUIRE(l2.GetEffectiveBounds() == Bounds(0, 0, 200, 300));

	REQUIRE(l2.GetTargetSize() == Size(200, 300));

	l1.Move(10, 15);

	REQUIRE(l1.GetEffectiveBounds() == Bounds(10, 15, 210, 315));

	REQUIRE(l2.GetEffectiveBounds() == Bounds(0, 0, 200, 300));

	l2.Move(20, 25);

	REQUIRE(l2.GetEffectiveBounds() == Bounds(20, 25, 200, 300));

	REQUIRE(l2.GetTargetSize() == Size(180, 275));

	l2.Resize(40, 50);

	REQUIRE(l2.GetEffectiveBounds() == Bounds(20, 25, 60, 75));

	REQUIRE(l2.GetEffectiveBounds().GetSize() == Size(40, 50));

	REQUIRE(l2.GetTargetSize() == Size(40, 50));



	Graphics::Layer l3;

	l1.Add(l3);

	l3.Resize(60, 70);

	REQUIRE(l3.GetEffectiveBounds() == Bounds(0, 0, 60, 70));

	l3.Move(30, 35);

	REQUIRE(l3.GetEffectiveBounds() == Bounds(30, 35, 90, 105));

	REQUIRE(l3.GetOrder() == 1);

	REQUIRE(l3.GetTargetSize() == Size(60, 70));

	Layer l4;

	l1.Insert(l4, 1);

	REQUIRE(l4.GetOrder() == 1);
}

TEST_CASE("Window", "[Layer]") {

	Initialize("Unittestlayer");

	Window l1({200, 300}, "Test");

	REQUIRE(l1.GetWidth() == 200);

	REQUIRE(l1.GetHeight() == 300);

	REQUIRE(l1.GetSize() == Size(200, 300));

	REQUIRE(l1.GetLeft() == 0);

	REQUIRE(l1.GetTop() == 0);

	REQUIRE(l1.GetLocation() == Point(0, 0));

	REQUIRE(l1.GetEffectiveBounds() == Bounds(0, 0, 200, 300));

	l1.Move(150, 100);
    
#ifdef X11
    //wait a little
    for(auto i=0; i<10; i++) {
        NextFrame();
        usleep(5000);
    }
#endif

    //Window manager might reposition the window as it wants,
    //but should get it close to where we want.
	REQUIRE(l1.GetPosition().Distance(Point(150, 100)) < 20);

	//moving window should not move it as a layer, window (0,0) is always
	//the start of the window layer.
	REQUIRE(l1.GetLocation() == Point(0, 0));



	Graphics::Layer l2;

	l1.Add(l2);

	REQUIRE(l2.GetSize() == Size(0, 0));

	REQUIRE(l2.GetEffectiveBounds() == Bounds(0, 0, 200, 300));

	REQUIRE(l2.GetTargetSize() == Size(200, 300));

	REQUIRE(l2.GetEffectiveBounds() == Bounds(0, 0, 200, 300));

	l2.Move(20, 25);

	REQUIRE(l2.GetEffectiveBounds() == Bounds(20, 25, 200, 300));

	REQUIRE(l2.GetTargetSize() == Size(180, 275));

	l2.Resize(40, 50);

	REQUIRE(l2.GetEffectiveBounds() == Bounds(20, 25, 60, 75));

	REQUIRE(l2.GetEffectiveBounds().GetSize() == Size(40, 50));

	REQUIRE(l2.GetTargetSize() == Size(40, 50));



	Graphics::Layer l3;

	l1.Add(l3);

	l3.Resize(60, 70);

	REQUIRE(l3.GetEffectiveBounds() == Bounds(0, 0, 60, 70));

	l3.Move(30, 35);

	REQUIRE(l3.GetEffectiveBounds() == Bounds(30, 35, 90, 105));

	REQUIRE(l3.GetOrder() == 1);

	REQUIRE(l3.GetTargetSize() == Size(60, 70));

	Layer l4;

	l1.Insert(l4, 1);

	REQUIRE(l4.GetOrder() == 1);
}
