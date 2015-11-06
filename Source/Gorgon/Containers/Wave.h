#pragma once

#include <vector>

#include "../Types.h"
#include "../Geometry/Point.h"
#include "../Geometry/Size.h"
#include "../Graphics/Color.h"

namespace Gorgon {
	namespace Containers {

		/// This class is a container for wave data. It supports different color modes and access to the
		/// underlying data through () operator. This object implements move semantics. Since copy constructor is
		/// expensive, it is deleted against accidental use. If a copy of the object is required, use Duplicate function.
		class Wave {
		public:

			/// Constructs an empty wave data
			Wave() {

			}

			/// Constructs a new wave data with the given number of samples and channels. This constructor 
			/// does not initialize data inside the wave
			Wave(unsigned long size, int channels): size(size), channels(channels) {
				data = (float*)malloc(size * channels * sizeof(float));
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
			void Resize(unsigned long size, int channels) {
				
				this->channels = channels;

				// Check if resize is really necessary
				if(this->size == size)
					return;

				this->size = size;

				if(data) {
					free(data);
				}
				data = (float*)malloc(size * channels * sizeof(float));
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
				data = (float*)malloc(size * channels * sizeof(float));
			}

			/// 
			void Assign(float *newdata, unsigned long size) {
				this->size = size;

				if(data) {
					free(data);
				}
				if(size > 0) {
					data = (float*)malloc(size * channels * sizeof(float));
					memcpy(data, newdata, size * channels * sizeof(float));
				}
				else {
					data = nullptr;
				}
			}
			
			/// 
			void Assign(float *newdata, unsigned long size, int channels) {
				this->size = size;
				
				this->channels = channels;

				if(data) {
					free(data);
				}
				if(size > 0) {
					data = (float*)malloc(size * channels * sizeof(float));
					memcpy(data, newdata, size * channels * sizeof(float));
				}
				else {
					data = nullptr;
				}
			}

			/// 
			void Assign(float *newdata) {
				memcpy(data, newdata, size * channels * sizeof(float));
			}

			/// 
			void Assume(float *newdata, unsigned long size) {
				this->size = size;

				if(data && newdata!=data) {
					free(data);
				}
				data = newdata;
			}

			/// Assumes the ownership of the given data. This variant changes the size and
			/// color mode of the wave. The given data should have the size of 
			/// width*height*Graphics::GetBytesPerPixel(mode)*sizeof(Byte). This function
			/// does not perform any checks for the data size while assuming it.
			/// newdata could be nullptr however, in this case
			/// width, height should be 0. mode is not assumed to be ColorMode::Invalid while
			/// the wave is empty, therefore it could be specified as any value.
			void Assume(float *newdata, unsigned long size, int channels) {
				this->size = size;
				this->channels = channels;

				if(data && newdata!=data) {
					free(data);
				}
				data = newdata;
			}

			/// Assumes the ownership of the given data. The size and color mode of the wave stays the same.
			/// The given data should have the size of width*height*Graphics::GetBytesPerPixel(mode)*sizeof(Byte).
			/// This function does not perform any checks for the data size while assuming it.
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
				memset(data, 0.f, size * sizeof(float));
			}

			/// Destroys this wave by setting width and height to 0 and freeing the memory
			/// used by its data. Also color mode is set to ColorMode::Invalid
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

			/// 
			float &operator()(unsigned long p) {
#ifndef NDEBUG
				if(p < 0 || p >= size - 1) {
					throw std::runtime_error("Index out of bounds");
				}
#endif
				return data[p];
			}

			/// 
			float operator()(unsigned long p) const {
#ifndef NDEBUG
				if(p < 0 || p >= size - 1) {
					throw std::runtime_error("Index out of bounds");
				}
#endif
				return data[p];
			}

			/// 
			float Get(unsigned long p) const {
#ifndef NDEBUG
				if(p < 0 || p >= size - 1) {
					throw std::runtime_error("Index out of bounds");
				}
#endif
				return data[p];
			}

			/// Returns the size of the wave
			unsigned long GetSize() const {
				return size;
			}

			/// Returns the bytes occupied by a single pixel of this wave
			unsigned long GetSampleRate() const {
				return samplerate;
			}

		protected:
			/// Data that stores pixels of the wave
			float *data = nullptr;

			/// Width of the wave
			unsigned long size;

			/// Number of channels
			unsigned int channels = 0;

			/// Sampling rate of the wave
			unsigned long samplerate = 0;
		};

		/// Swaps two waves. Should be used unqualified for ADL.
		inline void swap(Wave &l, Wave &r) {
			l.Swap(r);
		}

	}
}
