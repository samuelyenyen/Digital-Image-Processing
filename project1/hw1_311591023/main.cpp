#include <fstream>
#include "Image.h"
#include <iostream>

int main() {
    ////////////////////////////////////////////////////first image
    ///////////////////////////////////flip
    BMPHeader header1;
    std::vector<uint8_t> imageData1;
    if (!readBMP("input1.bmp", header1, imageData1)) {
        return 1;
    }
    int channels1;
    if (header1.bitsPerPixel==24) channels1 = 3;
    else if (header1.bitsPerPixel==32) channels1 = 4;
    int width1 = header1.width;
    int height1 = header1.height;
    flipX(imageData1, width1, height1, channels1);
    if (!writeBMP("output1_flip.bmp", header1, imageData1)) {
        return 1;
    }
    ///////////////////////////////////resolution
    ///////////////6bit
    if (!readBMP("input1.bmp", header1, imageData1)) {
        return 1;
    }
    int color_bit1 = 6;
    setResol(imageData1, width1, height1, channels1, color_bit1);
    if (!writeBMP("output1_1.bmp", header1, imageData1)) {
        return 1;
    }
    ///////////////4bit
    if (!readBMP("input1.bmp", header1, imageData1)) {
        return 1;
    }
    color_bit1 = 4;
    setResol(imageData1, width1, height1, channels1, color_bit1);
    if (!writeBMP("output1_2.bmp", header1, imageData1)) {
        return 1;
    }
    ///////////////2bit
    if (!readBMP("input1.bmp", header1, imageData1)) {
        return 1;
    }
    color_bit1 = 2;
    setResol(imageData1, width1, height1, channels1, color_bit1);
    if (!writeBMP("output1_3.bmp", header1, imageData1)) {
        return 1;
    }
    ///////////////////////////////////scaling
    ///////////////enlarge
    if (!readBMP("input1.bmp", header1, imageData1)) {
        return 1;
    }  
    float scale = 1.5; // Resize factor
    bool zoom = 1;
    BMPHeader newHeader1;
    std::vector<uint8_t> newData1;
    reScale(imageData1, header1, width1, height1, channels1, newHeader1, newData1, scale, zoom);
    if (!writeBMP("output1_up.bmp", newHeader1, newData1)) {
        return 1;
    }
    ///////////////small
    if (!readBMP("input1.bmp", header1, imageData1)) {
        return 1;
    }
    zoom = 0;
    reScale(imageData1, header1, width1, height1, channels1, newHeader1, newData1, scale, zoom);
    if (!writeBMP("output1_down.bmp", newHeader1, newData1)) {
        return 1;
    }
    ////////////////////////////////////////////////////second image
    ///////////////////////////////////flip
    BMPHeader header2;
    std::vector<uint8_t> imageData2;
    if (!readBMP("input2.bmp", header2, imageData2)) {
        return 1;
    }
    int channels2;
    if (header2.bitsPerPixel==24) channels2 = 3;
    else if (header2.bitsPerPixel==32) channels2 = 4;
    int width2 = header2.width;
    int height2 = header2.height;
    flipX(imageData2, width2, height2, channels2);
    if (!writeBMP("output2_flip.bmp", header2, imageData2)) {
        return 1;
    }
    ///////////////////////////////////resolution
    ///////////////6bit
    if (!readBMP("input2.bmp", header2, imageData2)) {
        return 1;
    }
    int color_bit2 = 6;
    setResol(imageData2, width2, height2, channels2, color_bit2);
    if (!writeBMP("output2_1.bmp", header2, imageData2)) {
        return 1;
    }
    ///////////////4bit
    if (!readBMP("input2.bmp", header2, imageData2)) {
        return 1;
    }
    color_bit2 = 4;
    setResol(imageData2, width2, height2, channels2, color_bit2);
    if (!writeBMP("output2_2.bmp", header2, imageData2)) {
        return 1;
    }
    ///////////////2bit
    if (!readBMP("input2.bmp", header2, imageData2)) {
        return 1;
    }
    color_bit2 = 2;
    setResol(imageData2, width2, height2, channels2, color_bit2);
    if (!writeBMP("output2_3.bmp", header2, imageData2)) {
        return 1;
    }
    ///////////////////////////////////scaling
    ///////////////enlarge
    if (!readBMP("input2.bmp", header2, imageData2)) {
        return 1;
    }  
    scale = 1.5; // Resize factor
    zoom = 1;
    BMPHeader newHeader2;
    std::vector<uint8_t> newData2;
    reScale(imageData2, header2, width2, height2, channels2, newHeader2, newData2, scale, zoom);
    if (!writeBMP("output2_up.bmp", newHeader2, newData2)) {
        return 1;
    }
    ///////////////small
    if (!readBMP("input2.bmp", header2, imageData2)) {
        return 1;
    } 
    zoom = 0;
    reScale(imageData2, header2, width2, height2, channels2, newHeader2, newData2, scale, zoom);
    if (!writeBMP("output2_down.bmp", newHeader2, newData2)) {
        return 1;
    }


    return 0;
}