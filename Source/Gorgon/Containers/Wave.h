#pragma once

#include <vector>

#include "../Types.h"
#include "../Geometry/Point.h"
#include "../Geometry/Size.h"
#include "../Graphics/Color.h"

#include "../Audio/Basic.h"

namespace Gorgon {
	namespace Containers {

		/// This class is a container for wave data. It supports different color modes and access to the
		/// underlying data through () operator. This object implements move semantics. Since copy constructor is
		/// expensive, it is deleted against accidental use. If a copy of the object is required, use Duplicate function.
		class Wave {
		public:
			
			/**
			 * Represents a sample in the Wave data.
			 */
			class Sample {
				friend class Wave;
			public:
				Sample() { }
				
				float &Channel(unsigned channel) { 
#ifndef NDEBUG
					if(channel >= channels) {
						throw std::runtime_error("Index out of bounds");
					}
#endif

					return current[channel]; 
				}
				
				float Channel(unsigned channel) const { 
#ifndef NDEBUG
					if(channel >= channels) {
						throw std::runtime_error("Index out of bounds");
					}
#endif

					return current[channel];
				}
				
				float &operator[](unsigned channel) { 
					return Channel(channel);
				}
				
				float operator[](unsigned channel) const { 
					return Channel(channel);
				}
				
			private:
				Sample(float *current, unsigned channels) : current(current), channels(channels) { }
				
				float *current = nullptr;
				unsigned channels = 0;
			};
		
			/**
			* Iterates the elements of a Wave. This iterator is not checked, so modifications to Wave will not be
			* propagated to the iterator and it will never know if it gets invalidated. This iterator does not support
			* Gorgon Iterators. 
			*/
			class Iterator {
				friend class Wave;
			public:
				
				typedef unsigned long difference_type;
				typedef Sample value_type;
				typedef Sample& reference;
				typedef Sample* pointer;
				typedef std::random_access_iterator_tag iterator_category;

				
				Iterator() = default;
				
				Iterator& operator=(const Iterator&) = default;
				
				Iterator& operator++() {
					current += channels;
					
					return *this;
				}
				
				Iterator operator++(int) {
					auto temp = *this;
					
					current += channels;
					
					return temp;
				}
				
				Iterator& operator--() {
					current -= channels;
					
					return *this;
				}
				
				Iterator operator--(int) {
					auto temp = *this;
					
					current -= channels;
					
					return temp;
				}
				
				Sample operator*() const {
					return {current, channels};
				}
				
				bool operator< (const Iterator &r) {
					return current<r.current;
				}
				
				bool operator> (const Iterator &r) {
					return current>r.current;
				}
				
				bool operator<=(const Iterator &r) {
					return current<=r.current;
				}
				
				bool operator>=(const Iterator &r) {
					return current>=r.current;
				}
				
				bool operator==(const Iterator &r) {
					return current==r.current;
				}
				
				bool operator!=(const Iterator &r) {
					return current!=r.current;
				}
				
				Sample operator[](unsigned long pos) const {
					return {current, channels};
				}
				
				Iterator& operator+=(unsigned long diff) {
					current+=diff*channels;
					return *this;
				}
				
				Iterator  operator+(unsigned long diff) const {
					auto temp=*this;
					temp.current+=diff*channels;
					
					return temp;
				}
				
				Iterator& operator-=(unsigned long diff) {
					current-=diff*channels;
					return *this;
				}
				
				Iterator  operator-(unsigned long diff) const {
					auto temp=*this;
					temp.current-=diff*channels;
					
					return temp;
				}

				unsigned long operator-(const Iterator &r) const {
					return (current-r.current)/channels;
				}
				
			private:
				Iterator(float *current, unsigned channels) : current(current), channels(channels) {}
					
				float *current;
				unsigned channels;
			};

			/// Constructs an empty wave data
			Wave() {

			}

			/// Constructs a new wave data with the given number of samples and channels. This constructor 
			/// does not initialize data inside the wave
			explicit Wave(unsigned long size, unsigned samplerate, std::vector<Audio::Channel> channels = {Audio::Channel::Mono}): size(size), samplerate(samplerate), channels(channels) {
				data = (float*)malloc(size * channels.size() * sizeof(float));
			}

			/// Copy constructor is disabled
			Wave(const Wave &) = delete;

			/// Move constructor
			Wave(Wave &&data): Wave() {
				Swap(data);
			}

			/// Copy assignment is disabled
			Wave &operator=(const Wave &) = delete;

			/// Move assignment
			Wave &operator=(Wave &&other) {
				Destroy();
				Swap(other);
				
				return *this;
			}

			/// Destructor
			~Wave() {
				Destroy();
			}

			/// Duplicates this wave, essentially performing the work of copy constructor
			Wave Duplicate() const {
				Wave data;
				data.Assign(this->data, size);

				return data;
			}

			/// Resizes the wave to the given size and channels. This function discards the contents
			/// of the wave and does not perform any initialization.
			void Resize(unsigned long size, std::vector<Audio::Channel> channels) {
				
				this->channels = std::move(channels);

				// Check if resize is really necessary
				if(this->size == size)
					return;

				this->size = size;

				if(data) {
					free(data);
				}
				data = (float*)malloc(size * channels.size() * sizeof(float));
			}

			/// Resizes the wave to the given size. This function discards the contents
			/// of the wave and does not perform any initialization. Previously set number
			/// of channels is used
			void Resize(unsigned long size) {

				// Check if resize is really necessary
				if(this->size == size)
					return;

				this->size = size;

				if(data) {
					free(data);
				}
				data = (float*)malloc(size * channels.size() * sizeof(float));
			}

			/// Copies the given data assigns the new data to this object, size is the number of samples. 
			/// Assumes number of channels stays the same. newdata should have size*channels number of
			/// entries
			void Assign(float *newdata, unsigned long size) {
				this->size = size;

				if(data) {
					free(data);
				}
				if(size > 0) {
					data = (float*)malloc(size * channels.size() * sizeof(float));
					memcpy(data, newdata, size * channels.size() * sizeof(float));
				}
				else {
					data = nullptr;
				}
			}
			
			/// Copies the given data assigns the new data to this object, size is the number of samples. 
			/// newdata should have size*channels number of entries
			void Assign(float *newdata, unsigned long size, std::vector<Audio::Channel> channels) {
				this->size = size;
				
				this->channels = std::move(channels);

				if(data) {
					free(data);
				}
				if(size > 0) {
					data = (float*)malloc(size * channels.size() * sizeof(float));
					memcpy(data, newdata, size * channels.size() * sizeof(float));
				}
				else {
					data = nullptr;
				}
			}

			/// Copies the given data assigns the new data to this object, size is the number of samples. 
			/// Assumes number of channels and samples stays the same. newdata should have size*channels 
			/// number of entries 
			void Assign(float *newdata) {
				memcpy(data, newdata, size * channels.size() * sizeof(float));
			}

			/// Assumes the ownership of the data.
			void Assume(float *newdata, unsigned long size) {
				this->size = size;

				if(data && newdata!=data) {
					free(data);
				}
				data = newdata;
			}

			/// Assumes the ownership of the given data. This variant changes the size and channels of the wave.
			/// The given data should have the size of size*channels. This function does not perform any checks 
			/// for the data size while assuming it. newdata could be nullptr however, in this case size should 
			/// be 0.
			void Assume(float *newdata, unsigned long size, std::vector<Audio::Channel> channels) {
				this->size = size;
				this->channels = channels;

				if(data && newdata!=data) {
					free(data);
				}
				data = newdata;
			}

			void Assume(float *newdata) {
				if(data && newdata!=data) {
					free(data);
				}
				
				data = newdata;
			}

			/// Returns and disowns the current data buffer. If wave is empty, this method will return a nullptr.
			float *Release() {
				auto temp = data;
				data = nullptr;
				Destroy();

				return temp;
			}

			/// Cleans the contents of the buffer by setting every byte it contains to 0.
			void Clean() {
				memset(data, 0, size * channels.size() * sizeof(float));
			}

			/// Destroys this wave by setting its size to 0 and freeing the memory
			/// used by its data.
			void Destroy() {
				if(data) {
					free(data);
					data = nullptr;
				}
				size = 0;
			}

			/// Swaps this wave with another. This function is used to implement move semantics.
			void Swap(Wave &other) {
				using std::swap;

				swap(size, other.size);
				swap(channels, other.channels);
				swap(data, other.data);
			}

			/// Returns the raw data pointer
			float *RawData() {
				return data;
			}

			/// Returns the raw data pointer
			const float *RawData() const {
				return data;
			}

			/// Allows access to individual members
			float &operator()(unsigned long p, unsigned ch) {
#ifndef NDEBUG
				if(p >= size || ch >= channels.size()) {
					throw std::runtime_error("Index out of bounds");
				}
#endif
				return data[p*channels.size()+ch];
			}

			/// Allows access to individual members
			float operator()(unsigned long p, unsigned ch) const {
#ifndef NDEBUG
				if(p >= size || ch >= channels.size()) {
					throw std::runtime_error("Index out of bounds");
				}
#endif
				return data[p*channels.size()+ch];
			}

			/// Allows access to individual members
			float Get(unsigned long p, unsigned ch) const {
#ifndef NDEBUG
				if(p >= size || ch >= channels.size()) {
					throw std::runtime_error("Index out of bounds");
				}
#endif
				return data[p*channels.size() + ch];
			}

			/// Returns the size of the wave
			unsigned long GetSize() const {
				return size;
			}
			
			/// Returns the length of the wave data in seconds
			float GetLength() const {
				return (double)size/samplerate;
			}
			
			/// Returns the number of channels that this wave data has.
			unsigned GetChannelCount() const {
				return channels.size();
			}

			/// Returns the type of the channel at the given index
			Audio::Channel GetChannelType(int channel) const {
				if(channel<0 || channel>=channels.size())
					return Audio::Channel::Unknown;
				
				return channels[channel];
			}
			
			/// Returns the index of the given channel. If the given channel does not exists, this function returns -1
			int FindChannel(Audio::Channel channel) const {
				for(int i=0; i<channels.size(); i++)  {
					if(channels[i] == channel) return i;
				}
				
				return -1;
			}

			/// Returns the number of samples per second
			unsigned GetSampleRate() const {
				return samplerate;
			}
			
			/// Sets the number samples per second
			void SetSampleRate(unsigned rate) {
				samplerate = rate;
			}
			
			Iterator begin() {
				return {data, channels.size()};
			}
			
			Iterator end() {
				return {data+size*channels.size(), channels.size()};
			}

		protected:
			/// Data that stores pixels of the wave
			float *data = nullptr;

			/// Number of samples in the wave
			unsigned long size = 0;

			/// Number of channels
			std::vector<Audio::Channel> channels;

			/// Sampling rate of the wave
			unsigned samplerate = 0;
		};

		/// Swaps two waves. Should be used unqualified for ADL.
		inline void swap(Wave &l, Wave &r) {
			l.Swap(r);
		}

	}
}
