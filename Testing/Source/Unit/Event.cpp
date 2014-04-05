#include <iostream>

#include <Source/Event.h>

class A {
public:
	explicit A(int a, float=0.f) : a(a) { }

	//A(const A&) =delete;
	//A &operator=(const A&)=delete;

	void print() { std::cout<<a<<std::endl; }

	void p(int i, int j) { std::cout<<a<<" "<<i<<" "<<j<<std::endl; }

	int a;
};

void fn(A &a, int i,int j) {
	//std::cout<<a.a<<" event: "<<i<<" "<<j<<std::endl;
	std::cout<<"working..."<<std::endl;
}

int main() {
	A a(2), b(5);
	Gorgon::Event<A, int, int> event(a);

	a.a=5;

	std::function<void(A&,int,int)> f=fn;
	event.Register(b, &A::p);

	event(3, 4);
}