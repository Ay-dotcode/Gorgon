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

	IMMDeviceEnumerator *Enumerator = nullptr;
	IMMDevice *Device = nullptr;
	IAudioClient *AudioClient = nullptr;
	IAudioRenderClient *RenderClient = nullptr;
	bool available = false;

	constexpr int REFTIMES_PER_SECONDS = 10000000;

	namespace internal {
		extern std::thread audiothread;

		extern int   BufferSize;
		extern float BufferDuration;
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

	std::vector<Channel> StandardChannels(int channelcount) {
		std::vector<Channel> channels;
		switch(channelcount) {
			case 1:
				return {Channel::Mono};
			case 2:
				return {Channel::FrontLeft, Channel::FrontRight};
			case 3:
				return {Channel::FrontLeft, Channel::FrontRight, Channel::Center};
			case 4:
				return {Channel::FrontLeft, Channel::FrontRight, Channel::BackLeft, Channel::BackRight};
			case 5:
				return {Channel::FrontLeft, Channel::FrontRight, Channel::Center, Channel::BackLeft, Channel::BackRight};
			case 6:
				return {Channel::FrontLeft, Channel::FrontRight, Channel::Center, Channel::LowFreq, Channel::BackLeft, Channel::BackRight};
			default:
				return {};
		}
	}

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

		hr = Enumerator->GetDefaultAudioEndpoint(
			eRender, eConsole, &Device);

		if(FAILED(hr)) {
			Log << "Cannot get default device.";
			return;
		}

		hr = Device->Activate(
			__uuidof(IAudioClient), CLSCTX_ALL,
			NULL, (void**)&AudioClient);


		if(FAILED(hr)) {
			Log << "Cannot activate default device.";
			return;
		}

		WAVEFORMATEX *wavefmt;

		hr = AudioClient->GetMixFormat(&wavefmt);

		if(FAILED(hr)) {
			Log << "Cannot get playback format.";
			return;
		}

		if(wavefmt->wFormatTag != WAVE_FORMAT_EXTENSIBLE) {
			Log << "Unexpected device format.";
			return;
		}

		WAVEFORMATEXTENSIBLE *fmt = (WAVEFORMATEXTENSIBLE *)wavefmt;

		fmt->SubFormat = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;

		hr = AudioClient->Initialize(
			AUDCLNT_SHAREMODE_SHARED,
			0,
			long(internal::BufferDuration * REFTIMES_PER_SECONDS),
			0,
			wavefmt,
			NULL
		);

		if(FAILED(hr)) {
			Log << "Cannot initialize audio client.";
			return;
		}

		hr = AudioClient->GetService(
			__uuidof(IAudioRenderClient),
			(void**)&RenderClient);

		if(FAILED(hr)) {
			Log << "Cannot get audio stream.";
			return;
		}

		// Get the actual size of the allocated buffer.
		UINT32 bs = 0;
		hr = AudioClient->GetBufferSize(&bs);

		internal::BufferSize = bs;
		
		if(FAILED(hr) || bs == 0) {
			Log << "Cannot obtain audio buffer size.";
			return;
		}

		//** Create default device.

		Log << "WASAPI Audio subsystem is ready.";

		available = true;

		Device::Refresh();

	}

	void Device::Refresh() {
		//for wstring conversion
		using convert_type=std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_type, wchar_t> converter;

		HRESULT hr;

		IMMDeviceCollection *devs = nullptr;

		IMMDevice *defaultdev = nullptr;
		hr = Enumerator->GetDefaultAudioEndpoint(
			eRender, eConsole, &defaultdev);

		std::string defaultdevid;

		wchar_t *id = nullptr;
		defaultdev->GetId(&id);
		if(id!=nullptr) {
			defaultdevid = converter.to_bytes(id);
			CoTaskMemFree(id);
		}

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
					channels = StandardChannels(fmt.nChannels);
				}
			}

			devices.push_back(Device(
				devid, devname, fmt.nSamplesPerSec, 
				Format::Float, headphones, channels
			));

			if(devid == defaultdevid)
				def = devices.back();

			dev->Release();
			props->Release();
		}

		devs->Release();
	}


	bool IsAvailable() {
		return available;
	}

} }