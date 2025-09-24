#include "Image.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <charconv>

// 计算3x3子矩阵的行列式
int determinant3x3(const std::vector<std::vector<int>>& matrix) {
    return matrix[0][0] * (matrix[1][1] * matrix[2][2] - matrix[1][2] * matrix[2][1]) -
           matrix[0][1] * (matrix[1][0] * matrix[2][2] - matrix[1][2] * matrix[2][0]) +
           matrix[0][2] * (matrix[1][0] * matrix[2][1] - matrix[1][1] * matrix[2][0]);
}

// 计算4x4矩阵的行列式
int determinant4x4(const std::vector<std::vector<int>>& matrix) {
    int det = 0.0;
    for (int i = 0; i < 4; i++) {
        std::vector<std::vector<int>> submatrix(3, std::vector<int>(3, 0.0));
        for (int j = 0; j < 3; j++) {
            int col = 0;
            for (int k = 0; k < 4; k++) {
                if (k != i) {
                    submatrix[j][col] = matrix[j + 1][k];
                    col++;
                }
            }
        }
        int subdet = determinant3x3(submatrix);
        if (i % 2 == 0) {
            det += matrix[0][i] * subdet;
        } else {
            det -= matrix[0][i] * subdet;
        }
    }
    return det;
}

// 使用Cramer's Rule 解线性方程组 Ax = b
std::vector<float> cramer(const std::vector<std::vector<int>>& A, const std::vector<int>& b, int detA) {
    int n = A.size();
    std::vector<float> x(n);
    // std::cout << detA << std::endl;


    for (int i = 0; i < n; i++) {
        // 创建一个临时矩阵，用于计算 x[i]
        std::vector<std::vector<int>> tempA = A;

        for (int j = 0; j < n; j++) {
            tempA[j][i] = b[j];
        }
        // std::cout << determinant4x4(tempA) << std::endl;
        // 计算 x[i]
        x[i] = determinant4x4(tempA) / detA;
    }

    return x;
}


// Function to perform bilinear interpolation
uint8_t interpolate(std::vector<std::vector<int>> A, int detA, uint8_t f0, uint8_t f1, uint8_t f2, uint8_t f3, float oldX, float oldY) {
    

    // 定义右侧向量 b (C)
    std::vector<int> b = {f0, f1, f2, f3};
    // std::cout << b[0] << std::endl;
    // std::cout << b[1] << std::endl;
    // std::cout << b[2] << std::endl;
    // std::cout << b[3] << std::endl;
    std::vector<float> solution = cramer(A, b, detA);

    // 输出解
    // std::cout << "Solution for B:" << std::endl;
    // for (int i = 0; i < solution.size(); i++) 
        // std::cout << "B" << i << " = " << solution[i] << std::endl;
    // std::cout << x1 << std::endl;
    // std::cout << x2 << std::endl;
    // std::cout << y1 << std::endl;
    // std::cout << y2 << std::endl;
    return static_cast<uint8_t>(solution[0] + solution[1] * oldX + solution[2] * oldY + solution[3] * oldX * oldY);
}

void flipX(std::vector<uint8_t>& image, int width, int height, int channels) {
    int start;
    int end;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width/2; ++x) {
            start = (x + y * width) * channels;
            end = ((width - 1 - x) + y * width) * channels;
            for (int i = 0; i < channels; ++i) {
                std::swap(image[start+i],image[end+i]);
            }
        }
    }
}

void setResol(std::vector<uint8_t>& image, int width, int height, int channels, int color_bit) {
    int pix_addr;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            for (int i = 0; i < channels; ++i) {
                pix_addr = ((x + y*width)*channels) + i;
                uint8_t pixelValue = image[pix_addr];
                if (color_bit==6){
                    uint8_t twoBitValue = pixelValue >> 2;
                    image[pix_addr] = twoBitValue <<2;
                }
                else if (color_bit==4){
                    uint8_t twoBitValue = pixelValue >> 4;
                    image[pix_addr] = twoBitValue <<4;
                }
                else if (color_bit==2){
                    uint8_t twoBitValue = pixelValue >> 6;
                    image[pix_addr] = twoBitValue <<6;
                }
            }
        }
    }
}

void reScale(std::vector<uint8_t>& image, BMPHeader& header, int width, int height, int channel, BMPHeader& newHeader, std::vector<uint8_t>& newData, float scale, bool zoom) {
    if (zoom) {
        // Calculate new dimensions
        // std::ifstream inputFile("input1.bmp", std::ios::binary);
        // std::cout <<width <<std::endl;
        // std::cout <<height <<std::endl;
        int newWidth = static_cast<int>(width * scale);
        int newHeight = static_cast<int>(height * scale);
        // Create a new BMP header for the resized image
        newHeader = header;
        newHeader.width = newWidth;
        newHeader.height = newHeight;
        int imageSize = newWidth * newHeight * channel;
        newHeader.fileSize = sizeof(BMPHeader) + imageSize;
        // std::cout <<newHeader.width <<std::endl;
        // std::cout <<newHeader.height <<std::endl;

        // std::cout <<header.imageSize <<std::endl;
        // std::cout <<newHeader.imageSize <<std::endl;
        // Create a vector to store the new image data
        newData.resize(imageSize);
        // Calculate scaling factors
        // float xScale = static_cast<float>(header.width) / newWidth;
        // float yScale = static_cast<float>(header.height) / newHeight;
        for (int y = 0; y < newHeight; ++y) {
            for (int x = 0; x < newWidth; ++x) {
                float oldX = static_cast<float>(x) / scale;
                float oldY = static_cast<float>(y) / scale;

                int x1 = static_cast<int>(floor(oldX));
                int x2 = x1 + 1;
                int y1 = static_cast<int>(floor(oldY));
                int y2 = y1 + 1 ;

                int newIndex = (y * newWidth + x) * channel;

                // Interpolate pixel values using bilinear interpolation
                for (int channels = 0; channels < channel; ++channels) {
                    uint8_t f0 = image[(y1 * header.width + x1) * channel + channels];
                    uint8_t f1 = image[(y2 * header.width + x1) * channel + channels];
                    uint8_t f2 = image[(y1 * header.width + x2) * channel + channels];
                    uint8_t f3 = image[(y2 * header.width + x2) * channel + channels];

                    // 定义系数矩阵 A
                    std::vector<std::vector<int>> A = {
                        {1, x1, y1, x1 * y1},
                        {1, x1, y2, x1 * y2},
                        {1, x2, y1, x2 * y1},
                        {1, x2, y2, x2 * y2}
                    };
                    int detA = determinant4x4(A);
                    if (std::abs(detA) < 1e-10) {
                        newData[newIndex + channels] = f0;
                    }
                    else {
                        newData[newIndex + channels] = interpolate(A, detA, f0, f1, f2, f3, oldX, oldY);
                    }
                }
            }
        }
    }
    else {
        int newWidth = ((static_cast<int>(width / scale))>>2)<<2;
        int newHeight = static_cast<int>(height / scale);
        // Create a new BMP header for the resized image
        newHeader = header;
        newHeader.width = newWidth;
        newHeader.height = newHeight;
        int imageSize = newWidth * newHeight * channel;
        newHeader.fileSize = sizeof(BMPHeader) + imageSize;
        // std::cout <<width <<std::endl;
        // std::cout <<height <<std::endl;
        // std::cout <<newHeader.width <<std::endl;
        // std::cout <<newHeader.height <<std::endl;

        // std::cout <<header.imageSize <<std::endl;
        // std::cout <<newHeader.imageSize <<std::endl;
        // Create a vector to store the new image data
        newData.resize(imageSize);
        for (int y = 0; y < newHeader.height; ++y) {
            for (int x = 0; x < newHeader.width; ++x) {
                float oldX = static_cast<float>(x) * scale;
                float oldY = static_cast<float>(y) * scale;

                int x1 = static_cast<int>(floor(oldX));
                int x2 = x1 + 1;
                int y1 = static_cast<int>(floor(oldY));
                int y2 = y1 + 1 ;

                int newIndex = (y * newWidth + x) * channel;

                // Interpolate pixel values using bilinear interpolation
                for (int channels = 0; channels < channel; ++channels) {
                    uint8_t f0 = image[(y1 * header.width + x1) * channel + channels];
                    uint8_t f1 = image[(y2 * header.width + x1) * channel + channels];
                    uint8_t f2 = image[(y1 * header.width + x2) * channel + channels];
                    uint8_t f3 = image[(y2 * header.width + x2) * channel + channels];

                    // 定义系数矩阵 A
                    std::vector<std::vector<int>> A = {
                        {1, x1, y1, x1 * y1},
                        {1, x1, y2, x1 * y2},
                        {1, x2, y1, x2 * y1},
                        {1, x2, y2, x2 * y2}
                    };
                    int detA = determinant4x4(A);
                    if (std::abs(detA) < 1e-10) {
                        newData[newIndex + channels] = f0;
                    }
                    else {
                        newData[newIndex + channels] = interpolate(A, detA, f0, f1, f2, f3, oldX, oldY);
                    }
                }
            }
        }
    }
}

