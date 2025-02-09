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


void addNoise(Mat& res, int num)
{
    int x, y;
    for(int i = 0; i < num; i++)
    {
        x = rand() % res.cols;
        y = rand() % res.rows;

        res.at<uchar>(y,x) = 255;
    }
}


int calc_convolution(uchar* data, int width, int height, int x, int y, double* kernel, int size)
{
    int start = size / 2;
    double res = 0;
    double ksum = 0;
    for(int i = -start; i <= start; i++)
    {
        for(int j = -start; j <= start; j++)
        {
            if(y + i < height && y + i >= 0 && x + j >= 0 && x + j < width)
            {
                res += data[(y + i) * width + (x + j)] * kernel[(i + start) * size + (j + start)]; 
                ksum += kernel[(i + start) * size + (j + start)];
            }
        }
    }

    return res;
}

void apply_mask(Mat& src, double* kernel, int size)
{
    uchar* srcData = src.data;

    for(int i = 0; i < src.rows; i++)
    {
        for(int j = 0; j < src.cols; j++)
        {
            srcData[i * src.cols + j] = calc_convolution(srcData, src.cols, src.rows, j, i, kernel, size);  
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




int main()
{
    Mat img = imread("./imgs/gear.jpg", 0);
    Mat dst = copyImg(img);
    
    addNoise(dst, 5000);

    Mat dst2 = copyImg(dst);

    double kernel[81];

    create_gaussian_filter(kernel, 9, 10);

    apply_mask(dst2, kernel, 9);

    imshow("Original Image", img);
    imshow("Noised Image", dst);
    imshow("Gaussian Filter Applied to Noise", dst2);

    waitKey(0);
}