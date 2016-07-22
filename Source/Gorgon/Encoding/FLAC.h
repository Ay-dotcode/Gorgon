#pragma once

#include "../Containers/Wave.h"


namespace Gorgon { namespace Encoding {

	/**
	 * Provides FLAC encoding support. 
	 */
	class FLAC {
	public:
		FLAC(int buffersize = 1024);

		~FLAC();

		/// Encodes the given wave data to %FLAC compressed data. bps is the bit/sample. It is best to use original
		/// bit/sample if re-saving a file. Lower values will save disk space while sacrificing quality. Current FLAC
		/// implementation supports 4 - 24 bit/sample. Channel layout is saved as is, and it is the responsibility of
		/// the caller to make it compatible with the standard layout. This function is not thread safe, you need
		/// one FLAC object per thread.
		void Encode(const Containers::Wave &input, std::ostream &output, int bps = 16);

		/// Encodes the given wave data to %FLAC compressed data. bps is the bit/sample. It is best to use original
		/// bit/sample if re-saving a file. Lower values will save disk space while sacrificing quality. Current FLAC
		/// implementation supports 4 - 24 bit/sample. Channel layout is saved as is, and it is the responsibility of
		/// the caller to make it compatible with the standard layout. This function is not thread safe, you need
		/// one FLAC object per thread.
		void Encode(const Containers::Wave &input, const std::string &filename, int bps = 16) {
			std::ofstream stream(filename, std::ios::binary);
			Encode(input, stream, bps);
		}

		/// Encodes the given wave data to %FLAC compressed data. bps is the bit/sample. It is best to use original
		/// bit/sample if re-saving a file. Lower values will save disk space while sacrificing quality. Current FLAC
		/// implementation supports 4 - 24 bit/sample. Channel layout is saved as is, and it is the responsibility of
		/// the caller to make it compatible with the standard layout.
		void Encode(const Containers::Wave &input, std::vector<Byte> &output, int bps = 16);

		/// Decodes given %FLAC compressed data and returns a wave container.
		Containers::Wave Decode(std::istream &input);

		/// Decodes given %FLAC compressed data and returns a wave container.
		Containers::Wave Decode(const std::vector<Byte> &input);

		/// Decodes given %FLAC compressed file and returns a wave container.
		Containers::Wave Decode(const std::string &filename) {
            std::ifstream stream(filename, std::ios::binary);
            return Decode(stream);
        }

	private:
		void *prepencode(const Containers::Wave &input, int bps);

		void encode(void *enc, const Containers::Wave &input, int bps);

		void *prepdecode();


		int buffersize;
		std::vector<int32_t> buffer;
	};

	class FlacStream {
	public:
		template <class I_>
		FlacStream(const I_ &input, Containers::Wave &output);

		bool Eof() const;

		void Read();

		void Seek(std::size_t destination);


	};

	extern FLAC Flac;

} }
