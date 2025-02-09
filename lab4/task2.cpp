#include "opencv2/opencv.hpp"

using namespace cv;

void exMyGrubCut(Mat& src, Mat& dst, Mat& mask,int x1, int y1, int x2, int y2)
{
    Rect rect = Rect(Point(x1, y1), Point(x2, y2));
    Mat bg_model, fg_model;
    grabCut(src, dst, rect, bg_model, fg_model, 5, GC_INIT_WITH_RECT);

    compare(dst, GC_PR_FGD, dst, CMP_EQ);

    src.copyTo(mask, dst);
    rectangle(src, rect, Scalar(255, 255, 255), 2, 8, 0);

}


int main()
{
    Mat img = imread("./imgs/dog_rocks.webp");
    resize(img, img, Size(img.cols / 2, img.rows / 2), 0, 0, INTER_AREA);
    Mat dst(img.size(), img.type());

    Mat mask(img.size(), CV_8UC3, Scalar(255, 255, 255));

    exMyGrubCut(img, dst, mask, 105, 145, 305, 450);

    imshow("image", img);
    imshow("mask", mask);
    imshow("result", dst);

    waitKey(0);

}

//boundary for differen images x1, y1, x2, y2 respectivly
//cat.png: 0, 50, 370, 430
//sheperd.jpg 160, 50, 410, 330
//zebra.jpg 35, 45, 410, 350