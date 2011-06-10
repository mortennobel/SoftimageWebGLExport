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
	void exportVertexPositions(CMeshFileWriter &mfw, XSI::CGeometryAccessor& in_ga);
	void exportVertexNormals(CMeshFileWriter &mfw, XSI::CGeometryAccessor& in_ga);
	void exportVertexTextureCoords(CMeshFileWriter &mfw, XSI::CGeometryAccessor& in_ga);
	void exportIndices(CMeshFileWriter &mfw, XSI::CGeometryAccessor& in_ga);
	void OutputPolygonComponentsJSON(CMeshFileWriter& in_mfw, XSI::CLongArray& in_indexArray, XSI::CLongArray& in_polySizeArray);

	XSI::X3DObject &object;
	XSI::CString name;
	XSI::siConstructionMode constMode;
	XSI::siSubdivisionRuleType subdType; 
	LONG subdLevel;
};

