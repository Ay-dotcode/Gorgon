#include <Source/Utils/Assert.h>
#include <thread>

void testassert() {
	int a=5;
	int b=4;
	ASSERT(a+2 < 4 && b == a || b != 4, Gorgon::String::Concat("B should be ", a, " not ", b), 0, 2);
}

int main() {
	
	int a;
	int b;
	
	testassert();
	
	return 0;
}
