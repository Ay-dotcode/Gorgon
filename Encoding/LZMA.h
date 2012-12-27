#pragma once
#include <string.h>
#include <vector>
#include <functional>
#include <iosfwd>
#include <istream>
#include <cstring>
#include "../Utils/UtilsBase.h"


namespace gge { namespace encoding { 


	namespace lzma {

		//Streamer bases
		class Reader {
		public:
			int (*Read)(void *p, void *buf, size_t *size);
		};

		class Writer {
		public:
			int (*Write)(void *p, const void *buf, size_t size);
		};



		//Vector streamers
		class VectorReader;
		int ReadVector(void *p, void *buf, size_t *size);

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
		inline int ReadVector(void *p, void *buf, size_t *size) {
			VectorReader *reader = (VectorReader*)p;
			*size = std::min(*size, reader->Buf.size() - reader->BufPos);
			if (*size)
				std::memcpy(buf, &reader->Buf[reader->BufPos], *size);
			reader->BufPos += *size;
			return 0;		
		}

		class VectorWriter;
		int WriteVector(void *p, const void *buf, size_t size);
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
		inline int WriteVector(void *p, const void *buf, size_t size) {
			VectorWriter *writer = (VectorWriter*)p;
			if (size)
			{
				unsigned oldSize = writer->Buf.size();
				writer->Buf.resize(oldSize + size);
				std::memcpy(&writer->Buf[oldSize], buf, size);
			}
			return size;		
		}


		//Array streamers
		class ArrayReader;
		int ReadArray(void *p, void *buf, size_t *size);

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
		inline int ReadArray(void *p, void *buf, size_t *size) {
			ArrayReader *reader = (ArrayReader*)p;
			std::memcpy(buf, &reader->Buf[reader->BufPos], *size);
			reader->BufPos += *size;
			return 0;		
		}

		class ArrayWriter;
		int WriteArray(void *p, const void *buf, size_t size);
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
		inline int WriteArray(void *p, const void *buf, size_t size) {
			ArrayWriter *writer = (ArrayWriter*)p;
			if (size)
			{
				std::memcpy(&writer->Buf[writer->BufPos], buf, size);
				writer->BufPos+=size;
			}
			return size;
		}


		//String streamers
		class StringReader;
		int ReadString(void *p, void *buf, size_t *size);

		class StringReader : public Reader {
		public:
			StringReader(const std::string &Buf) : Buf(Buf), BufPos(0) {
				Read=&ReadString;
			}

			const std::string &Buf;
			unsigned BufPos;
		};
		inline Reader *ReadyReadStruct(const std::string &vec) {
			return new StringReader(vec);
		}
		inline unsigned long long GetReadSize(const std::string &vec) {
			return vec.size();
		}
		inline void StringSeek(Reader *r, long long addr) {
			StringReader *reader=(StringReader *)r;
			reader->BufPos+=(unsigned)addr;
		}
		inline std::function<void(Reader*, long long)> SeekFn(const std::string &vec) {
			return &StringSeek;
		}
		inline int ReadString(void *p, void *buf, size_t *size) {
			StringReader *reader = (StringReader*)p;
			*size = std::min(*size, reader->Buf.size() - reader->BufPos);
			if (*size)
				std::memcpy(buf, &reader->Buf[reader->BufPos], *size);
			reader->BufPos += *size;
			return 0;		
		}

		class StringWriter;
		int WriteString(void *p, const void *buf, size_t size);
		class StringWriter : public Writer {
		public:
			StringWriter(std::string &Buf) : Buf(Buf), BufPos(0) {
				Write=&WriteString;
			}

			std::string &Buf;
			unsigned BufPos;
		};
		inline Writer *ReadyWriteStruct(std::string &vec) {
			return new StringWriter(vec);
		}
		inline int WriteString(void *p, const void *buf, size_t size) {
			StringWriter *writer = (StringWriter*)p;
			if (size)
			{
				unsigned oldSize = writer->Buf.size();
				writer->Buf.resize(oldSize + size);
				std::memcpy(&writer->Buf[oldSize], buf, size);
			}
			return size;		
		}

		//File streamers
		class FileReader;
		int ReadFile(void *p, void *buf, size_t *size);
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
		inline int ReadFile(void *p, void *buf, size_t *size) {
			FileReader *reader = (FileReader*)p;
			reader->Buf.read((char*)buf, *size);
			*size=(size_t)reader->Buf.gcount();
			if(size>0 && reader->Buf.fail())
				reader->Buf.clear();
			return 0;
		}

		class FileWriter;
		int WriteFile(void *p, const void *buf, size_t size);
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
		inline int WriteFile(void *p, const void *buf, size_t size) {
			FileWriter *writer = (FileWriter*)p;
			writer->Buf.write((char*)buf, size);

			return size;		
		}
	}

	typedef std::function<void(float)> LZMAProgressNotification;

	//The main idea of this system is to reduce the amount of the code.
	//There are reader structures that can read data from various sources.
	//These sources are automatically created by encode/decode template
	//functions. After creating these structures, internal encode/decode
	//function is called. Creating an new read/write structure is enough 
	//to support that type of container
	class LZMA {
	public:

		LZMA(bool UseUncompressedSize=true) : UseUncompressedSize(UseUncompressedSize) { }

		//throws runtime error
		//Using this system with arrays is extremely dangerous make sure your arrays are big enough
		template <class I_, class O_>
		void Encode(I_ &input, O_ &output) {
			encode(lzma::ReadyReadStruct(input), lzma::ReadyWriteStruct(output), lzma::GetReadSize(input), NULL);
		}
		//throws runtime error
		//Using this system with arrays is extremely dangerous make sure your arrays are big enough
		template <class I_, class O_>
		void Encode(I_ &input, O_ &output, LZMAProgressNotification notifier) {
			encode(lzma::ReadyReadStruct(input), lzma::ReadyWriteStruct(output), lzma::GetReadSize(input), &notifier);
		}



		//throws runtime error
		//Using this system with arrays is extremely dangerous make sure your arrays are big enough
		template <class I_, class O_>
		void Decode(I_ &input, O_ &output, Byte *CompressionProperties=NULL, unsigned long long fsize=(unsigned long long)(long long)-1) {
			decode(lzma::ReadyReadStruct(input), lzma::ReadyWriteStruct(output), lzma::GetReadSize(input), lzma::SeekFn(input), CompressionProperties, fsize, NULL);
		}
		//throws runtime error
		//Using this system with arrays is extremely dangerous make sure your arrays are big enough
		template <class I_, class O_>
		void Decode(I_ &input, O_ &output, LZMAProgressNotification notifier, Byte *CompressionProperties=NULL, unsigned long long fsize=(unsigned long long)(long long)-1) {
			decode(lzma::ReadyReadStruct(input), lzma::ReadyWriteStruct(output), lzma::GetReadSize(input), lzma::SeekFn(input), CompressionProperties, fsize, &notifier);
		}

		int PropertySize();

		bool UseUncompressedSize;

	protected:
		void encode(lzma::Reader *reader,lzma::Writer *writer, unsigned long long size, LZMAProgressNotification *notifier);
		void decode(lzma::Reader *reader,lzma::Writer *writer, unsigned long long size,std::function<void(lzma::Reader*,long long)> seekfn, Byte *cprops, unsigned long long fsize, LZMAProgressNotification *notifier);

	};

	extern LZMA Lzma;

	}}