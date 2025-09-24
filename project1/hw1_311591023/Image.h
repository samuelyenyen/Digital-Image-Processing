#include <vector>
#include <string>
#include "Process_Bmp.h"

void flipX(std::vector<uint8_t>& image, int width, int height, int channels);
void setResol(std::vector<uint8_t>& image, int width, int height, int channels, int color_bit);
void reScale(std::vector<uint8_t>& image, BMPHeader& header, int width, int height, int channel, BMPHeader& newHeader, std::vector<uint8_t>& newData, float scale, bool zoom);