#pragma once

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <functional>
#include <map>
#include <memory>

#include "Base.h"
#include "Folder.h"
#include "../Filesystem.h"


namespace Gorgon { namespace Resource {

	/// This class defines a resource loader. The set of predefined resource loaders exists within
	/// the resource system, however, it is possible to register custom loaders for custom objects.
	class Loader {
	public:
		/// This is Resource loader function prototype. Please note that, it is possible to use your own
		/// type derived from Base for the return type. First parameter is the stream object to read data
		/// from. The stream object would be opened in binary mode. Second parameter is the size of the 
		/// data chunk. After reading the object, file pointer should be moved exactly the amount of data
		/// chunk. Last parameter is the file object that manages the current load operation. This function 
		/// should return nullptr in case of an error, otherwise it should return the newly loaded object. 
		/// The ownership of this object will be transferred to the parent of the object and it will be 
		/// deleted with its parent unless detached from the tree.
		typedef std::function<Base* (std::istream &, unsigned long, File &)> LoaderFunction;

		/// Filling constructor
		Loader(GID::Type gid, LoaderFunction handler) : GID(gid), Handler(handler) { }

		/// Empty constructor
		Loader() : GID(Resource::GID::None) { }

		/// Gorgon ID of the resource that this loaded can load.
		Resource::GID::Type GID;

		/// Load handler function
		LoaderFunction Handler;
	};


	/// This class represents a loading error
	class LoadError : public std::runtime_error {
	public:

		/// Error types
		enum ErrorType {
			/// An unknown error occurred
			Unknown			= 0,

			/// Cannot find the given file
			FileNotFound	= 1,

			/// File does not contain correct signature and is probably not a Gorgon resource
			Signature		= 2,

			/// Version in the file is not recognized
			VersionMismatch	= 3,

			/// There is no containing root folder
			Containment		= 4,

			/// There is an unknown node in the file. This is never thrown in release mode.
			UnknownNode		= 5,

			/// Cannot open the given file
			FileCannotBeOpened = 6,
		};

		/// Regular constructor, creates error text from error number
		LoadError(ErrorType number=Unknown) : runtime_error(ErrorStrings[number]), number(number) {

		}

		/// A constructor to allow custom text for the error
		LoadError(ErrorType number, const std::string &text) : runtime_error(text), number(number) {

		}

		/// The type of loading error occurred
		ErrorType number;

		/// Strings for error codes
		static const std::string ErrorStrings[7];
	};


	/// This class represents a logical resource file. A file class is necessary for loading a resource
	/// file from the disk. However, it is not required to create a resource tree in memory. Notice that 
	/// Gorgon::Resource does not support saving, Gorgon::Resource::Editing has the support for saving
	/// resource files.
	class File {
	public:

		/// Default constructor
		File() : root(new Folder), LoadNames(false) { }

		/// Destroys file object. If the root is not detached, it will destroy resource tree as well.
		~File() { 
			delete root;
			delete file; // deleting nullptr is safe
		}


		/// Returns the root folder of the file
		Folder &Root() { return *root; }

		/// Detaches the root of the File from the File object. The ownership of the root will be transferred
		/// to the caller.
		std::unique_ptr<Folder> Release() { 
			std::unique_ptr<Folder> r(root);
			root=new Folder; 

			return r;
		}

		/// Prepares all resources in this file to be used. This function can be issued for individual objects
		/// and their children rather than whole file.
		void Prepare() {
			root->Prepare();
		}

		/// Discards any data that is not required after preparation. The stored data might have significance in
		/// some applications, other than those, there is no reason to keep prepared data. Also discards guid
		/// mapping
		void Discard() {
			root->Discard();

			decltype(mapping) temp;
			mapping.swap(temp);
			delete file;
			file=nullptr;
		}

		/// Destroys the Gorgon resource tree that this file holds. This file object can still be used after
		/// destroy is issued
		void Destroy() {
			delete root;
			root=new Folder;
			filename="";
			fileversion=0;
			filetype=GID::None;
			isloaded=false;
			delete file;
			file=nullptr;
		}


		/// Returns the filename used for the last load operation
		std::string GetFilename() const { return filename; }

		/// Loads the given file. A prepare function call is necessary to be able to use some resources.
		/// The file could be left open if there are objects marked to be loaded when requested.
		/// If the filename not found and there is a filename.lzma file, this function extracts the compressed
		/// file and tries to load uncompressed version.
		/// @throws LoadError
		/// @throws std::runtime_error
		void LoadFile(const std::string &Filename) { load(Filename, false, false); }

		/// Loads only the first object of the given file. Useful to retrieve header information.
		/// If the filename not found and there is a filename.lzma file, this function extracts the compressed
		/// file and tries to load uncompressed version.
		/// @throws LoadError
		/// @throws std::runtime_error
		void LoadFirst(const std::string &Filename) { load(Filename, true, false); }

		/// Loads only the first tier of objects. Only folders refrain from loading its children. Therefore,
		/// any other object will be loaded fully. This function should be use carefully in presence of links
		/// Any links that are reaching out to unloaded parts of the file will not be resolved. This may cause
		/// cast errors. Useful to selectively load a large file. Use Folder::Load function to load the contents
		/// of a specific folder. This function leaves the file open for further load attempts.
		/// If the filename not found and there is a filename.lzma file, this function extracts the compressed
		/// file and tries to load uncompressed version.
		/// @throws LoadError
		/// @throws runtime_error
		void LoadShallow(const std::string &Filename) { load(Filename, false, true); }

		/// Returns if a file is loaded
		bool IsLoaded() const { return isloaded; }

		/// Loads a resource object from the given file, GID and size. This function may return nullptr
		/// in cases that the object cannot be loaded or no loader exists for the given gid. Both cases
		/// will throw in debug mode. This behavior is different from Editing library where it throws if
		/// an object cannot be loaded and reports a warning and stores the data of unknown object in a
		/// special container.
		/// @warning This function is intended to be used while loading a resource. It can be used for
		///          any purpose, however, would not be very useful outside its prime use
		Base *LoadObject(std::istream &data, GID::Type gid, unsigned long size);

		/// Keeps the file open even after loading is completed. This guarantees that the file is readable at a later
		/// point to read more data. Discard function closes the file.
		void KeepOpen() {
			keepopen=true;
		}

		/// **For resource loaders**, opens the datafile for reading. A file should at least partially loaded to be able to
		/// perform this operation. Even if the file is discarded, it will be tried to open and returned.
		/// @throw Filesystem::PathNotFoundError if the file no longer exists
		/// @throw std::logic_error if file is never opened in the first place
		std::ifstream &open() {
			if(!isloaded) throw std::logic_error("File is not opened");

			if(!file) {
				file=new std::ifstream(filename);
				if(!file->is_open()) {
					file=nullptr;
					throw Filesystem::PathNotFoundError("Cannot find: "+filename);
				}
			}

			return *file;
		}

		/// Resource Loaders. You may add or remove any loaders that is necessary. Initially a file loads all
		/// internal resources. 
		std::map<GID::Type, Loader>	 Loaders;

		/// Whether to load names or not. Notice that this value effects if the objects have indeed a name.
		/// Additionally, folders will build named indexes if significant names property is set. This prevents
		/// unnecessary traversal of non properly named directories. Even though mappings are not built, if
		/// this variable is set, all existing names will be loaded.
		bool LoadNames = false;

		/// **INTERNAL**, allows guid to object mapping. This information is not kept fresh about changes in the
		/// tree. This information is used for link and object tracking and is consumed right after file is
		/// loaded. This information is kept until a discard is issued.
		/// @warning Stale information.
		mutable std::map<SGuid, Base*> mapping;

	protected:
		void load(const std::string &filename, bool first, bool shallow);

		/// The root folder, root changes while loading a file
		Folder *root;

		/// Whether the file is loaded from the disk
		bool isloaded;

		/// Name of the file that is loaded from the disk. This should be canonicalized to avoid directory change
		/// issues.
		std::string filename;

		/// Type of the loaded file
		GID::Type filetype;

		/// Keeps the file open even after loading is completed. This guarantees that the file is readable at a later
		/// point to read more data.
		bool keepopen=false;

		/// The input file, can be left open. It should be deleted and set to nullptr after closing.
		std::ifstream *file=nullptr;

		/// Version of the loaded file. Currently there is only a single version. This version does not relate to
		/// the versions of the resources in the resource file.
		unsigned long fileversion;

	private:
	};

	/// @name Platform independent data readers
	/// @{
	/// These functions allow platform independent data reading capability. In worst case, where the platform
	/// cannot be supported, they stop compilation instead of generating incorrectly working system.

	/// Reads a 32-bit integer from the stream. A long is at least 32 bits, could be more
	/// however, only 4 bytes will be read from the stream
	inline long ReadInt32(std::istream &file) {
		int32_t r;
		file.read(reinterpret_cast<char*>(&r), 4);

		return r;
	}

	/// Reads a 32-bit unsigned integer from the stream. An unsigned long is at least 32 bits, could be more
	/// however, only 4 bytes will be read from the stream
	inline long ReadUInt32(std::istream &file) {
		uint32_t r;
		file.read(reinterpret_cast<char*>(&r), 4);

		return r;
	}

	/// Reads a 16-bit integer from the stream. An int is at least 16 bits, could be more
	/// however, only 2 bytes will be read from the stream
	inline long ReadInt16(std::istream &file) {
		int16_t r;
		file.read(reinterpret_cast<char*>(&r), 2);

		return r;
	}

	/// Reads a 32-bit unsigned integer from the stream. An unsigned int is at least 32 bits, could be more
	/// however, only 2 bytes will be read from the stream
	inline long ReadUInt16(std::istream &file) {
		uint16_t r;
		file.read(reinterpret_cast<char*>(&r), 2);

		return r;
	}

	/// Reads a 32 bit IEEE floating point number from the file. This function only works on systems that
	/// that have native 32 bit floats.
	inline float ReadFloat(std::istream &file) {
		static_assert(sizeof(float) == 4, "Current implementation only supports 32bit floats");

		float r;
		file.read(reinterpret_cast<char*>(&r), 4);

		return r;
	}

	/// Reads a 64 bit IEEE double precision floating point number from the file. This function only works 
	/// on systems that have native 64 bit doubles.
	inline float ReadDouble(std::istream &file) {
		static_assert(sizeof(double) == 8, "Current implementation only supports 64bit floats");

		float r;
		file.read(reinterpret_cast<char*>(&r), 4);

		return r;
	}

	/// Reads a string from a given stream. Assumes the size of the string is appended before the string as
	/// 32-bit unsigned value.
	inline std::string ReadString(std::istream &file) {
		int temp;
		std::string ret;
		temp=ReadUInt32(file);
		ret.resize(temp);
		file.read(&ret[0], temp);

		return ret;
	}

	/// Reads a GID from the given stream.
	inline GID::Type ReadGID(std::istream &file) {
		return GID::Type(ReadUInt32(file));
	}

	/// Reads a GUID from the given stream.
	inline SGuid ReadGuid(std::istream &file) {
		return SGuid(file);
	}

	/// Removes a chunk of data with the given size from the stream
	inline void EatChunk(std::istream &file, std::streamoff size) {
		file.seekg(size, std::ios::cur);
	}

	/// Removes a chunk of data from the stream, the size will be read from the stream
	inline void EatChunk(std::istream &file) {
		long size=ReadUInt32(file);
		file.seekg(size, std::ios::cur);
	}

	/// @}

} }

/// Adds an integer to streampos
inline long operator +(const std::streampos &l, long r) {
	return (long)l + r;
}

/// Adds an unsigned integer to streampos
inline unsigned long operator +(const std::streampos &l, unsigned long r) {
	return (long)l + r;
}
