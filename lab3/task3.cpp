#include "opencv2/opencv.hpp"
#include <time.h>

using namespace cv;


void create_gaussian_filter(double* kernel,int size, double sigma)
{
    double dig1 = (2.0 * sigma * sigma * M_PI); 
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

void apply_mask(Mat& src, Mat& dst, double* kernel, int size)
{
    uchar* srcData = src.data;
    uchar* dstData = dst.data;
    for(int i = 0; i < src.rows; i++)
    {
        for(int j = 0; j < src.cols; j++)
        {
            dstData[i * src.cols + j] = calc_convolution(srcData, src.cols, src.rows, j, i, kernel, size);  
        }
    }
}

void apply_edge_detection(Mat& src, Mat& dst, double* kernel, int size)
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

void apply_gradient(Mat& vertical, Mat& horizontal, Mat& dst)
{
    uchar* verData = vertical.data;
    uchar* horData = horizontal.data;
    uchar* dstData = dst.data;
    uchar verch, horch;
    for(int i = 0; i < vertical.rows; i++)
    {
        for(int j = 0; j < vertical.cols; j++)
        {
            verch = verData[i * vertical.cols + j];
            horch = horData[i * vertical.cols + j];
            dstData[i * vertical.cols + j] = sqrt(verch * verch + horch * horch);
        }
    }
}

void sobel_gradient(Mat& img, Mat& res)
{
    Mat dst; img.copyTo(dst);
    double kernel[25];
    create_gaussian_filter(kernel, 5, 2);
    // apply_mask(img, dst, kernel, 5);
    
    Mat vert(dst.size(), dst.type());
    Mat horz(dst.size(), dst.type());
    Mat grad(dst.size(), dst.type());
    double vertKernel[9] = {
        1, 2, 1,
        0, 0, 0,
        -1, -2, -1
    };

    double horzKernel[9] = {
        -1, 0, 1,
        -2, 0, 2,
        -1, 0, 1
    };

    apply_edge_detection(dst, vert, vertKernel, 3);
    apply_edge_detection(dst, horz, horzKernel, 3);

    apply_gradient(vert, horz, res);
}

int main()
{
    Mat img = imread("./imgs/edge_test.jpg", IMREAD_GRAYSCALE);
    //resize(img, img, Size(img.cols / 2, img.rows / 2));
    Mat dst(img.size(), img.type());
    Mat dst1(img.size(), img.type());
    sobel_gradient(img, dst1);
    
    
    double threshold1[] = { 120, 150, 180 };
    double threshold2[] = { 200, 220, 240 };
    size_t size = sizeof(threshold1) / sizeof(double);
    double duration;

    Mat resultGrid(img.rows * size, img.cols * size, img.type());
    for(int i = 0; i < size; i++)
    {
        for(int j = 0; j < size; j++)
        {
            clock_t str = clock();
            Canny(img, dst, threshold1[j], threshold2[i]);
            clock_t fin = clock();
            duration = ((double)(fin - str)) / CLOCKS_PER_SEC;
            dst.copyTo(resultGrid(Rect(j * img.cols, i * img.rows, img.cols, img.rows)));
            
            printf("[INFO]Time-duration for thershold1 = %f thershold2 = %f: %f\n", threshold1[j], threshold2[i], duration);
        }
    }
    
    
    imshow("Sobel", dst1);
    imshow("Canny table for different threshold", resultGrid);
    imshow("Canny", dst);
    waitKey(0);
    return 0;   
}