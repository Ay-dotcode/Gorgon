#pragma once

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <functional>
#include <map>
#include <memory>

#include "../Utils/Assert.h"
#include "Base.h"
#include "Folder.h"
#include "../Filesystem.h"
#include "../IO/Stream.h"


namespace Gorgon { namespace Resource {

	class Reader;

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
		typedef std::function<Base* (std::weak_ptr<File>, std::shared_ptr<Reader>, unsigned long)> LoaderFunction;

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

			/// There is no file object associated with the resource. Generally thrown during late loading
			NoFileObject = 7,
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
		static const std::string ErrorStrings[8];
	};

	/**
	 * This class allows resource objects to read data from a stream. It provides functionality
	 * to read data platform independently.
	 */
	class Reader {
	public:

		/// Marks a target position in file. Can be checked if it is reached.
		/// Asserts to make sure target is not passed.
		class Mark {
		public:
			/// Constructs a target using a reader and absolute file position.
			/// Prefer using Reader::Target function to create a target.
			Mark(Reader &reader, unsigned long target) :
			reader(&reader), target(target) { }

			/// Copy constructor
			Mark(const Mark &) = default;

			/// Checks if the target is reached
			explicit operator bool() const {
				auto pos=reader->Tell();
				if(pos<target) return false;

				ASSERT(pos==target, "Reading operation passed the target.");					
					
				return true;
			}

			/// Tells the position of this mark
			unsigned long Tell() const {
				return target;
			}

		private:
			Reader *reader;
			unsigned long target;
		};

		Reader() { }

		virtual ~Reader() {
		}

		/// Request reader to keep reading stream open. There is an internal counter that makes 
		/// sure that the reader is closed when it is no longer needed.
		void KeepOpen() {
			keepopenrequests++;
		}

		/// Marks that this reader is no longer needed. This action will decrement requests made
		/// to keep the stream open. If no object requires this reader, it is closed. Note that
		/// some readers cannot be reopened.
		void NoLongerNeeded() {
			if(--keepopenrequests) {
				close();
				stream=nullptr;
			}
		}

		/// Opens the reader. If this operation fails, it will throw LoadError.
		void Open() {
			if(!stream) {
				open(true);
			}

			ASSERT(stream, "Reader did not set the stream.");
		}

		/// Checks if the stream is open
		bool IsOpen() const {
			return stream;
		}

		/// Checks if the stream is open and it can be read from
		bool IsGood() const {
			return stream && stream->good();
		}

		/// Tries to open the stream, if it fails, this function returns false.
		/// If this function returns true, read operations can be carried.
		bool TryOpen() {
			if(!stream) {
				return open(false);
			}

			return true;
		}


		/// This should be last resort, use if the actual stream is needed.
		std::istream &GetStream() {
			ASSERT(stream, "Reader is not opened.");
			ASSERT(IsGood(), "Reader is failed.");

			return *stream;
		}

		/// @name Platform independent data readers
		/// @{
		/// These functions allow platform independent data reading capability. In worst case, where the platform
		/// cannot be supported, they stop compilation instead of generating incorrectly working system. These functions
		/// might differ in encoding depending on the file version. Make sure a file is open before invoking these functions
		/// no runtime checks will be performed.

		/// Reads an enumeration as 32-bit integer from the stream.
		template<class E_>
		E_ ReadEnum32() {
			ASSERT(stream, "Reader is not opened.");
			ASSERT(IsGood(), "Reader is failed.");

			return IO::ReadEnum32<E_>(*stream);
		}

		/// Reads a 32-bit integer from the stream. A long is at least 32 bits, could be more
		/// however, only 4 bytes will be read from the stream
		long ReadInt32() {
			ASSERT(stream, "Reader is not opened.");
			ASSERT(IsGood(), "Reader is failed.");

			return IO::ReadInt32(*stream);
		}

		/// Reads a 32-bit unsigned integer from the stream. An unsigned long is at least 32 bits, could be more
		/// however, only 4 bytes will be read from the stream
		unsigned long ReadUInt32() {
			ASSERT(stream, "Reader is not opened.");
			ASSERT(IsGood(), "Reader is failed.");

			return IO::ReadUInt32(*stream);
		}

		/// Reads a 16-bit integer from the stream. An int is at least 16 bits, could be more
		/// however, only 2 bytes will be read from the stream
		int ReadInt16() {
			ASSERT(stream, "Reader is not opened.");
			ASSERT(IsGood(), "Reader is failed.");

			return IO::ReadInt16(*stream);
		}

		/// Reads a 16-bit unsigned integer from the stream. An unsigned int is at least 32 bits, could be more
		/// however, only 2 bytes will be read from the stream
		unsigned ReadUInt16() {
			ASSERT(stream, "Reader is not opened.");
			ASSERT(IsGood(), "Reader is failed.");

			return IO::ReadUInt16(*stream);
		}

		/// Reads an 8-bit integer from the stream. A char is at least 8 bits, could be more
		/// however, only 1 byte will be read from the stream
		char ReadInt8() {
			ASSERT(stream, "Reader is not opened.");
			ASSERT(IsGood(), "Reader is failed.");

			return IO::ReadInt8(*stream);
		}

		/// Reads an 8-bit unsigned integer from the stream. A char is at least 8 bits, could be more
		/// however, only 1 byte will be read from the stream
		Byte ReadUInt8() {
			ASSERT(stream, "Reader is not opened.");
			ASSERT(IsGood(), "Reader is failed.");

			return IO::ReadUInt8(*stream);
		}

		/// Reads a 32 bit IEEE floating point number from the file. This function only works on systems that
		/// that have native 32 bit floats.
		float ReadFloat() {
			ASSERT(stream, "Reader is not opened.");
			ASSERT(IsGood(), "Reader is failed.");

			return IO::ReadFloat(*stream);
		}

		/// Reads a 64 bit IEEE double precision floating point number from the file. This function only works 
		/// on systems that have native 64 bit doubles.
		float ReadDouble() {
			ASSERT(stream, "Reader is not opened.");
			ASSERT(IsGood(), "Reader is failed.");

			return IO::ReadDouble(*stream);
		}

		/// Reads a boolean value. In resource 1.0, booleans are stored as 32bit integers
		bool ReadBool() {
			ASSERT(stream, "Reader is not opened.");
			ASSERT(IsGood(), "Reader is failed.");

			return IO::ReadBool(*stream);
		}

		/// Reads a string from a given stream. Assumes the size of the string is appended before the string as
		/// 32-bit unsigned value.
		std::string ReadString() {
			ASSERT(stream, "Reader is not opened.");
			ASSERT(IsGood(), "Reader is failed.");

			return IO::ReadString(*stream);
		}

		/// Reads a string with the given size.
		std::string ReadString(unsigned long size) {
			ASSERT(stream, "Reader is not opened.");
			ASSERT(IsGood(), "Reader is failed.");

			return IO::ReadString(*stream, size);
		}

		/// Reads an array from the file. Array type should be given a fixed size construct, otherwise
		/// a mismatch between binary formats will cause trouble.
		/// @param  data is the data to be read from the file
		/// @param  size is the number of elements to be read
		template<class T_>
		void ReadArray(T_ *data, unsigned long size) {
			ASSERT(stream, "Reader is not opened.");
			ASSERT(IsGood(), "Reader is failed.");

			IO::ReadArray<T_>(*stream, data, size);
		}

		/// Reads a GID from the given stream.
		GID::Type ReadGID() {
			ASSERT(stream, "Reader is not opened.");
			ASSERT(IsGood(), "Reader is failed.");

			return GID::Type(ReadUInt32());
		}

		/// Reads a GUID from the given stream.
		SGuid ReadGuid() {
			ASSERT(stream, "Reader is not opened.");
			ASSERT(IsGood(), "Reader is failed.");

			return IO::ReadGuid(*stream);
		}

		/// Reads chunk size from a stream
		unsigned long ReadChunkSize() {
			ASSERT(stream, "Reader is not opened.");
			ASSERT(IsGood(), "Reader is failed.");

			return ReadUInt32();
		}

		/// Removes a chunk of data with the given size from the stream
		void EatChunk(long size) {
			ASSERT(stream, "Reader is not opened.");
			ASSERT(IsGood(), "Reader is failed.");

			stream->seekg(size, std::ios::cur);
		}

		/// Removes a chunk of data from the stream, the size will be read from the stream
		void EatChunk() {
			ASSERT(stream, "Reader is not opened.");
			ASSERT(IsGood(), "Reader is failed.");

			long size=ReadUInt32();
			stream->seekg(size, std::ios::cur);
		}

		/// Tells the current position
		unsigned long Tell() const {
			ASSERT(stream, "Reader is not opened.");
			ASSERT(IsGood(), "Reader is failed.");

			return (unsigned long)stream->tellg();
		}

		/// Seeks to the given position
		void Seek(unsigned long pos) {
			ASSERT(stream, "Reader is not opened.");
			ASSERT(IsGood(), "Reader is failed.");

			stream->seekg((std::streampos)pos, std::ios::beg);
		}

		/// Seeks to the given position
		void Seek(const Mark &pos) {
			ASSERT(stream, "Reader is not opened.");
			ASSERT(IsGood(), "Reader is failed.");

			stream->seekg((std::streampos)pos.Tell(), std::ios::beg);
		}

		/// Creates mark to the the target that is delta distance from current
		/// point in file
		Mark Target(unsigned long delta) {
			return Mark(*this, Tell()+delta);
		}

		bool ReadCommonChunk(Base &self, GID::Type gid, unsigned long size);

		/// @}

	protected:
		/// This function should close the stream. The pointer will be unset
		/// by Reader class
		virtual void close() = 0;

		/// This function should open the stream and set stream member. If thrw is set
		/// to true and stream cannot be opened, a LoadError should be thrown. Otherwise
		/// this function is not allowed to throw.
		virtual bool open(bool thrw) = 0;

		/// This is the stream that will be used to read data from. Underlying readers
		/// can have specialized copies of this member.
		std::istream *stream = nullptr;

	private:
		int keepopenrequests = 0;
	};

	/**
	 * This class allows resource objects to save their data to a stream. It provides functionality
	 * to write data platform independently. This class also allows back and forth writing to easy
	 * writing Gorgon resources.
	 */
	class Writer {
	};
	
	/// This is a file reader. Allows a Gorgon Resource to be loaded from a file
	class FileReader : public Reader {
	public:
		/// Constructor requires a file to be opened later.
		FileReader(const std::string &filename) : filename(filename) {
			try {
				this->filename=Filesystem::Canonical(filename);
			} catch(...) { }
		}


	protected:
		virtual void close() override {
			delete file;
		}

		virtual bool open(bool thrw) override {
			file = new std::ifstream(filename, std::ios::binary);

			if(!file->is_open()) {
				delete file;
				if(thrw) {
					throw LoadError(LoadError::FileNotFound, "Cannot open file: "+filename);
				}

				return false;
			}

			stream = file;
			this->filename=Filesystem::Canonical(filename);

			return true;
		}

	private:
		std::string filename;
		std::ifstream *file = nullptr;
	};

	/// This class represents a logical resource file. A file class is necessary for loading a resource
	/// file from the disk. However, it is not required to create a resource tree in memory. Notice that 
	/// Gorgon::Resource does not support saving, Gorgon::Resource::Editing has the support for saving
	/// resource files.
	class File {
	public:

		/// Default constructor
		File();

		/// Destroys file object. If the root is not detached, it will destroy resource tree as well.
		virtual ~File() {
			delete root;
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
		}

		/// Destroys the Gorgon resource tree that this file holds. This file object can still be used after
		/// destroy is issued
		void Destroy() {
			delete root;
			root=new Folder;
			fileversion=0;
			filetype=GID::None;
		}


		/// Loads the given file. A prepare function call is necessary to be able to use some resources.
		/// The file could be left open if there are objects marked to be loaded when requested.
		/// If the filename not found and there is a filename.lzma file, this function extracts the compressed
		/// file and tries to load uncompressed version.
		/// @throws LoadError
		/// @throws std::runtime_error
		void LoadFile(const std::string &filename) { 
			createfilereader(filename);

			load(false, false); 
		}

		/// Loads only the first object of the given file. Useful to retrieve header information.
		/// If the filename not found and there is a filename.lzma file, this function extracts the compressed
		/// file and tries to load uncompressed version.
		/// @throws LoadError
		/// @throws std::runtime_error
		void LoadFirst(const std::string &filename) { 
			createfilereader(filename);

			load(true, false);
		}

		/// Loads only the first tier of objects. Only folders refrain from loading its children. Therefore,
		/// any other object will be loaded fully. This function should be use carefully in presence of links
		/// Any links that are reaching out to unloaded parts of the file will not be resolved. This may cause
		/// cast errors. Useful to selectively load a large file. Use Folder::Load function to load the contents
		/// of a specific folder. This function leaves the file open for further load attempts.
		/// If the filename not found and there is a filename.lzma file, this function extracts the compressed
		/// file and tries to load uncompressed version.
		/// @throws LoadError
		/// @throws runtime_error
		void LoadShallow(const std::string &filename) { 
			createfilereader(filename);

			load(false, true);
		}

		/// Loads a resource object from the given file, GID and size. This function may return nullptr
		/// in cases that the object cannot be loaded or no loader exists for the given gid. Both cases
		/// will throw in debug mode. Also handles SGuid and name
		/// @warning This function is intended to be used while loading a resource. It can be used for
		///          any purpose, however, would not be very useful outside its prime use
		Base *LoadChunk(Base &self, GID::Type gid, unsigned long size, bool skipobjects=false);

		/// Returns a weak reference to this file. This returned reference can then be used to test if this
		/// object is still in memory.
		std::weak_ptr<File> Self() const {
			return self;
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
		/// This is the actual load function
		void load(bool first, bool shallow);

		/// The root folder, root changes while loading a file
		Folder *root;

		/// Type of the loaded file
		GID::Type filetype;

		/// Keeps the file open even after loading is completed. This guarantees that the file is readable at a later
		/// point to read more data.
		bool keepopen=false;

		/// Version of the loaded file. Currently there is only a single version. This version does not relate to
		/// the versions of the resources in the resource file.
		unsigned long fileversion;

		/// The reader that would be used to read the file
		std::shared_ptr<Reader> reader;

	private:
		void createfilereader(std::string filename);
		std::shared_ptr<File> self;
	};
} }
