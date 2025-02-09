#include "opencv2/opencv.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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


void make_darker_bottom_top(Mat& src)
{
    for(int i = 0; i < src.rows; i++)
    {
        double scale = i / (double)src.rows;
        for(int j = 0; j < src.cols; j++)
        {
            src.at<uchar>(i, j) = (unsigned char)(round(double(src.at<uchar>(i, j)) * scale * scale)); 
        }
    }
}

void make_darker_top_bottom(Mat& src)
{
    for(int i = 0; i < src.rows; i++)
    {
        double scale = i / (double)src.rows;
        scale = 1 - scale;
        for(int j = 0; j < src.cols; j++)
        {
            src.at<uchar>(i, j) = (unsigned char)(round(double(src.at<uchar>(i, j)) * scale * scale)); 
        }
    }
}


int main(int argc, char* argv[])
{
    Mat image = imread("./imgs/img2.jpg", IMREAD_GRAYSCALE);
    
    Mat dark_img1, dark_img2;
    
    image.copyTo(dark_img1);
    image.copyTo(dark_img2);
    make_darker_bottom_top(dark_img1);
    make_darker_top_bottom(dark_img2);

    
    // subtract(image, Scalar(100, 100, 100), dark_img1);
    // subtract(image, Scalar(100, 100, 100), dark_img2);

    Mat histogram_org, histImg_org;
    Mat histogram1, histImg1;
    Mat histogram2, histImg2; 

    generate_histogram(image, histogram_org, histImg_org);
    generate_histogram(dark_img1, histogram1, histImg1);
    generate_histogram(dark_img2, histogram2, histImg2);

    printf("Intersection: %f\n", compareHist(histogram1, histogram2, HISTCMP_INTERSECT));
    printf("Correlance: %f\n", compareHist(histogram1, histogram2, HISTCMP_CORREL));
    printf("Chisquare: %f\n", compareHist(histogram1, histogram2, HISTCMP_CHISQR));
    printf("Hellinger: %f\n", compareHist(histogram1, histogram2, HISTCMP_HELLINGER));


    imshow("Original image", image);
    imshow("Original image histogram", histImg_org);
    imshow("Darker image from bottom to top", dark_img1);
    imshow("Darker image from top to bottom", dark_img2);
    imshow("Histogram of bottom to top", histImg1);
    imshow("Histogram of top to bottom", histImg2);
    
    while (true) {
        char key = waitKey(10);
        if (key == 'q') {
            break; 
        }
    }
    
    return 0;

}