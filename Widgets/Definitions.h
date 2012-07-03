#pragma once

#include "../Resource/Definitions.h"


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
		static const Type ResizableObj_Names= 0x05130102;
		static const Type ResizableObj_Name	= 0x05130103;

		static const Type BorderData		= 0x05140000;
		static const Type BorderData_Props	= 0x05140101;
		static const Type BorderData_Names	= 0x05140102;
		static const Type BorderData_Name	= 0x05140103;

		static const Type Placeholder		= 0x05150000;
		static const Type Placeholder_Props	= 0x05150101;



		static const Type Checkbox				= 0x05850000;
		static const Type Checkbox_Props		= 0x05850101;
		static const Type Checkbox_Name			= 0x05850102;
		static const Type Checkbox_Names		= 0x05850103;

		static const Type Checkbox_Group		= 0x05854000;
		static const Type Checkbox_Group_Props	= 0x05854001;
		static const Type Checkbox_Group_Name	= 0x05854002;
		static const Type Checkbox_Group_Names	= 0x05854003;

		static const Type Checkbox_Element		= 0x05855000;
		static const Type Checkbox_Element_Props= 0x05855001;
		static const Type Checkbox_Element_Name	= 0x05855002;
		static const Type Checkbox_Element_Names= 0x05855003;

		static const Type Checkbox_Line			= 0x05856000;
		static const Type Checkbox_Line_Props	= 0x05856001;
		static const Type Checkbox_Line_Name	= 0x05856002;
		static const Type Checkbox_Line_Names	= 0x05856003;



		static const Type Slider				= 0x05860000;
		static const Type Slider_Props			= 0x05860101;

		static const Type Slider_Group			= 0x05864000;
		static const Type Slider_Group_Props	= 0x05864001;

		static const Type Slider_Element		= 0x05865000;
		static const Type Slider_Element_Props	= 0x05865001;



		static const Type Panel					= 0x05880000;
		static const Type Panel_Props			= 0x05880101;

		static const Type Panel_Element			= 0x05885000;
		static const Type Panel_Element_Props	= 0x05885001;


		static const Type Textbox				= 0x05830000;
		static const Type Textbox_Props			= 0x05830101;
		static const Type Textbox_Names			= 0x05830002;
		static const Type Textbox_Name			= 0x05830003;

		static const Type Textbox_Element		= 0x05835000;
		static const Type Textbox_Element_Props	= 0x05835001;
		static const Type Textbox_Element_Names	= 0x05835002;
		static const Type Textbox_Element_Name	= 0x05835003;


		static const Type Listbox				= 0x05870000;
		static const Type Listbox_Props			= 0x05870101;
		

		static const Type Combobox				= 0x05890000;
		static const Type Combobox_Props		= 0x05890101;


		static const Type Tabpanel				= 0x058a0000;
		static const Type Tabpanel_Props		= 0x058a0101;


		static const Type WR					= 0x05100000;
	}

}}
