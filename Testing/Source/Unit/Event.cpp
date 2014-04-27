#include <iostream>

#include <Source/Event.h>
#include <Source/WindowManager.h>
#include <Source/Window.h>
#include <Source/OS.h>
#include <Source/Resource/GID.h>
#include <Source/SGuid.h>

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
	//std::cout<<"Run: "<<Gorgon::OS::Open("http://darkgaze.org")<<std::endl;
	
	Gorgon::Window win({50,20,200, 300}, "mywind", "My window");
	win.Move({300, 200});
	win.ActivatedEvent.Register([] {std::cout<<"activated"<<std::endl; });
	win.DeactivatedEvent.Register([] {std::cout<<"deactivated"<<std::endl; });
	win.DestroyedEvent.Register([]{std::cout<<"destroyed"<<std::endl;exit(0);});
	win.CharacterEvent.Register([](Gorgon::Keyboard::Char c) { std::cout<<c<<std::endl; });
	win.KeyEvent.Register([](Gorgon::Input::Key k, float amount) -> bool {
		std::cout<<k<<": "<<amount<<std::endl;
		return k=='c';
	});
	//auto t=win.KeyEvent.Register([](Gorgon::Input::Key k, float amount) -> bool {
	//	std::cout<<"> "<<k<<": "<<amount<<std::endl;
	//	return false;
	//});
	//win.KeyEvent.Activate(t);
	//win.Resize({400,500});
	
	while(true) {
		Gorgon::OS::processmessages();
	}
}
