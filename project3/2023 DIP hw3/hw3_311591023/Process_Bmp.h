
#include <vector>
#include <string>

#pragma pack(push, 1)//保存之前的對齊方式並改為一BYTE對齊
struct BMPHeader {
    uint16_t signature;
    uint32_t fileSize;
    uint32_t reserved;
    uint32_t dataOffset;
    uint32_t infoheaderSize;
    int32_t  width;
    int32_t  height;
    uint16_t planes;
    uint16_t bitsPerPixel;
    uint32_t compression;
    uint32_t imageSize;
    int32_t  xPixelsPerMeter;
    int32_t  yPixelsPerMeter;
    uint32_t colorsUsed;
    uint32_t colorsImportant;//432
};

#pragma pack(pop)

bool readBMP(const std::string& fileName, BMPHeader& header, std::vector<uint8_t>& imageData);
bool writeBMP(const std::string& fileName, BMPHeader& header, std::vector<uint8_t>& imageData);
