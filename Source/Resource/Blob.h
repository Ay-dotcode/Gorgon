#pragma once

#include <vector>

#include "GRE.h"
#include "Base.h"

namespace gge { namespace resource {
	class File;
	class Blob;
	
	////This function loads a sound resource from the given file
	Blob *LoadBlobResource(File &File, std::istream &data, int Size);

	////This function loads a sound resource from the given file
	void LoadBlob(Blob *snd, std::istream &data, int Size);

	////This is sound resource. It may contain 22kHz or 44kHz mono or stereo wave files.
	/// Also supports LZMA compression. No native sound compression is supported.
	class Blob : public Base {
		friend void LoadBlob(Blob *snd, std::istream &data, int Size);
	public:
		////04010000h (Extended, Blob)
		virtual GID::Type GetGID() const { return GID::Blob; }

		////Type
		int Type;
		//Does not work yet
		bool LateLoading;

		Blob() : Type(0) { }
		
		int GetSize() const { return data.size(); }
		
		////Destroys used data
		void destroy() { std::vector<Byte> t; data.swap(t); }
		
		////Destroys used data
		virtual ~Blob() { destroy(); }
		
		std::vector<Byte> &GetData() {
			return data;
		}

		bool ImportFile(const std::string &filename) { return ImportFile(filename, Type); }
		bool ImportFile(const std::string &filename, int type);
		bool AppendFile(const std::string &filename);
	protected:
		////Blob data
		std::vector<Byte> data;

	};
} }
