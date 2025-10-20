#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include "img_format.h"



class ImagePDFWriter {
private:
	ImageFormat img_type;
	std::vector<unsigned char> img_data;
	std::vector<long> offsets;
	long img_size;
	int img_width;
	int img_height;
	int img_comp;
	bool ok;
	void add_offset(std::ofstream* out);
	bool WriteJPEG(char* pdf_path);
	bool WritePNG(char* pdf_path);
public:
	ImagePDFWriter();
	ImagePDFWriter(char* img_path, ImageFormat img_type);
	bool Write(char* pdf_path);

};
