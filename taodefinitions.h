

#ifndef TAODEFINITIONS_H
#define TAODEFINITIONS_H

#include <opencv2/opencv.hpp>
#include "opencv2/core.hpp"

#include <cstdio>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <algorithm>

using namespace cv;
using namespace std;


struct CVLine
{
    Point2f point1;
    Point2f point2;
};


class PointMatchDist
{
public:
    PointMatchDist();

    PointMatchDist(Point inputPoint, Point foundPoint, float dist);

    float getMatchDist();


    Point curPoint;
    Point matchPoint;
    float matchDist;
};

struct BoundaryPointPosition
{
    Point boundPoint;
    int pos;  // top -- 1, left -- 2, bottom -- 3, right -- 4 (counter-clockwise)
};


struct CompareMatchDist
{
    bool operator()(PointMatchDist dist1, PointMatchDist dist2)
        { return (dist1.getMatchDist() < dist2.getMatchDist()); }
};


struct CompareStdVariance
{
    bool operator()(PointMatchDist variance1, PointMatchDist variance2)
        { return (variance1.getMatchDist() < variance2.getMatchDist()); }
};


#endif // TAODEFINITIONS_H
