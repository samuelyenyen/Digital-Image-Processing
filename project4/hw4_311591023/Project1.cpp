#include <iostream>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <fstream>
using namespace cv;
using namespace std;



void getPSF(Mat& outputImg, Size filterSize, int length, double theta)
{
    Mat PSF(filterSize, CV_32F, Scalar(0));
    int width = filterSize.width;
    int height = filterSize.height;
    Point point(width / 2, height / 2);
    ellipse(PSF, point, Size(0, cvRound(float(length) / 2.0)), 90.0 - theta, 0, 360, Scalar(255,0,0), FILLED);
    Scalar summa = sum(PSF);
    outputImg = PSF / summa[0];
}
void fftshift(const Mat& inputImg, Mat& outputImg)
{
    outputImg = inputImg.clone();
    int cx = outputImg.cols / 2;
    int cy = outputImg.rows / 2;

    Mat temp;
    Mat d0(outputImg, Rect(0, 0, cx, cy));
    Mat d1(outputImg, Rect(cx, 0, cx, cy));
    Mat d2(outputImg, Rect(0, cy, cx, cy));
    Mat d3(outputImg, Rect(cx, cy, cx, cy));
    /*temp = d0;
    d0 = d3;
    d3 = temp;
    temp = d1;
    d1 = d2;
    d2 = temp;*/
    d0.copyTo(temp);
    d3.copyTo(d0);
    temp.copyTo(d3);
    d1.copyTo(temp);
    d2.copyTo(d1);
    temp.copyTo(d2);
}
void filter2d_infreq(const Mat& inputImage, Mat& outputImage, const Mat& PSF)
{
    Mat cmx[2];
    cmx[0] = Mat_<float>(inputImage.clone());
    cmx[1] = Mat::zeros(inputImage.size(), CV_32F);
    Mat complexI;
    merge(cmx, 2, complexI);
    dft(complexI, complexI, DFT_SCALE);
    Mat cmxH[2];
    cmxH[0] = Mat_<float>(PSF.clone());
    cmxH[1] = Mat::zeros(PSF.size(), CV_32F);
    Mat complexH;
    merge(cmxH, 2, complexH);
    Mat complexIH;
    mulSpectrums(complexI, complexH, complexIH, 0);
    idft(complexIH, complexIH);
    
    Mat output[2];
    split(complexIH, output);
    outputImage = output[0];
}
void getWFilter(const Mat& PSF, double nsr, Mat& output)
{
    Mat PSF_shifted;
    fftshift(PSF, PSF_shifted);
    Mat cmx[2];
    cmx[0] = Mat_<float>(PSF_shifted.clone());
    cmx[1] = Mat::zeros(PSF_shifted.size(), CV_32F);

    Mat complexI;
    merge(cmx, 2, complexI);
    dft(complexI, complexI);
    split(complexI, cmx);
    Mat denom;
    pow(abs(cmx[0]), 2, denom);
    denom += nsr;
    divide(cmx[0], denom, output);
}


int image_restoration(const string& fileName, double SRN, int Length, double Theta, Mat& mer_imgOut)
{

    Mat imgIn;
    imgIn = imread(fileName, IMREAD_COLOR);

    Mat channels[3];
    split(imgIn, channels);

    Mat imgOut[3];
    for (int i = 0; i < 3; ++i) {
        Mat WF, PSF;
        //calculate PSF
        getPSF(PSF, channels[i].size(), Length, Theta);
        getWFilter(PSF, SRN, WF);
        //Do the filtering
        channels[i].convertTo(channels[i], CV_32F);
        filter2d_infreq(channels[i], imgOut[i], WF);
        imgOut[i].convertTo(imgOut[i], CV_8U);
    }


    merge(imgOut, 3, mer_imgOut);

    return true;
}


int main(int argc, char* argv[])
{   
    double SRN = 0.09;
    int Length = 31;
    int Theta = 45;
   
    //first image
    Mat output1;
    image_restoration("input1.bmp", SRN, Length, Theta, output1);
    imwrite("output1.bmp", output1);
    imshow("output1", output1);

    //second image
    Mat output2;
    image_restoration("input2.bmp", SRN, Length, Theta, output2);
    imwrite("output2.bmp", output2);
    waitKey(0);

    return 0;
}