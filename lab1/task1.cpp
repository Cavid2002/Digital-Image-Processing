#include "opencv2/opencv.hpp"
#include <stdio.h>
#include <stdlib.h>

using namespace cv;



void addNoise(Mat& res, int num)
{
    int x, y;
    for(int i = 0; i < num; i++)
    {
        x = rand() % res.cols;
        y = rand() % res.rows;

        res.at<Vec3b>(y,x)[i % 3] = 255;
    }
}



int main(int argc, char* argv[])
{
    fprintf(stdout, "Enter the number of pixels: ");
    int num; fscanf(stdin, "%d", &num);
    Mat img = imread("./imgs/img1.jpg");
    Mat resImg;
    
    img.copyTo(resImg);

    addNoise(resImg, num);

    imshow("Original image", img);
    imshow("Salted image",resImg);
    imwrite("./imgs/img1-res.png", resImg);
    

    waitKey(0);
    destroyAllWindows();
    return 0;
}