#include "opencv2/opencv.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

using namespace cv;


void add_letters(Mat& img1,Mat& img2)
{
    int ycenter = img1.rows / 2;
    int xcenter = img1.cols / 2;
    
    int starty = ycenter - img2.rows / 2 + 100;
    int startx = xcenter - img2.cols / 2;

    for(int i = 0; i < img2.rows; i++)
    {
        for(int j = 0; j < img2.cols; j++)
        {
            if(img2.at<Vec3b>(i , j)[0] < 200 && img2.at<Vec3b>(i , j)[1] < 200 && img2.at<Vec3b>(i , j)[2] < 200)
            {
                img1.at<Vec3b>(starty + i, startx + j)[0] = img2.at<Vec3b>(i , j)[0];
                img1.at<Vec3b>(starty + i, startx + j)[1] = img2.at<Vec3b>(i , j)[1];
                img1.at<Vec3b>(starty + i, startx + j)[2] = img2.at<Vec3b>(i , j)[2];
            }
        }
    }
}

int main()
{
    Mat image3 = imread("./imgs/img3.jpg", 1);
    Mat image4 = imread("./imgs/img4.jpg", 1);
    Mat image5 = imread("./imgs/img5.jpg", 1);
    

    Mat dst1;
    resize(image4, image4, Size(image3.cols, image3.rows));
    subtract(image3, image4, dst1);

    add_letters(dst1, image5);

    imshow("", dst1);
    // imshow("", image5);
    waitKey(0);

    return 0;
}