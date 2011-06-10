/// License: Creative Commons Attribution 3.0 Unported (http://creativecommons.org/licenses/by/3.0/)
/// 2011 Morten Nobel-Joergensen / Vaida Laganeckiene
/// https://github.com/mortennobel/SoftimageWebGLExport


//*****************************************************************************
/*!	\file helper.cpp
	\brief Helper classes for reading and writing mesh data.	
*/
//*****************************************************************************

#include <xsi_string.h>
#include <xsi_value.h>
#include <xsi_value.h>
#include <xsi_floatarray.h>

#include <xsi_status.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <direct.h> 
#include <errno.h>

#include "helper.h"

#include <wchar.h>
#include <string.h>

#define MAX_CHARS 200

// forward declarations
bool createDirectoryIfNotExist(const XSI::CString& directory);

// logger
JSONFileWriter::JSONFileWriter() : p (NULL) {}
JSONFileWriter::~JSONFileWriter()
{
	if (p)
	{
		fclose(p);
	}
}

XSI::CStatus JSONFileWriter::Init(const XSI::CString& directory,const XSI::CString& in_outFile )
{
	XSI::CString file;
	file += directory;
	if (directory.GetAt(directory.Length()-1) != '\\'){
		file += L"\\" ;
	}
	file += in_outFile;

	bool res = createDirectoryIfNotExist(directory);
	if (!res){
		return XSI::CStatus::AccessDenied;
	}
	const wchar_t* fileWideStr = file.GetWideString();
	p = _wfopen( fileWideStr, L"wt" );
	
	if (!p) return XSI::CStatus::InvalidArgument;
	
	return XSI::CStatus::OK;	
}

void JSONFileWriter::Header( const XSI::CString& in_strText )
{	
	Write( L"\n" );
	XSI::CString str = in_strText;
	str += L"\n{\n";
	
	Write( str );
}

void JSONFileWriter::Footer()
{
	Write( L"}\n\n" );
}

void JSONFileWriter::Write( const XSI::CString& in_str, bool quoted )
{	
	if (quoted) fwrite("\"",sizeof(char),1,p);
	XSI::CStringArray sArray = in_str.Split("\\");
	for (int i=0;i<sArray.GetCount();i++){
		if (i>0){
			fwrite("\\\\",sizeof(char),2,p); // escape backslash
		}
		const char* s = sArray[i].GetAsciiString();
		size_t nLen = strlen(s);
		fwrite( s, sizeof(char), nLen, p );
	}
	if (quoted) fwrite("\"",sizeof(char),1,p);
}

void JSONFileWriter::Write( XSI::CFloatArray values){
	Write(L"[");
	for (int i=0;i<values.GetCount();i++){
		if (i>0){
			Write(L",");
		}
		Write(XSI::CValue (values[i]));
	}
	Write(L"]");
}


void JSONFileWriter::Write( const wchar_t* in_pstr, bool quoted )
{
	XSI::CString str(in_pstr);
	Write( str,quoted );
}

void JSONFileWriter::Write( const char* in_pstr, bool quoted ){
	const XSI::CString s(in_pstr);
	Write(s,quoted);
}

void JSONFileWriter::Write( XSI::CValue in_val )
{
	Write( in_val.GetAsText().GetWideString() );
}

void JSONFileWriter::EOL(bool addComma)
{
	if (addComma){
		Write( L"," );
	}
	Write( L"\n" );
}

void JSONFileWriter::WriteParamHead(const wchar_t *name, int tabIndent){
	for (int i=0;i<tabIndent;i++){
		Write( L"\t" );
	}
	Write( L"\"" );
	Write( name );
	Write( L"\" : " );
}

void JSONFileWriter::WriteCBracketEnd(int tabIndent,bool addComma ){
	for (int i=0;i<tabIndent;i++){
		Write( L"\t" );
	}
	Write( L"}" );
	EOL(addComma);
}

bool createDirectoryIfNotExist(const XSI::CString& directory){
	int e;
	struct _stat sb;
	const wchar_t* directoryAscii = directory.GetWideString();
	e = _wstat(directoryAscii, &sb);
	if (e == 0)	{
		if (sb.st_mode & S_IFDIR)
			return true; // is directory
		if (sb.st_mode & S_IFREG)
			return false; // is regular file
	} else {
		if (errno = ENOENT)
		{
			// The directory does not exist. Creating new directory...
			// Add more flags to the mode if necessary.
			e = _wmkdir(directoryAscii);
			if (e != 0)
			{
				// mkdir failed; errno=errno
				return false; // is regular file
			}
			else
			{
				return true; // created the directory %s\n",name
			}
		}
	}
	return 0;
}


