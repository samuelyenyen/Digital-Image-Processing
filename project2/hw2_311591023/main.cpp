#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <math.h>
#include <cmath>
#include <array>

#pragma pack(push, 1) // Ensure there's no padding in the BMP header
struct BMPHeader {
    uint16_t signature;
    uint32_t fileSize;
    uint32_t reserved;
    uint32_t dataOffset;
    uint32_t headerSize;
    int32_t  width;
    int32_t  height;
    uint16_t planes;
    uint16_t bitsPerPixel;
    uint32_t compression;
    uint32_t imageSize;
    int32_t  xPixelsPerMeter;
    int32_t  yPixelsPerMeter;
    uint32_t colorsUsed;
    uint32_t colorsImportant;
};
#pragma pack(pop)

// Define a structure to represent an RGB pixel
struct Pixel {
    unsigned char r, g, b;
};

// Function to apply histogram equalization
void method1(std::vector<Pixel>& image, std::vector<int>& histogramR, std::vector<int>& histogramG, std::vector<int>& histogramB, int& totalPixels) {
    for (const Pixel& pixel : image) {
        histogramR[pixel.r]++;
        histogramG[pixel.g]++;
        histogramB[pixel.b]++;
    }
    totalPixels = image.size();
}
void method2(std::vector<Pixel>& image, std::vector<int>& histogramR, std::vector<int>& histogramG, std::vector<int>& histogramB, int& totalPixels) {
    for (int i = image.size()/3; i < image.size()*2/3; i++) {
        histogramR[image[i].r] = histogramR[image[i].r] +1;
        histogramG[image[i].g] = histogramG[image[i].g] +1;
        histogramB[image[i].b] = histogramB[image[i].b] +1;
    }
    totalPixels = image.size()*1/3;
}

void applyHistogramEqualization(std::vector<Pixel>& image , int method) {
    // Calculate histograms for each color channel
    std::vector<int> histogramR(256, 0);
    std::vector<int> histogramG(256, 0);
    std::vector<int> histogramB(256, 0);
    // Normalize cumulative histograms
    int totalPixels;
    if (method==1)
        method1(image, histogramR, histogramG, histogramB ,totalPixels);
    else if(method==2)
        method2(image, histogramR, histogramG, histogramB ,totalPixels);
    // Calculate cumulative histograms
    std::vector<int> cumulativeHistR(256, 0);
    std::vector<int> cumulativeHistG(256, 0);
    std::vector<int> cumulativeHistB(256, 0);

    // std::sort(histogramR.begin(), histogramR.end(), std::greater<int>());
    // std::sort(histogramG.begin(), histogramG.end(), std::greater<int>());
    // std::sort(histogramB.begin(), histogramB.end(), std::greater<int>());
    cumulativeHistR[0] = histogramR[0];
    cumulativeHistG[0] = histogramG[0];
    cumulativeHistB[0] = histogramB[0];

    for (int i = 1; i < 256; i++) {
        cumulativeHistR[i] = cumulativeHistR[i - 1] + histogramR[i];
        cumulativeHistG[i] = cumulativeHistG[i - 1] + histogramG[i];
        cumulativeHistB[i] = cumulativeHistB[i - 1] + histogramB[i];
    }

    
    // // std::cout <<totalPixels<<std::endl;
    for (int i = 0; i < 256; i++) {
        cumulativeHistR[i] = static_cast<int>(std::round((cumulativeHistR[i] * 255.0) / totalPixels));
        cumulativeHistG[i] = static_cast<int>(std::round((cumulativeHistG[i] * 255.0) / totalPixels));
        cumulativeHistB[i] = static_cast<int>(std::round((cumulativeHistB[i] * 255.0) / totalPixels));
    }

    // Apply the new pixel values using the cumulative histograms
    for (Pixel& pixel : image) {
        pixel.r = static_cast<unsigned char>(cumulativeHistR[pixel.r]);
        pixel.g = static_cast<unsigned char>(cumulativeHistG[pixel.g]);
        pixel.b = static_cast<unsigned char>(cumulativeHistB[pixel.b]);
    }
}

// Function to apply a Laplacian filter for sharpness enhancement
void applyLaplacianFilter1(std::vector<Pixel>& image, int width, int height) {
    std::vector<Pixel> resultImage = image; // Create a copy of the image to store the result

    // Define the Laplacian kernel
    int kernel[3][3] = {
        {0, -1, 0},
        {-1, 5, -1},
        {0, -1, 0}
    };

    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            int sumR = 0, sumG = 0, sumB = 0;
            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    int pixelIndex = (y + ky) * width + (x + kx);
                    sumR += image[pixelIndex].r * kernel[ky + 1][kx + 1];
                    sumG += image[pixelIndex].g * kernel[ky + 1][kx + 1];
                    sumB += image[pixelIndex].b * kernel[ky + 1][kx + 1];
                }
            }
            resultImage[y * width + x].r = std::max(0, std::min(255, sumR));
            resultImage[y * width + x].g = std::max(0, std::min(255, sumG));
            resultImage[y * width + x].b = std::max(0, std::min(255, sumB));
        }
    }

    image = resultImage; // Replace the original image with the sharpened image

}
void applyLaplacianFilter2(std::vector<Pixel>& image, int width, int height) {
    std::vector<Pixel> resultImage = image; // Create a copy of the image to store the result

    // Define the Laplacian kernel
    int kernel[3][3] = {
        {-1, -1, -1},
        {-1, 9, -1},
        {-1, -1, -1}
    };

    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            int sumR = 0, sumG = 0, sumB = 0;
            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    int pixelIndex = (y + ky) * width + (x + kx);
                    sumR += image[pixelIndex].r * kernel[ky + 1][kx + 1];
                    sumG += image[pixelIndex].g * kernel[ky + 1][kx + 1];
                    sumB += image[pixelIndex].b * kernel[ky + 1][kx + 1];
                }
            }
            resultImage[y * width + x].r = std::max(0, std::min(255, sumR));
            resultImage[y * width + x].g = std::max(0, std::min(255, sumG));
            resultImage[y * width + x].b = std::max(0, std::min(255, sumB));
        }
    }

    image = resultImage; // Replace the original image with the sharpened image

}

void applyMedianFilter2(std::vector<Pixel>& image, int width, int height) {
    std::vector<Pixel> resultImage = image; // Create a copy of the image to store the result

    // Define the median filter kernel size (adjust as needed)
    int kernelSize = 2; // 3x3 kernel in this example

    for (int y = kernelSize / 2; y < height - kernelSize / 2; y++) {
        for (int x = kernelSize / 2; x < width - kernelSize / 2; x++) {
            
            std::array<unsigned char, 4> rValues;
            std::array<unsigned char, 4> gValues;
            std::array<unsigned char, 4> bValues;

            int index = 0;

            for (int ky = -kernelSize / 2; ky < kernelSize / 2; ky++) {
                for (int kx = -kernelSize / 2; kx < kernelSize / 2; kx++) {
                    int pixelIndex = (y + ky) * width + (x + kx);
                    rValues[index] = image[pixelIndex].r;
                    gValues[index] = image[pixelIndex].g;
                    bValues[index] = image[pixelIndex].b;
                    index++;
                }
            }

            // Sort the values to find the median
            std::sort(rValues.begin(), rValues.end());
            std::sort(gValues.begin(), gValues.end());
            std::sort(bValues.begin(), bValues.end());

            // Set the median values
            resultImage[y * width + x].r = rValues[2];
            resultImage[y * width + x].g = gValues[2];
            resultImage[y * width + x].b = bValues[2];
        }
    }

    image = resultImage; // Replace the original image with the filtered image
}

void applyMedianFilter3(std::vector<Pixel>& image, int width, int height) {
    std::vector<Pixel> resultImage = image; // Create a copy of the image to store the result

    // Define the median filter kernel size (adjust as needed)
    int kernelSize = 3; // 3x3 kernel in this example

    for (int y = kernelSize / 2; y < height - kernelSize / 2; y++) {
        for (int x = kernelSize / 2; x < width - kernelSize / 2; x++) {
            
            std::array<unsigned char, 9> rValues;
            std::array<unsigned char, 9> gValues;
            std::array<unsigned char, 9> bValues;

            int index = 0;

            for (int ky = -kernelSize / 2; ky <= kernelSize / 2; ky++) {
                for (int kx = -kernelSize / 2; kx <= kernelSize / 2; kx++) {
                    int pixelIndex = (y + ky) * width + (x + kx);
                    rValues[index] = image[pixelIndex].r;
                    gValues[index] = image[pixelIndex].g;
                    bValues[index] = image[pixelIndex].b;
                    index++;
                }
            }

            // Sort the values to find the median
            std::sort(rValues.begin(), rValues.end());
            std::sort(gValues.begin(), gValues.end());
            std::sort(bValues.begin(), bValues.end());

            // Set the median values
            resultImage[y * width + x].r = rValues[4];
            resultImage[y * width + x].g = gValues[4];
            resultImage[y * width + x].b = bValues[4];
        }
    }

    image = resultImage; // Replace the original image with the filtered image
}
void applyMedianFilter5(std::vector<Pixel>& image, int width, int height) {
    std::vector<Pixel> resultImage = image; // Create a copy of the image to store the result

    // Define the median filter kernel size (adjust as needed)
    int kernelSize = 5; // 3x3 kernel in this example

    for (int y = kernelSize / 2; y < height - kernelSize / 2; y++) {
        for (int x = kernelSize / 2; x < width - kernelSize / 2; x++) {
            std::array<unsigned char, 25> rValues;
            std::array<unsigned char, 25> gValues;
            std::array<unsigned char, 25> bValues;

            int index = 0;

            for (int ky = -kernelSize / 2; ky <= kernelSize / 2; ky++) {
                for (int kx = -kernelSize / 2; kx <= kernelSize / 2; kx++) {
                    int pixelIndex = (y + ky) * width + (x + kx);
                    rValues[index] = image[pixelIndex].r;
                    gValues[index] = image[pixelIndex].g;
                    bValues[index] = image[pixelIndex].b;
                    index++;
                }
            }

            // Sort the values to find the median
            std::sort(rValues.begin(), rValues.end());
            std::sort(gValues.begin(), gValues.end());
            std::sort(bValues.begin(), bValues.end());

            // Set the median values
            resultImage[y * width + x].r = rValues[13];
            resultImage[y * width + x].g = gValues[13];
            resultImage[y * width + x].b = bValues[13];
        }
    }

    image = resultImage; // Replace the original image with the filtered image
}


// Function to load an image from a BMP file
std::vector<Pixel> loadImage(const std::string& filename, BMPHeader& header) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Error: Could not open the input image file.");
    }

    file.read(reinterpret_cast<char*>(&header), sizeof(header));

    // Check if it's a valid BMP file
    if (header.signature != 0x4D42) {
        throw std::runtime_error("Error: Not a valid BMP file.");
    }

    // Get image dimensions
    int width = header.width;
    int height = header.height;

    // Move to the pixel data
    file.seekg(header.dataOffset);

    // Read pixel data
    std::vector<Pixel> image(width * height);
    file.read(reinterpret_cast<char*>(image.data()), sizeof(Pixel) * width * height);

    return image;
}

// Function to save an image to a BMP file
void saveImage(const std::string& filename, const std::vector<Pixel>& image, BMPHeader& header) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Error: Could not create the output image file.");
    }

    // Write the BMP header
    file.write(reinterpret_cast<char*>(&header), sizeof(header));

    // Write pixel data
    file.write(reinterpret_cast<const char*>(image.data()), sizeof(Pixel) * header.width * header.height);
}

int main() {
    BMPHeader header;
    std::vector<Pixel> inputImage = loadImage("input1.bmp", header);
    std::vector<Pixel> outputImage1 = inputImage;
    std::vector<Pixel> outputImage2 = inputImage;
    applyHistogramEqualization(outputImage1,1);
    applyHistogramEqualization(outputImage2,2);
    saveImage("output1_1.bmp", outputImage1, header);
    saveImage("output1_2.bmp", outputImage2, header);

    BMPHeader header2;
    std::vector<Pixel> inputImage2 = loadImage("input2.bmp", header2);
    int width = header2.width;
    int height= header2.height;
    std::vector<Pixel> outputImage3 = inputImage2;
    std::vector<Pixel> outputImage4 = inputImage2;
    // Apply the Laplacian filter for sharpness enhancement
    applyLaplacianFilter1(outputImage3, width, height);
    applyLaplacianFilter2(outputImage4, width, height);
    saveImage("output2_1.bmp", outputImage3, header2);
    saveImage("output2_2.bmp", outputImage4, header2);

    BMPHeader header3;
    std::vector<Pixel> inputImage3 = loadImage("input3.bmp", header3);
    int width1 = header3.width;
    int height1= header3.height;
    std::vector<Pixel> outputImage5 = inputImage3;
    std::vector<Pixel> outputImage6 = inputImage3;
    int ker_size;
    applyMedianFilter2(outputImage5, width1, height1);
    applyMedianFilter3(outputImage6, width1, height1);
    saveImage("output3_1.bmp", outputImage5, header3);
    saveImage("output3_2.bmp", outputImage6, header3);

    return 0;
}
