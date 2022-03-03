

#ifndef TAOPIXELCOMPLETION_H
#define TAOPIXELCOMPLETION_H

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/photo/photo.hpp>
#include "opencv2/core.hpp"

#include <cstdio>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <algorithm>

using namespace cv;
using namespace std;

class TaoPixelCompletion
{
public:
    TaoPixelCompletion(Mat &inImage, Mat &inMask, Rect inMaskROI, int bkgdPixelRadius);

    ~TaoPixelCompletion();

    void completeImage();

private:
    void buildKDTree(flann::Index &tree, Mat &features);

    void getAdjacentPixelMean(Point curPoint, Vec3b &pixelVal);

    float getSearchDist(Vec3b &pointVal, flann::Index &tree, int *featureRowNum);
//    Point curPoint,

    void completeEdge(flann::Index &tree, vector<Point> &edgePoints, Mat &bkgdFeatures);

    void getBkgdROI(Rect *bkgdROI);

    void findMaskEdgePoints(vector<Point> &edgePoints, Mat &inMask, Rect &inMaskROI);

    bool checkIsMaskEdgePoint(const Point &inPoint, Mat &mask);

    bool checkIsImagePoint(int pointx, int pointy, int cols, int rows);

    Mat inputImage;  // allocated outside the class, mask area filled

    Mat inputMask;  // allocated outside the class, bkgd --- 0, frgd --- 255

    Rect maskROI;

    int iBkgdPixelRadius;
};

#endif // TAOPIXELCOMPLETION_H
