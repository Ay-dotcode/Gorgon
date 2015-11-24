#pragma once

#include "Utils/Logging.h"
#include "Event.h"

#include <vector>
#include <string>

namespace Gorgon {
/// Contains audio subsystem. For audio primitives look into multimedia namespace.
namespace Audio {

	/// Starts audio subsystem with the default device
	void Initialize();
	
	/// Whether the audio is available
	bool IsAvailable();
	
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
	
	/// Sample format. For now only Float will be used and all conversions are done
	/// by the underlying library.
	enum class Format {
		PCM8,
		PCM16,
		Float
	};
	
	/// Represents an audio device.
	class Device {
	public:
		
		Device() = default;
		
		Device(const std::string &id, const std::string &name, int rate, Format format, const std::vector<Channel> &channels) : 
		name(name), id(id), rate(rate), format(format), channels(channels) 
		{ }
		
		/// Returns the ID of this device
		std::string GetID() const {
			return id;
		}
		
		/// Returns the readable name of this device
		std::string GetName() const {
			return name;
		}
		
		/// Returns number of samples per second
		int GetSampleRate() const {
			return rate;
		}
		
		/// Returns the format of this device. In some cases, this format
		/// might be different than the actual device format.
		Format GetFormat() const {
			return format;
		}
		
		/// Returns the number of channels available
		int GetChannelCount() const {
			return channels.size();
		}
		
		/// Returns the channel type with the given index
		Channel GetChannel(int index) const {
			return channels[index];
		}
		
		/// Returns the devices in the current system
		static const std::vector<Device> &Devices() {
			return devices;
		}
		
		/// Returns the default device of the current system
		static Device Default() {
			return def;
		}
		
		/// Refreshes the list of audio devices. Calling this function triggers ChangedEvent.
		static void Refresh();
		
		/// Name based device lookup. Fires std::runtime_error if the given device cannot be
		/// found.
		static Device Find(const std::string &name) {
			for(auto &dev : devices) {
				if(dev.name == name) return dev;
			}
			
			throw std::runtime_error("Cannot find device: "+name);
		}
		
		/// Triggers when the audio device configuration is changed. Depending on the operating
		/// system this event might be unavailable. When this even is triggered, current audio
		/// configuration must be checked but should not be recreated unless there is a change.
		static Event<> ChangedEvent;
		
	private:
		std::string id;
		std::string name;
		
		int rate;
		Format format;
		
		std::vector<Channel> channels;
		
		static std::vector<Device> devices;
		static Device def;
	};
	
	extern Utils::Logger Log;
}
}
