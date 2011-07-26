#pragma once

namespace gge { namespace widgets {
	class IProgressor {
	public:
		virtual void SetRange(float min, float max)=0;
		virtual void setValue(float value)=0;
	};
} }