#pragma once

#include "../Enum.h"

namespace Gorgon { namespace Audio {
	/// Sample format. For now only Float will be used and all conversions are done
	/// by the underlying library.
	enum class Format {
		PCM8,
		PCM16,
		Float
	};	
	
	/// Names for channels
	enum class Channel {
		Unknown,
		Mono,
		FrontLeft,
		FrontRight,
		BackLeft,
		BackRight,
		Center,
		LowFreq
	};
    
    DefineEnumStrings(Channel, 
        { Channel::Unknown    , "Unknown"     },
        { Channel::Mono       , "Mono"        },
        { Channel::FrontLeft  , "Front left"  },
        { Channel::FrontRight , "Front right" },
        { Channel::BackLeft   , "Back left"   },
        { Channel::BackRight  , "Back right"  },
        { Channel::Center     , "Center"      },
        { Channel::LowFreq    , "Bass"        },
    );
} }
