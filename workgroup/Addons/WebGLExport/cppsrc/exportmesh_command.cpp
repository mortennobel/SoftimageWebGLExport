#include <xsi_application.h>
#include <xsi_context.h>
#include <xsi_pluginregistrar.h>
#include <xsi_factory.h>
#include <xsi_status.h>
#include <xsi_string.h>
#include <xsi_value.h>
#include <xsi_argument.h>
#include <xsi_command.h>
#include <xsi_menu.h>
#include <xsi_light.h>
#include <xsi_vector3.h>
#include <xsi_model.h>
#include <xsi_ogllight.h>
#include <xsi_parameter.h>
#include <xsi_light.h>
#include <xsi_project.h>
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
#include <time.h>

#include "helper.h"
#include "DependencyExporter.h"
#include "WebGLExportCommand.h"

extern XSI::CustomProperty GetWebGLExportProp();

//*****************************************************************************
/*!	Specify the arguments and return value of the WebGLExportScene command. 
	\param in_ctxt The context that encapsulates the command to initialize.
 */
//*****************************************************************************

XSIPLUGINCALLBACK XSI::CStatus WebGLExportScene_Init( XSI::CRef& in_ctxt )
{
	XSI::Context ctxt( in_ctxt );
	XSI::Command oCmd;
	oCmd = ctxt.GetSource();
	
	// Specify that the command returns a value
	oCmd.EnableReturnValue(true);

	// Add arguments to the command
	XSI::ArgumentArray oArgs;
	oArgs = oCmd.GetArguments();

	oArgs.Add(L"CanvasWidth",(LONG)600);
	oArgs.Add(L"CanvasHeight",(LONG)400);
	oArgs.Add(L"BackgroundColorR",(double)0.0);
	oArgs.Add(L"BackgroundColorG",(double)0.0);
	oArgs.Add(L"BackgroundColorB",(double)0.0);

	return XSI::CStatus::OK;
}


//*****************************************************************************
/*!	Implementation of the WebGLExportScene command. This command uses the 
	CGeometryAccessor to extract the data from the geometry to export.
	\param in_ctxt The context that encapsulates the command to execute.
 */
//*****************************************************************************

XSIPLUGINCALLBACK XSI::CStatus WebGLExportScene_Execute( XSI::CRef& in_ctxt )
{
	// Unpack the command argument values
	XSI::Context ctxt( in_ctxt );
	XSI::CValueArray args = ctxt.GetAttribute(L"Arguments");

	XSI::siConstructionMode constMode = XSI::siConstructionModeModeling;
	
	XSI::siSubdivisionRuleType subdType = XSI::siCatmullClark;
	XSI::Application app;
	app.LogMessage("Starting WebGL export");
	long subdLevel = 0;

	double backgroundColorRGB[3];

	long canvasWidth = args[0];
	long canvasHeight = args[1];
	
	backgroundColorRGB[0] = args[2];
	backgroundColorRGB[1] = args[3];
	backgroundColorRGB[2] = args[4];

	WebGLExportCommand exportCommand(constMode,subdType,subdLevel,canvasWidth,canvasHeight,backgroundColorRGB);
	exportCommand.runExport();
	app.LogMessage("WebGL export done");
	
	return XSI::CStatus::OK;
}