/// @file HashMap.h contains HashMap, a map of references

#pragma once

#pragma warning(error: 4239)

#include <map>
#include <stdexcept>
#include <algorithm>

#include "Iterator.h"


namespace Gorgon { 
	namespace Containers {

		template<class K_, class T_>
		class HashMap {
		public:
			
			void Add(const K_ &key, T_ &obj);
			
			void Remove(const K_ &key);
			
			void Delete(const K_ &key);
			
			void RemoveAll();
			
			void DeleteAll();
			
			long GetCount() const;
			
			/// If not found throws.
			T_ &operator [](const K_ &key);
			
			bool Exists(const K_ &key);
			
		private:
			std::map<K_, T_*> mapping;
		};
		
	}
}