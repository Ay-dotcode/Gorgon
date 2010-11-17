#include "Multimedia.h"

namespace gge {
	void Multimedia::ProcessMsg() {
		// Make sure that we don't access the media event interface
		// after it has already been released.
		if (!pME || isDestroyed)
			return;

		// Process all queued events
	    LONG evCode, evParam1, evParam2;
		while(pME->GetEvent(&evCode, (LONG_PTR *) &evParam1,(LONG_PTR *) &evParam2, 0)==0)
		{
			pME->FreeEventParams(evCode, evParam1, evParam2);
			if(EC_COMPLETE == evCode) {
				Finished();

				if(AutoDestroy)
					_destroy();

				return;
			}
		}
	}

	void Multimedia::_destroy() {
		isDestroyed=true;
		pGB->Abort();
		pME->SetNotifyWindow((int)main->getWindow(), WM_VIDEO_NOTIFY, NULL);
		pControl->Release();
		pME->Release();
		pVW->Release();
		pGB->Release();
		
		pME=NULL;
		pVW=NULL;
		pGB=NULL;

		delete pGB;
		
		UpdateWindow((HWND)main->getWindow());
	}

	Multimedia::Multimedia(GGEMain *main) :
		Finished("Finished", this),
		isDestroyed(false),
		main(main),
		Width(main->getWidth()),
		Height(main->getHeight()),
		X(0), Y(0), AutoDestroy(true)
	{
		int hr=CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER,IID_IGraphBuilder, (void **)&pGB);
		hr = pGB->QueryInterface(IID_IMediaControl, (void **)&pControl);
		pGB->QueryInterface(IID_IMediaEventEx, (void **)&pME);
		pGB->QueryInterface(IID_IVideoWindow, (void **)&pVW);
	}

	void Multimedia::Loadfile(wchar_t *Filename) {
		pGB->RenderFile(Filename, NULL);

		pVW->put_Owner((int)main->getWindow());
		pVW->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
		pVW->SetWindowPosition(X, Y, Width, Height);
		pME->SetNotifyWindow((int)main->getWindow(), WM_VIDEO_NOTIFY,(int)this);
	}
}
