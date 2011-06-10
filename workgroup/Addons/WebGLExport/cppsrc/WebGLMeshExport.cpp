/// License: Creative Commons Attribution 3.0 Unported (http://creativecommons.org/licenses/by/3.0/)
/// 2011 Morten Nobel-Joergensen / Vaida Laganeckiene
/// https://github.com/mortennobel/SoftimageWebGLExport

#include "WebGLMeshExport.h"

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

WebGLMeshExport::WebGLMeshExport(XSI::siConstructionMode constMode, XSI::siSubdivisionRuleType subdType, LONG subdLevel,
	XSI::X3DObject &object,XSI::CString name)
	:constMode(constMode),subdType(subdType),subdLevel(subdLevel),object(object),name(name)
{
}


WebGLMeshExport::~WebGLMeshExport(void)
{
}

bool WebGLMeshExport::exportJSONObject(XSI::CString outputDirectory){
	// Get a geometry accessor from the selected object	
	XSI::Primitive prim = object.GetActivePrimitive();
	if (!prim.IsValid()) return false;
	XSI::PolygonMesh mesh = prim.GetGeometry();
	if (!mesh.IsValid()) return false;

	XSI::CString filename = name+".json";

	// prepare the output text file
	JSONFileWriter mfw;
	XSI::CStatus st = mfw.Init(outputDirectory,  filename);
	if (st!=XSI::CStatus::OK) return false;

	XSI::CGeometryAccessor ga = mesh.GetGeometryAccessor( constMode, subdType, subdLevel );
	if (!ga.IsValid()) return false;
	mfw.Write( L"{" );
	mfw.EOL();
	mfw.WriteParamHead( L"vertexPositions");
	exportVertexPositions(mfw,ga);
	mfw.EOL(true);
	mfw.WriteParamHead( L"vertexNormals");
	exportVertexNormals(mfw,ga);
	mfw.EOL(true);
	mfw.WriteParamHead( L"vertexTextureCoords");
	exportVertexTextureCoords(mfw,ga);
	mfw.EOL(true);
	mfw.WriteParamHead( L"indices");
	exportIndices(mfw, ga);
	mfw.EOL();
	mfw.Write( L"}" );
	return true;
}

void WebGLMeshExport::exportVertexPositions(JSONFileWriter &mfw, XSI::CGeometryAccessor& in_ga){
	// problem: We are interested in vertex per node, but only have vertex index per node
	// to solve this must loop though all nodes and fill in the correct vertices
	
	// polygon Vertex Positions positions
	XSI::CDoubleArray vtxPosArray;
	XSI::CStatus st = in_ga.GetVertexPositions(vtxPosArray);
	st.AssertSucceeded( L"GetVertexPositions" );

	LONG nodeCount =  in_ga.GetNodeCount();

	XSI::CDoubleArray data(nodeCount*3);

	XSI::CLongArray polyVtxIdxArray;
	st = in_ga.GetVertexIndices(polyVtxIdxArray);
	st.AssertSucceeded( L"GetVertexIndices" );

	XSI::CLongArray polySizeArray;
	st = in_ga.GetPolygonVerticesCount(polySizeArray);
	
	XSI::CLongArray polyNodeIdxArray;
	st = in_ga.GetNodeIndices(polyNodeIdxArray);
	st.AssertSucceeded( L"GetNodeIndices" );

	for (LONG i=0, offset=0; i<polyVtxIdxArray.GetCount(); i++)
	{	
		for (LONG j=0; j<polyVtxIdxArray[i]; j++)
		{
			int polyVertexIndex = polyVtxIdxArray[offset];
			int polyNodeIndex = polyNodeIdxArray[offset];	
			// now copy vertex polyVertexIndex into position polyNodeIndex
			for (int k=0;k<3;k++){
				data[polyNodeIndex*3+k] = vtxPosArray[polyVertexIndex*3+k];
			}
			offset++;
		}
	}
	OutputJSONArray( mfw, data );
}

void WebGLMeshExport::exportVertexNormals(JSONFileWriter &mfw, XSI::CGeometryAccessor& in_ga){
	// polygon node normals
	XSI::CFloatArray nodeArray;
	XSI::CStatus st = in_ga.GetNodeNormals(nodeArray);
	st.AssertSucceeded( L"GetNodeNormals" );
	
	OutputJSONArray( mfw, nodeArray );
}

void WebGLMeshExport::exportVertexTextureCoords(JSONFileWriter &mfw, XSI::CGeometryAccessor& in_ga){
	// problem: We are interested in vertex per node, but only have vertex index per node
	// to solve this must loop though all nodes and fill in the correct vertices
	
	// polygon Vertex Positions positions
	// uv props: siClusterPropertyUVType
	XSI::CRefArray uvProps = in_ga.GetUVs();
	LONG nVals = uvProps.GetCount();
	LONG nValueSize = 0;
	XSI::CFloatArray uvValueArray;
	XSI::CBitArray flags;
	if (nVals>0){
		XSI::ClusterProperty cp(uvProps[0]); // only use first UV coordinates
		cp.GetValues( uvValueArray, flags );
		nValueSize = cp.GetValueSize();
	}
	LONG nodeCount =  in_ga.GetNodeCount();

	XSI::CDoubleArray data(nodeCount*2);

	for (LONG i=0; i<nodeCount; i++)
	{	
		// now copy vertex polyVertexIndex into position polyNodeIndex
		if (nVals>0){
			data[i*2] = uvValueArray[i*nValueSize];
			data[i*2+1] = uvValueArray[i*nValueSize+1];
		} else {
			data[i*2] = 0.0;
			data[i*2+1] = 0.0;
		}
	}
	OutputJSONArray( mfw, data );
}

void WebGLMeshExport::exportIndices(JSONFileWriter &mfw, XSI::CGeometryAccessor& in_ga){
	// polygon triangle vertex indices
	XSI::CLongArray triVtxIdxArray;
	XSI::CStatus st = in_ga.GetTriangleVertexIndices(triVtxIdxArray);
	st.AssertSucceeded( L"GetTriangleVertexIndices" );

	XSI::CLongArray triSizeArray(in_ga.GetTriangleCount());
	{
		for (LONG i=0;i<triSizeArray.GetCount();i++) triSizeArray[i]=3;
	}

	// polygon triangle node indices
	XSI::CLongArray triNodeIdxArray;
	st = in_ga.GetTriangleNodeIndices(triNodeIdxArray);
	st.AssertSucceeded( L"GetTriangleNodeIndices" );

	OutputPolygonComponentsJSON(mfw, triNodeIdxArray, triSizeArray);	
}

void WebGLMeshExport::OutputPolygonComponentsJSON(JSONFileWriter& in_mfw, XSI::CLongArray& in_indexArray, 
		XSI::CLongArray& in_polySizeArray){
	in_mfw.Write( L"[" );
	for (LONG i=0, offset=0; i<in_polySizeArray.GetCount(); i++)
	{	
		if (i!=0){
			in_mfw.Write( L"," );
		}
		in_mfw.Write( XSI::CString(in_indexArray[offset]));
		for (LONG j=1; j<in_polySizeArray[i]; j++)
		{
			in_mfw.Write( L"," );
			in_mfw.Write( XSI::CString(in_indexArray[offset+j]));
		}
		offset += in_polySizeArray[i];
	}
	in_mfw.Write( L"]" );
}