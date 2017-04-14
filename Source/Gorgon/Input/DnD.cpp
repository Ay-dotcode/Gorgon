#include "DnD.h"

namespace Gorgon { namespace Input {

	void DragInfo::AddTextData(const std::string &text) {
		AssumeData(*new TextData(text));
	}

	void DragInfo::AddFileData(const std::string &file) {
		AssumeData(*new FileData(file));
	}

	void DragInfo::AddData(ExchangeData &value) {
		data.Add(value);
	}

	void DragInfo::AssumeData(ExchangeData &value) {
		data.Add(value);
		destroylist.Add(value);
	}


	bool DragInfo::HasData(Resource::GID::Type type) const {
		for(auto &d : data) {
			if(d.Type() == type)
				return true;
		}

		return false;
	}

	ExchangeData &DragInfo::GetData(Resource::GID::Type type) const {
		for(auto &d : data) {
			if(d.Type() == type)
				return d;
		}

		throw std::runtime_error("Cannot find data of type "+String::From(type));
	}

}
}