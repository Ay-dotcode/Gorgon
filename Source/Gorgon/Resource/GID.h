/// @file GID.h contains Gorgon IDs

#pragma once

#include <string>
#include <stdint.h>

///@cond INTERNAL
#ifndef CONSTEXPR
/*#	ifdef WIN32 
#		define CONSTEXPR	static
#		define CONSTEXPRFN
#	else*/
#		define CONSTEXPR	constexpr
#		define CONSTEXPRFN  constexpr
//#	endif
#endif
///@endcond

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
				CONSTEXPRFN Type() : value() { }
				
				/// Constructor to create a new GID from the given value
				explicit CONSTEXPRFN Type(uint32_t value) : value(value) { }
				
				/// Returns the value of the GID as an integer
				CONSTEXPRFN uint32_t AsInteger() const {
					return value;
				}
				
				/// Returns the value of the GID as a character array
				CONSTEXPRFN const char *AsChars() const {
					return (const char*)&value;
				}
				
				/// Checks if this type information is empty
				CONSTEXPRFN bool IsEmpty() const { return value==0; }

				/// Compares two GIDs
				CONSTEXPRFN bool operator == (const Type &type) const { return type.value==value; }

				/// Compares two GIDs
				CONSTEXPRFN bool operator != (const Type &type) const { return type.value!=value; }

				/// For std::less
				CONSTEXPRFN bool operator < (const Type &type) const { return type.value<value; }

			private:
				uint32_t value;
			};
			
			/// @name System 
			/// @{
			/// System module GIDs

			CONSTEXPR Type None{0x00000000};

			/// Folder resource
			//, \ref Gorgon::Resource::Folder
			CONSTEXPR Type Folder				{0x01010000};
			/// @deprecated Used to store names and captions
			CONSTEXPR Type Folder_Names			{0x01010101};
			/// @deprecated Used to store name and caption of an item
			CONSTEXPR Type Folder_Name			{0x01010102};
			/// Properties of a folder
			CONSTEXPR Type Folder_Props			{0x01010103};

			/// Link node resource 
			//\ref Gorgon::Resource::LinkNode
			CONSTEXPR Type LinkNode				{0x01020000};
			/// Stores the target of the link
			CONSTEXPR Type LinkNode_Target		{0x01020010};
			/// @}

			/// @name Special
			/// @{
			/// Special GIDs to be used as constants
			
			/// @deprecated Used to identify resources
			CONSTEXPR Type Guid					{0x00000010};
			/// Identifies resources
			CONSTEXPR Type SGuid				{0x00000011};
			/// Name of a resource, names are not required to be unique
			CONSTEXPR Type Name					{0x00000012};

			/// LZMA compression
			CONSTEXPR Type LZMA					{0xF0030100};
			/// JPEG compression
			CONSTEXPR Type JPEG					{0xF0030300};
			/// PNG compression
			CONSTEXPR Type PNG					{0xF0030400};
			/// @}
			
			/// @name Basic Resources
			/// @{
			/// Contains GIDs for basic resources
			
			/// @deprecated Used to store text data
			CONSTEXPR Type Text					{0x02010000};

			/// Image resource
			CONSTEXPR Type Image				{0x02020000};
			/// Image properties
			CONSTEXPR Type Image_Props			{0x02020101};
			/// Image compression properties
			CONSTEXPR Type Image_Cmp_Props		{0x02020102};
			/// Uncompressed image data
			CONSTEXPR Type Image_Data			{0x02020501};
			/// Compressed image data
			CONSTEXPR Type Image_Cmp_Data		{0x02020601};
			/// Image palette, not active yet
			CONSTEXPR Type Image_Palette		{0x02020502};
			/// Denotes the image is null
			CONSTEXPR Type Image_NULL			{0x0202A100};

			/// Data resource
			CONSTEXPR Type Data					{0x02030000};
			
			CONSTEXPR Type Data_Text			{0x02030C01};
			CONSTEXPR Type Data_Int				{0x02030C02};
			CONSTEXPR Type Data_Float			{0x02030C03};
			CONSTEXPR Type Data_Point			{0x02030C04};
			CONSTEXPR Type Data_Pointf			{0x02030C09};
			CONSTEXPR Type Data_Rectangle		{0x02030C05};
			CONSTEXPR Type Data_Link			{0x02030C07};
			CONSTEXPR Type Data_Names			{0x02030101};
			CONSTEXPR Type Data_Name			{0x02030102};
			CONSTEXPR Type Data_Font			{0x03300C01};
			CONSTEXPR Type Data_Color			{0x02030D02};
			CONSTEXPR Type Data_Size			{0x02030D03};
			CONSTEXPR Type Data_Bounds			{0x02030D04};
			CONSTEXPR Type Data_Margins			{0x02030D05};			
			CONSTEXPR Type Data_Object			{0x02030D06};

			//////////////////////////////////////////////////////////////////////////
			// Gaming resources
			CONSTEXPR Type Animation			{0x03100000};
			CONSTEXPR Type Animation_Image		{0x03110000};
			CONSTEXPR Type Animation_Durations	{0x03100101};
			CONSTEXPR Type Animation_Names		{0x03100102};
			CONSTEXPR Type Animation_Name		{0x03100103};

			CONSTEXPR Type Pointer				{0x03D10000};
			CONSTEXPR Type Pointer_Props		{0x03D10101};

			CONSTEXPR Type Font					{0x03200000};
			CONSTEXPR Type Font_Charmap			{0x03200101};
			CONSTEXPR Type Font_Names			{0x03200102};
			CONSTEXPR Type Font_Name			{0x03200103};
			CONSTEXPR Type Font_Image			{0x03210000};
			CONSTEXPR Type Font_Props			{0x03200804};

			CONSTEXPR Type FontTheme			{0x03300000};
			CONSTEXPR Type FontTheme_Font		{0x03301001};
			CONSTEXPR Type FontTheme_Shadow		{0x03301002};
			CONSTEXPR Type FontTheme_Props		{0x03300804};

			//////////////////////////////////////////////////////////////////////////
			// Extended resources
			CONSTEXPR Type Sound				{0x04010000};
			CONSTEXPR Type Sound_Props			{0x04010101};
			CONSTEXPR Type Sound_Wave			{0x04010801};
			CONSTEXPR Type Sound_Cmp_Wave		{0x04010802};
			CONSTEXPR Type Sound_Cmp_Props		{0x04010803};
			
			CONSTEXPR Type Blob					{0x04020000};
			CONSTEXPR Type Blob_Props			{0x04020101};
			CONSTEXPR Type Blob_Data			{0x04020801};
			CONSTEXPR Type Blob_Cmp_Data		{0x04020802};
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
