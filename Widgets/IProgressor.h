#pragma once

namespace gorgonwidgets {
	class IProgressor {
	public:
		virtual void SetRange(float min, float max)=0;
		virtual void setValue(float value)=0;
	};
}