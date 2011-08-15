#pragma once

#include "..\Resource\Definitions.h"


namespace gge { namespace widgets {

	namespace GID {
		typedef resource::GID::Type Type;

		static const Type FileType			= 0x050100ff;



		static const Type Line				= 0x05110000;
		static const Type Line_Props		= 0x05110101;

		static const Type Rectangle			= 0x05120000;
		static const Type Rectangle_Props	= 0x05120101;

		static const Type ResizableObj		= 0x05130000;
		static const Type ResizableObj_Props= 0x05130101;

		static const Type BorderData		= 0x05140000;
		static const Type BorderData_Props	= 0x05140101;

		static const Type Placeholder		= 0x05150000;
		static const Type Placeholder_Props	= 0x05150101;



		static const Type Checkbox				= 0x05850000;
		static const Type Checkbox_Props		= 0x05850101;

		static const Type Checkbox_Group		= 0x05854000;
		static const Type Checkbox_Group_Props	= 0x05854001;

		static const Type Checkbox_Element		= 0x05855000;
		static const Type Checkbox_Element_Props= 0x05855001;

		static const Type Checkbox_Line			= 0x05856000;
		static const Type Checkbox_Line_Props	= 0x05856001;


	}

}}
