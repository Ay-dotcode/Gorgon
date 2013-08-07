#include "JPEG.h"

#ifndef HAVE_BOOLEAN
typedef unsigned char boolean;
#define HAVE_BOOLEAN
#endif

#include "../External/JPEG/jpeglib.h"

namespace gge { namespace encoding {

	struct myerror {
		jpeg_error_mgr pub;
		std::string message;
	};

	void my_error_exit (j_common_ptr cinfo) {
		throw std::runtime_error(((myerror*)cinfo->err)->message);
	}

	void my_output_message(j_common_ptr cinfo) {
		char buffer[JMSG_LENGTH_MAX];

		(*cinfo->err->format_message) (cinfo, buffer);

		((myerror*)cinfo->err)->message=buffer;
	}

	JPEG::Info JPEG::decode(jpg::Reader *reader,jpg::Buffer *buffer) {
		struct jpeg_decompress_struct cinfo;
		myerror jerr;

		JPEG::Info info;

		cinfo.err = jpeg_std_error(&jerr.pub);
		cinfo.err->error_exit=&my_error_exit;
		cinfo.err->output_message=&my_error_exit;
		jpeg_create_decompress(&cinfo);

		reader->Attach(cinfo);

		try {
			jpeg_read_header(&cinfo, TRUE);
			jpeg_start_decompress(&cinfo);

			info.Width = cinfo.output_width;
			info.Height = cinfo.output_height;
			info.RowBytes = cinfo.output_width * cinfo.output_components ;
			info.Color = cinfo.output_components>1;
			info.Alpha=cinfo.output_components>3;

			buffer->Resize(info.RowBytes*info.Height);

			int offset=0;
			while (cinfo.output_scanline < cinfo.output_height) {
				Byte *line=buffer->Offset(offset);
				jpeg_read_scanlines(&cinfo, (JSAMPARRAY)&line, 1);
				offset+=info.RowBytes;
			}
		}
		catch(...) {
			jpeg_finish_decompress(&cinfo);
			jpeg_destroy_decompress(&cinfo);

			delete reader;
			delete buffer;

			throw;
		}

		jpeg_finish_decompress(&cinfo);
		jpeg_destroy_decompress(&cinfo);

		delete reader;
		delete buffer;

		return info;
	}


	namespace jpg {
		class SourceManager : public jpeg_source_mgr {
		public:
			Reader *reader;
		};


		void FileReader::Attach(jpeg_decompress_struct &cinfo) {
			cinfo.src=this->manager;
			cinfo.src->bytes_in_buffer=0;
			cinfo.src->next_input_byte=NULL;
		}

		void FileReader::init_source(jpeg_decompress_struct &cinfo) {
			//do nothing file should be open and ready to read
		}

		bool FileReader::fill_input_buffer(jpeg_decompress_struct &cinfo) {
			file.read((char*)buffer, 1024);
			cinfo.src->next_input_byte=buffer;
			if(file.bad()) {
				cinfo.src->bytes_in_buffer=0;
			}
			else {
				cinfo.src->bytes_in_buffer=(size_t)file.gcount();
			}

			return true;
		}

		void FileReader::skip_input_data(jpeg_decompress_struct &cinfo, long size) {
			if(size>(long)cinfo.src->bytes_in_buffer) {
				file.seekg(size-cinfo.src->bytes_in_buffer, std::ios::cur);
				fill_input_buffer(cinfo);
			}
			else {
				cinfo.src->bytes_in_buffer-=size;
				cinfo.src->next_input_byte+=size;
			}
		}

		bool FileReader::resync_to_restart(jpeg_decompress_struct &cinfo, int desired) {
			return jpeg_resync_to_restart(&cinfo, desired)!=0;
		}

		void FileReader::term_source(jpeg_decompress_struct &cinfo) {
			//do nothing, its not our duty to close the file
		}


		void init_source(jpeg_decompress_struct *cinfo) {
			((SourceManager*)cinfo->src)->reader->init_source(*cinfo);
		}

		boolean fill_input_buffer(jpeg_decompress_struct *cinfo) {
			return ((SourceManager*)cinfo->src)->reader->fill_input_buffer(*cinfo);
		}

		void skip_input_data(jpeg_decompress_struct *cinfo, long size) {
			((SourceManager*)cinfo->src)->reader->skip_input_data(*cinfo, size);
		}

		boolean resync_to_restart(jpeg_decompress_struct *cinfo, int desired) {
			return ((SourceManager*)cinfo->src)->reader->resync_to_restart(*cinfo, desired);
		}

		void term_source(jpeg_decompress_struct *cinfo) {
			((SourceManager*)cinfo->src)->reader->term_source(*cinfo);
		}

		Reader::Reader() {
			manager=new SourceManager;
			manager->reader=this;
			manager->init_source=&jpg::init_source;
			manager->fill_input_buffer=&jpg::fill_input_buffer;
			manager->skip_input_data=&jpg::skip_input_data;
			manager->resync_to_restart=&jpg::resync_to_restart;
			manager->term_source=&jpg::term_source;
		}

		Reader::~Reader() {
			delete manager;
		}
	}
	JPEG Jpg;

} }
