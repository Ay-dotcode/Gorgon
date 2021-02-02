#pragma once

#include "Utils/Logging.h"
#include "Event.h"
#include "Audio/Basic.h"

#include <vector>
#include <string>
#include <thread>

namespace Gorgon {
/// Contains audio subsystem. For audio primitives look into multimedia namespace.
namespace Audio {

	/// Starts audio subsystem with the default device
	void Initialize();
	
	/// Whether the audio is available
	bool IsAvailable();
	
	/// Changes the master volume
	void SetVolume(float volume);
	
	/// Changes the volume of a channel. If the channel is not found, nothing is done
	/// except if the channel is mono. In that case all channel's volume is changed.
	void SetVolume(Channel channel, float volume);
	
	/// Returns the master volume
	float GetVolume();
	
	/// Returns the volume of a channel. If the channel does not exists, this function
	/// will return 0.
	float GetVolume(Channel channel);
 	
	/// Represents an audio device.
	class Device {
	public:
		
		Device() = default;
		
		Device(const std::string &id, const std::string &name, int rate, Format format, bool headphones, const std::vector<Channel> &channels) : 
		id(id), name(name), headphones(headphones), rate(rate), format(format), channels(channels) 
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
			return (int)channels.size();
		}
		
		/// Returns the channel type with the given index
		Channel GetChannel(int index) const {
			return channels[index];
		}
		
		/// Returns the index of the given type of channel. If that channel type does not exists, -1 is returned.
		int FindChannel(Channel type) const {
            for(int i = 0; i<(int)channels.size(); i++)
                if(type==channels[i])
                    return i;
                
            return -1;
        }
		
		/// Returns if this is a valid device
		bool IsValid() const {
			return rate != 0;
		}

		/// Returns if this device is connected to headphones
		bool IsHeadphones() const {
			return headphones;
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
		static Device Find(const std::string &id) {
			for(auto &dev : devices) {
				if(dev.id == id) return dev;
			}
			
			throw std::runtime_error("Cannot find device: "+id);
		}
		
		/// Triggers when the audio device configuration is changed. Depending on the operating
		/// system this event might be unavailable. When this even is triggered, current audio
		/// configuration must be checked but should not be recreated unless there is a change.
		static Event<> ChangedEvent;
		
	private:
		std::string id;
		std::string name;

		bool headphones = false;
		
		int rate = 0;
		Format format;
		
		std::vector<Channel> channels;
		
		static std::vector<Device> devices;
		static Device def;
	};
	
	extern Utils::Logger Log;
	extern Device Current;
	
	namespace internal {
		extern std::thread audiothread;
		
		extern float mastervolume;
		extern std::vector<float> volume;
	}
}
}
