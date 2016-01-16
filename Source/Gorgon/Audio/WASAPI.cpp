#undef UNICODE

#include "../Audio.h"
#include "../Main.h"
#include <Audioclient.h>
#include <Mmdeviceapi.h>
#include <comdef.h>
#include <locale>
#include <codecvt>

namespace Gorgon { namespace Audio {

	IMMDeviceEnumerator *Enumerator = NULL;
	bool available = false;

	void Initialize() {
		HRESULT hr;

		hr = CoInitialize(NULL);

		if(FAILED(hr)) {
			Log << "Cannot initialize COM subsystem.";
			return;
		}

		hr = CoCreateInstance(
			CLSID_MMDeviceEnumerator, NULL,
			CLSCTX_ALL, IID_IMMDeviceEnumerator,
			(void**)&Enumerator);

		if(FAILED(hr)) {
			Log << "Cannot create enumerator.";
			return;
		}



	}

	void Device::Refresh() {
		//for wstring conversion
		using convert_type=std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_type, wchar_t> converter;

		HRESULT hr;

		IMMDeviceCollection *devs = nullptr;

		devices.clear();

		hr = Enumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &devs);

		if(FAILED(hr)) {
			Log << "Device enumeration is failed.";
			return;
		}

		unsigned count = 0;
		devs->GetCount(&count);

		for(unsigned i=0; i<count; i++) {
			IMMDevice *dev = nullptr;
			devs->Item(i, &dev);

			std::string devid;

			wchar_t *id = nullptr;
			dev->GetId(&id);
			if(id!=nullptr) {
				devid = converter.to_bytes(id);
				CoTaskMemFree(id);
			}



			devices.push_back(Device(
			));

			dev->Release();
		}

		devs->Release();
	}

} }