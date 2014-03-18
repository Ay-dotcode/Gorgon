#pragma once

#include <string>
#include <stdexcept>
#include <vector>

namespace Gorgon {

	/// Contains filesystem functions. All file related functions in Gorgon
	/// uses forward slash as directory separator. If necessary, the functions
	/// will convert them.
	namespace Filesystem {
		
		/// This object is thrown from functions that return
		/// information rather than status.
		class PathNotFoundError : public std::runtime_error {
		public:
			///
			/// Default constructor
			PathNotFoundError() : std::runtime_error("File not found") { }
			
			///
			/// Constructor that sets error text
			PathNotFoundError(const std::string &what) : std::runtime_error(what) { }
		};
	
		/// Creates a new directory. This function works recursively to
		/// create any missing parent directories as well. If directory
		/// exists, this function returns true.
		/// @param path is the path of the directory to be created. Should contain
		///        forward slash as directory separator.
		/// @return true if the directory is ready to be used, false
		///         otherwise
		bool CreateDirectory(const std::string &path);
		
		/// Checks whether the given path is a directory.
		/// @param path is the directory to be checked. Should contain
		///        forward slash as directory separator.
		/// @return true if the given path is present and a directory
		bool IsDirectory(const std::string &path);

		/// Checks whether the given path is a file
		/// @param path is the file to be checked. Should contain
		///        forward slash as directory separator.
		/// @return true if the given path is present and a file
		bool IsFile(const std::string &path);
		
		/// Checks whether the given path exists
		/// @param path is the file to be checked. Should contain
		///        forward slash as directory separator.
		/// @return true if the given path is present
		bool IsExists(const std::string &path);

		/// Checks whether the given path is writable
		/// @param  path is the directory or file to be checked. Should contain
		///         forward slash as directory separator.
		/// @return true if the given file/path exists and is writable. 
		bool IsWritable(const std::string &path);

		/// Checks whether the given path is hidden
		/// @param  path is the directory to be checked. Should contain
		///         forward slash as directory separator.
		/// @return true if the path should be hidden. 
		bool IsHidden(const std::string &path);

		/// Canonizes a given relative path
		/// @param  path is the file/directory to be canonized. Should contain
		///         forward slash as directory separator.
		/// @return the full path of the given relative path. Forward slashes are 
		///         used as directory separator
		/// @throw  std::runtime_error if canonize fails. This may or may not be
		///         be related to path being inexisting or inaccessible.
		std::string Canonize(const std::string &path);

		/// Deletes the given file or directory. If the directory is not empty,
		/// this function will delete all its contents.
		/// @param  path is the file/directory to be deleted. Should contain
		///         forward slash as directory separator.
		/// @return true if the given path is deleted. If the given path does
		///         not exists, this function will still return true.
		bool Delete(const std::string &path);
		
		/// Changes current working directory.
		/// @param  path is the directory to become current directory. Should contain
		///         forward slash as directory separator.
		/// @return false if directory does not exist
		bool ChangeDirectory(const std::string &path);
		
		/// Returns the current working directory.
		/// @return the current working directory Forward slashes are 
		///         used as directory separator
		std::string CurrentDirectory();
		
		/// Copies a file or directory from the given source to destination.
		/// @param source file or directory. Should either be a single file, or single directory
		/// @param target is the new filename or directory name
		/// @return true on success
		bool Copy(const std::string &source, const std::string &target);
		
		/// Copies list of files and/or directories from the given source to destination.
		/// @param  source list of source file or directories
		/// @param  target is directory to copy files or directories into
		/// @return true on success
		bool Copy(const std::vector<std::string> &source, const std::string &target);
		
		/// Returns the size of the given file. If the file is not found 0 is returned.
		/// @param  filename is the name of the file
		/// @return size of the given file
		unsigned long long Size(const std::string &filename);
		
		/// Moves a given file or directory. Target is the new path rather than
		/// the target directory. This function can also be used to rename a file or
		/// directory.
		/// @param  source file or directory to be moved or renamed
		/// @param  target path
		/// @return true on success. On Windows this function may not work across
		///         different drives. If this case is possible, use Copy then Delete.
		bool Move(const std::string &source, const std::string &target);
		
		/// Saves a given data into the filename. If the file exists, it will be appended
		/// if append parameter is set to true. This function can handle binary data.
		bool Save(const std::string &filename, const std::string &data, bool append=false);
		
		/// Loads the given file and returns it in a string form. Notice that there is no
		/// size restriction over the file. This function can handle binary data. Throws
		/// PathNotFoundError if the file cannot be found. Before deciding on file is not 
		/// found, this function checks if there is a lzma compressed file as filename.lzma
		/// @param  filename is the file to be loaded
		/// @return the data loaded from the file
		/// @throw  PathNotFoundError if the file cannot be read or does not exits
		std::string Load(const std::string &filename);
		
		/// Locates the given file or directory. If localonly is true, this function only
		/// searches locations that are in the working directory. If it is set to false
		/// standard system locations like user home directory or application data directory
		/// is also searched. While looking for the resource, if the directory parameter is 
		/// not empty, the resource is expected to be in the given directory under the local
		/// or system wide directory. Additionally, if the file is found as a lzma compressed 
		/// file, it will be extracted. For instance, if directory parameter is "images", 
		/// localonly is false, systemname is system and we are looking for icon.png, this 
		/// function checks whether file exists in the following forms. 
		/// The first one found is returned, if none exists, PathNotFoundError exception is thrown.
		/// The following list assume user path to be ~ application data path to be ~/apps
		/// images/icon.png, ../images/icon.png, icon.png, ../icon.png,
		/// images/icon.png.lzma, ../images/icon.png.lzma, icon.png.lzma, ../icon.png.lzma,
		/// ~/.system/images/icon.png, ~/.system/images/icon.png.lzma, ~/apps/.system/images/icon.png,
		/// ~/apps/.system/images/icon.png.lzma, ~/system/images/icon.png, ~/system/images/icon.png.lzma, 
		/// ~/apps/system/images/icon.png, ~/apps/system/images/icon.png.lzma, ~/images/icon.png,
		/// ~/images/icon.png.lzma, ~/apps/images/icon.png, ~/apps/images/icon.png.lzma, 
		/// ~/system/icon.png, ~/system/icon.png.lzma, ~/apps/system/icon.png, 
		/// ~/apps/system/icon.png.lzma
		/// If compressed file is found, it fill be extracted in place, and the extracted filename
		/// will be returned. Therefore, its not necessary to check if the file is compressed or not.
		/// @param  path is the filename or directory to be searched. Only compressed files are handled.
		/// @param  directory is the directory the resource expected to be in. Should be relative.
		/// @param  localonly if set, no system or user directories will be searched
		/// @return the full path of the resource
		/// @throw  PathNotFoundError if the file cannot be found
		/// @throw  std::runtime_error if the file cannot be read
		std::string LocateResource(const std::string &path, const std::string &directory="", bool localonly=true);
		
		/// Returns the directory where the program is started from. This will always return the same value 
		/// through out the execution.
		std::string StartupDirectory();

		/// This class represents filesystem entry points (roots, drives). On Linux like systems, the
		/// only entry point is '/', however, user home directory is also listed. On Windows all drives as listed.
		class EntryPoint {
		public:
		
			///
			/// Default constructor
			EntryPoint() : Readable(), Writable() { }
		
			/// The path of the entry point
			std::string Path;
			
			/// Whether the entry point is readable. Currently all entry points are readable
			bool Readable;
			
			/// Whether the entry point is writable. Notice that even an entry point is writable
			/// it doesn't mean that the immediate path of the entry point is writable. It is 
			/// possible that the user has no write access to the root of the entry point. If false
			/// this denotes the entry point is fully read-only (like a CDROM)
			bool Writable;
						
			/// Name or label of the entry point.
			std::string Name;
		};
		
		/// This function returns all entry points in the current system. This function does not
		/// perform caching and should be used sparingly. It may cause Windows systems to read 
		/// external devices.
		/// @return The list of entry points
		std::vector<EntryPoint> EntryPoints();

	}

}
