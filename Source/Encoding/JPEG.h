#pragma once

#include <vector>
#include "../Utils/UtilsBase.h"
#include <fstream>
#include <functional>

extern "C" {
	struct jpeg_decompress_struct;
}

namespace gge { namespace encoding {


	namespace jpg {

		class Reader;
		class SourceManager;

		void init_source(jpeg_decompress_struct *cinfo);
		unsigned char fill_input_buffer(jpeg_decompress_struct *cinfo);
		void skip_input_data(jpeg_decompress_struct *cinfo, long size);
		unsigned char resync_to_restart(jpeg_decompress_struct *cinfo, int desired);
		void term_source(jpeg_decompress_struct *cinfo);

		class Reader {
		public:
			Reader();

			virtual void Attach(jpeg_decompress_struct &cinfo) = 0;

			virtual void init_source(jpeg_decompress_struct &cinfo) = 0;
			virtual bool fill_input_buffer(jpeg_decompress_struct &cinfo) = 0;
			virtual void skip_input_data(jpeg_decompress_struct &cinfo, long size) = 0;
			virtual bool resync_to_restart(jpeg_decompress_struct &cinfo, int desired) = 0;
			virtual void term_source(jpeg_decompress_struct &cinfo) = 0;

			SourceManager *manager;
			virtual ~Reader();
		};

		class FileReader : public Reader {
		public:
			FileReader(std::ifstream &file) : file(file) { }

			virtual void Attach(jpeg_decompress_struct &cinfo);

			virtual void init_source(jpeg_decompress_struct &cinfo);
			virtual bool fill_input_buffer(jpeg_decompress_struct &cinfo);
			virtual void skip_input_data(jpeg_decompress_struct &cinfo, long size);
			virtual bool resync_to_restart(jpeg_decompress_struct &cinfo, int desired);
			virtual void term_source(jpeg_decompress_struct &cinfo);

		protected:
			std::ifstream &file;
			Byte buffer[1024];
		};

		template <class T_>
		inline Reader *ReadyReadStruct(T_ &data) {
			throw NULL;
		}

		template <>
		inline Reader *ReadyReadStruct<std::ifstream>(std::ifstream &data) {
			return new FileReader(data);
		}

		class Writer {

		};

		class Buffer {
		public:
			virtual ~Buffer() {}
			
			virtual unsigned char *Offset(int offset)=0;
			virtual void Resize(int size)=0;
		};


		class VectorBuffer : public Buffer {
		public:
			VectorBuffer(std::vector<Byte> &vector) : vector(vector) {

			}

			virtual unsigned char *Offset(int offset) {
				return &vector[offset];
			}
			virtual void Resize(int size) {
				vector.resize(size);
			}

		protected:
			std::vector<Byte> &vector;
		};

		inline Buffer *CreateBuffer(std::vector<Byte> &vector) {
			return new VectorBuffer(vector);
		}


		class ArrayBuffer : public Buffer {
		public:
			ArrayBuffer(Byte *&array) : array(array) {

			}

			virtual unsigned char *Offset(int offset) {
				return &array[offset];
			}
			virtual void Resize(int size) {
				if(array)
					array=(Byte*)std::realloc(array, size);
				else
					array=(Byte*)std::malloc(size);
			}

		protected:
			Byte *&array;
		};

		inline Buffer *CreateBuffer(Byte *&array) {
			return new ArrayBuffer(array);
		}


	}



	//currently only works for RGBA
	//see LZMA for template class decisions
	class JPEG {
	public:

		class Info {
		public:
			int Width, Height;
			bool Alpha;
			bool Color;
			int RowBytes;
		};

		JPEG() { }

		////throws runtime error
		////Using this system with arrays is extremely dangerous make sure your arrays are big enough
		//template <class I_, class O_>
		//void Encode(I_ &input, O_ &output, int width, int height) {
		//	encode(jpg::CreateBuffer(input), png::ReadyWriteStruct(output), width, height);
		//}

		//throws runtime error
		//Using this system with arrays is extremely dangerous make sure your arrays are big enough
		template <class I_, class O_>
		Info Decode(I_ &input, O_ &output) {
			return decode(jpg::ReadyReadStruct(input), jpg::CreateBuffer(output));
		}


	protected:
		void encode(jpg::Buffer *buffer,jpg::Writer *write, int width, int height) { utils::NotImplemented(); }
		Info decode(jpg::Reader *reader,jpg::Buffer *buffer);

	};

	extern JPEG Jpg;

} }