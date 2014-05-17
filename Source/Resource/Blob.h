#pragma once

#include <vector>
#include <memory>

#include "../Types.h"
#include "Base.h"

namespace Gorgon { namespace Resource {
	class File;
	class Blob;
	
	////This is sound resource. It may contain 22kHz or 44kHz mono or stereo wave files.
	/// Also supports LZMA compression. No native sound compression is supported.
	class Blob : public Base {
	public:

		/// The type information related to the blob
		typedef int Type;

		/// Default constructor
		Blob() { }

		/// Destructor
		virtual ~Blob() { }

		/// 04010000h (Extended, Blob)
		virtual GID::Type GetGID() const { return GID::Blob; }

		/// Size of the blob
		unsigned long GetSize() const { return data.size(); }

		/// Returns the type of the blob
		Type GetType() const { return type; }

		/// Readies the blob for data writing. Erases previous data, sets current size and type. Also
		/// marks blob as loaded.
		std::vector<Byte> &Ready(unsigned long size, Type type=0) {
			data.resize(0);
			data.resize(size);

			this->type=type;
			isloaded=true;
		}
		
		/// Destroys the data stored in the blob
		void Destroy() { 
			type=0;
			std::vector<Byte> t; data.swap(t); 
			isloaded=false;
		}

		/// Loads the blob from the disk. This function should only be called if the blob is not loaded yet.
		bool Load();

		/// Returns whether the blob data is loaded
		bool IsLoaded() const { return isloaded; }
		
		/// Returns the data stored in this blob. It is safe to change its contents, even its size.
		/// However, its better to use reset to adjust the size and the type of the blob
		std::vector<Byte> &GetData() { return data; }

		/// Imports the given file as data without changing the type of the blob
		bool ImportFile(const std::string &filename) { 
			return ImportFile(filename, type); 
		}

		/// Imports the given file as data and sets the type
		bool ImportFile(const std::string &filename, Type type);

		/// Appends the given file to the end of the blob data
		bool AppendFile(const std::string &filename);

		////This function loads a sound resource from the given file
		static Blob *LoadResource(File &File, std::istream &data, unsigned long size);

	protected:

		/// Loads the blob from the data stream
		bool load(std::istream &data, unsigned long size, bool forceload);

		/// Entry point of this resource within the physical file. This value is stored for 
		/// late loading purposes
		unsigned long entrypoint = -1;

		/// Used to handle late loading
		std::weak_ptr<File> file;

		/// Whether this blob is loaded or not
		bool isloaded = false;

		/// Type of the blob data
		Type type = 0;

		/// Blob data
		std::vector<Byte> data;

	};
} }
