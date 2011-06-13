Softimage WebGL Exporter
====================================

## Description

Let you export a Autodesk Softimage scene into a WebGL project.

The project is written as a SoftImage C++ plugin and the the export

will create a WebGL project (json, html, javascript and images).

The project contains a simple WebGL renderer written in javascript.

## Installation (Softimage)

The current project requires Autodesk Softimage 2011 64-bit on Windows.
 
(If you are using a different version the code must be recompiled)

Install plugin
* Open Softimage
* Go the the menu File -> Plug-in Manager
* Choose the second tab 'Workgroups'
* Click on the 'Connect...'-button
* Add the workgroup folder (located in the same directory as this file)
* The plugin should now be installed and the menu item 'WebGL Export' should appear.

## Usage

Checkout the help videos located in the folder 'help_videos'

For documentation on how to use exporter, open the WebGL Exporter wizard and click on the questionmark next to WebGLExportProp.

## Compilation

The environment variable XSISDK_ROOT needs to be set to 'C:\Program Files\Autodesk\Softimage 2011\XSISDK' (or where ever SoftImage XSISDK is installed).

Microsoft Visual Studio 2010 will be used for compiling the project.

The MSVC project file is located here: 

workgroup\Addons\WebGLExport\cppsrc\WebGLExport.vcxproj

## Known issues

* WebGL renderer: Mouse movement in browser: Sometimes mouse key events are lost and movement continues
* WebGL renderer: Mouse movement in browser: Vertical movement does not always work.
* Softimage plugin: The exporter ignores everything it doesn't understand. Instead it should give error messages in some situations.
* Softimage plugin: Should verify that the texture dimensions is valid.
* Softimage plugin: More shaders should be better supported (Lambert and constant shader)

## License

License: Creative Commons Attribution 3.0 Unported ( http://creativecommons.org/licenses/by/3.0/ )

Copyright: 2011 Morten Nobel-Joergensen / Vaida Laganeckiene

https://github.com/mortennobel/SoftimageWebGLExport