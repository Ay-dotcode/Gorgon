#pragma once

namespace gge { namespace resource {
	static const int CurrentVersion			=	0x00010000;


	//Gorgon IDs for standard resource types
	namespace GID {
		typedef int Type;

		//////////////////////////////////////////////////////////////////////////
		// System GIDs
		static const Type Folder				= 0x01010000;
		static const Type Folder_Names			= 0x01010101;
		static const Type Folder_Name			= 0x01010102;
		static const Type Folder_Props			= 0x01010103;

		static const Type LinkNode				= 0x01020000;
		static const Type LinkNode_Target		= 0x01020010;
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// Special
		static const Type Guid					= 0x00000010;
		static const Type SGuid					= 0x00000011;

		static const Type LZMA					= 0xF0030100;
		static const Type JPEG					= 0xF0030300;
		static const Type PNG					= 0xF0030400;
		//////////////////////////////////////////////////////////////////////////
		
		//////////////////////////////////////////////////////////////////////////
		// Basic resources
		static const Type Text					= 0x02010000;

		static const Type Image					= 0x02020000;
		static const Type Image_Props			= 0x02020101;
		static const Type Image_Cmp_Props		= 0x02020102;
		static const Type Image_Data			= 0x02020501;
		static const Type Image_Cmp_Data		= 0x02020601;
		static const Type Image_Palette			= 0x02020502;
		static const Type Image_NULL			= 0x0202A100;

		static const Type Data					= 0x02030000;
		static const Type Data_Text				= 0x02030C01;
		static const Type Data_Int				= 0x02030C02;
		static const Type Data_Float			= 0x02030C03;
		static const Type Data_Point			= 0x02030C04;
		static const Type Data_Point2D			= 0x02030C09;
		static const Type Data_Rect				= 0x02030C05;
		static const Type Data_Link				= 0x02030C07;
		static const Type Data_Names			= 0x02030101;
		static const Type Data_Name				= 0x02030102;
		static const Type Data_Font				= 0x03300C01;
		static const Type Data_Color			= 0x02030D02;
		static const Type Data_Size				= 0x02030D03;
		static const Type Data_Bounds			= 0x02030D04;
		static const Type Data_Margins			= 0x02030D05;

		//////////////////////////////////////////////////////////////////////////
		// Gaming resources
		static const Type Animation				= 0x03100000;
		static const Type Animation_Image		= 0x03110000;
		static const Type Animation_Durations	= 0x03100101;
		static const Type Animation_Names		= 0x03100102;
		static const Type Animation_Name		= 0x03100103;

		static const Type Pointer				= 0x03D10000;
		static const Type Pointer_Props			= 0x03D10101;

		static const Type Font					= 0x03200000;
		static const Type Font_Charmap			= 0x03200101;
		static const Type Font_Image			= 0x03210000;
		static const Type Font_Props			= 0x03200804;

		static const Type FontTheme				= 0x03300000;
		static const Type FontTheme_Font		= 0x03301001;
		static const Type FontTheme_Shadow		= 0x03301002;
		static const Type FontTheme_Props		= 0x03300804;

		//////////////////////////////////////////////////////////////////////////
		// Extended resources
		static const Type Sound					= 0x04010000;
		static const Type Sound_Props			= 0x04010101;
		static const Type Sound_Wave			= 0x04010801;
		static const Type Sound_Cmp_Wave		= 0x04010802;
		static const Type Sound_Cmp_Props		= 0x04010803;
	};
} }
