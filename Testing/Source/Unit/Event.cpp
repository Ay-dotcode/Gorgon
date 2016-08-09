#define CATCH_CONFIG_MAIN

#include <catch.h>
#include <typeinfo>

#include <iostream>

#include <Gorgon/Event.h>


using namespace Gorgon;

int sum = 0;

void fn(int i, int j) {
	sum += i + j;
}

void fn2() {
    sum++;
}

class C1 {
public:
    int x = 1;
    
    void fn() {
        sum += x;
    }
};

class C2 {
public:
    Event<C2> MakeEv() {
        Event<C2> ev(*this);
        t = ev.Register(*this, &C2::fired);
        myev = &ev;
        
        return ev;
    }
    
    void fired() {
        x++;
        myev->Unregister(t);
    }
    
    int x = 5;
    Event<C2>::Token t;
    Event<C2> *myev;
};

TEST_CASE("Event") {
	Event<void, int, int> ev;
    
    auto tok = ev.Register(&fn);
    
    REQUIRE(tok != ev.EmptyToken);
    
    ev(2, 3);
    
    REQUIRE(sum == 5);
    
    ev.Register(&fn2);
    
    ev(0, 1);
    
    REQUIRE(sum == 7);
    
    ev.Unregister(tok);
    
    ev(1, 1);
    
    REQUIRE(sum == 8);
    
    C1 c1;
    
    auto tok2 = ev.Register(c1, &C1::fn);
    
    ev(0, 0);
    
    REQUIRE(sum == 10);
    c1.x = 9;
    
    ev(0, 0);
    REQUIRE(sum == 20);
    
    ev.Unregister(tok2);
    
    ev(0, 0);
    REQUIRE(sum == 21);
    
    C2 c2;
    auto ev2 = c2.MakeEv();
    
    ev2.Register(&fn2);
    ev2();
    
    REQUIRE(sum == 22);
    REQUIRE(c2.x == 6);
    
    c1.x = 1;
    
    ev2.Register(c1, &C1::fn);
    ev2.Register([&]() {
        sum+=3;
    });
    
    ev2();
    
    REQUIRE(sum == 27);
    
    REQUIRE(c2.x == 6);
}
