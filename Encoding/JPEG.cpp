#include "JPEG.h"

#include "../External/JPEG/jpeglib.h"


namespace gge { namespace encoding {

	JPEG::Info JPEG::decode(jpg::Reader *reader,jpg::Buffer *buffer) {
		struct jpeg_decompress_struct cinfo;
		struct jpeg_error_mgr jerr;

		JPEG::Info info;

		//FILE * infile;    	/* source file */
		//if ((infile = fopen("test.jpg", "rb")) == NULL) 
		//{
		//	fprintf(stderr, "can't open \n");
		//}

		cinfo.err = jpeg_std_error(&jerr);
		jpeg_create_decompress(&cinfo);

		reader->Attach(cinfo);
		//jpeg_stdio_src(&cinfo, infile);

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
	}

	void jpg::FileReader::Attach(jpeg_decompress_struct &cinfo) {
		cinfo.src=this->manager;
		cinfo.src->bytes_in_buffer=0;
		cinfo.src->next_input_byte=NULL;
	}

	void jpg::FileReader::init_source(jpeg_decompress_struct &cinfo) {
		//do nothing file should be open and ready to read
	}

	bool jpg::FileReader::fill_input_buffer(jpeg_decompress_struct &cinfo) {
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

	void jpg::FileReader::skip_input_data(jpeg_decompress_struct &cinfo, long size) {
		if(size>(long)cinfo.src->bytes_in_buffer) {
			file.seekg(size-cinfo.src->bytes_in_buffer, std::ios::cur);
			fill_input_buffer(cinfo);
		}
		else {
			cinfo.src->bytes_in_buffer-=size;
			cinfo.src->next_input_byte+=size;
		}
	}

	bool jpg::FileReader::resync_to_restart(jpeg_decompress_struct &cinfo, int desired) {
		return jpeg_resync_to_restart(&cinfo, desired)!=0;
	}

	void jpg::FileReader::term_source(jpeg_decompress_struct &cinfo) {
		//do nothing, its not our duty to close the file
	}


	void jpg::init_source(jpeg_decompress_struct *cinfo) {
		((SourceManager*)cinfo->src)->reader->init_source(*cinfo);
	}

	boolean jpg::fill_input_buffer(jpeg_decompress_struct *cinfo) {
		return ((SourceManager*)cinfo->src)->reader->fill_input_buffer(*cinfo);
	}

	void jpg::skip_input_data(jpeg_decompress_struct *cinfo, long size) {
		((SourceManager*)cinfo->src)->reader->skip_input_data(*cinfo, size);
	}

	boolean jpg::resync_to_restart(jpeg_decompress_struct *cinfo, int desired) {
		return ((SourceManager*)cinfo->src)->reader->resync_to_restart(*cinfo, desired);
	}

	void jpg::term_source(jpeg_decompress_struct *cinfo) {
		((SourceManager*)cinfo->src)->reader->term_source(*cinfo);
	}

	jpg::Reader::Reader() {
		manager=new SourceManager;
		manager->reader=this;
		manager->init_source=&jpg::init_source;
		manager->fill_input_buffer=&jpg::fill_input_buffer;
		manager->skip_input_data=&jpg::skip_input_data;
		manager->resync_to_restart=&jpg::resync_to_restart;
		manager->term_source=&jpg::term_source;
	}

	jpg::Reader::~Reader() {
		delete manager;
	}

	JPEG Jpg;

} }
