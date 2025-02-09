#include<stdlib.h>
#include<stdio.h>
#include<math.h>

#include "opencv2/opencv.hpp"

using namespace cv;


Mat copyImg(Mat& src)
{
    Mat dst(src.size(), CV_8UC1);

    uchar* srcData = src.data;
    uchar* dstData = dst.data;

    for(int i = 0; i <  src.rows; i++)
    {
        for(int j = 0; j < src.cols; j++)
        {
            dstData[i * src.cols + j] = srcData[i * src.cols + j];
        }
    }

    return dst;
}


int calc_convolution(uchar* data, int width, int height, int x, int y, double* kernel, int size)
{
    int start = size / 2;
    double res = 0;
    for(int i = -start; i <= start; i++)
    {
        for(int j = -start; j <= start; j++)
        {
            if(y + i < height && y + i >= 0 && x + j >= 0 && x + j < width)
            {
                res += data[(y + i) * width + (x + j)] * kernel[(i + start) * size + (j + start)]; 
            }
        }
    }

    return res;
}

void apply_sobel_mask(Mat& src, Mat& dst, double* kernel, int size)
{
    uchar* srcData = src.data;
    uchar* dstData = dst.data;

    for(int i = 0; i < src.rows; i++)
    {
        for(int j = 0; j < src.cols; j++)
        {
            dstData[i * src.cols + j] = abs(calc_convolution(srcData, src.cols, src.rows, j, i, kernel, size));
        }
    }
}

void combine_results(Mat& img1, Mat& img2, Mat& res)
{
    uchar* img1Data = img1.data;
    uchar* img2Data = img2.data;
    uchar* resData = res.data;

    uchar img1ch;
    uchar img2ch;
    

    for(int i = 0; i < img1.rows; i++)
    {
        for(int j = 0; j < img1.cols; j++)
        {
            img1ch = img1Data[i * img1.cols + j];
            img2ch = img2Data[i * img2.cols + j];
            resData[i * img1.cols + j] = sqrt(img1ch * img1ch + img2ch * img2ch);
        }
    }

}


int main()
{
    Mat img = imread("./imgs/gear.jpg", 0);
    double sobelKernel_45[] = {
        -2.0, -1.0 , 0.0,
        -1.0,  0.0 ,  1.0,
        0.0, 1.0, 2.0
    };

    double sobelKernel_135[] = {
        0.0, 1.0 , 2.0,
        -1.0,  0.0 , 1.0,
        -2.0,  -1.0, 0.0
    };

    Mat dst_45 = copyImg(img);
    Mat dst_135 = copyImg(img);
    Mat dst_com = copyImg(img);

    apply_sobel_mask(img, dst_45, sobelKernel_45, 3);
    apply_sobel_mask(img, dst_135, sobelKernel_135, 3);
    combine_results(dst_45, dst_135, dst_com);    

    imshow("Sobel Filter 45 degree", dst_45);
    imshow("Sobel Filter 135 degree", dst_135);
    imshow("Combined 135 and 45 degree filters", dst_com);

    waitKey(0);

}