#include "opencv2/opencv.hpp"

using namespace cv;

void exMeanShift(Mat& src, Mat& dst)
{
    resize(src, src, Size(src.cols / 2, src.rows / 2), 0, 0, INTER_AREA);
    
    pyrMeanShiftFiltering(src, dst, 20, 40);

}
int main(int argc, char** argv)
{
    Mat img = imread("./imgs/cat.jpg");
    Mat dst(img.size(), img.type());

    exMeanShift(img, dst);
    
    imshow("Original", img);
    imshow("Mean Shifted", dst);

    waitKey(0);
    return 0;

}