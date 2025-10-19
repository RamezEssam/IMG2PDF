#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>

// TODO: Add support for other image formats
enum class ImageType {
	JPEG
};

class ImagePDFWriter {
private:
	ImageType img_type;
	std::vector<unsigned char> img_data;
	std::vector<long> offsets;
	long img_size;
	int img_width;
	int img_height;
	int img_comp;
	bool ok;
	void add_offset(std::ofstream* out);
	bool WriteJPEG(char* pdf_path);
public:
	ImagePDFWriter(char* img_path, ImageType img_type);
	bool Write(char* pdf_path);

};
