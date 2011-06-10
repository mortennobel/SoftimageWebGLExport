#pragma once
#include <xsi_application.h>
#include <xsi_model.h>
#include <xsi_scene.h>
#include <xsi_project.h>
#include <xsi_string.h>

#include "helper.h"

class WebGLExportCommand
{
public:
	WebGLExportCommand(XSI::siConstructionMode constMode,XSI::siSubdivisionRuleType subdType,LONG subdLevel,
		long canvasWidth,long canvasHeight,double *backgroundColorRGB);
	~WebGLExportCommand(void);
	bool runExport();
	
private:
	void exportSceneProperties(CMeshFileWriter &mfw);
	void exportSceneTextures(CMeshFileWriter &mfw);
	void exportSceneMaterials(CMeshFileWriter &mfw);
	void exportSceneMaterial(CMeshFileWriter &mfw,
		XSI::CString name, 
		XSI::CString shaderName,
		XSI::CString diffuseTexture,
		XSI::CFloatArray diffuseColor,
		XSI::CFloatArray specularColor,
		XSI::CFloatArray ambientColor,
		float specularExponent);
	void exportSceneObjects(CMeshFileWriter &mfw, bool &first);
	void exportSceneObjects(CMeshFileWriter &mfw,XSI::X3DObject &obj, XSI::CString parent, bool &first);
	bool exportCamera(CMeshFileWriter &mfw, XSI::Camera cameraObj);
	bool exportJSONObject(XSI::X3DObject &object);
	void exportLight(CMeshFileWriter &mfw, XSI::Light light);
	XSI::CString getValidName(XSI::CString &string);
	void writeExportedObjectUrl(CMeshFileWriter &mfw,XSI::X3DObject &obj);
	void writeExportedMaterialRef(CMeshFileWriter &mfw,XSI::X3DObject &obj);
	XSI::CString getFilename(XSI::CString fullFilePath);
	void copyFile(XSI::CString fullFilePath, XSI::CString filename);

	XSI::siConstructionMode constMode;
	XSI::siSubdivisionRuleType subdType; 
	LONG subdLevel;
	long canvasWidth;
	long canvasHeight;
	double *backgroundColorRGB;
	
	XSI::Application app;	
	XSI::Model sceneRoot;
	XSI::Project project;
	XSI::Scene scene;
};

