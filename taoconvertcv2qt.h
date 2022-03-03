

#ifndef CONVERTCV2QT_H
#define CONVERTCV2QT_H

#include <QPainter>
#include <QRect>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QPoint>
#include <QPolygon>
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

class TaoConvertCV2Qt
{
public:
    static bool convertMat2QImage(Mat &inputMat, Mat &inputMat4Buffer, QImage &outputQImage);

    static bool convertQImage2Mat(QImage &inputQImage, Mat &outputMat);

    static void convertQPointF2CVPoint2f(QPointF &qtPt, Point2f &cvPt);
};

#endif // CONVERTCV2QT_H
