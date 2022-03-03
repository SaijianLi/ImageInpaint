

#include "taopropagateedge.h"


//Mat &outImage, Mat &outMask,
int TaoPropagateEdge::propagateLineEdge(Mat &inImage, Mat &inMask, Mat &pushPulledImage,
                                    Point2f &point1, Point2f &point2,
                                    int patchSize)
{
    vector<Point> linePoints;

    convertLine2VectorPoints(inImage.rows, inImage.cols, patchSize,
                             point1, point2, linePoints);

    cout << " line start: " << point1.x << " " << point1.y << endl;
    cout << " line end: " << point2.x << " " << point2.y << endl;

    for (int icount = 0; icount < (int)linePoints.size(); icount++)
    {
//        Point curPoint = linePoints[icount];
//        cout << curPoint.x << " " << curPoint.y << endl;
        ;
    }

    return propagateEdgePoints(&inImage, &inMask, &pushPulledImage, linePoints, patchSize);
}


//Mat &outImage, Mat &outMask,
int TaoPropagateEdge::propagateCurveEdge(Mat &inImage, Mat &inMask, Mat &pushPulledImage,
                                        QPainterPath &path,
                                        int patchSize)
{
    vector<Point> curvePoints;

    convertQPath2VectorPoints(inImage.rows, inImage.cols, patchSize, &path, curvePoints);

    for (int icount = 0; icount < (int)curvePoints.size(); icount++)
    {
//        Point curPoint = curvePoints[icount];
//        cout << curPoint.x << " " << curPoint.y << endl;
        ;
    }

    return propagateEdgePoints(&inImage, &inMask, &pushPulledImage, curvePoints, patchSize);
}


void TaoPropagateEdge::convertLine2VectorPoints(int imageRows, int imageCols ,int patchSize,
                                                Point2f &startPoint, Point2f &endPoint,
                                                 vector<Point> &vecPts)
{
    int rows = (int)abs(startPoint.y - endPoint.y);
    int cols = (int)abs(startPoint.x - endPoint.x);

    int rowSign = (startPoint.y - endPoint.y > 0)? 1: -1;
    int colSign = (startPoint.x - endPoint.x > 0)? 1: -1;

    int halfPatchSize = patchSize / 2;

    int steps = (rows > cols)? rows: cols;

    for (int istep = 1; istep <= steps; istep++)
    {
        double slope;
        Point curPoint;

        // process the slope for row as variable or col as variable
        if (rows > cols)
        {
            slope = istep * (startPoint.x - endPoint.x) * 1.0 / (startPoint.y - endPoint.y);
            curPoint.x = (int)(startPoint.x - rowSign * slope);
            curPoint.y = (int)(startPoint.y - rowSign * istep);
        }
        else
        {
            slope = istep * (startPoint.y - endPoint.y) * 1.0 / (startPoint.x - endPoint.x);
            curPoint.x = (int)(startPoint.x - colSign * istep);
            curPoint.y = (int)(startPoint.y - colSign * slope);
        }

        if ((curPoint.x - halfPatchSize < 0) || (curPoint.x + halfPatchSize >= imageCols)
                || (curPoint.y - halfPatchSize < 0) || (curPoint.y + halfPatchSize >= imageRows))
            continue;

        vecPts.push_back(curPoint);
    }
}


void TaoPropagateEdge::propagateConstLine(Mat &inImage, Mat &inMask,
                                        Point2f &startPoint, Point2f &endPoint, int patchSize)
{
    vector<Point> linePoints;

    int lineRows = (int)abs(startPoint.y - endPoint.y);
    int lineCols = (int)abs(startPoint.x - endPoint.x);

    int rowSign = (startPoint.y - endPoint.y > 0)? 1: -1;
    int colSign = (startPoint.x - endPoint.x > 0)? 1: -1;

    int halfPatchSize = patchSize / 2;

    int steps = (lineRows > lineCols)? lineRows: lineCols;

    for (int istep = 1; istep <= steps; istep++)
    {
        double slope;
        Point curPoint;

        // process the slope for row as variable or col as variable
        if (lineRows > lineCols)
        {
            slope = istep * (startPoint.x - endPoint.x) * 1.0 / (startPoint.y - endPoint.y);
            curPoint.x = (int)(startPoint.x - rowSign * slope);
            curPoint.y = (int)(startPoint.y - rowSign * istep);

            if ((curPoint.x - halfPatchSize < 0) || (curPoint.x + halfPatchSize >= inImage.cols))
                continue;
        }
        else
        {
            slope = istep * (startPoint.y - endPoint.y) * 1.0 / (startPoint.x - endPoint.x);
            curPoint.x = (int)(startPoint.x - colSign * istep);
            curPoint.y = (int)(startPoint.y - colSign * slope);

            if ((curPoint.y - halfPatchSize < 0) || (curPoint.y + halfPatchSize >= inImage.rows))
                continue;
        }

        linePoints.push_back(curPoint);
    }

//    cout << " line points: " << linePoints.size() << endl;
//    cout << linePoints << endl;

    Mat edgePatch;
    Mat maskPatch;
    // make a patch
    if (lineRows > lineCols)
    {
        edgePatch = Mat(inImage, Rect(startPoint.x - halfPatchSize, startPoint.y, patchSize, 1));
        maskPatch = Mat(inMask, Rect(startPoint.x - halfPatchSize+1, startPoint.y, patchSize-2, 1));
    }
    else
    {
        edgePatch = Mat(inImage, Rect(startPoint.x, startPoint.y  - halfPatchSize, 1, patchSize));
        maskPatch = Mat(inMask, Rect(startPoint.x, startPoint.y  - halfPatchSize+1, 1, patchSize-2));
    }

    // copy the patch
    for (int countPt = 0; countPt < (int)linePoints.size(); countPt++)
    {
        // dst ROI
        Mat dstPatch;
        Mat dstMask;
        Point curPoint = linePoints[countPt];

        if ((curPoint.x - halfPatchSize < 0) || (curPoint.x + halfPatchSize >= inImage.cols))
            continue;

        if ((curPoint.y - halfPatchSize < 0) || (curPoint.y + halfPatchSize >= inImage.rows))
            continue;

        if (lineRows > lineCols)
        {
            dstPatch = Mat(inImage, Rect(curPoint.x - halfPatchSize, curPoint.y, patchSize, 1));
            dstMask = Mat(inMask, Rect(curPoint.x - halfPatchSize+1, curPoint.y, patchSize-2, 1));
        }
        else
        {
            dstPatch = Mat(inImage, Rect(curPoint.x, curPoint.y  - halfPatchSize, 1, patchSize));
            dstMask = Mat(inMask, Rect(curPoint.x, curPoint.y  - halfPatchSize+1, 1, patchSize-2));
        }

        edgePatch.copyTo(dstPatch);
        maskPatch.copyTo(dstMask);
    }
}


void TaoPropagateEdge::convertQPath2VectorPoints(int imageRows, int imageCols ,int patchSize,
                                                 QPainterPath *path, vector<Point> &vecPts)
{
    QRectF rectPath = path->boundingRect();

    int halfPatchSize = patchSize / 2;

    int maxPoints = rectPath.width() > rectPath.height()? (int)rectPath.width():(int)rectPath.height();

    if (maxPoints == 0)
        return;

//    cout << " inside convertQPath2VectorPoints\n";

    for (int icount = 0; icount <= maxPoints; icount++)
    {
        qreal pct = 1.0 * (icount)/ maxPoints;
        QPointF pointAtPct = path->pointAtPercent(pct);
        Point  curPoint;

        curPoint.x = (int)pointAtPct.x();
        curPoint.y = (int)pointAtPct.y();

        if ((curPoint.x - halfPatchSize < 0) || (curPoint.x + halfPatchSize >= imageCols)
                || (curPoint.y - halfPatchSize < 0) || (curPoint.y + halfPatchSize >= imageRows))
            continue;

        vecPts.push_back(curPoint);
    }
}


int TaoPropagateEdge::propagateEdgePoints(Mat *inputImage, Mat *inputMask, Mat *pushPulledImage,
                                vector<Point> &edgePoints, int patchSize)
{
    int numPoints = edgePoints.size();

//    cout << "numPoints: " << numPoints << endl;

    int halfPatchSize = patchSize / 2;

    Mat bkgdEdgeFragments;
    vector<Point> bkgdEdgePoints;

    int startPoint2Mask = -1;
    int endPoint2Mask = -1;

    // get the direction for propagate edge
    for (int icount = 0; icount < numPoints; icount++)
    {
        Point curPoint = edgePoints[icount];
        Rect rectPatch(curPoint.x - halfPatchSize, curPoint.y- halfPatchSize, patchSize, patchSize);

        // make sure the patch is in the image
        if ((curPoint.x - halfPatchSize < 0) || (curPoint.x + halfPatchSize >= inputImage->cols)
                || (curPoint.y - halfPatchSize < 0) || (curPoint.y + halfPatchSize >= inputImage->rows))
            continue;

        Mat curPatch(*inputMask, rectPatch);

        double minVal = -1;
        double maxVal = -1;

        // make sure all pixels are in the background
        minMaxLoc(curPatch,  &minVal, &maxVal);

        if ((int)maxVal > 0)
        {
            if (startPoint2Mask == -1)
            {
                startPoint2Mask = icount;
            }
            else
            {
                endPoint2Mask = numPoints - 1 - icount;
            };
        }
    }

//    cout << " startPoint2Mask/ end: " << startPoint2Mask << " " << endPoint2Mask << endl;

    // put that the point-patch is all background in a vector
    if (startPoint2Mask > endPoint2Mask)
    {
        for (int icount = 0; icount < numPoints; icount++)
        {
            Point curPoint = edgePoints[icount];
            Rect rectPatch(curPoint.x - halfPatchSize, curPoint.y- halfPatchSize, patchSize, patchSize);

            // make sure the patch is in the image
            if ((curPoint.x - halfPatchSize < 0) || (curPoint.x + halfPatchSize >= inputImage->cols)
                    || (curPoint.y - halfPatchSize < 0) || (curPoint.y + halfPatchSize >= inputImage->rows))
                continue;

            Mat curPatch(*inputMask, rectPatch);

            double minVal = -1;
            double maxVal = -1;

            // make sure all pixels are in the background
            minMaxLoc(curPatch,  &minVal, &maxVal);

            if ((int)maxVal == 0)
            {
                bkgdEdgePoints.push_back(curPoint);
            }
        }
    }
    else
    {
        for (int icount = numPoints - 1; icount > -1; icount--)
        {
            Point curPoint = edgePoints[icount];
            Rect rectPatch(curPoint.x - halfPatchSize, curPoint.y- halfPatchSize, patchSize, patchSize);

            // make sure the patch is in the image
            if ((curPoint.x - halfPatchSize < 0) || (curPoint.x + halfPatchSize >= inputImage->cols)
                    || (curPoint.y - halfPatchSize < 0) || (curPoint.y + halfPatchSize >= inputImage->rows))
                continue;

            Mat curPatch(*inputMask, rectPatch);

            double minVal = -1;
            double maxVal = -1;

            // make sure all pixels are in the background
            minMaxLoc(curPatch,  &minVal, &maxVal);

            if ((int)maxVal == 0)
            {
                bkgdEdgePoints.push_back(curPoint);
            }
        }
    }

//    cout<< "edge point fragment count: " << bkgdEdgePoints.size() << endl;

    if (bkgdEdgePoints.size() < 2)
        return 1;

    int fragmentSize = bkgdEdgePoints.size();

    bkgdEdgeFragments.create(fragmentSize, patchSize * patchSize * 3, CV_32F);

    bkgdEdgeFragments = Scalar::all(0);

    // copy background patch pixel data to build a KD tree
    for (int iFragCount = 0; iFragCount < fragmentSize; iFragCount++)
    {
        Point curPoint = bkgdEdgePoints[iFragCount];

        int valCount = 0;

        for (int curRow = curPoint.y - halfPatchSize; curRow <= curPoint.y + halfPatchSize; curRow++)
            for (int curCol = curPoint.x - halfPatchSize; curCol <= curPoint.x + halfPatchSize; curCol++)
            {
//                Vec3b curImagePointVal = inputImage->at<Vec3b>(curRow, curCol);
                Vec3b curImagePointVal = pushPulledImage->at<Vec3b>(curRow, curCol);

                bkgdEdgeFragments.at<float>(iFragCount, valCount++) = curImagePointVal[0];
                bkgdEdgeFragments.at<float>(iFragCount, valCount++) = curImagePointVal[1];
                bkgdEdgeFragments.at<float>(iFragCount, valCount++) = curImagePointVal[2];
            }
    }

    flann::Index edgePatchTree;
    flann::KDTreeIndexParams indexParams(4);

    edgePatchTree.build(bkgdEdgeFragments, indexParams);


    Point matchPoint;
    bool isSearched = false;

    // compare and copy patches, update mask
    for (int icount = 0; icount < numPoints; icount++)
    {
        Point curPoint = edgePoints[icount];

        // if the edge point is not in mask, does not need to restore
        if (inputMask->at<uchar>(curPoint.y, curPoint.x) == (unsigned char)0)
        {
//            cout << icount << " " << curPoint << " continue" << endl;
            continue;
        }

//        cout << icount << " " << curPoint.x << " " << curPoint.y << endl;

        Rect dstROI(curPoint.x - halfPatchSize, curPoint.y - halfPatchSize, patchSize, patchSize);

        // make sure the patch is inside the image, if not redefine the dstROI
        if ((curPoint.x - halfPatchSize < 0) || (curPoint.x + halfPatchSize >= inputImage->cols)
                || (curPoint.y - halfPatchSize < 0) || (curPoint.y + halfPatchSize >= inputImage->rows))
        {
            if (curPoint.x - halfPatchSize < 0)
                dstROI.x = 0;

            if (curPoint.x + halfPatchSize >= inputImage->cols)
                dstROI.x = inputImage->cols - patchSize;

            if (curPoint.y - halfPatchSize < 0)
                dstROI.y = 0;

            if (curPoint.y + halfPatchSize >= inputImage->rows)
                dstROI.y = inputImage->rows - patchSize;
        }

        Mat dstPatch(*inputImage, dstROI);
        Mat dstMaskPatch(*inputMask, dstROI);
        Mat patch(1, patchSize * patchSize *3, CV_32F);

        int pointCount = 0;
        cv::Mat matchIndices(1, 1, CV_32F);   // nearest neighbour row
        cv::Mat matchDists(1, 1, CV_32F);     // nearest neighbour distance

        // make a patch for search
        for (int curRow = curPoint.y - halfPatchSize; curRow <= curPoint.y + halfPatchSize; curRow++)
            for (int curCol = curPoint.x - halfPatchSize; curCol <= curPoint.x + halfPatchSize; curCol++)
            {
//                Vec3b curPointVal = inputImage->at<Vec3b>(curRow, curCol);
                Vec3b curPointVal = pushPulledImage->at<Vec3b>(curRow, curCol);

                patch.at<float>(0, pointCount++) = (float)curPointVal[0];
                patch.at<float>(0, pointCount++) = (float)curPointVal[1];
                patch.at<float>(0, pointCount++) = (float)curPointVal[2];
            }

        if (!isSearched) {
            edgePatchTree.knnSearch(patch, matchIndices, matchDists, 1);

            int index = matchIndices.at<int>(0,0);
            matchPoint = bkgdEdgePoints[index];

            isSearched = true;
        }

        Rect srcROI(matchPoint.x - halfPatchSize, matchPoint.y - halfPatchSize, patchSize, patchSize);
        Mat srcPatch(*inputImage, srcROI);
        Mat maskPatch(*inputMask, srcROI);

//        srcPatch.copyTo(dstPatch);
        for (int irow = 0; irow < dstROI.height; irow++)
            for (int icol = 0; icol < dstROI.width; icol++)
            {
                if (dstMaskPatch.at<uchar>(irow, icol) > 0)
                {
                    dstPatch.at<Vec3b>(irow, icol) = srcPatch.at<Vec3b>(irow, icol);
                }
            }

        maskPatch.copyTo(dstMaskPatch);
    }

    return 0;
}


void TaoPropagateEdge::propagateConstEdge(Mat &inImage, Point2f &startPoint,
                                          Point2f &endPoint, int patchSize)
{
    vector<Point> linePoints;

    int lineRows = (int)abs(startPoint.y - endPoint.y);
    int lineCols = (int)abs(startPoint.x - endPoint.x);

    int rowSign = (startPoint.y - endPoint.y > 0)? 1: -1;
    int colSign = (startPoint.x - endPoint.x > 0)? 1: -1;

    int halfPatchSize = patchSize / 2;

    int steps = (lineRows > lineCols)? lineRows: lineCols;

    for (int istep = 1; istep <= steps; istep++)
    {
        double slope;
        Point curPoint;

        // process the slope for row as variable or col as variable
        if (lineRows > lineCols)
        {
            slope = istep * (startPoint.x - endPoint.x) * 1.0 / (startPoint.y - endPoint.y);
            curPoint.x = (int)(startPoint.x - rowSign * slope);
            curPoint.y = (int)(startPoint.y - rowSign * istep);

            if ((curPoint.x - halfPatchSize < 0) || (curPoint.x + halfPatchSize >= inImage.cols))
                continue;
        }
        else
        {
            slope = istep * (startPoint.y - endPoint.y) * 1.0 / (startPoint.x - endPoint.x);
            curPoint.x = (int)(startPoint.x - colSign * istep);
            curPoint.y = (int)(startPoint.y - colSign * slope);

            if ((curPoint.y - halfPatchSize < 0) || (curPoint.y + halfPatchSize >= inImage.rows))
                continue;
        }

        linePoints.push_back(curPoint);
    }

//    cout << " line points: " << linePoints.size() << endl;
//    cout << linePoints << endl;
    int startx = startPoint.x - halfPatchSize > 0 ? startPoint.x - halfPatchSize : 0;
    int endx = startPoint.x + halfPatchSize < inImage.cols ? startPoint.x + halfPatchSize: inImage.cols -1;
    int starty = startPoint.y - halfPatchSize > 0 ? startPoint.y - halfPatchSize : 0;
    int endy = startPoint.y + halfPatchSize < inImage.rows ? startPoint.y + halfPatchSize: inImage.cols -1;


    Mat edgePatch;

    // make a patch
    if (lineRows > lineCols)
    {
//        edgePatch = Mat(inImage, Rect(startPoint.x - halfPatchSize, startPoint.y, patchSize, 1));
        edgePatch = Mat(inImage, Rect(startx, startPoint.y, endx - startx +1, 1));
    }
    else
    {
//        edgePatch = Mat(inImage, Rect(startPoint.x, startPoint.y  - halfPatchSize, 1, patchSize));
        edgePatch = Mat(inImage, Rect(startPoint.x, starty, 1, endy - starty + 1));
    }

    // copy the patch
    for (int countPt = 0; countPt < (int)linePoints.size(); countPt++)
    {
        // dst ROI
        Mat dstPatch;
        Point curPoint = linePoints[countPt];

//        if ((curPoint.x - halfPatchSize < 0) || (curPoint.x + halfPatchSize >= inImage.cols))
//            continue;

//        if ((curPoint.y - halfPatchSize < 0) || (curPoint.y + halfPatchSize >= inImage.rows))
//            continue;

//        if (lineRows > lineCols)
//        {
//            dstPatch = Mat(inImage, Rect(curPoint.x - halfPatchSize, curPoint.y, patchSize, 1));
//        }
//        else
//        {
//            dstPatch = Mat(inImage, Rect(curPoint.x, curPoint.y  - halfPatchSize, 1, patchSize));
//        }

        int curStartx = curPoint.x - halfPatchSize > 0 ? curPoint.x - halfPatchSize : 0;
        int curEndx = curPoint.x + halfPatchSize < inImage.cols ? curPoint.x + halfPatchSize: inImage.cols -1;
        int curStarty = curPoint.y - halfPatchSize > 0 ? curPoint.y - halfPatchSize : 0;
        int curEndy = curPoint.y + halfPatchSize < inImage.rows ? curPoint.y + halfPatchSize: inImage.cols -1;

        if (lineRows > lineCols)
        {
            dstPatch = Mat(inImage, Rect(curStartx, curPoint.y, curEndx - curStartx +1, 1));
        }
        else
        {
            dstPatch = Mat(inImage, Rect(curPoint.x, curStarty, 1, curEndy - curStarty +1));
        }

        edgePatch.copyTo(dstPatch);
    }
}
