#include "opencv2/opencv.hpp"

using namespace cv;


double bilateral_filter(uchar* data, int x, int y, int width, int height, double sigma_i, double sigma_s, int size)
{
    double iFiltered = 0;
    double wp = 0, res = 0, total = 0;
    int start = size / 2;
    double gi, gs;
    int dif;
    for(int i = -start; i <= start; i++)
    {
        for(int j = -start; j <= start; j++)
        {
            if(x + j < width && x + j > 0 && y + i < height && y + i > 0)
            {  
                gs = exp(-(i * i + j * j) / (2.0 * sigma_s * sigma_s)) / (2 * CV_PI * sigma_s * sigma_s); 
                dif = data[y * width + x] - data[(y + i) * width + (x + j)];
                gi = exp(-(dif * dif) / (2.0 * sigma_i * sigma_i)) / (2 * CV_PI * sigma_i * sigma_i);
                wp = gi * gs;
                res += data[(y + i) * width + (x + j)] * wp;
                total += wp;
            }
        }
    }
    return res / total;
}


void apply_bilateral_filter(Mat& src, Mat& dst, double sigma_s, double sigma_i, int size)
{
    uchar* srcData = src.data;
    uchar* dstData = dst.data;
    for(int i = 0; i < src.rows; i++)
    {
        for(int j = 0; j < src.cols; j++)
        {
            dstData[i * src.cols + j] = bilateral_filter(srcData, j, i, src.cols, src.rows, sigma_i, sigma_s, size);
        }
    }
}



int main()
{
    Mat img = imread("./imgs/rock.png", 0);
    resize(img, img, Size(img.cols / 2, img.rows / 2));
    Mat dst1(img.size(), img.type());
    Mat dst2(img.size(), img.type());

    double sigma_s[] = { 8, 40, 120 };
    double sigma_r[] = { 5, 30, 100 };
    double kernel[25];
    int size = sizeof(sigma_s) / sizeof(double);
    Mat resultGrid(img.rows * size, img.cols * size, img.type());
    for(int i = 0; i < size; i++)
    {
        for(int j = 0; j < size; j++)
        {
            apply_bilateral_filter(img, dst1, sigma_s[j], sigma_r[i], 5);
            dst1.copyTo(resultGrid(Rect(j * img.cols, i * img.rows, img.cols, img.rows)));
        }

    }

    imshow("Original", img);
    imshow("GRID", resultGrid);
    
    waitKey(0);
}