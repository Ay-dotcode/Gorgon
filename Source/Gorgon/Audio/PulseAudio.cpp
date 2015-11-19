#include "../Audio.h"
#include <pulse/pulseaudio.h>

namespace Gorgon { namespace Audio {
	
	pa_mainloop *pa_main = nullptr;
	pa_context  *pa_ctx  = nullptr;
	
	void waitpa(pa_mainloop *pam) {
		//while(pastate==pa_waiting) {
		//	pa_mainloop_iterate(pam,0,NULL);
		//}
	}

	
	void pa_state_cb(pa_context *ctx, void *) {
		
	}
	
	void Initialize() {
		pa_main = 
	}
	
} }