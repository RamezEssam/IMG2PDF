#define _CRT_SECURE_NO_WARNINGS
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "img_pdf_writer.h"



ImagePDFWriter::ImagePDFWriter(char* img_path, ImageType img_type) {

    FILE* f = nullptr;

    switch (img_type) {
    case ImageType::JPEG:
        f = fopen(img_path, "rb");
        if (!f) {
            std::cerr << "Cannot open image: " << img_path << "\n";
            ok = false;
        }
        else {
            fseek(f, 0, SEEK_END);
            img_size = ftell(f);
            fseek(f, 0, SEEK_SET);
            img_data.resize(img_size);
            fread(img_data.data(), 1, img_size, f);
            fclose(f);

            
            if (!stbi_info(img_path, &img_width, &img_height, &img_comp)) {
                std::cerr << "Invalid image " << img_path << "\n"; 
                ok = false;
            }
            ok = true;
        }
        break;
    default:
        std::cerr << "Only JPEG image formats are currently supported" << "\n";
        ok = false;
    }


    
    
}

void ImagePDFWriter::add_offset(std::ofstream* out) {
    offsets.push_back(out->tellp());
}

bool ImagePDFWriter::WriteJPEG(char* pdf_path) {
    if (ok) {

        std::ofstream out(pdf_path, std::ios::binary);
        if (!out) { std::cerr << "Cannot create PDF\n"; return false; }

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
    else {
        return false;
    }

    
    
}

bool ImagePDFWriter::Write(char* pdf_path) {
    switch (img_type) {
    case ImageType::JPEG:
        return WriteJPEG(pdf_path);
        break;
    default:
        std::cerr << "Only JPEG image formats are currently supported" << "\n";
        return false;
        break;
    }
}
