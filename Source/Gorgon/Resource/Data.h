#include "Base.h"

namespace Gorgon { namespace Resource {
	
	class DataItem {
	};
	
	class Data : public Base {
	public:
		
		Data() { }
		
		virtual GID::Type GetGID() const override { 
			return GID::Data;
		}
		
		template<class T_>
		T_ Get(int index) const;
		
		template<class T_>
		T_ Get(const std::string &name) const;
		
		DataItem &GetItem(int index) const;
		
		DataItem &GetItem(const std::string &name) const;
		
		int FindIndex(const std::string &name) const;
		
		int GetCount() const;
		
		void Remove(int index);
		
		void Remove(const std::string &name);
		
		/// Ownership of the item is transferred to this Data
		void Add(DataItem &item);
		
		void Insert(DataItem &item, int before);
		
		
		static Data *LoadResource(std::weak_ptr<File> file, std::shared_ptr<Reader> data, unsigned long size);
		
	private:
		virtual void save(Writer &writer) { }
	};
	
} }