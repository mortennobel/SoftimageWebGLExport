/// License: Creative Commons Attribution 3.0 Unported (http://creativecommons.org/licenses/by/3.0/)
/// 2011 Morten Nobel-Joergensen / Vaida Laganeckiene
/// https://github.com/mortennobel/SoftimageWebGLExport

#pragma once

#include <xsi_x3dobject.h>
#include <xsi_application.h>
#include <xsi_geometryaccessor.h>
#include "helper.h"

class WebGLMeshExport
{
public:
	WebGLMeshExport(XSI::siConstructionMode constMode, XSI::siSubdivisionRuleType subdType, LONG subdLevel,
		XSI::X3DObject &object,XSI::CString name);
	~WebGLMeshExport(void);
	
	bool exportJSONObject(XSI::CString outputDirectory);
	
private:
	void exportVertexPositions(JSONFileWriter &mfw, XSI::CGeometryAccessor& in_ga);
	void exportVertexNormals(JSONFileWriter &mfw, XSI::CGeometryAccessor& in_ga);
	void exportVertexTextureCoords(JSONFileWriter &mfw, XSI::CGeometryAccessor& in_ga);
	void exportIndices(JSONFileWriter &mfw, XSI::CGeometryAccessor& in_ga);
	void OutputPolygonComponentsJSON(JSONFileWriter& in_mfw, XSI::CLongArray& in_indexArray, XSI::CLongArray& in_polySizeArray);

	XSI::X3DObject &object;
	XSI::CString name;
	XSI::siConstructionMode constMode;
	XSI::siSubdivisionRuleType subdType; 
	LONG subdLevel;
};

