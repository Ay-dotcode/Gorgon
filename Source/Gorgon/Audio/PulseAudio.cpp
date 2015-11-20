#include "../Audio.h"

#include "../Main.h"

#include <pulse/pulseaudio.h>

namespace Gorgon { namespace Audio {
	///@cond Internal
	
	pa_mainloop *pa_main = nullptr;
	pa_context  *pa_ctx  = nullptr;

	enum { pa_waiting, pa_ready, pa_failed, pa_timeout } pa_state=pa_waiting;
	
	void pa_wait_connection(pa_mainloop *pam, unsigned long timeout = 1000) {
		auto start = Time::GetTime();
		while(pa_state == pa_waiting) {
			pa_mainloop_iterate(pam, 0, NULL);
			
			if(Time::GetTime()-start > timeout) {
				pa_state = pa_timeout;
				return;
			}
		}
	}


	void pa_state_cb(pa_context *ctx, void *userdata) {
		pa_context_state_t state;
		
		state = pa_context_get_state(ctx);
		switch  (state) {
			// There are just here for reference
			case PA_CONTEXT_UNCONNECTED:
			case PA_CONTEXT_CONNECTING:
			case PA_CONTEXT_AUTHORIZING:
			case PA_CONTEXT_SETTING_NAME:
			default:
				break;
			case PA_CONTEXT_FAILED:
			case PA_CONTEXT_TERMINATED:
				pa_state = pa_failed;
				break;
			case PA_CONTEXT_READY:
				pa_state=pa_ready;
				break;
		}
	}
	
	void Initialize() {
		Log << "Starting pulse audio initialization...";
		
		//ready structs
		pa_main = pa_mainloop_new();
		
		if(!pa_main) {
			Log << "Pulse audio main loop creation failed.";
			pa_state = pa_failed;
			return;
		}
		
		pa_ctx  = pa_context_new(pa_mainloop_get_api(pa_main), GetSystemName().c_str());
		
		if(!pa_ctx) {
			Log << "Pulse audio context creation failed.";
			pa_state = pa_failed;
			return;
		}
		
		//set call back
		pa_context_set_state_callback(pa_ctx, pa_state_cb, nullptr);
		
		//try connect
		pa_context_connect(pa_ctx, nullptr, PA_CONTEXT_NOFLAGS, nullptr);
		
		//wait for connection
		pa_wait_connection(pa_main
#ifdef PA_CONNECTION_TIMEOUT
			, PA_CONNECTION_TIMEOUT
#endif
		);
		
		if(pa_state == pa_failed) {
			Log << "Pulse audio connection failed: " << pa_strerror(pa_context_errno(pa_ctx));
			return;
		}
		
		if(pa_state == pa_timeout) {
			Log << "Pulse audio connection timed out";
			return;
		}
		
		Log << "Pulse audio is ready and available.";
	}
	
	bool IsAvailable() {
		return pa_state == pa_ready;
	}

	///@endcond
} }