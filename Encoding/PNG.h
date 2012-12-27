#pragma once

#include <vector>
#include "../Utils/UtilsBase.h"
#include <fstream>
#include <functional>

extern "C" {
	struct png_struct_def;
}

namespace gge { namespace encoding {


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
	
	class Info {
	public:
		int Width, Height;
		bool Alpha;
		int RowBytes;
	};

	//currently only works for RGBA
	//see LZMA for template class decisions
	class PNG {
	public:

		PNG() { }

		//throws runtime error
		//Using this system with arrays is extremely dangerous make sure your arrays are big enough
		template <class I_, class O_>
		void Encode(I_ &input, O_ &output, int width, int height) {
			encode(png::CreateBuffer(input), png::ReadyWriteStruct(output), width, height);
		}

		//throws runtime error
		//Using this system with arrays is extremely dangerous make sure your arrays are big enough
		template <class I_, class O_>
		Info Decode(I_ &input, O_ &output) {
			return decode(png::ReadyReadStruct(input), png::CreateBuffer(output));
		}

		int PropertySize();

	protected:
		void encode(png::Buffer *buffer,png::Writer *write, int width, int height);
		Info decode(png::Reader *reader,png::Buffer *buffer);

	};

	extern PNG Png;

} }