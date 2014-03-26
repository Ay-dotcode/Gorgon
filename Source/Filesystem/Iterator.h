///@file Iterator.h contains filesystem Iterator. Lists file and directories.

#pragma once

#include <iterator>

namespace Gorgon {
	
	namespace Filesystem {
		
		///
		/// @cond INTERNAL
		namespace internal {
			class iterator_data;
		}
		/// @endcond
		///
		
		/// This iterator allows iteration of directories. It is a forward only
		/// iterator. An empty iterator can be used for end(). Also instead of
		/// comparing the iterator with end, IsValid() function could be used.
		class Iterator : public std::iterator<std::forward_iterator_tag, std::string> {
		public:

			/// Creates a new iterator from the given directory and pattern.
			/// @param  directory is the directory to be iterated. Should exists, otherwise
			///         it will throw PathNotFoundError
			/// @param  pattern wildcard pattern to match paths against
			/// @throw  PathNotFoundError if the given directory does not exists
			Iterator(const std::string &directory, const std::string &pattern="*");
			
			///
			/// Move constructor
			Iterator(Iterator &&dir) : data(dir.data), basedir(std::move(dir.basedir)), 
			current(std::move(dir.current)) { 
				dir.data=nullptr;
			}
			
			///
			/// Copy constructor
			Iterator(const Iterator &other);
			
			///
			/// Empty constructor. Effectively generates end iterator
			Iterator() : data(nullptr) { }
			
			///
			/// Assignment
			Iterator &operator =(Iterator other) { Swap(other); return *this; }
			
			///
			/// Destructor
			~Iterator() { Destroy(); }

			///
			/// Swaps iterators, used for move semantics
			void Swap(Iterator &other) {
				using std::swap;
				
				swap(data, other.data);
				swap(basedir, other.basedir);
				swap(current, other.current);
			}
			
			/// Returns the current path.
			/// @throw std::runtime_error (debug only) if the iterator is not vali
			std::string Get() const {
#ifdef DEBUG
				if(!data || current=="") {
					throw std::runtime_error("Invalid iterator.");
				}
#endif
					
				return current;
			}

			/// Returns the current path.
			/// @throw std::runtime_error (debug only) if the iterator is not vali
			const std::string *operator ->() {
#ifdef DEBUG
				if(!data || current=="") {
					throw std::runtime_error("Invalid iterator.");
				}
#endif
					
				return &current;
			}

			/// Returns the current path.
			/// @throw std::runtime_error (debug only) if the iterator is not valid
			operator std::string() const {
				return Get();
			}

			/// Returns the current path.
			/// @throw std::runtime_error (debug only) if the iterator is not valid
			std::string operator *() const {
				return Get();
			}

			/// Returns the current path.
			/// @throw std::runtime_error (debug only) if the iterator is not valid
			std::string Current() {
				return Get();
			}

			///
			/// Move to the next path in the directory
			Iterator &operator ++() {
				Next();

				return *this;
			}

			///
			/// Moves directory by i elements
			Iterator &operator +=(int i) {
				for(int j=0;j<i;j++)
					++(*this);
				
				return *this;
			}			
			
			///
			/// Move to the next path in the directory, return unmodified iterator
			Iterator operator ++(int) {
				Iterator it=*this;
				
				Next();
				return it;
			}

			/// Next path in the directory
			/// @return true if the iterator is valid
			/// @throw std::runtime_error (debug only) if the iterator is not valid
			bool Next();
			
			/// Destroys the current iterator.
			/// @cond INTERNAL
			/// Should set both data and current to empty
			/// @endcond
			void Destroy();

			///
			/// Checks whether the iterator is valid
			bool IsValid() const {
				return data && current!="";
			}

			///
			/// Compares two iterators
			bool operator ==(const Iterator &other) const {
				if(!data)
					return other.data==nullptr;
				else if(!other.data)
					return false;
				
				return other.basedir==basedir && other.current==current;
			}

			///
			/// Compares two iterators
			bool operator !=(const Iterator &other) const {
				if(!data)
					return other.data!=nullptr;
				else if(!other.data)
					return true;
				
				return other.basedir!=basedir || other.current!=current;
			}

		private:
			internal::iterator_data *data;
			std::string basedir;
			std::string current;
		};
		

		///
		/// Swaps two iterators
		inline void swap(Iterator &l, Iterator &r) {
			l.Swap(r);
		}
		
	}
}