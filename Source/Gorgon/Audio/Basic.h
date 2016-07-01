#pragma once

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
} }
