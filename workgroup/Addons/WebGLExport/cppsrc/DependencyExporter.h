#pragma once

#include <xsi_string.h>
#include <string>

class DependencyExporter
{
public:
	DependencyExporter(void);
	~DependencyExporter(void);

	void exportAllFiles(XSI::CString directory);

	void setWidth(int w){this->width = w;}
	void setHeight(int h){this->height = h;}
private:
	void exportUpdateCanvasSize(XSI::CString outputFile, const char *data, unsigned int dataLength);
	void exportFile(XSI::CString directory, std::string fileName, const char *data, unsigned int dataLength);

	long width;
	long height;
};

