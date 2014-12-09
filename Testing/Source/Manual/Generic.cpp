#include <Source/Utils/Assert.h>
#include <thread>
#include "../../../Source/Containers/Collection.h"

void testassert() {
	int a=5;
	int b=4;
	ASSERT(a+2 < 4 && b == a || b != 4, Gorgon::String::Concat("B should be ", a, " not ", b), 0, 2);
}

int main() {
	
	int a=4;
	int b;
	
	Gorgon::Containers::Collection<int> C;
	C.Push(a);
	
	std::cout<<C.Pop();
	std::cout<<": "<<C.GetCount()<<std::endl;
	
	testassert();
	
	return 0;
}
