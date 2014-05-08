#pragma once

#include <vector>
#include <fstream>
#include <functional>

#include "../Types.h"
#include "../Geometry/Size.h"

extern "C" {
	struct png_struct_def;
}

namespace Gorgon { namespace Encoding {

	/// @cond INTERNAL
	namespace png {

		//Streamer bases
		class Reader {
		public:
			void (*Read)(png_struct_def *p, unsigned char *buf, size_t size);
		};

		class Writer {
		public:
			void (*Write)(png_struct_def *p, unsigned char *buf, size_t size);
		};

		class Buffer {
		public:
			virtual ~Buffer() {}
			
			virtual unsigned char *Offset(int offset)=0;
			virtual void Resize(int size)=0;
		};


		//Vector streamers
		class VectorReader;
		void ReadVector(png_struct_def *p, unsigned char *buf, size_t size);

		class VectorReader : public Reader {
		public:
			VectorReader(const std::vector<Byte> &Buf) : Buf(Buf), BufPos(0) {
				Read=&ReadVector;
			}

			const std::vector<Byte> &Buf;
			unsigned BufPos;
		};
		inline Reader *ReadyReadStruct(const std::vector<Byte> &vec) {
			return new VectorReader(vec);
		}
		inline unsigned long long GetReadSize(const std::vector<Byte> &vec) {
			return vec.size();
		}
		inline void VectorSeek(Reader *r, long long addr) {
			VectorReader *reader=(VectorReader *)r;
			reader->BufPos+=(unsigned)addr;
		}
		inline std::function<void(Reader*, long long)> SeekFn(const std::vector<Byte> &vec) {
			return &VectorSeek;
		}

		class VectorWriter;
		void WriteVector(png_struct_def *p, unsigned char *buf, size_t size);
		class VectorWriter : public Writer {
		public:
			VectorWriter(std::vector<Byte> &Buf) : Buf(Buf) {
				Write=&WriteVector;
			}

			std::vector<Byte> &Buf;
		};
		inline Writer *ReadyWriteStruct(std::vector<Byte> &vec) {
			return new VectorWriter(vec);
		}
		
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


		//Array streamers
		class ArrayReader;
		void ReadArray(png_struct_def *p, unsigned char *buf, size_t size);

		class ArrayReader : public Reader {
		public:
			ArrayReader(const Byte *Buf) : Buf(Buf), BufPos(0) {
				Read=&ReadArray;
			}

			const Byte *Buf;
			unsigned BufPos;
		};
		inline Reader *ReadyReadStruct(const Byte *vec) {
			return new ArrayReader(vec);
		}
		inline unsigned long long GetReadSize(const Byte *vec) {
			return (unsigned long long)(long long)-1;
		}
		inline void ArraySeek(Reader *r, long long addr) {
			ArrayReader *reader=(ArrayReader *)r;
			reader->BufPos+=(unsigned)addr;
		}
		inline std::function<void(Reader*, long long)> SeekFn(const Byte *vec) {
			return &ArraySeek;
		}

		class ArrayWriter;
		void WriteArray(png_struct_def *p, unsigned char *buf, size_t size);
		class ArrayWriter : public Writer {
		public:
			ArrayWriter(Byte *Buf) : Buf(Buf), BufPos(0) {
				Write=&WriteArray;
			}

			Byte *Buf;
			unsigned BufPos;
		};
		inline Writer *ReadyWriteStruct(Byte *vec) {
			return new ArrayWriter(vec);
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


		//File streamers
		class FileReader;
		void ReadFile(png_struct_def *p, unsigned char *buf, size_t size);
		unsigned long long GetReadSize(std::istream &f);
		class FileReader : public Reader {
		public:
			FileReader(std::istream &Buf) : Buf(Buf) {
				Read=&ReadFile;
			}

			std::istream &Buf;
		};
		inline Reader *ReadyReadStruct(std::istream &f) {
			return new FileReader(f);
		}
		inline unsigned long long GetReadSize(std::istream &f) {
			auto c=f.tellg();
			f.seekg(0, std::ios::end);
			auto e=f.tellg();
			f.seekg(c, std::ios::beg);
			return e-c;
		}
		inline void FileSeek(Reader *r, long long addr) {
			FileReader *reader=(FileReader *)r;
			reader->Buf.seekg(addr, std::ios::cur);
		}
		inline std::function<void(Reader*, long long)> SeekFn(std::istream &vec) {
			return &FileSeek;
		}

		class FileWriter;
		void WriteFile(png_struct_def *p, unsigned char *buf, size_t size);
		class FileWriter : public Writer {
		public:
			FileWriter(std::ostream &Buf) : Buf(Buf) {
				Write=&WriteFile;
			}

			std::ostream &Buf;
		};
		inline Writer *ReadyWriteStruct(std::ostream &f) {
			return new FileWriter(f);
		}
	}
	/// @endcond
	
	/// Encodes or decodes PNG compression. This class only returns and creates RGB and RGBA images. This property might be
	/// fixed along with Task #13
	/// @see Gorgon::Encoding::LZMA for template class decisions.
	class PNG {
	public:

		/// A structure to contain decoded image information.
		struct Info {
			/// The size of the image
			Geometry::Size Size;

			/// Whether the alpha channel is available
			bool Alpha;
		};


		/// Encodes a given input with the specified size to the output. Currently this system supports vector, array and stream
		/// reader and writers. Both vectors and arrays are resized automatically.
		/// @warning Array write buffer should either be a nullptr of type Byte or an array allocated with malloc. This system uses
		/// realloc or malloc to resize raw arrays.
		/// @throws runtime_error in case of an encoding error
		template <class I_, class O_>
		void Encode(I_ &input, O_ &output, const Geometry::Size &size) {
			encode(png::CreateBuffer(input), png::ReadyWriteStruct(output), size.Width, size.Height);
		}

		/// Decodes the given PNG data. This function returns necessary information that is extracted from the 
		/// data. Currently this system supports vector, array and stream reader and writers. Both vectors and 
		/// arrays are resized automatically. Use Gorgon::Resource::Image::Import function to create an image from
		/// the given PNG data.
		/// @warning Array write buffer should either be a nullptr of type Byte or an array allocated with malloc. This system uses
		/// realloc or malloc to resize raw arrays.
		/// @throws runtime_error in case of a read error
		template <class I_, class O_>
		Info Decode(I_ &input, O_ &output) {
			return decode(png::ReadyReadStruct(input), png::CreateBuffer(output));
		}


	protected:
		/// Performs actual encoding
		void encode(png::Buffer *buffer,png::Writer *write, int width, int height);

		/// Performs actual decoding
		Info decode(png::Reader *reader,png::Buffer *buffer);

	};

	/// A ready to use PNG class
	extern PNG Png;

} }