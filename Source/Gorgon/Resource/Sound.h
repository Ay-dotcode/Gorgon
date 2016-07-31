#pragma once

#include <vector>
#include <memory>

#include "../Types.h"
#include "Base.h"
#include "../Containers/Wave.h"

namespace Gorgon {
	namespace Resource {
		class File;
		class Reader;

		////This is sound resource. It may contain 22kHz or 44kHz mono or stereo wave files.
		/// Also supports LZMA compression. No native sound compression is supported.
		class Sound : public Base {
		public:

			/// Default constructor
			Sound() {}

			/// Destructor
			virtual ~Sound() {}

			/// 04010000h (Extended, Sound)
			virtual GID::Type GetGID() const override { return GID::Sound; }

			const Containers::Wave &GetWave() const {
				return data;
			}

			Containers::Wave &GetWave() {
				return data;
			}

			/// Destroys the data stored in the sound
			void Destroy() {
				isloaded=false;
			}

			/// Loads the sound from the disk. If sound is already loaded, this function will return true
			bool Load();

			/// Returns whether the sound data is loaded
			bool IsLoaded() const { return isloaded; }

			/// This function loads a sound resource from the given file
			static Sound *LoadResource(std::weak_ptr<File> file, std::shared_ptr<Reader> reader, unsigned long size);

			/// Assigns the sound to the copy of the given data. Ownership of the given data
			/// is not transferred. If the given data is not required elsewhere, consider using
			/// Assume function. Also sets IsLoaded.
			void Assign(const Containers::Wave &wave) {
				data=wave.Duplicate();

				isloaded = true;
			}

			/// Assumes the contents of the given wave as wave data. The given parameter is moved from
			/// and will become empty. Also sets IsLoaded.
			void Assume(Containers::Wave &wave) {
				data = std::move(wave);

				isloaded = true;
			}

			int GetBits() const {
				return bits;
			}

			void SetBits(int bits) {
				if(compression == GID::FLAC) {
					if(bits<4 || bits>24)
						throw std::runtime_error("Unsupported bits/sample");
				}
				else {
					if(bits!=8 && bits!=16)
						throw std::runtime_error("Unsupported bits/sample");
				}

				this->bits = bits;
			}


		protected:

			/// Loads the sound from the data stream
			bool load(std::shared_ptr<Reader> reader, unsigned long size, bool forceload);

			void save(Writer &writer) override;

			/// Entry point of this resource within the physical file. This value is stored for 
			/// late loading purposes
			unsigned long entrypoint = -1;

			/// Used to handle late loading
			std::shared_ptr<Reader> reader;

			/// Whether this sound is loaded or not
			bool isloaded = false;

			/// Compression mode of this sound resource
			GID::Type compression = GID::FLAC;

			/// Whether to load this sound during initial loading
			bool lateloading = false;
			
			/// Sound data
			Containers::Wave data;

			/// Number of bits per sample
			int bits = 16;
		};
	}
}
