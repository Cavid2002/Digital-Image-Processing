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

Mat upsample(Mat& src)
{
    int width = src.cols * 2;
    int height = src.rows * 2;
    Mat dst(height, width, src.type());

    uchar* srcData = src.data;
    uchar* dstData = dst.data;

    for(int i = 0; i < height; i++)
    {
        for(int j = 0; j < width; j++)
        {
            for(int k = 0; k < 3; k++)
            {
                dstData[(i * width + j) * 3 + k] = srcData[((i / 2) * (width / 2) + (j / 2)) * 3 + k];
            }
        }
    }

    return dst;
}

void apply_gaussianPyramid(Mat& src, Mat* arr, int size)
{
    arr[0] = copyImg(src);
    double kernel[25];

    create_gaussian_filter(kernel, 5, 5);

    apply_mask(src, arr[0], kernel, 5);
    
    for(int i = 1; i < size; i++)
    {
        Mat dst(arr[i - 1].size(), arr[i - 1].type());
        apply_mask(arr[i - 1], dst, kernel, 5);
        arr[i] = downsample(dst);
    }

}

Mat subtract_data(Mat img1,Mat img2)
{
    Mat dst(img1.size(), img1.type());
    
    uchar* imgData = img1.data;
    uchar* img2Data = img2.data;
    uchar* dstData = dst.data;
    int width = img1.cols;
    int height = img2.rows;

    for(int i = 0; i < height; i++)
    {
        for(int j = 0; j < width; j++)
        {
            for(int k = 0; k < 3; k++)
            {
                
                dstData[(i * width + j) * 3 + k] = imgData[(i * width + j) * 3 + k] - img2Data[(i * width + j) * 3 + k];
            }
        }
    }

    return dst;
}

void apply_laplacianPyramid(Mat& src,Mat* dest,Mat* gaussianPyramid, int size)
{

    for(int i = 1; i < size; i++)
    {       
        Mat temp = upsample(gaussianPyramid[i]);
        dest[i - 1] = subtract_data(gaussianPyramid[i - 1], temp);
    }
}

int main()
{
    int level = 5;
    Mat img = imread("./imgs/gear.jpg");
    Mat arr[level];
    apply_gaussianPyramid(img, arr, level);
    Mat arr2[level];
    apply_laplacianPyramid(img, arr2, arr, level);

    for (int i = 0; i < level; i++) {
        imshow("Gaussian " + std::to_string(i), arr[i]);
    }

    for(int i = 0; i < level - 1; i++)
    {
        imshow("Laplacian " + std::to_string(i), arr2[i]);
    }
    
    waitKey(0);

}
