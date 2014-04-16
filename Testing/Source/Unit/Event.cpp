#include <iostream>

#include <Source/Event.h>
#include <Source/WindowManager.h>
#include <Source/Window.h>
#include <Source/OS.h>

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

	event.Register(b, &A::p);
	event.Register(fn);
	auto token=event.EmptyToken;
	token=event.Register([&]{ 
		std::cout<<"working..."<<std::endl;
		event.Unregister(token);
		token=event.EmptyToken;
	});

	event(3, 4);
	
	event(2, 2);
	
	//Gorgon::OS::Initialize();
	
	
	std::cout<<Gorgon::OS::User::GetUsername()<<std::endl;
	
	Gorgon::WindowManager::Initialize();
	std::cout<<std::boolalpha;
	std::cout<<"Run: "<<Gorgon::OS::Open("http://darkgaze.org")<<std::endl;
	
	Gorgon::Window win({20, 20, 200, 300}, "My wind");
	//win.HidePointer();
	//win.Resize({400,500});
	
	std::cin.sync();
	std::cin>>a.a;
}