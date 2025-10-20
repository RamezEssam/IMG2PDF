#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "img_pdf_writer.h"


#define ARG_COUNT 5


char* img_path;
char* pdf_path;


static void print_usage() {
    std::cout << "\nConverts a JPEG image to a minimal PDF.\n\n"
        << "Usage:\n"
        << "  IMG2PDF -i <input.jpg> -o <output.pdf>\n\n";
}

static bool parse_cl_args(int argc, char** argv) {

    if (argc < ARG_COUNT) {
        std::cerr << "insufficient arguments\n";
        return false;
    }

    if (strcmp(argv[1], "-i") != 0 || strcmp(argv[3], "-o") != 0) {
        std::cerr << "invalid arguments\n";
        return false;
    }

    img_path = argv[2];
    pdf_path = argv[4];

    return true;

}


static bool run() {
    ImagePDFWriter writer;

    switch (detect_image_format(std::string(img_path))) {

    case ImageFormat::JPEG:
        writer = ImagePDFWriter(img_path, ImageFormat::JPEG);
        if (!writer.Write(pdf_path)) {
            return false;
        }
        break;
    case ImageFormat::PNG:
        writer = ImagePDFWriter(img_path, ImageFormat::PNG);
        if (!writer.Write(pdf_path)) {
            return false;
        }
        break;
    case ImageFormat::BMP:
        std::cerr << ImageFormat::BMP << " format currently not supported";
        return false;
        break;
    case ImageFormat::GIF:
        std::cerr << ImageFormat::GIF << " format currently not supported";
        return false;
        break;
    case ImageFormat::WEBP:
        std::cerr << ImageFormat::WEBP << " format currently not supported";
        return false;
        break;
    case ImageFormat::TIFF:
        std::cerr << ImageFormat::TIFF << " format currently not supported";
        return false;
        break;
    case ImageFormat::UNKNOWN:
        std::cout << "Unkwon Format" << "\n";
        return false;
        break;
    default:
        return false;
        break;
    }
}


int main(int argc, char** argv) {

    // Command line arguments parsing
    if (!parse_cl_args(argc, argv)) {
        print_usage();
        return 1;
    }

    if (!run()) {
        return 1;
    }
    
    return 0;
}
