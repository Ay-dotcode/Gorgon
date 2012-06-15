#include "PNG.h"

#include "../External/png/png.h"
#include "../External/png/pngstruct.h"


namespace gge { namespace encoding {
	namespace png {
		void ReadFile(png_struct_def *p, unsigned char *buf, size_t size) {
			FileReader *reader = (FileReader*)(p->io_ptr);
			reader->Buf.read((char*)buf, size);
			size=(size_t)reader->Buf.gcount();
			if(size>0 && reader->Buf.fail())
				reader->Buf.clear();
		}
		void WriteFile(png_struct *p, unsigned char *buf, size_t size) {
			FileWriter *writer = (FileWriter*)(p->io_ptr);
			writer->Buf.write((char*)buf, size);

		}
		void ReadArray(png_struct *p, unsigned char *buf, size_t size) {
			ArrayReader *reader = (ArrayReader*)(p->io_ptr);
			std::memcpy(buf, &reader->Buf[reader->BufPos], size);
			reader->BufPos += size;
		}
		void WriteArray(png_struct *p, unsigned char *buf, size_t size) {
			ArrayWriter *writer = (ArrayWriter*)(p->io_ptr);
			if (size)
			{
				std::memcpy(&writer->Buf[writer->BufPos], buf, size);
				writer->BufPos+=size;
			}
		}
		void ReadVector(png_struct *p, unsigned char *buf, size_t size) {
			VectorReader *reader = (VectorReader*)(p->io_ptr);
			size = std::min(size, reader->Buf.size() - reader->BufPos);
			if (size)
				std::memcpy(buf, &reader->Buf[reader->BufPos], size);
			reader->BufPos += size;
		}
		void WriteVector(png_struct *p, unsigned char *buf, size_t size) {
			VectorWriter *writer = (VectorWriter*)(p->io_ptr);
			if (size)
			{
				unsigned oldSize = writer->Buf.size();
				writer->Buf.resize(oldSize + size);
				std::memcpy(&writer->Buf[oldSize], buf, size);
			}
		}
	}

	Info PNG::decode(png::Reader *reader,png::Buffer *buffer) {
		Info inf;

		png_structp png_ptr;
		png_infop info_ptr;

		unsigned long width,height;
		int bit_depth,color_type;

		unsigned char sig[8];

		png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL,NULL);
		if (!png_ptr)
			throw std::runtime_error("Cannot create PNG read struct");

		info_ptr = png_create_info_struct(png_ptr);
		if (!info_ptr) {
			png_destroy_read_struct(&png_ptr, NULL, NULL);
			throw std::runtime_error("Cannot create PNG info struct");
		}

		png_set_read_fn(png_ptr, (void*)reader, reader->Read);

		reader->Read(png_ptr, sig, 8);
		if (!png_check_sig(sig, 8))
			throw std::runtime_error("PNG signature mismatch");

		png_set_sig_bytes(png_ptr, 8);
		png_read_info(png_ptr, info_ptr);

		png_get_IHDR(png_ptr, info_ptr, (png_uint_32*)&width, (png_uint_32*)&height, &bit_depth,
			&color_type, NULL, NULL, NULL);
		inf.Width=(int)width;
		inf.Height=(int)height;

		if (color_type == PNG_COLOR_TYPE_PALETTE)
			png_set_expand(png_ptr);
		if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
			png_set_expand(png_ptr);
		if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
			png_set_expand(png_ptr);
		if (color_type == PNG_COLOR_TYPE_PALETTE)
			png_set_palette_to_rgb(png_ptr);
		if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
			png_set_expand_gray_1_2_4_to_8(png_ptr);
		if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
			png_set_tRNS_to_alpha(png_ptr);
		if (bit_depth == 16)
			png_set_strip_16(png_ptr);
		if (color_type == PNG_COLOR_TYPE_GRAY ||
			color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
			png_set_gray_to_rgb(png_ptr);

		unsigned int  i, rowbytes;
		unsigned char **  row_pointers=new unsigned char*[height];

		png_read_update_info(png_ptr, info_ptr);

		rowbytes = png_get_rowbytes(png_ptr, info_ptr);
		int pChannels = (int)png_get_channels(png_ptr, info_ptr);

		buffer->Resize(height*rowbytes);
		for (i = 0;  i < height;  ++i)
			row_pointers[i] = buffer->Offset(i*rowbytes);



		png_read_image(png_ptr, row_pointers);

		png_read_end(png_ptr, NULL);

		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);



		//currently only RGB and RGBA is supported
		if(pChannels==4) {
			inf.Alpha=true;
		}
		else {
			inf.Alpha=false;
		}

		inf.RowBytes=rowbytes;


		delete[] row_pointers;

		delete reader;
		delete buffer; //this does not delete underlying buffer

		return inf;
	}
	
	void PNG::encode(png::Buffer *buffer,png::Writer *writer, int width, int height) {
		png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if (!png_ptr)
			throw std::runtime_error("Cannot create PNG compressor");

		png_infop info_ptr = png_create_info_struct(png_ptr);
		if (!info_ptr) {
			png_destroy_write_struct(&png_ptr,
				(png_infopp)NULL);
			throw std::runtime_error("Cannot create PNG info");
		}
		
		setjmp(png_jmpbuf(png_ptr));
		
		png_set_write_fn(png_ptr, (void*)writer, writer->Write, NULL);

		png_set_IHDR(png_ptr, info_ptr, width, height,
			8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);


		png_write_info(png_ptr, info_ptr);
		Byte **rows=new Byte*[height];
		for(int i=0;i<height;i++) {
			rows[i]=buffer->Offset(i*width*4);
		}
		png_write_image(png_ptr, rows);
		png_write_end(png_ptr, NULL);


		delete rows;
		delete writer;
		delete buffer;
	}

	PNG Png;
} }
