

#ifndef TAOPUSHPULLAPPROX_H
#define TAOPUSHPULLAPPROX_H

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/photo/photo.hpp>
#include <opencv2/core.hpp>

#include <cstdio>
#include <iostream>
#include <stdio.h>
#include <vector>

using namespace cv;
using namespace std;

class TaoPushPullApprox
{
public:
    // levels means the 1/levels of the input width/height, valid values: 16, 8, 4, 2, 1 ( >=1)
    static void pushPullApprox(Mat &outImage, Mat &inImage, Mat &inMask, int levels);
};


#endif // TAOPUSHPULLAPPROX_H
