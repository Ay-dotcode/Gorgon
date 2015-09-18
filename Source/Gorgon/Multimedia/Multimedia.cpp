#include "Multimedia.h"
#include "Music.h"


namespace gge { namespace multimedia {

	void Initialize(GGEMain &main) {
		//new thread that checks and processes buffers
		os::RunInNewThread([&]()->int{
			while(true) {
				StreamingMusic::CheckAll();
				//every buffer should at least have around 100ms of sound
				os::Sleep(50);
			}

			return 0;
		});
	}

}}

