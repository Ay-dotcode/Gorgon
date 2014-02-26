/*
www.sourceforge.net/projects/tinyxml
Original code by Lee Thomason (www.grinninglizard.com)

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any
damages arising from the use of this software.

Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must
not claim that you wrote the original software. If you use this
software in a product, an acknowledgment in the product documentation
would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution.
*/

//MODIFIED BY CK on 02/03/2012 to exist in xml namespace without prefixes on classes


#include <ctype.h>

#ifdef TIXML_USE_STL
#include <sstream>
#include <iostream>
#endif

#include "tinyxml.h"

namespace xml {

	FILE* FOpen( const char* filename, const char* mode );

	bool Base::condenseWhiteSpace = true;

	// Microsoft compiler security
	FILE* FOpen( const char* filename, const char* mode )
	{
		#if defined(_MSC_VER) && (_MSC_VER >= 1400 )
			FILE* fp = 0;
			errno_t err = fopen_s( &fp, filename, mode );
			if ( !err && fp )
				return fp;
			return 0;
		#else
			return fopen( filename, mode );
		#endif
	}

	void Base::EncodeString( const TIXML_STRING& str, TIXML_STRING* outString )
	{
		int i=0;

		while( i<(int)str.length() )
		{
			unsigned char c = (unsigned char) str[i];

			if (    c == '&' 
				 && i < ( (int)str.length() - 2 )
				 && str[i+1] == '#'
				 && str[i+2] == 'x' )
			{
				// Hexadecimal character reference.
				// Pass through unchanged.
				// &#xA9;	-- copyright symbol, for example.
				//
				// The -1 is a bug fix from Rob Laveaux. It keeps
				// an overflow from happening if there is no ';'.
				// There are actually 2 ways to exit this loop -
				// while fails (error case) and break (semicolon found).
				// However, there is no mechanism (currently) for
				// this function to return an error.
				while ( i<(int)str.length()-1 )
				{
					outString->append( str.c_str() + i, 1 );
					++i;
					if ( str[i] == ';' )
						break;
				}
			}
			else if ( c == '&' )
			{
				outString->append( entity[0].str, entity[0].strLength );
				++i;
			}
			else if ( c == '<' )
			{
				outString->append( entity[1].str, entity[1].strLength );
				++i;
			}
			else if ( c == '>' )
			{
				outString->append( entity[2].str, entity[2].strLength );
				++i;
			}
			else if ( c == '\"' )
			{
				outString->append( entity[3].str, entity[3].strLength );
				++i;
			}
			else if ( c == '\'' )
			{
				outString->append( entity[4].str, entity[4].strLength );
				++i;
			}
			else if ( c < 32 )
			{
				// Easy pass at non-alpha/numeric/symbol
				// Below 32 is symbolic.
				char buf[ 32 ];
			
				#if defined(TIXML_SNPRINTF)		
					TIXML_SNPRINTF( buf, sizeof(buf), "&#x%02X;", (unsigned) ( c & 0xff ) );
				#else
					sprintf( buf, "&#x%02X;", (unsigned) ( c & 0xff ) );
				#endif		

				//*ME:	warning C4267: convert 'size_t' to 'int'
				//*ME:	Int-Cast to make compiler happy ...
				outString->append( buf, (int)strlen( buf ) );
				++i;
			}
			else
			{
				//char realc = (char) c;
				//outString->append( &realc, 1 );
				*outString += (char) c;	// somewhat more efficient function call.
				++i;
			}
		}
	}


	Node::Node( NodeType _type ) : Base()
	{
		parent = 0;
		type = _type;
		firstChild = 0;
		lastChild = 0;
		prev = 0;
		next = 0;
	}


	Node::~Node()
	{
		Node* node = firstChild;
		Node* temp = 0;

		while ( node )
		{
			temp = node;
			node = node->next;
			delete temp;
		}	
	}


	void Node::CopyTo( Node* target ) const
	{
		target->SetValue (value.c_str() );
		target->userData = userData; 
		target->location = location;
	}


	void Node::Clear()
	{
		Node* node = firstChild;
		Node* temp = 0;

		while ( node )
		{
			temp = node;
			node = node->next;
			delete temp;
		}	

		firstChild = 0;
		lastChild = 0;
	}


	Node* Node::LinkEndChild( Node* node )
	{
		assert( node->parent == 0 || node->parent == this );
		assert( node->GetDocument() == 0 || node->GetDocument() == this->GetDocument() );

		if ( node->Type() == Node::TINYXML_DOCUMENT )
		{
			delete node;
			if ( GetDocument() ) 
				GetDocument()->SetError( TIXML_ERROR_DOCUMENT_TOP_ONLY, 0, 0, TIXML_ENCODING_UNKNOWN );
			return 0;
		}

		node->parent = this;

		node->prev = lastChild;
		node->next = 0;

		if ( lastChild )
			lastChild->next = node;
		else
			firstChild = node;			// it was an empty list.

		lastChild = node;
		return node;
	}


	Node* Node::InsertEndChild( const Node& addThis )
	{
		if ( addThis.Type() == Node::TINYXML_DOCUMENT )
		{
			if ( GetDocument() ) 
				GetDocument()->SetError( TIXML_ERROR_DOCUMENT_TOP_ONLY, 0, 0, TIXML_ENCODING_UNKNOWN );
			return 0;
		}
		Node* node = addThis.Clone();
		if ( !node )
			return 0;

		return LinkEndChild( node );
	}


	Node* Node::InsertBeforeChild( Node* beforeThis, const Node& addThis )
	{	
		if ( !beforeThis || beforeThis->parent != this ) {
			return 0;
		}
		if ( addThis.Type() == Node::TINYXML_DOCUMENT )
		{
			if ( GetDocument() ) 
				GetDocument()->SetError( TIXML_ERROR_DOCUMENT_TOP_ONLY, 0, 0, TIXML_ENCODING_UNKNOWN );
			return 0;
		}

		Node* node = addThis.Clone();
		if ( !node )
			return 0;
		node->parent = this;

		node->next = beforeThis;
		node->prev = beforeThis->prev;
		if ( beforeThis->prev )
		{
			beforeThis->prev->next = node;
		}
		else
		{
			assert( firstChild == beforeThis );
			firstChild = node;
		}
		beforeThis->prev = node;
		return node;
	}


	Node* Node::InsertAfterChild( Node* afterThis, const Node& addThis )
	{
		if ( !afterThis || afterThis->parent != this ) {
			return 0;
		}
		if ( addThis.Type() == Node::TINYXML_DOCUMENT )
		{
			if ( GetDocument() ) 
				GetDocument()->SetError( TIXML_ERROR_DOCUMENT_TOP_ONLY, 0, 0, TIXML_ENCODING_UNKNOWN );
			return 0;
		}

		Node* node = addThis.Clone();
		if ( !node )
			return 0;
		node->parent = this;

		node->prev = afterThis;
		node->next = afterThis->next;
		if ( afterThis->next )
		{
			afterThis->next->prev = node;
		}
		else
		{
			assert( lastChild == afterThis );
			lastChild = node;
		}
		afterThis->next = node;
		return node;
	}


	Node* Node::ReplaceChild( Node* replaceThis, const Node& withThis )
	{
		if ( !replaceThis )
			return 0;

		if ( replaceThis->parent != this )
			return 0;

		if ( withThis.ToDocument() ) {
			// A document can never be a child.	Thanks to Noam.
			Document* document = GetDocument();
			if ( document ) 
				document->SetError( TIXML_ERROR_DOCUMENT_TOP_ONLY, 0, 0, TIXML_ENCODING_UNKNOWN );
			return 0;
		}

		Node* node = withThis.Clone();
		if ( !node )
			return 0;

		node->next = replaceThis->next;
		node->prev = replaceThis->prev;

		if ( replaceThis->next )
			replaceThis->next->prev = node;
		else
			lastChild = node;

		if ( replaceThis->prev )
			replaceThis->prev->next = node;
		else
			firstChild = node;

		delete replaceThis;
		node->parent = this;
		return node;
	}


	bool Node::RemoveChild( Node* removeThis )
	{
		if ( !removeThis ) {
			return false;
		}

		if ( removeThis->parent != this )
		{	
			assert( 0 );
			return false;
		}

		if ( removeThis->next )
			removeThis->next->prev = removeThis->prev;
		else
			lastChild = removeThis->prev;

		if ( removeThis->prev )
			removeThis->prev->next = removeThis->next;
		else
			firstChild = removeThis->next;

		delete removeThis;
		return true;
	}

	const Node* Node::FirstChild( const char * _value ) const
	{
		const Node* node;
		for ( node = firstChild; node; node = node->next )
		{
			if ( strcmp( node->Value(), _value ) == 0 )
				return node;
		}
		return 0;
	}


	const Node* Node::LastChild( const char * _value ) const
	{
		const Node* node;
		for ( node = lastChild; node; node = node->prev )
		{
			if ( strcmp( node->Value(), _value ) == 0 )
				return node;
		}
		return 0;
	}


	const Node* Node::IterateChildren( const Node* previous ) const
	{
		if ( !previous )
		{
			return FirstChild();
		}
		else
		{
			assert( previous->parent == this );
			return previous->NextSibling();
		}
	}


	const Node* Node::IterateChildren( const char * val, const Node* previous ) const
	{
		if ( !previous )
		{
			return FirstChild( val );
		}
		else
		{
			assert( previous->parent == this );
			return previous->NextSibling( val );
		}
	}


	const Node* Node::NextSibling( const char * _value ) const 
	{
		const Node* node;
		for ( node = next; node; node = node->next )
		{
			if ( strcmp( node->Value(), _value ) == 0 )
				return node;
		}
		return 0;
	}


	const Node* Node::PreviousSibling( const char * _value ) const
	{
		const Node* node;
		for ( node = prev; node; node = node->prev )
		{
			if ( strcmp( node->Value(), _value ) == 0 )
				return node;
		}
		return 0;
	}


	void Element::RemoveAttribute( const char * name )
	{
		#ifdef TIXML_USE_STL
		TIXML_STRING str( name );
		xml::Attribute* node = attributeSet.Find( str );
		#else
		xml::Attribute* node = attributeSet.Find( name );
		#endif
		if ( node )
		{
			attributeSet.Remove( node );
			delete node;
		}
	}

	const Element* Node::FirstChildElement() const
	{
		const xml::Node* node;

		for (	node = FirstChild();
				node;
				node = node->NextSibling() )
		{
			if ( node->ToElement() )
				return node->ToElement();
		}
		return 0;
	}


	const Element* Node::FirstChildElement( const char * _value ) const
	{
		const xml::Node* node;

		for (	node = FirstChild( _value );
				node;
				node = node->NextSibling( _value ) )
		{
			if ( node->ToElement() )
				return node->ToElement();
		}
		return 0;
	}


	const Element* Node::NextSiblingElement() const
	{
		const Node* node;

		for (	node = NextSibling();
				node;
				node = node->NextSibling() )
		{
			if ( node->ToElement() )
				return node->ToElement();
		}
		return 0;
	}


	const Element* Node::NextSiblingElement( const char * _value ) const
	{
		const Node* node;

		for (	node = NextSibling( _value );
				node;
				node = node->NextSibling( _value ) )
		{
			if ( node->ToElement() )
				return node->ToElement();
		}
		return 0;
	}


	const Document* Node::GetDocument() const
	{
		const Node* node;

		for( node = this; node; node = node->parent )
		{
			if ( node->ToDocument() )
				return node->ToDocument();
		}
		return 0;
	}


	Element::Element (const char * _value)
		: Node( Node::TINYXML_ELEMENT )
	{
		firstChild = lastChild = 0;
		value = _value;
	}


	#ifdef TIXML_USE_STL
	Element::Element( const std::string& _value ) 
		: Node( Node::TINYXML_ELEMENT )
	{
		firstChild = lastChild = 0;
		value = _value;
	}
	#endif


	Element::Element( const Element& copy)
		: Node( Node::TINYXML_ELEMENT )
	{
		firstChild = lastChild = 0;
		copy.CopyTo( this );	
	}


	Element& Element::operator=( const Element& base )
	{
		ClearThis();
		base.CopyTo( this );
		return *this;
	}


	Element::~Element()
	{
		ClearThis();
	}


	void Element::ClearThis()
	{
		Clear();
		while( attributeSet.First() )
		{
			xml::Attribute* node = attributeSet.First();
			attributeSet.Remove( node );
			delete node;
		}
	}


	const char* Element::Attribute( const char* name ) const
	{
		const xml::Attribute* node = attributeSet.Find( name );
		if ( node )
			return node->Value();
		return 0;
	}


	#ifdef TIXML_USE_STL
	const std::string* Element::Attribute( const std::string& name ) const
	{
		const xml::Attribute* attrib = attributeSet.Find( name );
		if ( attrib )
			return &attrib->ValueStr();
		return 0;
	}
	#endif


	const char* Element::Attribute( const char* name, int* i ) const
	{
		const xml::Attribute* attrib = attributeSet.Find( name );
		const char* result = 0;

		if ( attrib ) {
			result = attrib->Value();
			if ( i ) {
				attrib->QueryIntValue( i );
			}
		}
		return result;
	}


	#ifdef TIXML_USE_STL
	const std::string* Element::Attribute( const std::string& name, int* i ) const
	{
		const xml::Attribute* attrib = attributeSet.Find( name );
		const std::string* result = 0;

		if ( attrib ) {
			result = &attrib->ValueStr();
			if ( i ) {
				attrib->QueryIntValue( i );
			}
		}
		return result;
	}
	#endif


	const char* Element::Attribute( const char* name, double* d ) const
	{
		const xml::Attribute* attrib = attributeSet.Find( name );
		const char* result = 0;

		if ( attrib ) {
			result = attrib->Value();
			if ( d ) {
				attrib->QueryDoubleValue( d );
			}
		}
		return result;
	}


	#ifdef TIXML_USE_STL
	const std::string* Element::Attribute( const std::string& name, double* d ) const
	{
		const xml::Attribute* attrib = attributeSet.Find( name );
		const std::string* result = 0;

		if ( attrib ) {
			result = &attrib->ValueStr();
			if ( d ) {
				attrib->QueryDoubleValue( d );
			}
		}
		return result;
	}
	#endif


	int Element::QueryIntAttribute( const char* name, int* ival ) const
	{
		const xml::Attribute* attrib = attributeSet.Find( name );
		if ( !attrib )
			return TIXML_NO_ATTRIBUTE;
		return attrib->QueryIntValue( ival );
	}


	int Element::QueryUnsignedAttribute( const char* name, unsigned* value ) const
	{
		const xml::Attribute* node = attributeSet.Find( name );
		if ( !node )
			return TIXML_NO_ATTRIBUTE;

		int ival = 0;
		int result = node->QueryIntValue( &ival );
		*value = (unsigned)ival;
		return result;
	}


	int Element::QueryBoolAttribute( const char* name, bool* bval ) const
	{
		const xml::Attribute* node = attributeSet.Find( name );
		if ( !node )
			return TIXML_NO_ATTRIBUTE;
	
		int result = TIXML_WRONG_TYPE;
		if (    StringEqual( node->Value(), "true", true, TIXML_ENCODING_UNKNOWN ) 
			 || StringEqual( node->Value(), "yes", true, TIXML_ENCODING_UNKNOWN ) 
			 || StringEqual( node->Value(), "1", true, TIXML_ENCODING_UNKNOWN ) ) 
		{
			*bval = true;
			result = TIXML_SUCCESS;
		}
		else if (    StringEqual( node->Value(), "false", true, TIXML_ENCODING_UNKNOWN ) 
				  || StringEqual( node->Value(), "no", true, TIXML_ENCODING_UNKNOWN ) 
				  || StringEqual( node->Value(), "0", true, TIXML_ENCODING_UNKNOWN ) ) 
		{
			*bval = false;
			result = TIXML_SUCCESS;
		}
		return result;
	}



	#ifdef TIXML_USE_STL
	int Element::QueryIntAttribute( const std::string& name, int* ival ) const
	{
		const xml::Attribute* attrib = attributeSet.Find( name );
		if ( !attrib )
			return TIXML_NO_ATTRIBUTE;
		return attrib->QueryIntValue( ival );
	}
	#endif


	int Element::QueryDoubleAttribute( const char* name, double* dval ) const
	{
		const xml::Attribute* attrib = attributeSet.Find( name );
		if ( !attrib )
			return TIXML_NO_ATTRIBUTE;
		return attrib->QueryDoubleValue( dval );
	}


	#ifdef TIXML_USE_STL
	int Element::QueryDoubleAttribute( const std::string& name, double* dval ) const
	{
		const xml::Attribute* attrib = attributeSet.Find( name );
		if ( !attrib )
			return TIXML_NO_ATTRIBUTE;
		return attrib->QueryDoubleValue( dval );
	}
	#endif


	void Element::SetAttribute( const char * name, int val )
	{	
		xml::Attribute* attrib = attributeSet.FindOrCreate( name );
		if ( attrib ) {
			attrib->SetIntValue( val );
		}
	}


	#ifdef TIXML_USE_STL
	void Element::SetAttribute( const std::string& name, int val )
	{	
		xml::Attribute* attrib = attributeSet.FindOrCreate( name );
		if ( attrib ) {
			attrib->SetIntValue( val );
		}
	}
	#endif


	void Element::SetDoubleAttribute( const char * name, double val )
	{	
		xml::Attribute* attrib = attributeSet.FindOrCreate( name );
		if ( attrib ) {
			attrib->SetDoubleValue( val );
		}
	}


	#ifdef TIXML_USE_STL
	void Element::SetDoubleAttribute( const std::string& name, double val )
	{	
		xml::Attribute* attrib = attributeSet.FindOrCreate( name );
		if ( attrib ) {
			attrib->SetDoubleValue( val );
		}
	}
	#endif 


	void Element::SetAttribute( const char * cname, const char * cvalue )
	{
		xml::Attribute* attrib = attributeSet.FindOrCreate( cname );
		if ( attrib ) {
			attrib->SetValue( cvalue );
		}
	}


	#ifdef TIXML_USE_STL
	void Element::SetAttribute( const std::string& _name, const std::string& _value )
	{
		xml::Attribute* attrib = attributeSet.FindOrCreate( _name );
		if ( attrib ) {
			attrib->SetValue( _value );
		}
	}
	#endif


	void Element::Print( FILE* cfile, int depth ) const
	{
		int i;
		assert( cfile );
		for ( i=0; i<depth; i++ ) {
			fprintf( cfile, "    " );
		}

		fprintf( cfile, "<%s", value.c_str() );

		const xml::Attribute* attrib;
		for ( attrib = attributeSet.First(); attrib; attrib = attrib->Next() )
		{
			fprintf( cfile, " " );
			attrib->Print( cfile, depth );
		}

		// There are 3 different formatting approaches:
		// 1) An element without children is printed as a <foo /> node
		// 2) An element with only a text child is printed as <foo> text </foo>
		// 3) An element with children is printed on multiple lines.
		xml::Node* node;
		if ( !firstChild )
		{
			fprintf( cfile, " />" );
		}
		else if ( firstChild == lastChild && firstChild->ToText() )
		{
			fprintf( cfile, ">" );
			firstChild->Print( cfile, depth + 1 );
			fprintf( cfile, "</%s>", value.c_str() );
		}
		else
		{
			fprintf( cfile, ">" );

			for ( node = firstChild; node; node=node->NextSibling() )
			{
				if ( !node->ToText() )
				{
					fprintf( cfile, "\n" );
				}
				node->Print( cfile, depth+1 );
			}
			fprintf( cfile, "\n" );
			for( i=0; i<depth; ++i ) {
				fprintf( cfile, "    " );
			}
			fprintf( cfile, "</%s>", value.c_str() );
		}
	}


	void Element::CopyTo( Element* target ) const
	{
		// superclass:
		xml::Node::CopyTo( target );

		// Element class: 
		// Clone the attributes, then clone the children.
		const xml::Attribute* attribute = 0;
		for(	attribute = attributeSet.First();
		attribute;
		attribute = attribute->Next() )
		{
			target->SetAttribute( attribute->Name(), attribute->Value() );
		}

		xml::Node* node = 0;
		for ( node = firstChild; node; node = node->NextSibling() )
		{
			target->LinkEndChild( node->Clone() );
		}
	}

	bool Element::Accept( Visitor* visitor ) const
	{
		if ( visitor->VisitEnter( *this, attributeSet.First() ) ) 
		{
			for ( const xml::Node* node=FirstChild(); node; node=node->NextSibling() )
			{
				if ( !node->Accept( visitor ) )
					break;
			}
		}
		return visitor->VisitExit( *this );
	}


	Node* Element::Clone() const
	{
		Element* clone = new Element( Value() );
		if ( !clone )
			return 0;

		CopyTo( clone );
		return clone;
	}


	const char* Element::GetText() const
	{
		const Node* child = this->FirstChild();
		if ( child ) {
			const Text* childText = child->ToText();
			if ( childText ) {
				return childText->Value();
			}
		}
		return 0;
	}


	Document::Document() : Node( Node::TINYXML_DOCUMENT )
	{
		tabsize = 4;
		useMicrosoftBOM = false;
		ClearError();
	}

	Document::Document( const char * documentName ) : Node( Node::TINYXML_DOCUMENT )
	{
		tabsize = 4;
		useMicrosoftBOM = false;
		value = documentName;
		ClearError();
	}


	#ifdef TIXML_USE_STL
	Document::Document( const std::string& documentName ) : Node( Node::TINYXML_DOCUMENT )
	{
		tabsize = 4;
		useMicrosoftBOM = false;
		value = documentName;
		ClearError();
	}
	#endif


	Document::Document( const Document& copy ) : Node( Node::TINYXML_DOCUMENT )
	{
		copy.CopyTo( this );
	}


	Document& Document::operator=( const Document& copy )
	{
		Clear();
		copy.CopyTo( this );
		return *this;
	}


	bool Document::LoadFile( Encoding encoding )
	{
		return LoadFile( Value(), encoding );
	}


	bool Document::SaveFile() const
	{
		return SaveFile( Value() );
	}

	bool Document::LoadFile( const char* _filename, Encoding encoding )
	{
		TIXML_STRING filename( _filename );
		value = filename;

		// reading in binary mode so that tinyxml can normalize the EOL
		FILE* file = FOpen( value.c_str (), "rb" );	

		if ( file )
		{
			bool result = LoadFile( file, encoding );
			fclose( file );
			return result;
		}
		else
		{
			SetError( TIXML_ERROR_OPENING_FILE, 0, 0, TIXML_ENCODING_UNKNOWN );
			return false;
		}
	}

	bool Document::LoadFile( FILE* file, Encoding encoding )
	{
		if ( !file ) 
		{
			SetError( TIXML_ERROR_OPENING_FILE, 0, 0, TIXML_ENCODING_UNKNOWN );
			return false;
		}

		// Delete the existing data:
		Clear();
		location.Clear();

		// Get the file size, so we can pre-allocate the string. HUGE speed impact.
		long length = 0;
		fseek( file, 0, SEEK_END );
		length = ftell( file );
		fseek( file, 0, SEEK_SET );

		// Strange case, but good to handle up front.
		if ( length <= 0 )
		{
			SetError( TIXML_ERROR_DOCUMENT_EMPTY, 0, 0, TIXML_ENCODING_UNKNOWN );
			return false;
		}

		// Subtle bug here. TinyXml did use fgets. But from the XML spec:
		// 2.11 End-of-Line Handling
		// <snip>
		// <quote>
		// ...the XML processor MUST behave as if it normalized all line breaks in external 
		// parsed entities (including the document entity) on input, before parsing, by translating 
		// both the two-character sequence #xD #xA and any #xD that is not followed by #xA to 
		// a single #xA character.
		// </quote>
		//
		// It is not clear fgets does that, and certainly isn't clear it works cross platform. 
		// Generally, you expect fgets to translate from the convention of the OS to the c/unix
		// convention, and not work generally.

		/*
		while( fgets( buf, sizeof(buf), file ) )
		{
			data += buf;
		}
		*/

		char* buf = new char[ length+1 ];
		buf[0] = 0;

		if ( fread( buf, length, 1, file ) != 1 ) {
			delete [] buf;
			SetError( TIXML_ERROR_OPENING_FILE, 0, 0, TIXML_ENCODING_UNKNOWN );
			return false;
		}

		// Process the buffer in place to normalize new lines. (See comment above.)
		// Copies from the 'p' to 'q' pointer, where p can advance faster if
		// a newline-carriage return is hit.
		//
		// Wikipedia:
		// Systems based on ASCII or a compatible character set use either LF  (Line feed, '\n', 0x0A, 10 in decimal) or 
		// CR (Carriage return, '\r', 0x0D, 13 in decimal) individually, or CR followed by LF (CR+LF, 0x0D 0x0A)...
		//		* LF:    Multics, Unix and Unix-like systems (GNU/Linux, AIX, Xenix, Mac OS X, FreeBSD, etc.), BeOS, Amiga, RISC OS, and others
		//		* CR+LF: DEC RT-11 and most other early non-Unix, non-IBM OSes, CP/M, MP/M, DOS, OS/2, Microsoft Windows, Symbian OS
		//		* CR:    Commodore 8-bit machines, Apple II family, Mac OS up to version 9 and OS-9

		const char* p = buf;	// the read head
		char* q = buf;			// the write head
		const char CR = 0x0d;
		const char LF = 0x0a;

		buf[length] = 0;
		while( *p ) {
			assert( p < (buf+length) );
			assert( q <= (buf+length) );
			assert( q <= p );

			if ( *p == CR ) {
				*q++ = LF;
				p++;
				if ( *p == LF ) {		// check for CR+LF (and skip LF)
					p++;
				}
			}
			else {
				*q++ = *p++;
			}
		}
		assert( q <= (buf+length) );
		*q = 0;

		Parse( buf, 0, encoding );

		delete [] buf;
		return !Error();
	}


	bool Document::SaveFile( const char * filename ) const
	{
		// The old c stuff lives on...
		FILE* fp = FOpen( filename, "w" );
		if ( fp )
		{
			bool result = SaveFile( fp );
			fclose( fp );
			return result;
		}
		return false;
	}


	bool Document::SaveFile( FILE* fp ) const
	{
		if ( useMicrosoftBOM ) 
		{
			const unsigned char TIXML_UTF_LEAD_0 = 0xefU;
			const unsigned char TIXML_UTF_LEAD_1 = 0xbbU;
			const unsigned char TIXML_UTF_LEAD_2 = 0xbfU;

			fputc( TIXML_UTF_LEAD_0, fp );
			fputc( TIXML_UTF_LEAD_1, fp );
			fputc( TIXML_UTF_LEAD_2, fp );
		}
		Print( fp, 0 );
		return (ferror(fp) == 0);
	}


	void Document::CopyTo( Document* target ) const
	{
		Node::CopyTo( target );

		target->error = error;
		target->errorId = errorId;
		target->errorDesc = errorDesc;
		target->tabsize = tabsize;
		target->errorLocation = errorLocation;
		target->useMicrosoftBOM = useMicrosoftBOM;

		Node* node = 0;
		for ( node = firstChild; node; node = node->NextSibling() )
		{
			target->LinkEndChild( node->Clone() );
		}	
	}


	Node* Document::Clone() const
	{
		Document* clone = new Document();
		if ( !clone )
			return 0;

		CopyTo( clone );
		return clone;
	}


	void Document::Print( FILE* cfile, int depth ) const
	{
		assert( cfile );
		for ( const Node* node=FirstChild(); node; node=node->NextSibling() )
		{
			node->Print( cfile, depth );
			fprintf( cfile, "\n" );
		}
	}


	bool Document::Accept( Visitor* visitor ) const
	{
		if ( visitor->VisitEnter( *this ) )
		{
			for ( const Node* node=FirstChild(); node; node=node->NextSibling() )
			{
				if ( !node->Accept( visitor ) )
					break;
			}
		}
		return visitor->VisitExit( *this );
	}


	const Attribute* Attribute::Next() const
	{
		// We are using knowledge of the sentinel. The sentinel
		// have a value or name.
		if ( next->value.empty() && next->name.empty() )
			return 0;
		return next;
	}

	/*
	Attribute* Attribute::Next()
	{
		// We are using knowledge of the sentinel. The sentinel
		// have a value or name.
		if ( next->value.empty() && next->name.empty() )
			return 0;
		return next;
	}
	*/

	const Attribute* Attribute::Previous() const
	{
		// We are using knowledge of the sentinel. The sentinel
		// have a value or name.
		if ( prev->value.empty() && prev->name.empty() )
			return 0;
		return prev;
	}

	/*
	Attribute* Attribute::Previous()
	{
		// We are using knowledge of the sentinel. The sentinel
		// have a value or name.
		if ( prev->value.empty() && prev->name.empty() )
			return 0;
		return prev;
	}
	*/

	void Attribute::Print( FILE* cfile, int /*depth*/, TIXML_STRING* str ) const
	{
		TIXML_STRING n, v;

		EncodeString( name, &n );
		EncodeString( value, &v );

		if (value.find ('\"') == TIXML_STRING::npos) {
			if ( cfile ) {
				fprintf (cfile, "%s=\"%s\"", n.c_str(), v.c_str() );
			}
			if ( str ) {
				(*str) += n; (*str) += "=\""; (*str) += v; (*str) += "\"";
			}
		}
		else {
			if ( cfile ) {
				fprintf (cfile, "%s='%s'", n.c_str(), v.c_str() );
			}
			if ( str ) {
				(*str) += n; (*str) += "='"; (*str) += v; (*str) += "'";
			}
		}
	}


	int Attribute::QueryIntValue( int* ival ) const
	{
		if ( TIXML_SSCANF( value.c_str(), "%d", ival ) == 1 )
			return TIXML_SUCCESS;
		return TIXML_WRONG_TYPE;
	}

	int Attribute::QueryDoubleValue( double* dval ) const
	{
		if ( TIXML_SSCANF( value.c_str(), "%lf", dval ) == 1 )
			return TIXML_SUCCESS;
		return TIXML_WRONG_TYPE;
	}

	void Attribute::SetIntValue( int _value )
	{
		char buf [64];
		#if defined(TIXML_SNPRINTF)		
			TIXML_SNPRINTF(buf, sizeof(buf), "%d", _value);
		#else
			sprintf (buf, "%d", _value);
		#endif
		SetValue (buf);
	}

	void Attribute::SetDoubleValue( double _value )
	{
		char buf [256];
		#if defined(TIXML_SNPRINTF)		
			TIXML_SNPRINTF( buf, sizeof(buf), "%g", _value);
		#else
			sprintf (buf, "%g", _value);
		#endif
		SetValue (buf);
	}

	int Attribute::IntValue() const
	{
		return atoi (value.c_str ());
	}

	double  Attribute::DoubleValue() const
	{
		return atof (value.c_str ());
	}


	Comment::Comment( const Comment& copy ) : Node( Node::TINYXML_COMMENT )
	{
		copy.CopyTo( this );
	}


	Comment& Comment::operator=( const Comment& base )
	{
		Clear();
		base.CopyTo( this );
		return *this;
	}


	void Comment::Print( FILE* cfile, int depth ) const
	{
		assert( cfile );
		for ( int i=0; i<depth; i++ )
		{
			fprintf( cfile,  "    " );
		}
		fprintf( cfile, "<!--%s-->", value.c_str() );
	}


	void Comment::CopyTo( Comment* target ) const
	{
		Node::CopyTo( target );
	}


	bool Comment::Accept( Visitor* visitor ) const
	{
		return visitor->Visit( *this );
	}


	Node* Comment::Clone() const
	{
		Comment* clone = new Comment();

		if ( !clone )
			return 0;

		CopyTo( clone );
		return clone;
	}


	void Text::Print( FILE* cfile, int depth ) const
	{
		assert( cfile );
		if ( cdata )
		{
			int i;
			fprintf( cfile, "\n" );
			for ( i=0; i<depth; i++ ) {
				fprintf( cfile, "    " );
			}
			fprintf( cfile, "<![CDATA[%s]]>\n", value.c_str() );	// unformatted output
		}
		else
		{
			TIXML_STRING buffer;
			EncodeString( value, &buffer );
			fprintf( cfile, "%s", buffer.c_str() );
		}
	}


	void Text::CopyTo( Text* target ) const
	{
		Node::CopyTo( target );
		target->cdata = cdata;
	}


	bool Text::Accept( Visitor* visitor ) const
	{
		return visitor->Visit( *this );
	}


	Node* Text::Clone() const
	{	
		Text* clone = 0;
		clone = new Text( "" );

		if ( !clone )
			return 0;

		CopyTo( clone );
		return clone;
	}


	Declaration::Declaration( const char * _version,
										const char * _encoding,
										const char * _standalone )
		: Node( Node::TINYXML_DECLARATION )
	{
		version = _version;
		encoding = _encoding;
		standalone = _standalone;
	}


	#ifdef TIXML_USE_STL
	Declaration::Declaration(	const std::string& _version,
										const std::string& _encoding,
										const std::string& _standalone )
		: Node( Node::TINYXML_DECLARATION )
	{
		version = _version;
		encoding = _encoding;
		standalone = _standalone;
	}
	#endif


	Declaration::Declaration( const Declaration& copy )
		: Node( Node::TINYXML_DECLARATION )
	{
		copy.CopyTo( this );	
	}


	Declaration& Declaration::operator=( const Declaration& copy )
	{
		Clear();
		copy.CopyTo( this );
		return *this;
	}


	void Declaration::Print( FILE* cfile, int /*depth*/, TIXML_STRING* str ) const
	{
		if ( cfile ) fprintf( cfile, "<?xml " );
		if ( str )	 (*str) += "<?xml ";

		if ( !version.empty() ) {
			if ( cfile ) fprintf (cfile, "version=\"%s\" ", version.c_str ());
			if ( str ) { (*str) += "version=\""; (*str) += version; (*str) += "\" "; }
		}
		if ( !encoding.empty() ) {
			if ( cfile ) fprintf (cfile, "encoding=\"%s\" ", encoding.c_str ());
			if ( str ) { (*str) += "encoding=\""; (*str) += encoding; (*str) += "\" "; }
		}
		if ( !standalone.empty() ) {
			if ( cfile ) fprintf (cfile, "standalone=\"%s\" ", standalone.c_str ());
			if ( str ) { (*str) += "standalone=\""; (*str) += standalone; (*str) += "\" "; }
		}
		if ( cfile ) fprintf( cfile, "?>" );
		if ( str )	 (*str) += "?>";
	}


	void Declaration::CopyTo( Declaration* target ) const
	{
		Node::CopyTo( target );

		target->version = version;
		target->encoding = encoding;
		target->standalone = standalone;
	}


	bool Declaration::Accept( Visitor* visitor ) const
	{
		return visitor->Visit( *this );
	}


	Node* Declaration::Clone() const
	{	
		Declaration* clone = new Declaration();

		if ( !clone )
			return 0;

		CopyTo( clone );
		return clone;
	}


	void Unknown::Print( FILE* cfile, int depth ) const
	{
		for ( int i=0; i<depth; i++ )
			fprintf( cfile, "    " );
		fprintf( cfile, "<%s>", value.c_str() );
	}


	void Unknown::CopyTo( Unknown* target ) const
	{
		Node::CopyTo( target );
	}


	bool Unknown::Accept( Visitor* visitor ) const
	{
		return visitor->Visit( *this );
	}


	Node* Unknown::Clone() const
	{
		Unknown* clone = new Unknown();

		if ( !clone )
			return 0;

		CopyTo( clone );
		return clone;
	}


	AttributeSet::AttributeSet()
	{
		sentinel.next = &sentinel;
		sentinel.prev = &sentinel;
	}


	AttributeSet::~AttributeSet()
	{
		assert( sentinel.next == &sentinel );
		assert( sentinel.prev == &sentinel );
	}


	void AttributeSet::Add( Attribute* addMe )
	{
		#ifdef TIXML_USE_STL
		assert( !Find( TIXML_STRING( addMe->Name() ) ) );	// Shouldn't be multiply adding to the set.
		#else
		assert( !Find( addMe->Name() ) );	// Shouldn't be multiply adding to the set.
		#endif

		addMe->next = &sentinel;
		addMe->prev = sentinel.prev;

		sentinel.prev->next = addMe;
		sentinel.prev      = addMe;
	}

	void AttributeSet::Remove( Attribute* removeMe )
	{
		Attribute* node;

		for( node = sentinel.next; node != &sentinel; node = node->next )
		{
			if ( node == removeMe )
			{
				node->prev->next = node->next;
				node->next->prev = node->prev;
				node->next = 0;
				node->prev = 0;
				return;
			}
		}
		assert( 0 );		// we tried to remove a non-linked attribute.
	}


	#ifdef TIXML_USE_STL
	Attribute* AttributeSet::Find( const std::string& name ) const
	{
		for( Attribute* node = sentinel.next; node != &sentinel; node = node->next )
		{
			if ( node->name == name )
				return node;
		}
		return 0;
	}

	Attribute* AttributeSet::FindOrCreate( const std::string& _name )
	{
		Attribute* attrib = Find( _name );
		if ( !attrib ) {
			attrib = new Attribute();
			Add( attrib );
			attrib->SetName( _name );
		}
		return attrib;
	}
	#endif


	Attribute* AttributeSet::Find( const char* name ) const
	{
		for( Attribute* node = sentinel.next; node != &sentinel; node = node->next )
		{
			if ( strcmp( node->name.c_str(), name ) == 0 )
				return node;
		}
		return 0;
	}


	Attribute* AttributeSet::FindOrCreate( const char* _name )
	{
		Attribute* attrib = Find( _name );
		if ( !attrib ) {
			attrib = new Attribute();
			Add( attrib );
			attrib->SetName( _name );
		}
		return attrib;
	}


	#ifdef TIXML_USE_STL	
	std::istream& operator>> (std::istream & in, Node & base)
	{
		TIXML_STRING tag;
		tag.reserve( 8 * 1000 );
		base.StreamIn( &in, &tag );

		base.Parse( tag.c_str(), 0, TIXML_DEFAULT_ENCODING );
		return in;
	}
	#endif


	#ifdef TIXML_USE_STL	
	std::ostream& operator<< (std::ostream & out, const Node & base)
	{
		Printer printer;
		printer.SetStreamPrinting();
		base.Accept( &printer );
		out << printer.Str();

		return out;
	}


	std::string& operator<< (std::string& out, const Node& base )
	{
		Printer printer;
		printer.SetStreamPrinting();
		base.Accept( &printer );
		out.append( printer.Str() );

		return out;
	}
	#endif


	Handle Handle::FirstChild() const
	{
		if ( node )
		{
			xml::Node* child = node->FirstChild();
			if ( child )
				return Handle( child );
		}
		return Handle( 0 );
	}


	Handle Handle::FirstChild( const char * value ) const
	{
		if ( node )
		{
			xml::Node* child = node->FirstChild( value );
			if ( child )
				return Handle( child );
		}
		return Handle( 0 );
	}


	Handle Handle::FirstChildElement() const
	{
		if ( node )
		{
			xml::Element* child = node->FirstChildElement();
			if ( child )
				return Handle( child );
		}
		return Handle( 0 );
	}


	Handle Handle::FirstChildElement( const char * value ) const
	{
		if ( node )
		{
			xml::Element* child = node->FirstChildElement( value );
			if ( child )
				return Handle( child );
		}
		return Handle( 0 );
	}


	Handle Handle::Child( int count ) const
	{
		if ( node )
		{
			int i;
			xml::Node* child = node->FirstChild();
			for (	i=0;
					child && i<count;
					child = child->NextSibling(), ++i )
			{
				// nothing
			}
			if ( child )
				return Handle( child );
		}
		return Handle( 0 );
	}


	Handle Handle::Child( const char* value, int count ) const
	{
		if ( node )
		{
			int i;
			xml::Node* child = node->FirstChild( value );
			for (	i=0;
					child && i<count;
					child = child->NextSibling( value ), ++i )
			{
				// nothing
			}
			if ( child )
				return Handle( child );
		}
		return Handle( 0 );
	}


	Handle Handle::ChildElement( int count ) const
	{
		if ( node )
		{
			int i;
			xml::Element* child = node->FirstChildElement();
			for (	i=0;
					child && i<count;
					child = child->NextSiblingElement(), ++i )
			{
				// nothing
			}
			if ( child )
				return Handle( child );
		}
		return Handle( 0 );
	}


	Handle Handle::ChildElement( const char* value, int count ) const
	{
		if ( node )
		{
			int i;
			xml::Element* child = node->FirstChildElement( value );
			for (	i=0;
					child && i<count;
					child = child->NextSiblingElement( value ), ++i )
			{
				// nothing
			}
			if ( child )
				return Handle( child );
		}
		return Handle( 0 );
	}


	bool Printer::VisitEnter( const Document& )
	{
		return true;
	}

	bool Printer::VisitExit( const Document& )
	{
		return true;
	}

	bool Printer::VisitEnter( const Element& element, const Attribute* firstAttribute )
	{
		DoIndent();
		buffer += "<";
		buffer += element.Value();

		for( const Attribute* attrib = firstAttribute; attrib; attrib = attrib->Next() )
		{
			buffer += " ";
			attrib->Print( 0, 0, &buffer );
		}

		if ( !element.FirstChild() ) 
		{
			buffer += " />";
			DoLineBreak();
		}
		else 
		{
			buffer += ">";
			if (    element.FirstChild()->ToText()
				  && element.LastChild() == element.FirstChild()
				  && element.FirstChild()->ToText()->CDATA() == false )
			{
				simpleTextPrint = true;
				// no DoLineBreak()!
			}
			else
			{
				DoLineBreak();
			}
		}
		++depth;	
		return true;
	}


	bool Printer::VisitExit( const Element& element )
	{
		--depth;
		if ( !element.FirstChild() ) 
		{
			// nothing.
		}
		else 
		{
			if ( simpleTextPrint )
			{
				simpleTextPrint = false;
			}
			else
			{
				DoIndent();
			}
			buffer += "</";
			buffer += element.Value();
			buffer += ">";
			DoLineBreak();
		}
		return true;
	}


	bool Printer::Visit( const Text& text )
	{
		if ( text.CDATA() )
		{
			DoIndent();
			buffer += "<![CDATA[";
			buffer += text.Value();
			buffer += "]]>";
			DoLineBreak();
		}
		else if ( simpleTextPrint )
		{
			TIXML_STRING str;
			Base::EncodeString( text.ValueTStr(), &str );
			buffer += str;
		}
		else
		{
			DoIndent();
			TIXML_STRING str;
			Base::EncodeString( text.ValueTStr(), &str );
			buffer += str;
			DoLineBreak();
		}
		return true;
	}


	bool Printer::Visit( const Declaration& declaration )
	{
		DoIndent();
		declaration.Print( 0, 0, &buffer );
		DoLineBreak();
		return true;
	}


	bool Printer::Visit( const Comment& comment )
	{
		DoIndent();
		buffer += "<!--";
		buffer += comment.Value();
		buffer += "-->";
		DoLineBreak();
		return true;
	}


	bool Printer::Visit( const Unknown& unknown )
	{
		DoIndent();
		buffer += "<";
		buffer += unknown.Value();
		buffer += ">";
		DoLineBreak();
		return true;
	}

}