#pragma once

#include <vector>
#include <Gorgon/Utils/Assert.h>
#include <fstream>
#include <functional>

#include "../Types.h"
#include "../Containers/Image.h"

extern "C" {
	struct jpeg_decompress_struct;
}

namespace Gorgon { namespace Encoding {


	namespace jpg {

		class Reader;
		class SourceManager;

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

	}



	//currently only works for RGB
	//see LZMA for template class decisions
	class JPEG {
	public:

		JPEG() { }

		////Using this system with arrays is extremely dangerous make sure your arrays are big enough
		//template <class I_, class O_>
		//void Encode(I_ &input, O_ &output, int width, int height) {
		//	encode(jpg::CreateBuffer(input), png::ReadyWriteStruct(output), width, height);
		//}

		/// Decodes given JPG data from the given input and creates the image.
		/// throws runtime error
		void Decode(std::ifstream &input, Containers::Image &output) {
            jpg::FileReader reader(input);
            
			decode(reader, output);
		}

		/// Decodes given JPG data from the given input and creates the image.
		/// throws runtime error
		void Decode(const std::string &input, Containers::Image &output) {
            std::ifstream file(input, std::ios::binary);
            if(!file.is_open()) throw std::runtime_error("Cannot open file");
            
            jpg::FileReader reader(file);
            
			decode(reader, output);
		}


	protected:
		//void encode(jpg::Buffer *buffer, jpg::Writer *write, int width, int height) { Utils::NotImplemented(); }
		void decode(jpg::Reader &reader, Containers::Image &output);

	};

	extern JPEG Jpg;

} }
