#include "opencv2/opencv.hpp"
#include <stdio.h>
#include <stdlib.h>

#define BLUE 0
#define GREEN 1
#define RED 2

using namespace cv;

unsigned int r_count = 0;
unsigned int g_count = 0;
unsigned int b_count = 0;

void readChannel(Vec3b& pixel)
{
    if(pixel[RED] == 255 && pixel[GREEN] != 255 && pixel[BLUE] != 255){
        r_count++;
    } 
    else if(pixel[GREEN] == 255 && pixel[RED] != 255 && pixel[BLUE] != 255)
    {
        g_count++;
    }
    else if(pixel[BLUE] == 255 && pixel[GREEN] != 255 && pixel[RED] != 255)
    {
        b_count++;
    }
}

void readRGBCount(Mat& img)
{
    for(int i = 0; i < img.rows; i++)
    {
        for(int j = 0; j < img.cols; j++)
        {
            readChannel(img.at<Vec3b>(i,j));
        }
    }
}

int main(int argc, char* argv[])
{
    Mat img = imread("./imgs/img1-res.png");
    imshow("Image", img);
    readRGBCount(img);

    fprintf(stdout, "Red pixels:%d\n", r_count);
    fprintf(stdout, "Green pixels:%d\n", g_count);
    fprintf(stdout, "Blue pixels:%d\n", b_count);
    
    waitKey(0);

    return 0;


    
}