#include<stdlib.h>
#include<stdio.h>
#include<math.h>

#include "opencv2/opencv.hpp"

using namespace cv;


void generate_histogram(Mat& image, Mat& histogram, Mat& histImage)
{
    int histSize = 256; 
    float range[] = {0.0, 255.0}; 
    const float* histRange = {range};
    calcHist(&image, 1, 0, Mat(), histogram, 1, &histSize, &histRange, true, false);

    int histWidth = 512;
    int histHeight = 400;
    int binWidth = cvRound(static_cast<double>(histWidth) / histSize);
    Mat temp(histHeight, histWidth, CV_8UC1, Scalar(0));

    temp.copyTo(histImage);

    normalize(histogram, histogram, 0, histImage.rows, NORM_MINMAX, -1, Mat());

    for (int i = 1; i < histSize; i++) {
        line(histImage, 
                Point(binWidth * (i - 1), 
                histHeight - cvRound(histogram.at<float>(i - 1))),
                Point(binWidth * i, histHeight - cvRound(histogram.at<float>(i))),
                Scalar(255), 2, 8, 0);
    }

}  




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
    Mat histogram, histImg;
    Mat histogram_blur, histImg_blur;

    Mat dst = copyImg(img);

    double gaussFilter[81];

    create_gaussian_filter(gaussFilter, 9, 20);

    apply_mask(dst, gaussFilter, 9);

    generate_histogram(img, histogram, histImg);
    generate_histogram(dst, histogram_blur, histImg_blur);

    imshow("Original", img);
    imshow("Original Histogram", histImg);
    imshow("Blured", dst);
    imshow("Blured Histogram", histImg_blur);


    waitKey(0);  
}