#include "DnD.h"
#include "../Window.h"

namespace Gorgon { namespace Input {

	bool needsclip(Input::Mouse::EventType event);

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
	
	void initdrag() {
        
    }

	void begindrag() {
		DragOperation = new DragInfo();
        
        initdrag();
	}

	void begindrag(DragSource &source) {
		DragOperation = new DragInfo(source);
        
        initdrag();
	}

	bool DropTarget::propagate_mouseevent(Input::Mouse::EventType event, Geometry::Point location, Input::Mouse::Button button, float amount, MouseHandler &handlers) {
		if(!IsDragging()) return false;
        
        auto &op = GetDragOperation();

		if(event == Input::Mouse::EventType::HitCheck) {
			dotransformandclip(true);

			mytransform = Transform;

			reverttransformandclip();
		}

		auto curlocation = mytransform * location;


		if(needsclip(event)) {
			dotransformandclip(true);

			bool out = false;
			if(
				curlocation.X < 0 ||
				curlocation.Y < 0 ||
				curlocation.X >= Clip.Width() ||
				curlocation.Y >= Clip.Height()
				)
				out = true;

			reverttransformandclip();

			if(out) return false;
		}

		if(hitcheck && !hitcheck(*this, curlocation))
			return false;

		if(event == Input::Mouse::EventType::Over) {
			if(over) {
				bool ret = over(*this, op);
                if(ret && op.HasSource() && op.GetSource().over) {
                    op.GetSource().over(op.GetSource(), op);
                }
            }

			return true;
		}

		return false;
	}

	DragInfo *DragOperation = nullptr;
    
	extern Event<void, DragInfo &> DragStarted;
    
	extern Event<void, DragInfo &, bool> DragEnded;
} }
