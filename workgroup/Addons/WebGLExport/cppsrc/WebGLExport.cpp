#include <xsi_pluginregistrar.h>
#include <xsi_status.h>
#include <xsi_decl.h>

using namespace XSI; 

//*****************************************************************************
/*!	Register the commands, menus, and properties that implement this plug-in.
	\param in_reg The PluginRegistrar created by Softimage for this plug-in.
 */
//*****************************************************************************

XSIPLUGINCALLBACK CStatus XSILoadPlugin( PluginRegistrar& in_reg )
{
	in_reg.PutAuthor(L"Morten Nobel-Joergensen / Vaida Laganeckiene");
	in_reg.PutName(L"WebGL Export Plug-in");
	in_reg.PutVersion(1,0);
	
	// Register commands for importing and exporting a polygon mesh
	in_reg.RegisterCommand(L"WebGLExportScene");
	
	// Install a top-level menu for the import/export tool
	in_reg.RegisterMenu(siMenuMainTopLevelID, L"WebGL Export", false,false);
		
	// Register a custom property to use as the import/export UI	
	in_reg.RegisterProperty(L"WebGLExportProp");
	
	return CStatus::OK;
}

XSIPLUGINCALLBACK CStatus XSIUnloadPlugin( const PluginRegistrar& in_reg )
{
	return CStatus::OK;
}
