

#ifndef TAOFRACTIONAUTOCOMPLETION_H
#define TAOFRACTIONAUTOCOMPLETION_H

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/photo/photo.hpp>
#include <opencv2/core.hpp>

#include <opencv2/stitching.hpp>
#include <opencv2/stitching/detail/blenders.hpp>

#include <cstdio>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <algorithm>

#include "taopushpullapprox.h"

#include "taodefinitions.h"

using namespace cv;
using namespace std;


class TaoTextureAutoCompletion
{
public:
    TaoTextureAutoCompletion(Mat &inImage, Mat &inMask, Rect inImageROI,
                             Rect inMaskROI, int pSize, int pixSpacing);

    ~TaoTextureAutoCompletion();

    int completeImage();

private:
    int buildBkgdTree4SideWt();

    double getQuantRectFactor(int height, int width, int stdSize);

    void getFrgdBoundPoints(vector<struct BoundaryPointPosition> &boundaryPoints, Mat &inputMask, Rect &imgROI);

//    void computeLine2CenterPos(vector<Point> &line2centerPos);

//    bool checkIsPointInBkgdBound(Point inputPoint, vector<Point> &lineBoundCenterPosList);

    // check if the point is a foreground boundary point, if yes, return true
    // special cases: 1. the point close to image boundary, part of the patch is outside the image,
    //                   move the point inside a little bit to make sure the patch is in the image
    //              2. the point is close to the adjacent foreground, part of the patch is in the adjacent foreground,
    //                   don't consider as a boundary point
    bool checkIsFrgdBoundPoint(Point *outPoint, int *position, const Point &inPoint, Mat &mask);

    bool checkIsImagePoint(int pointx, int pointy, int cols, int rows);

    void searchNsortMatchPointDist(vector<struct BoundaryPointPosition> &points, vector<PointMatchDist> &dist, Mat &inImage);

    void searchNsortBkgdStdVariance(vector<BoundaryPointPosition> &points, vector<PointMatchDist> &matchDist, Mat &inputImage);

    void copyPatch(Mat &inImage, Mat &inMask, vector<PointMatchDist> &points);

    void releaseMem();

    Mat inputImage;  // allocated outside the class, mask area filled

    Mat inputMask;  // allocated outside the class, bkgd --- 0, frgd --- 255

    flann::Index treeLeftWeight;
    flann::Index treeRightWeight;
    flann::Index treeTopWeight;
    flann::Index treeBottomWeight;

    vector<Point> bkgdPatchPointsList;

//    vector<struct CVLine> bkgdBorderLineList;
//    vector<Point> line2CenterPos;

    Mat bkgdPatchMatLeft;
    Mat bkgdPatchMatRight;
    Mat bkgdPatchMatTop;
    Mat bkgdPatchMatBottom;

    int patchSize;
    int bkgdPixSpacing;

    Rect imageROI;
    Rect maskROI;

    const float weightFactor = 8.0;
};


#endif // TAOFRACTIONAUTOCOMPLETION_H
