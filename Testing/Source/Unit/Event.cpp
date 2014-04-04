#include <Source/Event.h>

class A {
};

int main() {
	A a;
	Gorgon::Event<A> event(a);
	
	
}