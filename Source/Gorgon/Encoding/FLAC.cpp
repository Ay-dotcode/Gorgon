#include "FLAC.h"

#include "FLAC/stream_encoder.h"

#include <memory>

namespace Gorgon { namespace Encoding {

	namespace flac {
		struct vectorio {
			std::vector<Byte> *vector;

			std::size_t current;
		};

		class streamread {
		public:
			streamread(std::istream &stream) : stream(stream) {
				data = (float*)malloc(sizeof(float));
				datasize = 0;
			}

			~streamread() {
				if(data)
					free(data);
			}

			void allocateupto(std::size_t size) {
				auto newsize = currentpos + size;
				if(newsize <= datasize)
					return;

				datasize = newsize;

				data = (float*)realloc(data, datasize * sizeof(float));
			}


			void allocate(std::size_t size) {
				datasize = size;
				data = (float*)realloc(data, datasize * sizeof(float));
			}

			std::istream &stream;

			float *data;

			std::size_t datasize;

			int currentpos = 0;

			int channels = 0;

			int rate = 0;
		};
	}

	FLAC__StreamEncoderWriteStatus stream_encode_write(
		const FLAC__StreamEncoder *encoder, const FLAC__byte buffer[],
		size_t bytes, unsigned samples, unsigned current_frame, void *client_data) {

		std::ostream &output = *(std::ostream*)client_data;

		output.write((const char*)buffer, bytes);

		return (output.good() ? FLAC__STREAM_ENCODER_WRITE_STATUS_OK : FLAC__STREAM_ENCODER_WRITE_STATUS_FATAL_ERROR);
	}

	FLAC__StreamEncoderSeekStatus stream_encode_seek(
		const FLAC__StreamEncoder *encoder, FLAC__uint64 absolute_byte_offset, void *client_data) {

		std::ostream &output = *(std::ostream*)client_data;

		output.seekp((std::streamoff)absolute_byte_offset, std::ios::beg);

		return (output.good() ? FLAC__STREAM_ENCODER_SEEK_STATUS_OK : FLAC__STREAM_ENCODER_SEEK_STATUS_ERROR);
	}

	FLAC__StreamEncoderTellStatus stream_encode_tell(
		const FLAC__StreamEncoder *encoder, FLAC__uint64 *absolute_byte_offset, void *client_data) {

		std::ostream &output = *(std::ostream*)client_data;

		*absolute_byte_offset = output.tellp();

		return (output.good() ? FLAC__STREAM_ENCODER_TELL_STATUS_OK : FLAC__STREAM_ENCODER_TELL_STATUS_ERROR);
	}

	FLAC__StreamEncoderWriteStatus vector_encode_write(
		const FLAC__StreamEncoder *encoder, const FLAC__byte buffer[],
		size_t bytes, unsigned samples, unsigned current_frame, void *client_data) {

		auto &output = *(flac::vectorio*)client_data;

		output.vector->resize(output.vector->size() + bytes);
		std::memcpy(&(*output.vector)[output.current], buffer, bytes);
		output.current += bytes;

		return FLAC__STREAM_ENCODER_WRITE_STATUS_OK;
	}

	FLAC__StreamEncoderSeekStatus vector_encode_seek(
		const FLAC__StreamEncoder *encoder, FLAC__uint64 absolute_byte_offset, void *client_data) {

		auto &output = *(flac::vectorio*)client_data;

		if(absolute_byte_offset >= output.vector->size())
			return FLAC__STREAM_ENCODER_SEEK_STATUS_ERROR;

		output.current=(size_t)absolute_byte_offset;

		return FLAC__STREAM_ENCODER_SEEK_STATUS_OK;
	}

	FLAC__StreamEncoderTellStatus vector_encode_tell(
		const FLAC__StreamEncoder *encoder, FLAC__uint64 *absolute_byte_offset, void *client_data) {

		auto &output = *(flac::vectorio*)client_data;

		*absolute_byte_offset = output.current;

		return FLAC__STREAM_ENCODER_TELL_STATUS_OK;
	}

	FLAC__StreamDecoderReadStatus stream_decode_read(
		const FLAC__StreamDecoder *decoder, FLAC__byte buffer[], 
		size_t *bytes, void *client_data) {

		auto &input = ((flac::streamread*)client_data)->stream;

		input.read((char*)buffer, *bytes);

		*bytes = (size_t)input.gcount();

		if(input.eof())
			return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;

		return (input.good() ? FLAC__STREAM_DECODER_READ_STATUS_CONTINUE : FLAC__STREAM_DECODER_READ_STATUS_ABORT);
	}

	FLAC__StreamDecoderWriteStatus stream_decode_write(
		const FLAC__StreamDecoder *decoder, const FLAC__Frame *frame, 
		const FLAC__int32 *const buffer[], void *client_data) {

		auto &reader = *((flac::streamread*)client_data);

		reader.allocateupto(frame->header.blocksize * frame->header.channels);

		if(reader.channels == 0) {
			reader.channels = frame->header.channels;
			reader.rate     = frame->header.sample_rate;
		}

		if(reader.channels != frame->header.channels) {
			throw std::runtime_error("Channels should be fixed in the FLAC file.");
		}

		const int size     = frame->header.blocksize;
		const int channels = frame->header.channels;

		float multiplier = 1.f / ((1<<(frame->header.bits_per_sample-1))-1);

		for(int i=0; i<size; i++)
			for(int c=0; c<channels; c++)
				reader.data[reader.currentpos++] = buffer[c][i] * multiplier;

		return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
	}

	void decode_error(const FLAC__StreamDecoder *decoder, FLAC__StreamDecoderErrorStatus status, void *client_data) {
		throw std::runtime_error("Invalid FLAC file");
	}

	void *FLAC::prepencode(const Containers::Wave &input, int bps) {
		auto encoder = FLAC__stream_encoder_new();
		std::shared_ptr<FLAC__StreamEncoder> enc_guard(encoder, &FLAC__stream_encoder_delete);


		FLAC__bool ok = true;

		//if the file is too small, disable streaming to improve encoding
		if(input.GetSize()<10000)
			ok &= FLAC__stream_encoder_set_streamable_subset(encoder, false);


		ok &= FLAC__stream_encoder_set_verify(encoder, false);
		ok &= FLAC__stream_encoder_set_channels(encoder, input.GetChannelCount());
		ok &= FLAC__stream_encoder_set_bits_per_sample(encoder, bps);
		ok &= FLAC__stream_encoder_set_sample_rate(encoder, input.GetSampleRate());
		ok &= FLAC__stream_encoder_set_compression_level(encoder, 4);
		ok &= FLAC__stream_encoder_set_total_samples_estimate(encoder, input.GetSize());

		if(!ok)
			throw std::runtime_error("A problem with the FLAC settings occurred.");

		return encoder;
	}


	void FLAC::encode(void *enc, const Containers::Wave &input, int bps) {
		auto encoder = (FLAC__StreamEncoder *)enc;

		unsigned long i = 0;
		const auto size = input.GetSize();
		const int channels = input.GetChannelCount();
		int32_t multiplier = (1<<(bps-1))-1;

		while(i < size) {
			int tobeenc = std::min(buffersize, (int)(size - i) * channels) / channels;

			
			for(int j=0; j<tobeenc; j++) {
				for(int c=0; c<channels; c++) {
					buffer[j * channels + c] = (int32_t)(multiplier * input(i + j, c));
				}
			}

			i += tobeenc;

			FLAC__stream_encoder_process_interleaved(encoder, &buffer[0], tobeenc);
		}

		FLAC__stream_encoder_finish(encoder);
	}

	void *FLAC::prepdecode() {
		return FLAC__stream_decoder_new();
	}

	FLAC::FLAC(int buffersize) : buffersize(buffersize) {
		buffer.resize(buffersize);
	}

	FLAC::~FLAC() {
	}

	void FLAC::Encode(const Containers::Wave &input, std::ostream &output, int bps) {
		auto enc = (FLAC__StreamEncoder *)prepencode(input, bps);
		std::shared_ptr<FLAC__StreamEncoder> enc_guard(enc, &FLAC__stream_encoder_delete);

		if(!enc)
			throw std::runtime_error("Cannot initialize FLAC encoding.");

		auto res = FLAC__stream_encoder_init_stream(
			enc,
			&stream_encode_write,
			&stream_encode_seek,
			&stream_encode_tell,
			nullptr, &output
		);

		if(res != FLAC__STREAM_ENCODER_INIT_STATUS_OK)
			throw std::runtime_error("Cannot start FLAC encoder stream");

		encode(enc, input, bps);
	}

	void FLAC::Encode(const Containers::Wave &input, std::vector<Byte> &output, int bps) {
		auto enc = (FLAC__StreamEncoder *)prepencode(input, bps);
		std::shared_ptr<FLAC__StreamEncoder> enc_guard(enc, &FLAC__stream_encoder_delete);

		flac::vectorio vect = {&output, 0};

		if(!enc)
			throw std::runtime_error("Cannot initialize FLAC encoding.");

		auto res = FLAC__stream_encoder_init_stream(
			enc,
			&vector_encode_write,
			&vector_encode_seek,
			&vector_encode_tell,
			nullptr, &vect
		);

		if(res != FLAC__STREAM_ENCODER_INIT_STATUS_OK)
			throw std::runtime_error("Cannot start FLAC encoder stream");

		encode(enc, input, bps);
	}

	Containers::Wave FLAC::Decode(std::istream &input) {
		auto dec = (FLAC__StreamDecoder *)prepdecode();
		std::shared_ptr<FLAC__StreamDecoder> dec_guard(dec, &FLAC__stream_decoder_delete);

		if(!dec)
			throw std::runtime_error("Cannot initialize FLAC decoding.");

		flac::streamread stream(input);

		auto res = FLAC__stream_decoder_init_stream(
			dec,
			&stream_decode_read,
			nullptr, nullptr, nullptr, nullptr,
			&stream_decode_write, 
			nullptr, &decode_error,
			&stream
		);

		if(res != FLAC__STREAM_DECODER_INIT_STATUS_OK)
			throw std::runtime_error("Cannot start FLAC encoder stream");

		FLAC__stream_decoder_process_until_end_of_metadata(dec);

		FLAC__stream_decoder_process_single(dec);

		auto total = FLAC__stream_decoder_get_total_samples(dec) * FLAC__stream_decoder_get_channels(dec);
		if(total != 0) {
			stream.allocate(std::size_t(total));
		}

		FLAC__stream_decoder_process_until_end_of_stream(dec);

		Containers::Wave ret;

		ret.Assume(stream.data, stream.currentpos / stream.channels, Audio::StandardChannels(stream.channels));
		ret.SetSampleRate(stream.rate);

		stream.data=nullptr;

		return ret;
	}

	FLAC Flac;

} }