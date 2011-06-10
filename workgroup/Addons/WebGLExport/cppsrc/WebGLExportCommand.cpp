#include "WebGLExportCommand.h"
#include <istream>
#include <iostream>
#include <fstream>
#include <iterator>
#include <limits>
#include <xsi_application.h>
#include <xsi_context.h>
#include <xsi_pluginregistrar.h>
#include <xsi_status.h>
#include <xsi_value.h>
#include <xsi_argument.h>
#include <xsi_command.h>
#include <xsi_menu.h>
#include <xsi_light.h>
#include <xsi_vector3.h>
#include <xsi_ogllight.h>
#include <xsi_parameter.h>
#include <xsi_light.h>
#include <xsi_x3dobject.h>
#include <xsi_selection.h>
#include <xsi_primitive.h>
#include <xsi_polygonmesh.h>
#include <xsi_library.h>
#include <xsi_parameter.h>
#include <xsi_geometryaccessor.h>
#include <xsi_longarray.h>
#include <xsi_floatarray.h>
#include <xsi_doublearray.h>
#include <xsi_bitarray.h>
#include <xsi_envelopeweight.h>
#include <xsi_customproperty.h>
#include <xsi_griddata.h>
#include <xsi_clusterproperty.h>
#include <xsi_material.h>
#include <xsi_ppglayout.h>
#include <xsi_userdatamap.h>
#include <xsi_imageclip2.h>
#include <xsi_source.h>
#include <xsi_scene.h>
#include <xsi_texture.h>
#include <xsi_math.h>
#include <xsi_camera.h>
#include <xsi_progressbar.h>
#include <xsi_uitoolkit.h>
#include "DependencyExporter.h"
#include "WebGLExportCommand.h"
#include "WebGLMeshExport.h"

const float RADIAN_TO_DEGREE = 57.2957795;

// forward declarations
extern XSI::Parameter GetWebGLExportOption( const XSI::CString& in_strName );

WebGLExportCommand::WebGLExportCommand(XSI::siConstructionMode constMode,XSI::siSubdivisionRuleType subdType,LONG subdLevel,
	long canvasWidth,long canvasHeight,double *backgroundColorRGB)
	:constMode(constMode),subdType(subdType),subdLevel(subdLevel),
	 app(), sceneRoot(app.GetActiveSceneRoot()), project(app.GetActiveProject()), scene(project.GetActiveScene()),
	 canvasWidth(canvasWidth),canvasHeight(canvasHeight),backgroundColorRGB(backgroundColorRGB)
{
}


WebGLExportCommand::~WebGLExportCommand(void)
{
}

bool WebGLExportCommand::runExport(){
	if (!project.IsValid()) return false;
	if (!scene.IsValid()) return false;
	XSI::CString strOut = GetWebGLExportOption( L"ExportDir" ).GetValue();
	CMeshFileWriter mfw;
	XSI::CStatus st = mfw.Init(strOut,  L"scene.json");
	if (st!=XSI::CStatus::OK)
	{
		return false;
	}
	mfw.Write( L"{" );
	mfw.EOL();
	exportSceneProperties(mfw);
	mfw.WriteParamHead( L"shaders");
	mfw.Write( L"[" );
	mfw.Write( L"phong", true );
	mfw.Write( L"," );
	mfw.Write( L"phong_color", true );
	mfw.Write( L"]" );
	mfw.EOL(true);
	mfw.WriteParamHead( L"textures" );
	mfw.Write( L"[" );
	mfw.EOL();
	exportSceneTextures(mfw);
	mfw.Write( L"\t]" );
	mfw.EOL(true);
	mfw.WriteParamHead( L"materials");
	mfw.Write( L"[" );
	mfw.EOL();
	
	exportSceneMaterials(mfw);
	mfw.Write( L"\t]" );
	mfw.EOL(true);
	bool first = true;
	exportSceneObjects(mfw, first);
	mfw.Write( L"}" );
	
	DependencyExporter exp;
	exp.exportAllFiles(strOut);

	return true;
}

void WebGLExportCommand::exportSceneProperties(CMeshFileWriter &mfw){
	mfw.WriteParamHead( L"projectPath");
	mfw.Write( project.GetPath() ,true);
	mfw.EOL(true);
	
	mfw.WriteParamHead( L"sceneName");
	XSI::CParameterRefArray params = scene.GetParameters();
	XSI::Parameter p = params.GetItem( L"Filename" );
	XSI::CString sRealName = p.GetValue( double(1) );
	mfw.Write( sRealName , true);
	mfw.EOL(true);
	mfw.WriteParamHead( L"origin");
	mfw.Write( L"SoftImage (by WebGLExporter)", true );
	mfw.EOL(true);
	mfw.WriteParamHead( L"canvasWidth");
	mfw.Write(XSI::CValue(canvasWidth) );
	mfw.EOL(true);
	mfw.WriteParamHead( L"canvasHeight");
	mfw.Write(XSI::CValue(canvasHeight) );
	mfw.EOL(true);
	mfw.WriteParamHead( L"backgroundColor");
	mfw.Write("[");
	for (int i=0;i<3;i++){
		if (i>0){
			mfw.Write(L",");
		}
		mfw.Write(XSI::CValue(backgroundColorRGB[i]));
	}
	mfw.Write("]");
	mfw.EOL(true);
}


void WebGLExportCommand::exportSceneTextures(CMeshFileWriter &mfw){
	mfw.Write( L"\t\t{" );
	mfw.EOL();
	mfw.WriteParamHead( L"name",3);
	mfw.Write( L"defaultTexture",true );
	mfw.EOL(true);
	mfw.WriteParamHead( L"url",3);
	mfw.Write( L"default-texture.jpg",true );
	mfw.EOL();
	mfw.Write( L"\t\t}" );
	mfw.EOL();
	XSI::CRefArray imageClips = scene.GetImageClips();
	for (int i=0;i<imageClips.GetCount();i++){
		XSI::ImageClip2 imageClip(imageClips[i]);
		
		XSI::CString name = getValidName(imageClip.GetUniqueName());
		XSI::CString filename;
		XSI::CString filenameLower = imageClip.GetFileName();
		XSI::CString unsupportedFilename;
		filenameLower.Lower();
		bool isLegalFormat = filenameLower.FindString(XSI::CString(".png")) < ULONG_MAX || 
			filenameLower.FindString(XSI::CString(".jpg")) != ULONG_MAX ||
			filenameLower.FindString(XSI::CString(".jpeg")) != ULONG_MAX;

		if (isLegalFormat){
			filename = getFilename(imageClip.GetFileName());
			copyFile(imageClip.GetFileName(), filename);
		} else {
			filename = L"default-texture.jpg";
			unsupportedFilename = getFilename(imageClip.GetFileName());
		}
		mfw.EOL();

		mfw.Write( L"\t\t," );
		mfw.EOL();
		mfw.Write( L"\t\t{" );
		mfw.EOL();
		mfw.WriteParamHead( L"name",3);
		mfw.Write(name,true );
		mfw.EOL(true);
		if (unsupportedFilename.Length()>0){
			mfw.WriteParamHead( L"warning_unsupported_file_name",3);
			mfw.Write(unsupportedFilename,true );
			mfw.EOL(true);
		}
		mfw.WriteParamHead( L"url",3);
		mfw.Write( filename,true );
		mfw.EOL();
		mfw.Write( L"\t\t}" );
		mfw.EOL();
	}
}

void WebGLExportCommand::exportSceneMaterials(CMeshFileWriter &mfw){
	XSI::CFloatArray diffuse(3);
	XSI::CFloatArray specular(3);
	XSI::CFloatArray ambient(3);
	XSI::CString texture( L"defaultTexture");
	float specularExponent = 60.0f;
	for (int i=0;i<3;i++){
		diffuse[i] = 0.7;
		specular[i] = 1.0;
		ambient[i] = 0.7;
	}
	exportSceneMaterial(mfw,
		XSI::CString(L"default_material"),
		XSI::CString(L"phong"),
		XSI::CString(texture),
		diffuse, 
		specular,
		ambient,
		specularExponent);
	XSI::Library materialLibrary = scene.GetActiveMaterialLibrary();
	XSI::CRefArray materials = materialLibrary.GetItems();
	for (int i=0;i<materials.GetCount();i++){
		XSI::Material mat(materials[i]);
		
		XSI::CRefArray shaders = mat.GetShaders();
		for (int j=0;j<shaders.GetCount();j++){
			XSI::Shader shader(shaders[j]);

			XSI::CString shaderName(shader.GetName());
			shaderName.Lower();
			if (!shaderName.IsEqualNoCase("phong")){ // currently only support phong shader
				continue;
			}

			// write imageclips
			XSI::CRefArray imageClips = shader.GetImageClips();
			if (imageClips.GetCount()==0){
				texture = "";
			} else {
				XSI::ImageClip2 imageClip(imageClips[0]);
				texture = getValidName(imageClip.GetUniqueName());
			}

			float tmp;
			shader.GetColorParameterValue("diffuse",diffuse[0],diffuse[1],diffuse[2],tmp);
			shader.GetColorParameterValue("specular",specular[0],specular[1],specular[2],tmp);
			shader.GetColorParameterValue("ambient",ambient[0],ambient[1],ambient[2],tmp);
			
			XSI::CValue specular_inuse = shader.GetParameterValue("specular_inuse");
			if (specular_inuse==false){
				specular[0] = 0;
				specular[1] = 0;
				specular[2] = 0;
			}
			XSI::CValue shiny = shader.GetParameterValue("shiny");
			specularExponent = shiny;
		}
		mfw.Write( L"\t\t," );
		mfw.EOL();
		XSI::CString shaderName = texture.Length()>0?"phong":"phong_color";
		XSI::CString name = getValidName(mat.GetName());
		exportSceneMaterial(mfw,
			name,
			shaderName,
			texture,
			diffuse, 
			specular,
			ambient,
			specularExponent);		
	}
}

void WebGLExportCommand::exportSceneMaterial(CMeshFileWriter &mfw,
		XSI::CString name, 
		XSI::CString shaderName,
		XSI::CString diffuseTexture,
		XSI::CFloatArray diffuseColor,
		XSI::CFloatArray specularColor,
		XSI::CFloatArray ambientColor,
		float specularExponent){
	mfw.Write( L"\t\t{" );
	mfw.EOL();
	mfw.WriteParamHead( L"name",3);
	mfw.Write( name,true );
    mfw.EOL(true);
	mfw.WriteParamHead( L"shader",3);
	mfw.Write( shaderName,true );
    mfw.EOL(true);
	mfw.WriteParamHead( L"parameters",3);
	mfw.Write( L"{" );
	mfw.EOL();
	if (diffuseTexture.Length()>0){
		mfw.WriteParamHead( L"diffuseTexture",4);
		mfw.Write(diffuseTexture,true );
		mfw.EOL(true);
	}
	mfw.WriteParamHead( L"diffuseColor",4);
	mfw.Write(diffuseColor);
	mfw.EOL(true);
	mfw.WriteParamHead( L"specularColor",4);
	mfw.Write( specularColor);
	mfw.EOL(true);
	mfw.WriteParamHead( L"ambientColor",4);
	mfw.Write( ambientColor);
	mfw.EOL(true);
	mfw.WriteParamHead( L"specularExponent",4);
	mfw.Write( XSI::CValue(specularExponent).GetAsText());
	mfw.EOL( );
	mfw.Write( L"\t\t\t}");
	mfw.EOL( );
	mfw.Write( L"\t\t}");
	mfw.EOL( );	
}

void WebGLExportCommand::exportSceneObjects(CMeshFileWriter &mfw, bool &first){
	mfw.WriteParamHead( L"sceneObjects");
	mfw.Write( L"[" );
	mfw.EOL();
	XSI::Model root = scene.GetRoot();
	XSI::CRefArray children = root.GetChildren();
	for (int i=0;i<children.GetCount();i++){
		XSI::CRef childCRef = children.GetItem(i);
		if (childCRef.IsA(XSI::siX3DObjectID ) ) {
			XSI::X3DObject xobj(childCRef);
			exportSceneObjects(mfw, xobj, XSI::CString(""),first);
		}
	}
	mfw.Write( L"\t]" );
	mfw.EOL();
}

void WebGLExportCommand::exportSceneObjects(CMeshFileWriter &mfw,XSI::X3DObject &obj, XSI::CString parent, bool &first){
	XSI::CRefArray children = obj.GetChildren();
	
	bool hasParent = parent.Length()>0;
	
	XSI::CString uniqueName = getValidName(obj.GetUniqueName());
	
	for (int i=0;i<children.GetCount();i++){
		XSI::CRef childCRef = children.GetItem(i);
		if (childCRef.IsA(XSI::siX3DObjectID ) ) {
			XSI::X3DObject xobj(childCRef);
			exportSceneObjects(mfw,xobj,uniqueName, first);
			first = false;
		}
	}
	if (first){
		first = false;
	} else {
		mfw.Write( L"\t\t" );	
		mfw.EOL(true);
	}
 	mfw.Write( L"\t\t{" );
	mfw.EOL();
	mfw.WriteParamHead( L"name",3);
	mfw.Write(uniqueName ,true);
	mfw.EOL(true);
	mfw.WriteParamHead( L"displayName",3);
	mfw.Write(obj.GetName() ,true);
	mfw.EOL(true);
	mfw.WriteParamHead( L"parent",3);
	mfw.Write(parent ,true);
	mfw.EOL(true);
	mfw.WriteParamHead( L"transform",3);
	mfw.Write( L"{" );
	mfw.EOL();

	XSI::MATH::CRotation rotation = obj.GetLocalRotation();
	XSI::MATH::CVector3 translation = obj.GetLocalTranslation();
	XSI::MATH::CVector3 scaling = obj.GetLocalScaling();
	
	XSI::CDoubleArray rotationArray(3);
	XSI::CDoubleArray translationArray(3);
	XSI::CDoubleArray scalingArray(3);
	rotation.GetXYZAngles(rotationArray[0],rotationArray[1],rotationArray[2]);
	for (int i=0;i<3;i++){
		rotationArray[i] *= RADIAN_TO_DEGREE;
	}
	translation.Get(translationArray[0],translationArray[1],translationArray[2]);
	scaling.Get(scalingArray[0],scalingArray[1],scalingArray[2]);

	mfw.WriteParamHead( L"localTranslation",4);
	OutputJSONArray(mfw,translationArray);
	mfw.EOL(true);
	mfw.WriteParamHead( L"localRotation",4);
	OutputJSONArray(mfw,rotationArray);
	mfw.EOL(true);
	mfw.WriteParamHead( L"localScaling",4);
	OutputJSONArray(mfw,scalingArray);
	mfw.EOL();
	mfw.Write( L"\t\t\t\t}" );
	mfw.EOL(true);
	mfw.WriteParamHead( L"components",3);
	mfw.Write( L"[" );
	mfw.EOL();
	if (obj.IsA(XSI::siCameraID )){
		XSI::Camera cam = (XSI::Camera)obj;
		exportCamera(mfw,cam);
	}
	if (exportJSONObject(obj)){
		writeExportedObjectUrl(mfw,obj);
		writeExportedMaterialRef(mfw, obj);
	}
	if (obj.IsA(XSI::siLightID)){
		XSI::Light light = (XSI::Light)obj;
		exportLight(mfw, light);
	}
	mfw.Write( L"\t\t\t]" );
	mfw.EOL();
	mfw.Write( L"\t\t}" );
	mfw.EOL();
}

bool WebGLExportCommand::exportCamera(CMeshFileWriter &mfw, XSI::Camera cameraObj){
	XSI::MATH::CRotation rotation = cameraObj.GetLocalRotation();
	XSI::MATH::CVector3 translation = cameraObj.GetLocalTranslation();
	
	XSI::CDoubleArray rotationArray(3);
	XSI::CDoubleArray translationArray(3);
	rotation.GetXYZAngles(rotationArray[0],rotationArray[1],rotationArray[2]);
	for (int i=0;i<3;i++){
		rotationArray[i] *= RADIAN_TO_DEGREE;
	}
	translation.Get(translationArray[0],translationArray[1],translationArray[2]);
	bool isPerspective = cameraObj.GetParameterValue(XSI::CString(L"proj"))==1;
	XSI::CValue nearPlane = cameraObj.GetParameterValue(XSI::CString(L"near"));
	XSI::CValue farPlane = cameraObj.GetParameterValue(XSI::CString(L"far"));
	XSI::CValue fieldOfView = cameraObj.GetParameterValue(XSI::CString(L"fov"));
	XSI::CValue left = -10;
	XSI::CValue right = 10;
	XSI::CValue bottom = -10;
	XSI::CValue top = 10;
	mfw.Write(L"\t\t\t\t{");
	mfw.EOL();
	mfw.WriteParamHead( L"type",5);
	mfw.Write(L"camera",true);
	mfw.EOL(true);
	mfw.WriteParamHead( L"near",5);
	mfw.Write( XSI::CString(nearPlane));
	mfw.EOL(true);
	mfw.WriteParamHead( L"far",5);
	mfw.Write( XSI::CString(farPlane));
	mfw.EOL(true);
	if (isPerspective){
		mfw.WriteParamHead( L"cameraType",5);
		mfw.Write(L"perspective",true);
		mfw.EOL(true);
		mfw.WriteParamHead( L"fieldOfView",5);
		mfw.Write(XSI::CString(fieldOfView));
		mfw.EOL();
	} else { // ortogonale
		mfw.WriteParamHead( L"type",5);
		mfw.Write(L"ortogonale",true);
		mfw.EOL(true);
		mfw.WriteParamHead( L"left",5);
		mfw.Write(XSI::CString(left));
		mfw.EOL(true);
		mfw.WriteParamHead( L"right",5);
		mfw.Write(XSI::CString(right));
		mfw.EOL(true);
		mfw.WriteParamHead( L"bottom",5);
		mfw.Write(XSI::CString(bottom));
		mfw.EOL(true);
		mfw.WriteParamHead( L"top",5);
		mfw.Write(XSI::CString(top));
		mfw.EOL();
	}
	mfw.Write(L"\t\t\t\t}");
	mfw.EOL();
	return true;
}

bool WebGLExportCommand::exportJSONObject(XSI::X3DObject &object){
	if (!object.IsValid()) return false;
	
	XSI::CString name = getValidName(object.GetUniqueName());

	XSI::CString outputDir = GetWebGLExportOption( L"ExportDir" ).GetValue();
	
	WebGLMeshExport exportMesh(constMode,subdType, subdLevel,
		object,name);
	return exportMesh.exportJSONObject(outputDir);
}

void WebGLExportCommand::exportLight(CMeshFileWriter &mfw, XSI::Light light){
	XSI::OGLLight oglLight = light.GetOGLLight();
	XSI::siLightType lightType = oglLight.GetType();
	mfw.Write(L"\t\t\t\t{");
	mfw.EOL();
	mfw.WriteParamHead( L"type",5);
	mfw.Write(L"light",true);
	mfw.EOL(true);
	
	// "type":"light",
    // "lightType":"directionalLight",
    // "diffuse":[0.9,0.9,0.9],
    // "specular":[0.5,0.5,0.5],
    // "ambient":[0.1,0.1,0.1]
	if (lightType == XSI::siLightInfinite || lightType == XSI::siLightSun ){
		mfw.WriteParamHead( L"lightType",5);
		mfw.Write("directional",true);
		mfw.EOL(true);
	} else if (lightType == XSI::siLightSpot){
		mfw.WriteParamHead( L"lightType",5);
		mfw.Write("spot",true);
		mfw.EOL(true);
	} else if (lightType == XSI::siLightPoint){
		mfw.WriteParamHead( L"lightType",5);
		mfw.Write("point",true);
		mfw.EOL(true);
	}
	XSI::CColor color = oglLight.GetColor();
	XSI::CString colorStr = "[";
	colorStr += XSI::CString(color.r);
	colorStr += ",";
	colorStr += XSI::CString(color.g);
	colorStr += ",";
	colorStr += XSI::CString(color.b);
	colorStr += "]";
	mfw.WriteParamHead( L"diffuse",5);
	mfw.Write(colorStr);
	mfw.EOL(true);
	mfw.WriteParamHead( L"specular",5);
	mfw.Write(colorStr);
	mfw.EOL(true);
	mfw.WriteParamHead( L"ambient",5);
	mfw.Write(colorStr);
	mfw.EOL();
	mfw.Write(L"\t\t\t\t}");
	mfw.EOL();
}

XSI::CString WebGLExportCommand::getValidName(XSI::CString &string){
	XSI::CString output = L"";
	for (int i=0;i<string.Length();i++){
		char c = string.GetAt(i);
		if ((c>='a' && c<='z') || 
			(c>='A' && c<='Z') || 
			(c>='0' && c<='9')) {
				output += c;
		} else if (c=='<'){
			output += '(';
		} else if (c=='>'){
			output += ')';
		} else if (c==' '){
			output += '_';
		}
	}
	return output;
}

void WebGLExportCommand::writeExportedObjectUrl(CMeshFileWriter &mfw,XSI::X3DObject &obj){
	mfw.Write(L"\t\t\t\t{");
	mfw.EOL();
	mfw.WriteParamHead( L"type",5);
	mfw.Write(L"mesh",true);
	mfw.EOL(true);
	mfw.WriteParamHead( L"url",5);
	XSI::CString name =  getValidName(obj.GetUniqueName())+L".json";
	mfw.Write(name,true);
	mfw.EOL();
	mfw.Write(L"\t\t\t\t}");
	mfw.EOL();
}

void WebGLExportCommand::writeExportedMaterialRef(CMeshFileWriter &mfw,XSI::X3DObject &obj){
	mfw.Write(L"\t\t\t\t,");
	mfw.Write(L"\t\t\t\t{");
	mfw.EOL();
	mfw.WriteParamHead( L"type",5);
	mfw.Write(L"material",true);
	mfw.EOL(true);
	mfw.WriteParamHead( L"name",5);

	XSI::Material mat = obj.GetMaterial();
	
	XSI::CString name = getValidName(mat.GetName());
	mfw.Write(name,true);
	mfw.EOL();
	mfw.Write(L"\t\t\t\t}");
	mfw.EOL();
}

XSI::CString WebGLExportCommand::getFilename(XSI::CString fullFilePath){
	if (fullFilePath.Length() < 2){
		return fullFilePath;
	}
	for (int i=fullFilePath.Length()-2;i>=0;i--){
		if (fullFilePath.GetAt(i) == '\\'){
			return fullFilePath.GetSubString(i+1);
		}
	}
	return fullFilePath;
}

void WebGLExportCommand::copyFile(XSI::CString fullFilePath, XSI::CString filename){
	XSI::CString destFile = GetWebGLExportOption( L"ExportDir" ).GetValue();
	if (destFile.GetAt(destFile.Length()-1) != '\\'){
		destFile += L"\\" ;
	}
	destFile += filename;
	using namespace std;

	fstream f(fullFilePath.GetWideString(), fstream::in|fstream::binary);
	f << noskipws;
	istream_iterator<unsigned char> begin(f);
	istream_iterator<unsigned char> end;
 
	fstream f2(destFile.GetWideString(),fstream::out|fstream::trunc|fstream::binary);
	ostream_iterator<char> begin2(f2);
 
	copy(begin, end, begin2);
}