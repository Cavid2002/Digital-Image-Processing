#include "opencv2/opencv.hpp"
#include <string.h>

using namespace cv;

void swap(int& a, int& b)
{
    int temp = a;
    a = b;
    b = temp;
}

int sort_n_find(uchar* data, int x, int y, int width, int height, int size)
{
    int start = size / 2;
    int arr_size = size * size - 1;
    int arr[arr_size];
    memset(arr, 0, arr_size);
    for(int i = -start; i <= start; i++)
    {
        for(int j = -start; j <= start; j++)
        {
            if(i == 0 && j == 0) continue;
            arr[(i + start) * size + (j + start)] = data[(y + j) * width + (x + i)];
        }
    } 

    //selection sort
    for(int i = 0; i < arr_size ; i++)
    {
        int min_index = i;
        for(int j = i + 1; j < arr_size; j++)
        {
            if(arr[min_index] > arr[j])
            {
                min_index = j;
            }
        }
        swap(arr[min_index], arr[i]);
    }
    return arr[arr_size / 2];
}

void apply_median(Mat& src, Mat& dst, int size)
{
    uchar* dstData = dst.data;
    uchar* srcData = src.data;
    int width = src.cols;
    int height = src.rows;

    for(int i = (size / 2); i < height - (size / 2); i++)
    {
        for(int j = (size / 2); j < width - (size / 2); j++)
        {
            dstData[i * width + j] = sort_n_find(srcData, j, i, width, height, size);
        }
    }
}

int main()
{
    Mat img = imread("./imgs/salt_pepper.png", 0);
    
    Mat dst1(img.size(), img.type());
    Mat dst2(img.size(), img.type());

    apply_median(img, dst1, 3);
    apply_median(img, dst2, 5);
    imshow("Original", img);
    imshow("Result 3x3", dst1);
    imshow("Result 5x5", dst2);
    waitKey(0);

}