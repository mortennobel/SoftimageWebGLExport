//*****************************************************************************
/*!	\file importexport_menu.cpp
 	\brief Defines the callbacks that implement the Import Export demo menu.

	Copyright 2008 Autodesk, Inc.  All rights reserved.  
	Use of this software is subject to the terms of the Autodesk license agreement 
	provided at the time of installation or download, or which otherwise accompanies 
	this software in either electronic or hard copy form.   
 
 */
//*****************************************************************************


#include <xsi_context.h>
#include <xsi_ref.h>
#include <xsi_menu.h>
#include <xsi_customproperty.h>
#include <xsi_application.h>
#include <xsi_value.h>

extern XSI::CustomProperty GetWebGLExportProp();

//*****************************************************************************
/*!	Add the Import Export Demo menu item to the Demo Tool menu. This menu item
	triggers a callback that opens the Import Export Demo UI.
	\param in_ctxt The context that encapsulates the menu to initialize.
 */
//*****************************************************************************

XSIPLUGINCALLBACK XSI::CStatus WebGLExport_Init( XSI::CRef& in_ctxt )
{
	XSI::Context ctxt( in_ctxt );

	XSI::Menu oMenu;
	oMenu = ctxt.GetSource();		
	
	XSI::MenuItem oNewItem;
	oMenu.AddCallbackItem( L"WebGL export wizard", L"OnWebGLExportWizardClicked", oNewItem );
	
	return XSI::CStatus::OK;
}

//*****************************************************************************
/*!	Callback executed when a user clicks the Import Export Demo menu item. The 
	callback displays the Import Export Demo property page.
	\param in_ctxt The context that encapsulates the selected menu item.
 */
//*****************************************************************************

XSIPLUGINCALLBACK XSI::CStatus OnWebGLExportWizardClicked( XSI::CRef& in_ref)
{	
	XSI::CustomProperty prop = GetWebGLExportProp();

	XSI::CValue retVal ;
	
	XSI::CValueArray args(5) ;	
	args[0] = prop; 
	args[3] = (LONG)XSI::siLock;
	
	XSI::Application app;
	app.ExecuteCommand( L"InspectObj", args, retVal ) ;		

	return XSI::CStatus::OK;	
}
