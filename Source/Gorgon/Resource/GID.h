/// @file GID.h contains Gorgon IDs

#pragma once

#include <string>
#include <stdint.h>

namespace Gorgon { 
	namespace Resource {
		/// Current resource system version. Used to determine file
		/// capabilities. Note that this is different from versions of
		/// each resource. This version tag will not easily change in the
		/// future.
		static const unsigned long CurrentVersion				=	0x00010000;
		

		//Gorgon IDs for known resource types
		namespace GID {
			
			/// Type to store GID information. This class is necessary to require explicit
			/// cast from and to integer. It also allows String::From and printing to streams
			class Type {
			public:
				/// Default constructor creates an empty type
				constexpr Type() : value() { }
				
				/// Constructor to create a new GID from the given value
				explicit constexpr Type(uint32_t value) : value(value) { }
				
				/// Returns the value of the GID as an integer
				constexpr uint32_t AsInteger() const {
					return value;
				}
				
				/// Returns the value of the GID as a character array
				constexpr const char *AsChars() const {
					return (const char*)&value;
				}
				
				/// Checks if this type information is empty
				constexpr bool IsEmpty() const { return value==0; }

				/// Compares two GIDs
				constexpr bool operator == (const Type &type) const { return type.value==value; }

				/// Compares two GIDs
				constexpr bool operator != (const Type &type) const { return type.value!=value; }

				/// For std::less
				constexpr bool operator < (const Type &type) const { return type.value<value; }

			private:
				uint32_t value;
			};
			
			/// @name System 
			/// @{
			/// System module GIDs

			constexpr Type None					{0x00000000};

			/// Denotes this file is a game file
			constexpr Type GameFile				{0x030100FF};

			/// Folder resource
			/// \ref Gorgon::Resource::Folder
			constexpr Type Folder				{0x01010000};
			/// @deprecated Used to store names and captions
			constexpr Type Folder_Names			{0x01010101};
			/// @deprecated Used to store name and caption of an item
			constexpr Type Folder_Name			{0x01010102};
			/// Properties of a folder
			constexpr Type Folder_Props			{0x01010103};

			/// Link node resource 
			/// \ref Gorgon::Resource::LinkNode
			constexpr Type LinkNode				{0x01020000};
			/// Stores the target of the link
			constexpr Type LinkNode_Target		{0x01020010};
            /// Null resource
            constexpr Type Null                 {0x01030000};
			/// @}

			/// @name Special
			/// @{
			/// Special GIDs to be used as constants
			
			/// @deprecated Used to identify resources
			constexpr Type Guid					{0x00000010};
			/// Identifies resources
			constexpr Type SGuid				{0x00000011};
			/// Name of a resource, names are not required to be unique
			constexpr Type Name					{0x00000012};
			
			/// LZMA compression
			constexpr Type LZMA					{0xF0030100};
			/// LZMA compression
			constexpr Type FLAC					{0xF0040100};
			/// JPEG compression
			constexpr Type JPEG					{0xF0030300};
			/// PNG compression
			constexpr Type PNG					{0xF0030400};
			/// @}
			
			/// @name Basic Resources
			/// @{
			/// Contains GIDs for basic resources
			
			/// @deprecated Used to store text data
			constexpr Type Text					{0x02010000};

			/// Image resource
			constexpr Type Image				{0x02020000};
			/// Image properties
			constexpr Type Image_Props			{0x02020101};
			/// Image compression properties
			constexpr Type Image_Cmp_Props		{0x02020102};
			/// Uncompressed image data
			constexpr Type Image_Data			{0x02020501};
			/// Compressed image data
			constexpr Type Image_Cmp_Data		{0x02020601};
			/// Image palette, not active yet
			constexpr Type Image_Palette		{0x02020502};
			/// Denotes the image is null
			constexpr Type Image_NULL			{0x0202A100};

			/// Data resource
			constexpr Type Data					{0x02030000};
			
			constexpr Type Data_Text			{0x02030C01};
			constexpr Type Data_Int				{0x02030C02};
			constexpr Type Data_Float			{0x02030C03};
			constexpr Type Data_Point			{0x02030C04};
			constexpr Type Data_Pointf			{0x02030C09};
			constexpr Type Data_Rectangle		{0x02030C05};
			constexpr Type Data_Link			{0x02030C07};
			constexpr Type Data_Names			{0x02030101};
			constexpr Type Data_Name			{0x02030102};
			constexpr Type Data_Font			{0x03300C01};
			constexpr Type Data_Color			{0x02030D02};
			constexpr Type Data_Size			{0x02030D03};
			constexpr Type Data_Bounds			{0x02030D04};
			constexpr Type Data_Margins			{0x02030D05};			
			constexpr Type Data_Object			{0x02030D06};

			//////////////////////////////////////////////////////////////////////////
			// Gaming resources
			constexpr Type Animation			{0x03100000};
			constexpr Type Animation_Image		{0x03110000};
			constexpr Type Animation_Durations	{0x03100101};
			constexpr Type Animation_Names		{0x03100102};
			constexpr Type Animation_Name		{0x03100103};

			constexpr Type LegacyPointer		{0x03D10000};
			constexpr Type LegacyPointer_Props	{0x03D10101};

            constexpr Type Pointer		        {0x03D20000};
			constexpr Type Pointer_Props	    {0x03D20101};

			constexpr Type Font					{0x03200000};
			constexpr Type Font_Charmap			{0x03200101};
			constexpr Type Font_Names			{0x03200102};
			constexpr Type Font_Name			{0x03200103};
			constexpr Type Font_Image			{0x03210000};
			constexpr Type Font_Props			{0x03200804};
			constexpr Type Font_Charmap_II		{0x03200805};
			constexpr Type Font_BitmapProps		{0x03200806};

			constexpr Type FontTheme			{0x03300000};
			constexpr Type FontTheme_Font		{0x03301001};
			constexpr Type FontTheme_Shadow		{0x03301002};
			constexpr Type FontTheme_Props		{0x03300804};

			//////////////////////////////////////////////////////////////////////////
			// Extended resources
			constexpr Type Sound				{0x04010000};
			constexpr Type Sound_Props			{0x04010101};
			constexpr Type Sound_Wave			{0x04010801};
			constexpr Type Sound_Cmp_Wave		{0x04010802};
			constexpr Type Sound_Cmp_Props		{0x04010803};
			constexpr Type Sound_Fmt			{0x04010104};
			constexpr Type Sound_Channels		{0x04010105};

			constexpr Type Blob					{0x04020000};
			constexpr Type Blob_Props			{0x04020101};
			constexpr Type Blob_Data			{0x04020801};
			constexpr Type Blob_Cmp_Data		{0x04020802};

			//////////////////////////////////////////////////////////////////////////
			// Basic Widget resources
			constexpr Type Line					{0x05110000};
			constexpr Type Line_Props			{0x05110101};

			constexpr Type Rectangle			{0x05120000};
			constexpr Type Rectangle_Props		{0x05120101};
			constexpr Type Rectangle_Props_II	{0x05120102};
		}
	}
	
	namespace String {
		/// Creates a string from a GID
		inline std::string From(const Resource::GID::Type &value) {		
			static char hextable[]={"0123456789ABCDEF"};
			
			std::string ret;
			ret.resize(8);
			
			uint32_t v=value.AsInteger();
			for(int i=1;i<=8;i++) {
				ret[8-i]=hextable[v%16];
				v=v>>4;
			}
			
			return ret;
		}
	}
	
	namespace Resource {
		namespace GID {
			/// Inserts a GID to a stream
			inline std::ostream &operator <<(std::ostream &out, const Resource::GID::Type &value) {
				out<<String::From(value);
				
				return out;
			}
		}
	}
}
