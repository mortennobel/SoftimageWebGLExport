//*****************************************************************************
/*!	\file helper.h
	\brief Helper classes for reading and writing mesh data.
	
	
*/
//*****************************************************************************

#ifndef _HELPER_H_
#define _HELPER_H_

#include <stdio.h>

class XSI::CString;
class XSI::CValue;
class XSI::CStatus;
class XSI::CFloatArray;

//*****************************************************************************
/*! \class CMeshFileWriter helper.h
	Helper class for writing mesh data to a text file.
	A mesh exported with CMeshFileWriter can be imported with CMeshFileReader.
 */
//*****************************************************************************

class CMeshFileWriter
{
	public:

	CMeshFileWriter();
	~CMeshFileWriter();

	XSI::CStatus Init(const XSI::CString& directory,const XSI::CString& in_outFile);

	void Header( const XSI::CString& in_str );
	void Footer();
	void Write( const XSI::CString& in_str, bool quoted = false );
	void Write( const wchar_t* in_pstr, bool quoted = false );
	void Write( const char* in_pstr, bool quoted = false);
	void Write( XSI::CValue in_val );
	void Write( XSI::CFloatArray values);
	void WriteParamHead(const wchar_t *name, int tabIndent = 1);
	void WriteCBracketEnd(int tabIndent = 1,bool addComma = false);
	void EOL(bool addComma = false);
		
	private:
	FILE* p;

};

template< class T >
void OutputJSONArray( CMeshFileWriter& in_mfw, T& in_array ) {	
	int count = in_array.GetCount();

	in_mfw.Write( L"[" );
	for ( LONG i=0; i < count; i++ ) {
		if (i > 0){
			in_mfw.Write( L"," );
		}
		in_mfw.Write(XSI::CString(in_array[i]) );
	}
	in_mfw.Write( L"]" );
}


#endif