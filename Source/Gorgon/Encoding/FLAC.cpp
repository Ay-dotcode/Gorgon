#include "FLAC.h"

#include "FLAC/stream_encoder.h"

#include <memory>

namespace Gorgon { namespace Encoding {

	namespace flac {
		struct vectorio {
			std::vector<Byte> *vector;

			std::size_t current;
		};
        
        class streamdata {
        public:
 			streamdata() {
				data = (float*)malloc(sizeof(float));
				datasize = 0;
			}

			virtual ~streamdata() {
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
 
			float *data;

			std::size_t datasize;

			int currentpos = 0;

			int channels = 0;

			int rate = 0;
        };

		struct streamwrite {
			streamwrite(std::ostream &output, std::streampos &maxpos) : output(output), maxpos(maxpos)  { }
			std::ostream &output;
			std::streampos &maxpos;
		};

		class streamread : public streamdata {
		public:
			streamread(std::istream &stream) : stream(stream) {
			}

			std::istream &stream;
		};
        
		class vectorread : public streamdata {
		public:
			vectorread(const std::vector<Byte> &input) : input(input) {
			}

			const std::vector<Byte> &input;
            
            size_t readpos = 0;
		};
	}

	FLAC__StreamEncoderWriteStatus stream_encode_write(
		const FLAC__StreamEncoder *encoder, const FLAC__byte buffer[],
		size_t bytes, unsigned samples, unsigned current_frame, void *client_data) {

		std::ostream &output = ((flac::streamwrite*)client_data)->output;

		output.write((const char*)buffer, bytes);

		std::cout<<"Written: "<<bytes<<std::endl;

		return (output.good() ? FLAC__STREAM_ENCODER_WRITE_STATUS_OK : FLAC__STREAM_ENCODER_WRITE_STATUS_FATAL_ERROR);
	}

	FLAC__StreamEncoderSeekStatus stream_encode_seek(
		const FLAC__StreamEncoder *encoder, FLAC__uint64 absolute_byte_offset, void *client_data) {

		std::ostream &output = ((flac::streamwrite*)client_data)->output;

		auto p = output.tellp();
		if(((flac::streamwrite*)client_data)->maxpos < p)
			((flac::streamwrite*)client_data)->maxpos = p;

		output.seekp((std::streamoff)absolute_byte_offset, std::ios::beg);

		return (output.good() ? FLAC__STREAM_ENCODER_SEEK_STATUS_OK : FLAC__STREAM_ENCODER_SEEK_STATUS_ERROR);
	}

	FLAC__StreamEncoderTellStatus stream_encode_tell(
		const FLAC__StreamEncoder *encoder, FLAC__uint64 *absolute_byte_offset, void *client_data) {

		std::ostream &output = ((flac::streamwrite*)client_data)->output;

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

	FLAC__StreamDecoderReadStatus vector_decode_read(
		const FLAC__StreamDecoder *decoder, FLAC__byte buffer[], 
		size_t *bytes, void *client_data) {

		auto &input = *((flac::vectorread*)client_data);

        auto bytestoread = *bytes;
        if(input.input.size()-input.readpos<bytestoread)
            bytestoread = input.input.size()-input.readpos;
        
		memcpy(buffer, &input.input[input.readpos], bytestoread);
        
        input.readpos += bytestoread;

		*bytes = bytestoread;

		if(input.input.size() == input.readpos)
			return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;

		return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
	}

	FLAC__StreamDecoderWriteStatus decode_write(
		const FLAC__StreamDecoder *decoder, const FLAC__Frame *frame, 
		const FLAC__int32 *const buffer[], void *client_data) {

		auto &reader = *((flac::streamdata*)client_data);

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

		FLAC__bool ok = true;

		ok &= FLAC__stream_encoder_set_verify(encoder, true);
		ok &= FLAC__stream_encoder_set_channels(encoder, input.GetChannelCount());
		ok &= FLAC__stream_encoder_set_bits_per_sample(encoder, bps);
		ok &= FLAC__stream_encoder_set_sample_rate(encoder, input.GetSampleRate());
		ok &= FLAC__stream_encoder_set_compression_level(encoder, 5);
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

		flac::streamwrite s{output, this->maxpos};

		auto res = FLAC__stream_encoder_init_stream(
			enc,
			&stream_encode_write,
			&stream_encode_seek,
			&stream_encode_tell,
			nullptr, &s
		);

		if(res != FLAC__STREAM_ENCODER_INIT_STATUS_OK)
			throw std::runtime_error("Cannot start FLAC encoder stream");

		maxpos = 0;
		encode(enc, input, bps);

		if((int)maxpos != 0) {
			output.seekp(maxpos, std::ios::beg);
		}
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

	void FLAC::Decode(std::istream &input, Containers::Wave &wave) {
		auto dec = (FLAC__StreamDecoder *)prepdecode();
		std::shared_ptr<FLAC__StreamDecoder> dec_guard(dec, &FLAC__stream_decoder_delete);

		if(!dec)
			throw std::runtime_error("Cannot initialize FLAC decoding.");

		flac::streamread stream(input);

		auto res = FLAC__stream_decoder_init_stream(
			dec,
			&stream_decode_read,
			nullptr, nullptr, nullptr, nullptr,
			&decode_write, 
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


		wave.Assume(stream.data, stream.currentpos / stream.channels, Audio::StandardChannels(stream.channels));
		wave.SetSampleRate(stream.rate);

		stream.data=nullptr;
	}

	void FLAC::Decode(const std::vector<Byte> &input, Containers::Wave &wave) {
		auto dec = (FLAC__StreamDecoder *)prepdecode();
		std::shared_ptr<FLAC__StreamDecoder> dec_guard(dec, &FLAC__stream_decoder_delete);

		if(!dec)
			throw std::runtime_error("Cannot initialize FLAC decoding.");

		flac::vectorread vector(input);

		auto res = FLAC__stream_decoder_init_stream(
			dec,
			&vector_decode_read,
			nullptr, nullptr, nullptr, nullptr,
			&decode_write, 
			nullptr, &decode_error,
			&vector
		);

		if(res != FLAC__STREAM_DECODER_INIT_STATUS_OK)
			throw std::runtime_error("Cannot start FLAC encoder stream");

		FLAC__stream_decoder_process_until_end_of_metadata(dec);

		FLAC__stream_decoder_process_single(dec);

		auto total = FLAC__stream_decoder_get_total_samples(dec) * FLAC__stream_decoder_get_channels(dec);
		if(total != 0) {
			vector.allocate(std::size_t(total));
		}

		FLAC__stream_decoder_process_until_end_of_stream(dec);

		wave.Assume(vector.data, vector.currentpos / vector.channels, Audio::StandardChannels(vector.channels));
		wave.SetSampleRate(vector.rate);

		vector.data=nullptr;
	}

	FLAC Flac;

} }
