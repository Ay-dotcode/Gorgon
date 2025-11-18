#pragma once

#include <assert.h>

#include "Event.h"
#include "Containers/Collection.h"

namespace Gorgon { 
	
	/// This namespace contains general input related functionality.
	/// Also check Gorgon::Keyboard and Gorgon::Mouse.
	namespace Input {
		/// A type to represent an input key
		typedef int Key;
        
        
        /// During keyevent this variable can be set to true, if done so, it
        /// will allow character events even if key event is handled. However,
        /// doing so will not direct the up event to the handler directly.
        extern bool AllowCharEvent;
		
	}
}
