#include <iostream>

#include <Source/Event.h>

class A {
public:
	explicit A(int a, float=0.f) : a(a) { }

	//A(const A&) =delete;
	//A &operator=(const A&)=delete;

	void print() { std::cout<<a<<std::endl; }

	void p(A &other,int i, int j) { std::cout<<other.a<<":::"<<a<<" "<<i<<" "<<j<<std::endl; }

	int a;
};

void fn(int i,int j) {
	std::cout<<" event: "<<i<<" "<<j<<std::endl;
	
}

int main() {
	A a(2), b(5);
	Gorgon::Event<A, int, int> event(a);

	a.a=15;

	auto token=event.Register(b, &A::p);
	event.Register(fn);
	event.Register([]{ std::cout<<"working..."<<std::endl; });

	event.Unregister(token);

	event(3, 4);
}