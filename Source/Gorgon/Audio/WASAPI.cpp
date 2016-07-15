#undef UNICODE

#include "../Audio.h"
#include "../Main.h"
#include <initguid.h>
#include <Audioclient.h>
#include <Mmdeviceapi.h>
#include <comdef.h>
#include <locale>
#include <codecvt>
#include <functiondiscoverykeys_devpkey.h>

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
			__uuidof(MMDeviceEnumerator), NULL,
			CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),
			(void**)&Enumerator);

		if(FAILED(hr)) {
			Log << "Cannot create enumerator.";
			return;
		}

		Device::Refresh();

	}

	std::vector<Channel> ToChannels(unsigned f) {
		std::vector<Channel> channels;
		if(f & SPEAKER_FRONT_LEFT)
			channels.push_back(Channel::FrontLeft);

		if(f & SPEAKER_FRONT_RIGHT)
			channels.push_back(Channel::FrontRight);

		if(f & SPEAKER_FRONT_CENTER)
			channels.push_back(Channel::Center);

		if(f & SPEAKER_BACK_LEFT)
			channels.push_back(Channel::BackLeft);

		if(f & SPEAKER_BACK_RIGHT)
			channels.push_back(Channel::BackRight);

		if(f & SPEAKER_LOW_FREQUENCY)
			channels.push_back(Channel::LowFreq);

		if(channels.size() == 1 && channels[0] == Channel::Center) {
			channels[0] = Channel::Mono;
		}

		return channels;
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

			std::string devid, devname;
			std::vector<Channel> channels;
			bool headphones = false;

			wchar_t *id = nullptr;
			dev->GetId(&id);
			if(id!=nullptr) {
				devid = converter.to_bytes(id);
				CoTaskMemFree(id);
			}

			IPropertyStore *props = nullptr;

			dev->OpenPropertyStore(STGM_READ, &props);

			if(props == nullptr) {
				dev->Release();
				continue;
			}

			PROPVARIANT pv;

			props->GetValue(PKEY_Device_FriendlyName, &pv);
			
			devname = converter.to_bytes(pv.pwszVal);

			props->GetValue(PKEY_AudioEndpoint_FormFactor, &pv);

			if(pv.uintVal == 3) {
				headphones=true;
				channels = {Channel::FrontLeft, Channel::FrontRight};
			}
			else {
				props->GetValue(PKEY_AudioEndpoint_PhysicalSpeakers, &pv);

				channels = ToChannels(pv.uintVal);
			}

			props->GetValue(PKEY_AudioEngine_DeviceFormat, &pv);

			auto fmt = *(WAVEFORMATEX*)pv.blob.pBlobData;

			if(channels.size() == 0) {
				if(fmt.wFormatTag==WAVE_FORMAT_EXTENSIBLE) {
					auto efmt = *(WAVEFORMATEXTENSIBLE*)pv.blob.pBlobData;
					channels = ToChannels(efmt.dwChannelMask);
				}
				else {
					switch(fmt.nChannels) {
					case 1:
						channels ={Channel::Mono};
						break;
					case 2:
						channels ={Channel::FrontLeft, Channel::FrontRight};
						break;
					case 3:
						channels ={Channel::FrontLeft, Channel::FrontRight, Channel::Center};
						break;
					case 4:
						channels ={Channel::FrontLeft, Channel::FrontRight, Channel::BackLeft, Channel::BackRight};
						break;
					case 5:
						channels ={Channel::FrontLeft, Channel::FrontRight, Channel::Center, Channel::BackLeft, Channel::BackRight};
						break;
					case 6:
						channels ={Channel::FrontLeft, Channel::FrontRight, Channel::Center, Channel::LowFreq, Channel::BackLeft, Channel::BackRight};
						break;
					}
				}
			}

			devices.push_back(Device(
				devid, devname, fmt.nSamplesPerSec, 
				Format::Float, headphones, channels
			));

			dev->Release();
			props->Release();
		}

		devs->Release();
	}

} }