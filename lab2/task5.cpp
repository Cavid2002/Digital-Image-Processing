#include<stdlib.h>
#include<stdio.h>
#include<math.h>

#include "opencv2/opencv.hpp"

using namespace cv;


Mat copyImg(Mat& src)
{
    Mat dst(src.size(), src.type());

    uchar* srcData = src.data;
    uchar* dstData = dst.data;

    for(int i = 0; i <  src.rows; i++)
    {
        for(int j = 0; j < src.cols; j++)
        {
            for(int k = 0; k < 3; k++){
                dstData[(i * src.cols + j)* 3 + k] = srcData[(i * src.cols + j)* 3 + k];
            }
        }
    }

    return dst;
}


int calc_convolution(uchar* data, int width, int height, int x, int y, int channel, double* kernel, int size)
{
    int start = size / 2;
    double res = 0;
    for(int i = -start; i <= start; i++)
    {
        for(int j = -start; j <= start; j++)
        {
            if(y + i < height && y + i >= 0 && x + j >= 0 && x + j < width)
            {
                res += data[((y + i) * width + (x + j)) * 3 + channel] * kernel[(i + start) * size + (j + start)];
            }
        }
    }

    return res;
}

void apply_mask(Mat& src, Mat& dst, double* kernel, int size)
{
    uchar* srcData = src.data;
    uchar* dstData = dst.data;

    int height = src.rows;
    int width = src.cols;

    for(int i = 0; i < height; i++)
    {
        for(int j = 0; j < width; j++)
        {
            for(int k = 0; k < 3; k++)
            {
                dstData[(i * width + j)* 3 + k] = calc_convolution(srcData, width, height, j, i, k, kernel, size);
            }
        }
    }
}


void create_gaussian_filter(double* kernel,int size, double sigma)
{
    double dig1 = 2.0 * sigma * sigma * M_PI; 
    double dig2;
    int start = size / 2;

    double sum = 0.0; 
  
    for (int i = -start; i <= start; i++) { 
        for (int j = -start; j <= start; j++) {  
            dig2 = (-i * i - j * j) / (2 * sigma * sigma);
            kernel[(i + start) * size + (j + start)] = dig1 * exp(dig2); 
            sum += kernel[(i + start) * size + (j + start)]; 
        } 
    } 

    for (int i = 0; i < size * size; ++i)
    {
        kernel[i] /= sum;
    } 
    fprintf(stdout, "%f\n", sum);

    for (int i = -start; i <= start; i++)
    {
        for (int j = -start; j <= start; j++)
        {
            printf("%f ", kernel[(i + start) * size + (j + start)]);
        } 
        printf("\n");
    } 
    
}

Mat downsample(Mat& src)
{
    int width = src.cols / 2;
    int height = src.rows / 2;
    Mat dst(height, width, src.type());

    uchar* srcData = src.data;
    uchar* dstData = dst.data;

    for(int i = 0; i < height; i++)
    {
        for(int j = 0; j < width; j++)
        {
            for(int k = 0; k < 3; k++)
            {
                dstData[(i * width + j) * 3 + k] = srcData[((i * 2) * (width * 2) + (j * 2)) * 3 + k];
            }
        }
    }

    return dst;
}

void apply_gaussianPyramid(Mat& src, Mat* arr, int size)
{
    arr[0] = copyImg(src);
    

    double kernel[25];

    create_gaussian_filter(kernel, 5, 10);

    apply_mask(src, arr[0], kernel, 5);
    
    for(int i = 1; i < size; i++)
    {
        Mat dst(arr[i - 1].size(), arr[i - 1].type());
        apply_mask(arr[i - 1], dst, kernel, 5);
        arr[i] = downsample(dst);
    }

}

int main()
{
    Mat img = imread("./imgs/gear.jpg", 1);
    
    int levels = 4;

    Mat arr[levels];

    apply_gaussianPyramid(img, arr, levels);

    for (int i = 0; i < levels; i++) {
        imshow("Gaussian " + std::to_string(i), arr[i]);
    }


    waitKey(0);

    return 0;
}