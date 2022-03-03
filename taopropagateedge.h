

#ifndef TAOPROPAGATEEDGE_H
#define TAOPROPAGATEEDGE_H

#include <QPoint>
#include <QPainterPath>

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

class TaoPropagateEdge
{
public:
    // Mat &inImage, Mat &inMask will be modified in the function
    // if there is less than 2 patches for propagate, return 1; otherwise return 0
    static int propagateLineEdge(Mat &inImage, Mat &inMask, Mat &pushPulledImage,
                                  Point2f &point1, Point2f &point2,
                                  int patchSize);

    // Mat &inImage, Mat &inMask will be modified in the function
    // if there is less than 2 patches for propagate, return 1; otherwise return 0
    static int propagateCurveEdge(Mat &inImage, Mat &inMask, Mat &pushPulledImage,
                                   QPainterPath &path, int patchSize);

    static void propagateConstLine(Mat &inImage, Mat &inMask,
                                   Point2f &point1, Point2f &point2, int patchSize);

    static void propagateConstEdge(Mat &inImage, Point2f &point1, Point2f &point2, int patchSize);


private:
    static int propagateEdgePoints(Mat *inputImage, Mat *inputMask, Mat *pushPulledImage,
                                    vector<Point> &edgePoints, int patchSize);

    static void convertLine2VectorPoints(int rows, int cols ,int patchSize, Point2f &startPoint, Point2f &endPoint, vector<Point> &vecPts);

    // Here the QPainterPath has to be use as pointer, otherwise fail
    static void convertQPath2VectorPoints(int rows, int cols ,int patchSize, QPainterPath *path, vector<Point> &vecPts);


};

#endif // TAOPROPAGATEEDGE_H
