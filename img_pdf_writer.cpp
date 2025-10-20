#define _CRT_SECURE_NO_WARNINGS
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "img_pdf_writer.h"


std::ostream& operator<<(std::ostream& os, ImageFormat format) {
    switch (format) {
    case ImageFormat::JPEG: return os << "JPEG";
    case ImageFormat::PNG:  return os << "PNG";
    case ImageFormat::BMP: return os << "BMP";
    case ImageFormat::GIF: return os << "GIF";
    case ImageFormat::TIFF: return os << "TIFF";
    case ImageFormat::WEBP: return os << "WEBP";
    case ImageFormat::UNKNOWN: return os << "UNKOWN";
    default:                return os << "UNKNOWN";
    }
}


ImagePDFWriter::ImagePDFWriter()
    : img_type(ImageFormat::UNKNOWN),
    img_size(0),
    img_width(0),
    img_height(0),
    img_comp(0),
    ok(false)
{
    img_data.clear();
    offsets.clear();
}


ImagePDFWriter::ImagePDFWriter(char* img_path, ImageFormat img_type) {

    FILE* f = nullptr;
    f = fopen(img_path, "rb");
    if (!f) {
        std::cerr << "Cannot open image: " << img_path << "\n";
        ok = false;
    }
    else {
        // special check for WebP because stb_image doesn't support it
        if (img_type == ImageFormat::WEBP) {
            ok = false;
        }
        else if (!stbi_info(img_path, &img_width, &img_height, &img_comp)) {
            std::cerr << "Invalid image " << img_path << "\n";
            ok = false;
        }
        else {
            switch (img_type) {
            case ImageFormat::JPEG:
                this->img_type = img_type;
                fseek(f, 0, SEEK_END);
                img_size = ftell(f);
                fseek(f, 0, SEEK_SET);
                img_data.resize(img_size);
                fread(img_data.data(), 1, img_size, f);
                fclose(f);
                ok = true;
                break;
            case ImageFormat::PNG:
                this->img_type = img_type;
                int w, h, comp;
                unsigned char* data = stbi_load(img_path, &w, &h, &comp, 3); // force RGB
                img_width = w;
                img_height = h;
                img_comp = 3;
                img_data.assign(data, data + w * h * 3);
                stbi_image_free(data);
                ok = true;
                break;
            }
            
        }

        
    }  
}

void ImagePDFWriter::add_offset(std::ofstream* out) {
    offsets.push_back(out->tellp());
}

bool ImagePDFWriter::WritePNG(char* pdf_path) {
    std::ofstream out(pdf_path, std::ios::binary);
    if (!out) { std::cerr << "Cannot create PDF\n"; return false; }

    offsets.clear();

    // ---- Start writing PDF ----
    out << "%PDF-1.4\n";

    // --- Object 1: Catalog ---
    add_offset(&out);
    out << "1 0 obj\n"
        << "<< /Type /Catalog /Pages 2 0 R >>\n"
        << "endobj\n";

    // --- Object 2: Pages ---
    add_offset(&out);
    out << "2 0 obj\n"
        << "<< /Type /Pages /Kids [3 0 R] /Count 1 >>\n"
        << "endobj\n";

    // --- Object 3: Page ---
    add_offset(&out);
    out << "3 0 obj\n"
        << "<< /Type /Page /Parent 2 0 R /MediaBox [0 0 "
        << img_width << " " << img_height << "] /Contents 4 0 R /Resources << /XObject <</Im0 5 0 R>> >> >>\n"
        << "endobj\n";

    // --- Object 4: Content stream ---
    add_offset(&out);
    out << "4 0 obj\n";
    std::string content = "q\n" + std::to_string(img_width) + " 0 0 " +
        std::to_string(img_height) + " 0 0 cm /Im0 Do Q\n";

    out << "<< /Length " << content.size() << " >>\nstream\n"
        << content << "\nendstream\nendobj\n";

    // --- Object 5: Image data ---
    add_offset(&out);
    out << "5 0 obj\n"
        << "<< /Type /XObject /Subtype /Image\n"
        << "/Width " << img_width << "\n"
        << "/Height " << img_height << "\n"
        << "/ColorSpace /DeviceRGB\n"
        << "/BitsPerComponent 8\n"
        << "/Length " << img_width * img_height * 3 << " >>\n"
        << "stream\n";

    // Write raw RGB data
    out.write(reinterpret_cast<const char*>(img_data.data()), img_width * img_height * 3);

    out << "\nendstream\nendobj\n";

    // --- xref and trailer ---
    long xref_pos = out.tellp();
    out << "xref\n0 " << (offsets.size() + 1) << "\n";
    out << "0000000000 65535 f \n";
    for (auto o : offsets)
        out << std::setw(10) << std::setfill('0') << o << " 00000 n \n";

    out << "trailer\n<< /Size " << (offsets.size() + 1)
        << " /Root 1 0 R >>\nstartxref\n" << xref_pos << "\n%%EOF\n";

    out.close();

    return true;
}


bool ImagePDFWriter::WriteJPEG(char* pdf_path) {

    std::ofstream out(pdf_path, std::ios::binary);
    if (!out) { std::cerr << "Cannot create PDF\n"; return false; }
    
    offsets.clear();

    // ---- Start writing PDF ----
    out << "%PDF-1.4\n";

    // 1  Catalog
    add_offset(&out);
    out << "1 0 obj\n"
        "<< /Type /Catalog /Pages 2 0 R >>\n"
        "endobj\n";

    // 2  Pages
    add_offset(&out);
    out << "2 0 obj\n"
        "<< /Type /Pages /Kids [3 0 R] /Count 1 >>\n"
        "endobj\n";

    // 3  Page
    add_offset(&out);
    out << "3 0 obj\n"
        "<< /Type /Page /Parent 2 0 R /Resources << /XObject << /Im0 4 0 R >> >> "
        "/MediaBox [0 0 " << img_width << " " << img_height << "] "
        "/Contents 5 0 R >>\n"
        "endobj\n";

    // 4  Image XObject
    add_offset(&out);
    out << "4 0 obj\n"
        "<< /Type /XObject /Subtype /Image "
        "/Width " << img_width << " /Height " << img_height << " "
        "/ColorSpace /DeviceRGB /BitsPerComponent 8 "
        "/Filter /DCTDecode /Length " << img_size << " >>\n"
        "stream\n";
    out.write(reinterpret_cast<const char*>(img_data.data()), img_size);
    out << "\nendstream\nendobj\n";

    // 5  Content stream (draw image)
    std::string content = "q\n" +
        std::to_string(img_width) + " 0 0 " + std::to_string(img_height) + " 0 0 cm\n" +
        "/Im0 Do\nQ\n";

    add_offset(&out);
    out << "5 0 obj\n"
        "<< /Length " << content.size() << " >>\n"
        "stream\n" << content << "endstream\nendobj\n";

    // ---- xref ----
    long xref_pos = out.tellp();
    out << "xref\n0 " << (offsets.size() + 1) << "\n";
    out << "0000000000 65535 f \n";
    for (auto o : offsets)
        out << std::setw(10) << std::setfill('0') << o << " 00000 n \n";

    // ---- trailer ----
    out << "trailer\n"
        "<< /Size " << (offsets.size() + 1)
        << " /Root 1 0 R >>\n"
        "startxref\n" << xref_pos << "\n%%EOF";

    out.close();

    return true;

    
    
}

bool ImagePDFWriter::Write(char* pdf_path) {
    if (ok) {
        switch (img_type) {
        case ImageFormat::JPEG:
            return WriteJPEG(pdf_path);
            break;
        case ImageFormat::PNG:
            return WritePNG(pdf_path);
            break;
        default:
            std::cerr << img_type << " format not supported" << "\n";
            return false;
            break;
        }
    }
    
}
