
#include "SGuid.h"

namespace Gorgon { 

	unsigned SGuid::serial = 0;
	const SGuid SGuid::Empty = SGuid();
	
	const SGuid::NewGuidType SGuid::CreateNew;

} 
