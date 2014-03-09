#pragma once

namespace Gorgon {

	/// Contains filesystem functions. All file related functions in Gorgon
	/// uses forward slash as directory separator. If necessary, the functions
	/// will convert them.
	namespace Filesystem {
	
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

		/// Checks whether the given path is writable
		/// @param path is the directory or file to be checked. Should contain
		///        forward slash as directory separator.
		/// @return true if the given file/path exists and is writable. 
		bool IsWritable(const std::string &path);

		/// Checks whether the given path is hidden
		/// @param path is the directory to be checked. Should contain
		///        forward slash as directory separator.
		/// @return true if the path should be hidden. 
		bool IsHidden(const std::string &path);

		/// Canonizes a given relative path
		/// @param path is the file/directory to be canonized. Should contain
		///        forward slash as directory separator.
		/// @return the full path of the given relative path. Forward slashes are 
		///         used as directory separator
		std::string Canonize(const std::string &path);

		/// Deletes the given file or directory. If the directory is not empty,
		/// this function will delete all its contents.
		/// @param path is the file/directory to be deleted. Should contain
		///        forward slash as directory separator.
		/// @return true if the given path is deleted. If the given path does
		///         not exists, this function will still return true.
		bool Delete(const std::string &path);
		
		/// Changes current working directory.
		/// @param path is the directory to become current directory. Should contain
		///        forward slash as directory separator.
		/// @return false if directory does not exist
		bool ChangeDirectory(const std::string &path);
		
		/// Returns the current working directory.
		/// @return the current working directory Forward slashes are 
		///         used as directory separator
		std::string CurrentDirectory();
		
		bool Copy(const std::string &source, const std::string &target);
		
		unsigned long long Size(const std::string &filename);
		
		bool Move(const std::string &source, const std::string &target);
		
		bool Save(const std::string &filename, const std::string &data);
		
		//throws
		std::string Load(const std::string &filename);
		
		std::string LocateResource(std::string filename, std::string path="", bool localonly=true);

		class EntryPoint {
		public:
			std::string Path;
			bool Readable;
			bool Writable;
			unsigned Serial;
			std::string Name;
		};
		std::vector<EntryPoint> EntryPoints();

	}

}
