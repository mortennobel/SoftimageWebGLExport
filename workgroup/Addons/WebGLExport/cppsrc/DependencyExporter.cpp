/// License: Creative Commons Attribution 3.0 Unported (http://creativecommons.org/licenses/by/3.0/)
/// 2011 Morten Nobel-Joergensen / Vaida Laganeckiene
/// https://github.com/mortennobel/SoftimageWebGLExport


#include "DependencyExporter.h"
#include "Resource1.h"
#include "Resource2.h"
#include "Resource3.h"
#include "Resource4.h"
#include "Resource5.h"
#include "Resource6.h"
#include "Resource7.h"
#include "Resource8.h"
#include "Resource9.h"
#include "Resource10.h"
#include "Resource11.h"
#include "Resource12.h"
#include "Resource13.h"
#include "Resource14.h"

#include <fstream>

DependencyExporter::DependencyExporter(void)
	:width(600),height(480)
{
}


DependencyExporter::~DependencyExporter(void)
{
}

std::string replace(std::string input, std::string searchFor, std::string replacewith){
	size_t pos;
	size_t sizeOfSearchString = searchFor.length();
	while ((pos = input.find(searchFor)) != std::string::npos){
		input = input.replace(pos,sizeOfSearchString,replacewith);
	}
	return input;
}

std::string toString(long l){
	char buffer [50];
	sprintf (buffer, "%d", l);
	return std::string(buffer);
}

void DependencyExporter::exportUpdateCanvasSize(XSI::CString outputFile, const char *data, unsigned int dataLength){
	std::string htmlData(data,dataLength);

	htmlData = replace(htmlData, "width=\"1000\"", replace("width=\"1000\"","1000", toString(this->width)));
	htmlData = replace(htmlData, "height=\"500\"", replace("height=\"500\"","500", toString(this->height)));
	data = htmlData.c_str();
	dataLength = htmlData.length();
	std::ofstream myFile (outputFile.GetWideString(), std::ios::out | std::ios::binary);
    myFile.write (data, dataLength);
	myFile.close();
}

void DependencyExporter::exportFile(XSI::CString directory, std::string fileName, const char *data, unsigned int dataLength){
	XSI::CString outputFile = directory + fileName.c_str();

	bool updateCanvasSize = fileName.compare("index.html")==0;
	if (updateCanvasSize){
		exportUpdateCanvasSize(outputFile, data, dataLength);
		return;
	}

    std::ofstream myFile (outputFile.GetWideString(), std::ios::out | std::ios::binary);
    myFile.write (data, dataLength);
	myFile.close();
}

void DependencyExporter::exportAllFiles(XSI::CString directory){
	if (directory.GetAt(directory.Length()-1) != '\\'){
		directory += L"\\" ;
	}
	exportFile(directory, res_Resource1::Filename, res_Resource1::Data,res_Resource1::Length);
	exportFile(directory, res_Resource2::Filename, res_Resource2::Data,res_Resource2::Length);
	exportFile(directory, res_Resource3::Filename, res_Resource3::Data,res_Resource3::Length);
	exportFile(directory, res_Resource4::Filename, res_Resource4::Data,res_Resource4::Length);
	exportFile(directory, res_Resource5::Filename, res_Resource5::Data,res_Resource5::Length);
	exportFile(directory, res_Resource6::Filename, res_Resource6::Data,res_Resource6::Length);
	exportFile(directory, res_Resource7::Filename, res_Resource7::Data,res_Resource7::Length);
	exportFile(directory, res_Resource8::Filename, res_Resource8::Data,res_Resource8::Length);
	exportFile(directory, res_Resource9::Filename, res_Resource9::Data,res_Resource9::Length);
	exportFile(directory, res_Resource10::Filename, res_Resource10::Data,res_Resource10::Length);
	exportFile(directory, res_Resource11::Filename, res_Resource11::Data,res_Resource11::Length);
	exportFile(directory, res_Resource12::Filename, res_Resource12::Data,res_Resource12::Length);
	exportFile(directory, res_Resource13::Filename, res_Resource13::Data,res_Resource13::Length);
	exportFile(directory, res_Resource14::Filename, res_Resource14::Data,res_Resource14::Length);
}