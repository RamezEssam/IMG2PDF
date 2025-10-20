#pragma once
#include <string>
#include <fstream>
#include <array>
#include <algorithm>

enum class ImageFormat {
    JPEG,
    PNG,
    BMP,
    GIF,
    TIFF,
    WEBP,
    UNKNOWN
};

std::ostream& operator<<(std::ostream& os, ImageFormat format);

// --- Detect from file extension ---
inline ImageFormat detect_format_from_extension(const std::string& path) {
    auto dot = path.find_last_of('.');
    if (dot == std::string::npos)
        return ImageFormat::UNKNOWN;

    std::string ext = path.substr(dot + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext == "jpg" || ext == "jpeg") return ImageFormat::JPEG;
    if (ext == "png") return ImageFormat::PNG;
    if (ext == "bmp") return ImageFormat::BMP;
    if (ext == "gif") return ImageFormat::GIF;
    if (ext == "tif" || ext == "tiff") return ImageFormat::TIFF;
    if (ext == "webp") return ImageFormat::WEBP;

    return ImageFormat::UNKNOWN;
}

// --- Detect from magic bytes (header) ---
inline ImageFormat detect_format_from_magic(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file)
        return ImageFormat::UNKNOWN;

    //std::array<unsigned char, 12> header{};
    static char header[12];
    //file.read(reinterpret_cast<char*>(header.data()), header.size());
    file.read(header, 12);
    if (file.gcount() < 4) return ImageFormat::UNKNOWN;

    // JPEG
    if (header[0] == 0xFF && header[1] == 0xD8 && header[2] == 0xFF)
        return ImageFormat::JPEG;
    // PNG
    if (header[0] == 0x89 && header[1] == 'P' && header[2] == 'N' && header[3] == 'G')
        return ImageFormat::PNG;
    // GIF
    if (header[0] == 'G' && header[1] == 'I' && header[2] == 'F')
        return ImageFormat::GIF;
    // BMP
    if (header[0] == 'B' && header[1] == 'M')
        return ImageFormat::BMP;
    // TIFF (little-endian or big-endian)
    if ((header[0] == 'I' && header[1] == 'I' && header[2] == 0x2A && header[3] == 0x00) ||
        (header[0] == 'M' && header[1] == 'M' && header[2] == 0x00 && header[3] == 0x2A))
        return ImageFormat::TIFF;
    // WEBP (RIFF header + "WEBP" chunk)
    if (header[0] == 'R' && header[1] == 'I' && header[2] == 'F' && header[8] == 'W' && header[9] == 'E' && header[10] == 'B' && header[11] == 'P')
        return ImageFormat::WEBP;

    return ImageFormat::UNKNOWN;
}

// --- Combined version ---
inline ImageFormat detect_image_format(const std::string& path) {
    ImageFormat fmt = detect_format_from_magic(path);
    if (fmt != ImageFormat::UNKNOWN)
        return fmt;

    // fallback to extension
    return detect_format_from_extension(path);
}
