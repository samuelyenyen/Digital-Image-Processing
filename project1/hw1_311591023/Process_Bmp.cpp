#include "Process_Bmp.h"
#include <iostream>
#include <fstream>

bool readBMP(const std::string& fileName, BMPHeader& header, std::vector<uint8_t>& imageData) {
    std::ifstream file(fileName, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << fileName << std::endl;
        return false;
    }

    file.read(reinterpret_cast<char*>(&header), sizeof(BMPHeader));

    if (header.signature != 0x4D42) {
        std::cerr << "Error: Not a valid BMP file." << std::endl;
        return false;
    }
    // if (header.infoheaderSize != 40) {
    //     std::cout << "headerSize is " <<std::dec << header.infoheaderSize << std::endl;
    //     std::cerr << "Error: Not a valid BMP file. ifoheader not 40" << std::endl;
    //     return false;
    // }

    if (header.bitsPerPixel != 24 & header.bitsPerPixel != 32) {
        std::cerr << "Error: Only 24-bit and 32-bit BMP images are supported." << std::endl;
        std::cout << "bitsPerPixel is " << header.bitsPerPixel << std::endl;
        return false;
    }

    int width = header.width;
    int height = header.height;
    int channel;
    if (header.bitsPerPixel==24) channel = 3;
    else if (header.bitsPerPixel==32) channel = 4;
    else std::cerr << "Error: Only 24-bit and 32-bit BMP images are supported." << std::endl;
    

    imageData.resize(width * height * channel);
    file.read(reinterpret_cast<char*>(imageData.data()), imageData.size());
    file.close();

    return true;
}

bool writeBMP(const std::string& fileName, BMPHeader& header, std::vector<uint8_t>& imageData) {
    std::ofstream outputFile(fileName, std::ios::binary);
    if (!outputFile.is_open()) {
        std::cerr << "Error: Unable to create output file " << fileName << std::endl;
        return 1;
    }

    outputFile.write(reinterpret_cast<const char*>(&header), sizeof(BMPHeader));
    outputFile.write(reinterpret_cast<const char*>(imageData.data()), imageData.size());
    outputFile.close();

    return true;
}
