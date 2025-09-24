#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <cmath>
#include <algorithm>
#include "Process_Bmp.h"

// Function to perform white balance adjustment using max-RGB and Grey World methods
void adjustWhiteBalanceMaxRGB1(std::vector<uint8_t>& pixels, int channels) {
    
    // max-RGB method
    uint8_t maxValr;
    uint8_t maxValg;
    uint8_t maxValb;
    for (size_t i = 0; i < pixels.size(); i += channels) {
    	if (maxValr<pixels[i])
    		maxValr = pixels[i];
    	if (maxValg<pixels[i+1])
    		maxValg = pixels[i+1];
    	if (maxValb<pixels[i+2])
    		maxValb = pixels[i+2];
    }
    for (size_t i = 0; i < pixels.size(); i += channels) {
    	pixels[i] = static_cast<uint8_t>(pixels[i] * 255 / maxValr);
    	pixels[i + 1] = static_cast<uint8_t>(pixels[i + 1] * 255 / maxValg);
   		pixels[i + 2] = static_cast<uint8_t>(pixels[i + 2] * 255 / maxValb);
    }
}
// Function to perform white balance adjustment using max-RGB and Grey World methods
void adjustWhiteBalanceMaxRGB2(std::vector<uint8_t>& pixels, int channels) {
    // Grey World method
    double sumR = 0.0, sumG = 0.0, sumB = 0.0;
    size_t pixelCount = pixels.size() / channels;

    // Calculate the sum of R, G, B channels
    for (size_t i = 0; i < pixels.size(); i += channels) {
        sumR += pixels[i];
        sumG += pixels[i + 1];
        sumB += pixels[i + 2];
    }

    // Calculate average values for R, G, B channels
    double avgR = sumR / pixelCount;
    double avgG = sumG / pixelCount;
    double avgB = sumB / pixelCount;

    // // Calculate scaling factors for each channel using Grey World method
    // double scaleR = avgG / avgR;
    // double scaleB = avgG / avgB;

    // // Apply scaling factors from Grey World method
    // for (size_t i = 0; i < pixels.size(); i += channels) {
    //     pixels[i] = static_cast<uint8_t>(pixels[i] * scaleR);
    //     pixels[i + 2] = static_cast<uint8_t>(pixels[i + 2] * scaleB);
    // }

    // max-RGB method
    // uint8_t maxValr;
    // uint8_t maxValg;
    // uint8_t maxValb;
    // for (size_t i = 0; i < pixels.size(); i += channels) {
    // 	if (maxValr<pixels[i])
    // 		maxValr = pixels[i];
    // 	if (maxValg<pixels[i+1])
    // 		maxValg = pixels[i+1];
    // 	if (maxValb<pixels[i+2])
    // 		maxValb = pixels[i+2];
    // }
    for (size_t i = 0; i < pixels.size(); i += channels) {
    	pixels[i] = static_cast<uint8_t>(pixels[i] * 128 / avgR);
    	pixels[i + 1] = static_cast<uint8_t>(pixels[i + 1] * 128 / avgG);
   		pixels[i + 2] = static_cast<uint8_t>(pixels[i + 2] * 128 / avgB);
    }
}

void applyLaplacianFilter2(std::vector<uint8_t>& image, int width, int height) {
    std::vector<uint8_t> resultImage = image; // Create a copy of the image to store the result

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
                    int pixelIndex = ((y + ky) * width + (x + kx))*3;
                    sumR += image[pixelIndex] * kernel[ky + 1][kx + 1];
                    sumG += image[pixelIndex+1] * kernel[ky + 1][kx + 1];
                    sumB += image[pixelIndex+2] * kernel[ky + 1][kx + 1];
                }
            }
            resultImage[(y * width + x)*3] = std::max(0, std::min(255, sumR));
            resultImage[(y * width + x)*3 + 1] = std::max(0, std::min(255, sumG));
            resultImage[(y * width + x)*3 + 2] = std::max(0, std::min(255, sumB));
        }
    }

    image = resultImage; // Replace the original image with the sharpened image

}

// Function to convert RGB to HSV
void rgbToHsv(uint8_t r, uint8_t g, uint8_t b, double& h, double& s, double& v) {
    double R = r / 255.0;
    double G = g / 255.0;
    double B = b / 255.0;

    double Cmax = std::max({ R, G, B });
    double Cmin = std::min({ R, G, B });
    double delta = Cmax - Cmin;

    // Hue calculation
    if (delta == 0)
        h = 0;
    else if (Cmax == R)
        h = 60 * fmod(((G - B) / delta), 6);
    else if (Cmax == G)
        h = 60 * (((B - R) / delta) + 2);
    else
        h = 60 * (((R - G) / delta) + 4);

    // Saturation calculation
    if (Cmax == 0)
        s = 0;
    else
        s = delta / Cmax;

    // Value calculation
    v = Cmax;
}

// Function to convert HSV to RGB
void hsvToRgb(double h, double s, double v, uint8_t& r, uint8_t& g, uint8_t& b) {
    int hi = static_cast<int>(std::floor(h / 60)) % 6;
    double f = h / 60 - std::floor(h / 60);
    double p = v * (1 - s);
    double q = v * (1 - f * s);
    double t = v * (1 - (1 - f) * s);

    switch (hi) {
        case 0:
            r = static_cast<uint8_t>(v * 255);
            g = static_cast<uint8_t>(t * 255);
            b = static_cast<uint8_t>(p * 255);
            break;
        case 1:
            r = static_cast<uint8_t>(q * 255);
            g = static_cast<uint8_t>(v * 255);
            b = static_cast<uint8_t>(p * 255);
            break;
        case 2:
            r = static_cast<uint8_t>(p * 255);
            g = static_cast<uint8_t>(v * 255);
            b = static_cast<uint8_t>(t * 255);
            break;
        case 3:
            r = static_cast<uint8_t>(p * 255);
            g = static_cast<uint8_t>(q * 255);
            b = static_cast<uint8_t>(v * 255);
            break;
        case 4:
            r = static_cast<uint8_t>(t * 255);
            g = static_cast<uint8_t>(p * 255);
            b = static_cast<uint8_t>(v * 255);
            break;
        case 5:
            r = static_cast<uint8_t>(v * 255);
            g = static_cast<uint8_t>(p * 255);
            b = static_cast<uint8_t>(q * 255);
            break;
        default:
            break;
    }
}

// Function to enhance image saturation
void enhanceSaturation(std::vector<uint8_t>& pixels, int channels, double factor) {
    for (size_t i = 0; i < pixels.size(); i += channels) {
        uint8_t r = pixels[i];
        uint8_t g = pixels[i + 1];
        uint8_t b = pixels[i + 2];

        double h, s, v;
        rgbToHsv(r, g, b, h, s, v);

        // Increase saturation by the specified factor
        s *= factor;

        // Clamp saturation value between 0 and 1
        s = std::max(0.0, std::min(s, 1.0));

        hsvToRgb(h, s, v, r, g, b);

        pixels[i] = r;
        pixels[i + 1] = g;
        pixels[i + 2] = b;
    }
}

void applyHistogramEqualization(std::vector<uint8_t>& image) {
    // Calculate histograms for each color channel
    std::vector<int> histogramR(256, 0);
    std::vector<int> histogramG(256, 0);
    std::vector<int> histogramB(256, 0);
    // Normalize cumulative histograms
    for (size_t i = 0; i < image.size(); i += 3) {
        histogramR[image[i]]++;
        histogramG[image[i+1]]++;
        histogramB[image[i+2]]++;
    }
    int totalPixels = image.size()/3;
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
    for (size_t i = 0; i < image.size(); i += 3) {
        image[i] = static_cast<unsigned char>(cumulativeHistR[image[i]]);
        image[i+1] = static_cast<unsigned char>(cumulativeHistG[image[i+1]]);
        image[i+2] = static_cast<unsigned char>(cumulativeHistB[image[i+2]]);
    }
}

int main() {
    ////////////////////////////////////////////////////first image
    BMPHeader header;
    std::vector<uint8_t> imageData;
    if (!readBMP("input1.bmp", header, imageData)) {
        return 1;
    }
    adjustWhiteBalanceMaxRGB1(imageData, 3);
    if (!writeBMP("output1_1.bmp", header, imageData)) {
        return 1;
    }
    int width = header.width;
    int height = header.height;
    applyLaplacianFilter2(imageData, width, height);
    if (!writeBMP("output1_2.bmp", header, imageData)) {
        return 1;
    }

    if (!readBMP("input2.bmp", header, imageData)) {
        return 1;
    }
    adjustWhiteBalanceMaxRGB1(imageData, 3);
    if (!writeBMP("output2_1.bmp", header, imageData)) {
        return 1;
    }
    width = header.width;
    height = header.height;
    applyLaplacianFilter2(imageData, width, height);
    if (!writeBMP("output2_2.bmp", header, imageData)) {
        return 1;
    }

    if (!readBMP("input3.bmp", header, imageData)) {
        return 1;
    }
    adjustWhiteBalanceMaxRGB1(imageData,3);
    if (!writeBMP("output3_1.bmp", header, imageData)) {
        return 1;
    }
    width = header.width;
    height = header.height;
    double saturationFactor = 1.1;
    enhanceSaturation(imageData, 3, saturationFactor);
    if (!writeBMP("output3_2.bmp", header, imageData)) {
        return 1;
    }

    if (!readBMP("input4.bmp", header, imageData)) {
        return 1;
    }
    adjustWhiteBalanceMaxRGB2(imageData, 3);
    if (!writeBMP("output4_1.bmp", header, imageData)) {
        return 1;
    }
    width = header.width;
    height = header.height;
    saturationFactor = 1.1;
    enhanceSaturation(imageData, 3, saturationFactor);
    if (!writeBMP("output4_2.bmp", header, imageData)) {
        return 1;
    }
    
    return 0;
}