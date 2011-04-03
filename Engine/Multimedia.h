#pragma once


#include "GGEMain.h"
#include <dshow.h>
#include <atlbase.h>
#include "../Utils/Any.h"
#include "../Utils/EventChain.h"

namespace gge {
	////This class provides multimedia support, video or music files can be
	/// loaded and played with this.
	class Multimedia {
		IGraphBuilder *pGB;
		IVideoWindow  *pVW;
		IMediaEventEx *pME;
		IMediaControl *pControl;
		void _destroy();

	public:
		////Width of the video area, auto set to fill the screen
		int Width;
		////Height of the video area, auto set to fill the screen
		int Height;
		////X of the video area
		int X;
		////Y of the video area
		int Y;
		////Processes messages coming from the Multimedia subsystem
		void ProcessMsg();
		////Plays the media
		void Play() { pControl->Run(); }
		////Pauses the media
		void Pause() { pControl->Pause(); }
		////Stops the media. Even if AutoDestroy is set, stop will not trigger destruction
		void Stop() { pControl->Stop(); }
		////Loads a given file
		void Loadfile(wchar_t *Filename);
		////Whether this object is destroyed. A destroyed object can be restored by reloading
		/// media file
		bool isDestroyed;
		////Whether to destroy this object when playback is finished. Default value is true
		bool AutoDestroy;
		////Destroys the current object. All calls to play/pause/stop will fail after the object 
		/// is destroyed
		void Destroy() { _destroy(); }

		////Initializes the object
		Multimedia();

		////Fired when playback is finished, occurs before auto destroy if it is set.
		utils::EventChain<Multimedia> Finished;
	};
}
