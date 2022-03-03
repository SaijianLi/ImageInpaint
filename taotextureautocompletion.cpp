

#include "taotextureautocompletion.h"

#include <QTime>


TaoTextureAutoCompletion::TaoTextureAutoCompletion(Mat &inImage, Mat &inMask, Rect inImageROI,
                                                   Rect inMaskROI, int pSize, int pixSpacing)
{
    inputImage = inImage;
    inputMask = inMask;
    imageROI = inImageROI;
    maskROI = inMaskROI;
    patchSize = pSize;
    bkgdPixSpacing = pixSpacing;

//    cout << " in TaoTextureAutoCompletion, border lines: " << inVecLine.size() << endl;
}


TaoTextureAutoCompletion::~TaoTextureAutoCompletion()
{

}


int TaoTextureAutoCompletion::completeImage()
{
    // compute the border line to mask ROI position, used in build bkgd tree.
//    computeLine2CenterPos(line2CenterPos);

    int iResultBuildBkgdTree = -1;

    QTime curTime;

    curTime.start();

    iResultBuildBkgdTree = buildBkgdTree4SideWt();

    if (iResultBuildBkgdTree !=0)
        return 1;

//    cout << " build tree time: " << curTime.elapsed() <<endl;

    vector<struct BoundaryPointPosition> boundaryPoints;

    curTime.restart();

    getFrgdBoundPoints(boundaryPoints,  inputMask, imageROI);


//    cout << " get frgd bound points time 0: " << curTime.elapsed() <<endl;

    while (boundaryPoints.size() > 0)
    {
        vector<PointMatchDist> matchDist;

        curTime.restart();

        searchNsortMatchPointDist(boundaryPoints, matchDist, inputImage);

//        cout << " sort match points time: " << curTime.elapsed() <<endl;

        curTime.restart();

        copyPatch(inputImage, inputMask, matchDist);

//        cout << " copy patch time: " << curTime.elapsed() <<endl;

        boundaryPoints.clear();

        curTime.restart();

        getFrgdBoundPoints(boundaryPoints,  inputMask, imageROI);

//        cout << " get frgd bound points time: " << curTime.elapsed() <<endl;

        // for testing
//        break;
    }

    releaseMem();

    return 0;
}


int TaoTextureAutoCompletion::buildBkgdTree4SideWt()
{
    int halfPatchSize = patchSize / 2;
    int startx = imageROI.x;
    int starty = imageROI.y;
    int endx = imageROI.x + imageROI.width - 1;
    int endy = imageROI.y + imageROI.height - 1;

    // get the background patch centers
    for (int irow = starty + halfPatchSize; irow <= endy - halfPatchSize; irow++)
    {
        for (int icol = startx + halfPatchSize; icol <= endx - halfPatchSize; icol++)
        {
            // added to reduce the background patch centers to 1/4, choose the even number rows and cols
            // by Saijian Jan. 1, 2017
            if ((irow % bkgdPixSpacing != 0) || (icol % bkgdPixSpacing != 0))
                continue;
            // end of added

            Rect rectPatch(icol- halfPatchSize, irow - halfPatchSize, patchSize, patchSize);

            Mat curMaskPatch(inputMask, rectPatch);

            double minVal = -1;
            double maxVal = -1;

            // make sure all pixels are in the background
            minMaxLoc(curMaskPatch,  &minVal, &maxVal);

            if ((int)maxVal == 0)
                bkgdPatchPointsList.push_back(Point(icol, irow));
        }
    }

//    cout<< " patch count: " << bkgdPatchPointsList.size() << endl;

    if (bkgdPatchPointsList.size() == 0)
        return 1;

    int numBkgPatches = bkgdPatchPointsList.size();

    bkgdPatchMatLeft.create(numBkgPatches, patchSize * patchSize * 3 + 2, CV_32F);
    bkgdPatchMatLeft = Scalar::all(0);

    bkgdPatchMatRight.create(numBkgPatches, patchSize * patchSize * 3 + 2, CV_32F);
    bkgdPatchMatRight = Scalar::all(0);

    bkgdPatchMatTop.create(numBkgPatches, patchSize * patchSize * 3 + 2, CV_32F);
    bkgdPatchMatTop = Scalar::all(0);

    bkgdPatchMatBottom.create(numBkgPatches, patchSize * patchSize * 3 + 2, CV_32F);
    bkgdPatchMatBottom = Scalar::all(0);

    // copy background patch pixel data to a Matrix
    for (int irow = 0; irow < numBkgPatches; irow++)
    {
        Point curPoint = bkgdPatchPointsList[irow];

        int valCountLeft = 0;
        int valCountRight = 0;
        int valCountTop = 0;
        int valCountBottom = 0;

        for (int curRow = curPoint.y - halfPatchSize; curRow <= curPoint.y + halfPatchSize; curRow++)
            for (int curCol = curPoint.x - halfPatchSize; curCol <= curPoint.x + halfPatchSize; curCol++)
            {
                Vec3b curImagePointVal = inputImage.at<Vec3b>(curRow, curCol);

                if (curRow < curPoint.y)
                {
                    bkgdPatchMatTop.at<float>(irow, valCountTop++) = (float)curImagePointVal[0];
                    bkgdPatchMatTop.at<float>(irow, valCountTop++) = (float)curImagePointVal[1];
                    bkgdPatchMatTop.at<float>(irow, valCountTop++) = (float)curImagePointVal[2];
                }
                else
                {
                    bkgdPatchMatTop.at<float>(irow, valCountTop++) = (float)curImagePointVal[0] / weightFactor;
                    bkgdPatchMatTop.at<float>(irow, valCountTop++) = (float)curImagePointVal[1] / weightFactor;
                    bkgdPatchMatTop.at<float>(irow, valCountTop++) = (float)curImagePointVal[2] / weightFactor;
                }

                if (curRow > curPoint.y)
                {
                    bkgdPatchMatBottom.at<float>(irow, valCountBottom++) = (float)curImagePointVal[0];
                    bkgdPatchMatBottom.at<float>(irow, valCountBottom++) = (float)curImagePointVal[1];
                    bkgdPatchMatBottom.at<float>(irow, valCountBottom++) = (float)curImagePointVal[2];
                }
                else
                {
                    bkgdPatchMatBottom.at<float>(irow, valCountBottom++) = (float)curImagePointVal[0]/ weightFactor;
                    bkgdPatchMatBottom.at<float>(irow, valCountBottom++) = (float)curImagePointVal[1]/ weightFactor;
                    bkgdPatchMatBottom.at<float>(irow, valCountBottom++) = (float)curImagePointVal[2]/ weightFactor;
                }

                if (curCol < curPoint.x)
                {
                    bkgdPatchMatLeft.at<float>(irow, valCountLeft++) = (float)curImagePointVal[0];
                    bkgdPatchMatLeft.at<float>(irow, valCountLeft++) = (float)curImagePointVal[1];
                    bkgdPatchMatLeft.at<float>(irow, valCountLeft++) = (float)curImagePointVal[2];
                }
                else
                {
                    bkgdPatchMatLeft.at<float>(irow, valCountLeft++) = (float)curImagePointVal[0]/ weightFactor;
                    bkgdPatchMatLeft.at<float>(irow, valCountLeft++) = (float)curImagePointVal[1]/ weightFactor;
                    bkgdPatchMatLeft.at<float>(irow, valCountLeft++) = (float)curImagePointVal[2]/ weightFactor;
                }

                if (curCol > curPoint.x)
                {
                    bkgdPatchMatRight.at<float>(irow, valCountRight++) = (float)curImagePointVal[0];
                    bkgdPatchMatRight.at<float>(irow, valCountRight++) = (float)curImagePointVal[1];
                    bkgdPatchMatRight.at<float>(irow, valCountRight++) = (float)curImagePointVal[2];
                }
                else
                {
                    bkgdPatchMatRight.at<float>(irow, valCountRight++) = (float)curImagePointVal[0]/ weightFactor;
                    bkgdPatchMatRight.at<float>(irow, valCountRight++) = (float)curImagePointVal[1]/ weightFactor;
                    bkgdPatchMatRight.at<float>(irow, valCountRight++) = (float)curImagePointVal[2]/ weightFactor;
                }
            }

        // quantize the image
        double quantFactor = getQuantRectFactor(imageROI.height, imageROI.width, 8);

        float quantWidth = (curPoint.x - imageROI.x) * quantFactor;
        float quantHeight = (curPoint.y - imageROI.y) * quantFactor;

        bkgdPatchMatTop.at<float>(irow, valCountTop++) = quantWidth;
        bkgdPatchMatTop.at<float>(irow, valCountTop++) = quantHeight;

        bkgdPatchMatBottom.at<float>(irow, valCountBottom++) = quantWidth;
        bkgdPatchMatBottom.at<float>(irow, valCountBottom++) = quantHeight;

        bkgdPatchMatLeft.at<float>(irow, valCountLeft++) = quantWidth;
        bkgdPatchMatLeft.at<float>(irow, valCountLeft++) = quantHeight;

        bkgdPatchMatRight.at<float>(irow, valCountRight++) = quantWidth;
        bkgdPatchMatRight.at<float>(irow, valCountRight++) = quantHeight;
    }

    flann::KDTreeIndexParams indexParams(4);

    treeLeftWeight.build(bkgdPatchMatLeft, indexParams);
    treeRightWeight.build(bkgdPatchMatRight, indexParams);
    treeTopWeight.build(bkgdPatchMatTop, indexParams);
    treeBottomWeight.build(bkgdPatchMatBottom, indexParams);

//    cout << " end of buildSrcTree." << endl;

    return 0;
}


double TaoTextureAutoCompletion::getQuantRectFactor(int height, int width, int stdSize)
{
    return (height > width? (stdSize* 1.0 / height) : ( stdSize* 1.0/ width));
}


void TaoTextureAutoCompletion::getFrgdBoundPoints(vector<struct BoundaryPointPosition> &boundaryPoints, Mat &inputMask, Rect &imgROI)
{
    int startx = imgROI.x;
    int starty = imgROI.y;
    int endx = imgROI.x + imgROI.width;
    int endy = imgROI.y + imgROI.height;
    Point inPoint, outPoint;
    int position;

    // get the background patch centers
    for (int irow = starty; irow < endy; irow++)
        for (int icol = startx; icol < endx; icol++)
        {
            inPoint.x = icol;
            inPoint.y = irow;

            outPoint.x = icol;
            outPoint.y = irow;

            position = 0;

            // if it is a foreground point adjacent to background, add to the list.
            if (checkIsFrgdBoundPoint(&outPoint, &position, inPoint, inputMask))
            {
                struct BoundaryPointPosition curPointPos;
                curPointPos.boundPoint = outPoint;
                curPointPos.pos = position;

                boundaryPoints.push_back(curPointPos);
            }
        }

//    cout <<" patch size: " << patchSize << "  " << patchSize / 2 << endl;

//    cout<< "boundary points count: " << boundaryPoints.size() << endl;
}


bool TaoTextureAutoCompletion::checkIsFrgdBoundPoint(Point *outPoint, int *position, const Point &inPoint, Mat &mask)
{
    int halfSize = patchSize / 2;

    // if it is a background point
    if (mask.at<uchar>(inPoint.y, inPoint.x) == 0)
        return false;

    // check if part of the point-centered patch outside the image, if yes, move the point inward
    if (inPoint.x - halfSize < 0)
        outPoint->x = halfSize;

    if (inPoint.x + halfSize >= mask.cols)
        outPoint->x = mask.cols - halfSize - 1;

    if (inPoint.y - halfSize < 0)
        outPoint->y = halfSize;

    if (inPoint.y + halfSize >= mask.rows)
        outPoint->y = mask.rows - halfSize - 1;

    int leftBkgdCount = 0;
    int rightBkgdCount = 0;
    int topBkgdCount = 0;
    int bottomBkgdCount = 0;

    // check if there is side that has enough background points
    for (int irow = outPoint->y - halfSize; irow <= outPoint->y + halfSize; irow++)
        for (int icol = outPoint->x - halfSize; icol <= outPoint->x + halfSize; icol++)
        {
            // if it is background point
            if (mask.at<uchar>(irow, icol) == 0)
            {
                if (irow < outPoint->y)
                    topBkgdCount++;

                if (irow > outPoint->y)
                    bottomBkgdCount++;

                if (icol > outPoint->x)
                    rightBkgdCount++;

                if (icol < outPoint->x)
                    leftBkgdCount++;
            }
        }

//    cout <<  "Point x/y: "<< outPoint->x << " "<< outPoint->y << " bkgd count: top, bottom, left, right: " << topBkgdCount << " "
//         << bottomBkgdCount << " " << leftBkgdCount << " " << rightBkgdCount << endl;

    if (topBkgdCount == patchSize * halfSize)
        *position = 1;

    if (bottomBkgdCount == patchSize * halfSize)
        *position = 3;

    if (leftBkgdCount == patchSize * halfSize)
        *position = 2;

    if (rightBkgdCount == patchSize * halfSize)
        *position = 4;

    if ((topBkgdCount == patchSize * halfSize) || (bottomBkgdCount == patchSize * halfSize)
            || (leftBkgdCount == patchSize * halfSize) ||(rightBkgdCount == patchSize * halfSize))
        return true;
    else
        return false;
}


bool TaoTextureAutoCompletion::checkIsImagePoint(int pointx, int pointy, int cols, int rows)
{
    if ((pointx >= 0) && (pointx < cols) && (pointy >= 0) && (pointy < rows))
        return true;
    else
        return false;
}


void TaoTextureAutoCompletion::searchNsortMatchPointDist(vector<BoundaryPointPosition> &points, vector<PointMatchDist> &matchDist, Mat &inputImage)
{
    if (points.size() == 0)
        return;

//    cout << "inside searchNsortMatchPointDist " << endl;

    for (int ptCount = 0; ptCount < (int)points.size(); ptCount++)  //points.size()
    {
        // get the current point best match
        struct BoundaryPointPosition curBoundPoint = points[ptCount];
        Point curPoint = curBoundPoint.boundPoint;
        int curPosition = curBoundPoint.pos;

        Mat curPatch(1, patchSize * patchSize * 3 + 2, CV_32F);
        int pixelValCount = 0;

        int halfPatchSize = patchSize / 2;

        int bkgdCountLeft = 0;
        int bkgdCountRight = 0;
        int bkgdCountTop = 0;
        int bkgdCountBottom = 0;


        // make a patch for search
        for (int curRow = curPoint.y - halfPatchSize; curRow <= curPoint.y + halfPatchSize; curRow++)
            for (int curCol = curPoint.x - halfPatchSize; curCol <= curPoint.x + halfPatchSize; curCol++)
            {
                Vec3b curImagePointVal = inputImage.at<Vec3b>(curRow, curCol);

                if (curPosition == 1)
                {
                    if (curRow < curPoint.y)
                    {
                        curPatch.at<float>(0, pixelValCount++) = (float)curImagePointVal[0];
                        curPatch.at<float>(0, pixelValCount++) = (float)curImagePointVal[1];
                        curPatch.at<float>(0, pixelValCount++) = (float)curImagePointVal[2];

                        if (inputMask.at<uchar>(curRow, curCol) == 0)
                            bkgdCountTop++;
                    }
                    else
                    {
                        curPatch.at<float>(0, pixelValCount++) = (float)curImagePointVal[0]/ weightFactor;
                        curPatch.at<float>(0, pixelValCount++) = (float)curImagePointVal[1]/ weightFactor;
                        curPatch.at<float>(0, pixelValCount++) = (float)curImagePointVal[2]/ weightFactor;
                    }
                }

                if (curPosition == 3)
                {
                    if (curRow > curPoint.y)
                    {
                        curPatch.at<float>(0, pixelValCount++) = (float)curImagePointVal[0];
                        curPatch.at<float>(0, pixelValCount++) = (float)curImagePointVal[1];
                        curPatch.at<float>(0, pixelValCount++) = (float)curImagePointVal[2];

                        if (inputMask.at<uchar>(curRow, curCol) == 0)
                            bkgdCountBottom++;
                    }
                    else
                    {
                        curPatch.at<float>(0, pixelValCount++) = (float)curImagePointVal[0]/ weightFactor;
                        curPatch.at<float>(0, pixelValCount++) = (float)curImagePointVal[1]/ weightFactor;
                        curPatch.at<float>(0, pixelValCount++) = (float)curImagePointVal[2]/ weightFactor;
                    }
                }

                if (curPosition == 2)
                {
                    if (curCol < curPoint.x)
                    {
                        curPatch.at<float>(0, pixelValCount++) = (float)curImagePointVal[0];
                        curPatch.at<float>(0, pixelValCount++) = (float)curImagePointVal[1];
                        curPatch.at<float>(0, pixelValCount++) = (float)curImagePointVal[2];

                        if (inputMask.at<uchar>(curRow, curCol) == 0)
                            bkgdCountLeft++;
                    }
                    else
                    {
                        curPatch.at<float>(0, pixelValCount++) = (float)curImagePointVal[0]/ weightFactor;
                        curPatch.at<float>(0, pixelValCount++) = (float)curImagePointVal[1]/ weightFactor;
                        curPatch.at<float>(0, pixelValCount++) = (float)curImagePointVal[2]/ weightFactor;
                    }
                }

                if (curPosition == 4)
                {
                    if (curCol > curPoint.x)
                    {
                        curPatch.at<float>(0, pixelValCount++) = (float)curImagePointVal[0];
                        curPatch.at<float>(0, pixelValCount++) = (float)curImagePointVal[1];
                        curPatch.at<float>(0, pixelValCount++) = (float)curImagePointVal[2];

                        if (inputMask.at<uchar>(curRow, curCol) == 0)
                            bkgdCountRight++;
                    }
                    else
                    {
                        curPatch.at<float>(0, pixelValCount++) = (float)curImagePointVal[0]/ weightFactor;
                        curPatch.at<float>(0, pixelValCount++) = (float)curImagePointVal[1]/ weightFactor;
                        curPatch.at<float>(0, pixelValCount++) = (float)curImagePointVal[2]/ weightFactor;
                    }
                }
            }

        double normFactor = getQuantRectFactor(imageROI.height, imageROI.width, 8);

        float normWidth = (curPoint.x - imageROI.x) * normFactor;
        float normHeight =(curPoint.y - imageROI.y) * normFactor;

        curPatch.at<float>(0, pixelValCount++) = normWidth;
        curPatch.at<float>(0, pixelValCount++) = normHeight;

        cv::Mat matchIndices(3, 1, CV_32S);  //CV_16S
        cv::Mat matchDists(3, 1, CV_32F);

        // search for the nearest patch
        if (curPosition == 2)
        {
            treeLeftWeight.knnSearch(curPatch, matchIndices, matchDists, 3, cv::flann::SearchParams(cvflann::FLANN_CHECKS_AUTOTUNED, 0, true));
            // cvflann::FLANN_CHECKS_AUTOTUNED
//            cout << " left side top/bottom/left/right " << bkgdCountTop << " " << bkgdCountBottom
//                 << " " << bkgdCountLeft << " " << bkgdCountRight << endl;
        }

        if (curPosition == 4)
        {
            treeRightWeight.knnSearch(curPatch, matchIndices, matchDists, 3, cv::flann::SearchParams(cvflann::FLANN_CHECKS_AUTOTUNED, 0, true));
            // cvflann::FLANN_CHECKS_AUTOTUNED
//            cout << "right side top/bottom/left/right " << bkgdCountTop << " " << bkgdCountBottom
//                 << " " << bkgdCountLeft << " " << bkgdCountRight << endl;
        }

        if (curPosition == 1)
        {
            treeTopWeight.knnSearch(curPatch, matchIndices, matchDists, 3, cv::flann::SearchParams(cvflann::FLANN_CHECKS_AUTOTUNED, 0, true));
            // cvflann::FLANN_CHECKS_AUTOTUNED
//            cout << "top side top/bottom/left/right " << bkgdCountTop << " " << bkgdCountBottom
//                 << " " << bkgdCountLeft << " " << bkgdCountRight << endl;
        }

        if (curPosition == 3)
        {
            treeBottomWeight.knnSearch(curPatch, matchIndices, matchDists, 3, cv::flann::SearchParams(cvflann::FLANN_CHECKS_AUTOTUNED, 0, true));
            // cvflann::FLANN_CHECKS_AUTOTUNED
//            cout << "bottom side top/bottom/left/right " << bkgdCountTop << " " << bkgdCountBottom
//                 << " " << bkgdCountLeft << " " << bkgdCountRight << endl;
        }

        float patchDist = matchDists.at<float>(0,0)/(patchSize * (patchSize) * 3 + 2);

        int matchPointIndex = matchIndices.at<int>(0, 0);
        Point matchPoint = bkgdPatchPointsList[matchPointIndex];

//        matchPointIndex = matchIndices.at<int>(1, 0);
//        Point matchPoint2 = bkgdPatchPointsList[matchPointIndex];


//        cout << " point: " <<curPoint <<  "match dist: " << matchDists<< " match index: "<<matchIndices << " " << matchPointIndex<< endl;
//        cout << " bkgd point 1:  x/y: " << matchPoint.x << " " <<matchPoint.y  << endl;


        matchDist.push_back(PointMatchDist(curPoint, matchPoint, patchDist));
    }

//    cout << " matchDist.size() " << matchDist.size() << endl;

    CompareMatchDist compObj;

    std::sort(matchDist.begin(), matchDist.end(), compObj);

    for (int icount = 0; icount < (int)matchDist.size(); icount++)
    {
        PointMatchDist curMatchPoint = matchDist[icount];

//        cout << curMatchPoint.matchDist << " " << curMatchPoint.curPoint << " " << curMatchPoint.matchPoint << endl;

        // after the first point, if the point matchDist greater than (20*20), discard it.
        if ((icount > 0) && (curMatchPoint.matchDist > 400))
        {
            matchDist.erase(matchDist.begin() + icount, matchDist.end());
            break;
        }
    }

//    cout << " after truncate, matchDist.size() " << matchDist.size() << endl;
}

void TaoTextureAutoCompletion::searchNsortBkgdStdVariance(vector<BoundaryPointPosition> &points, vector<PointMatchDist> &matchDist, Mat &inputImage)
{
    if (points.size() == 0)
        return;

//    cout << "inside searchNsortMatchPointDist " << endl;

    for (int ptCount = 0; ptCount < (int)points.size(); ptCount++)  //points.size()
    {
        // get the current point best match
        struct BoundaryPointPosition curBoundPoint = points[ptCount];
        Point curPoint = curBoundPoint.boundPoint;
        int curPosition = curBoundPoint.pos;

        Mat curPatch(1, patchSize * patchSize * 3 + 2, CV_32F);
        int pixelValCount = 0;

        int halfPatchSize = patchSize / 2;

        int bkgdCountLeft = 0;
        int bkgdCountRight = 0;
        int bkgdCountTop = 0;
        int bkgdCountBottom = 0;


        // make a patch for search
        for (int curRow = curPoint.y - halfPatchSize; curRow <= curPoint.y + halfPatchSize; curRow++)
            for (int curCol = curPoint.x - halfPatchSize; curCol <= curPoint.x + halfPatchSize; curCol++)
            {
                Vec3b curImagePointVal = inputImage.at<Vec3b>(curRow, curCol);

                if (curPosition == 1)
                {
                    if (curRow < curPoint.y)
                    {
                        curPatch.at<float>(0, pixelValCount++) = (float)curImagePointVal[0];
                        curPatch.at<float>(0, pixelValCount++) = (float)curImagePointVal[1];
                        curPatch.at<float>(0, pixelValCount++) = (float)curImagePointVal[2];

                        if (inputMask.at<uchar>(curRow, curCol) == 0)
                            bkgdCountTop++;
                    }
                    else
                    {
                        curPatch.at<float>(0, pixelValCount++) = (float)curImagePointVal[0]/ weightFactor;
                        curPatch.at<float>(0, pixelValCount++) = (float)curImagePointVal[1]/ weightFactor;
                        curPatch.at<float>(0, pixelValCount++) = (float)curImagePointVal[2]/ weightFactor;
                    }
                }

                if (curPosition == 3)
                {
                    if (curRow > curPoint.y)
                    {
                        curPatch.at<float>(0, pixelValCount++) = (float)curImagePointVal[0];
                        curPatch.at<float>(0, pixelValCount++) = (float)curImagePointVal[1];
                        curPatch.at<float>(0, pixelValCount++) = (float)curImagePointVal[2];

                        if (inputMask.at<uchar>(curRow, curCol) == 0)
                            bkgdCountBottom++;
                    }
                    else
                    {
                        curPatch.at<float>(0, pixelValCount++) = (float)curImagePointVal[0]/ weightFactor;
                        curPatch.at<float>(0, pixelValCount++) = (float)curImagePointVal[1]/ weightFactor;
                        curPatch.at<float>(0, pixelValCount++) = (float)curImagePointVal[2]/ weightFactor;
                    }
                }

                if (curPosition == 2)
                {
                    if (curCol < curPoint.x)
                    {
                        curPatch.at<float>(0, pixelValCount++) = (float)curImagePointVal[0];
                        curPatch.at<float>(0, pixelValCount++) = (float)curImagePointVal[1];
                        curPatch.at<float>(0, pixelValCount++) = (float)curImagePointVal[2];

                        if (inputMask.at<uchar>(curRow, curCol) == 0)
                            bkgdCountLeft++;
                    }
                    else
                    {
                        curPatch.at<float>(0, pixelValCount++) = (float)curImagePointVal[0]/ weightFactor;
                        curPatch.at<float>(0, pixelValCount++) = (float)curImagePointVal[1]/ weightFactor;
                        curPatch.at<float>(0, pixelValCount++) = (float)curImagePointVal[2]/ weightFactor;
                    }
                }

                if (curPosition == 4)
                {
                    if (curCol > curPoint.x)
                    {
                        curPatch.at<float>(0, pixelValCount++) = (float)curImagePointVal[0];
                        curPatch.at<float>(0, pixelValCount++) = (float)curImagePointVal[1];
                        curPatch.at<float>(0, pixelValCount++) = (float)curImagePointVal[2];

                        if (inputMask.at<uchar>(curRow, curCol) == 0)
                            bkgdCountRight++;
                    }
                    else
                    {
                        curPatch.at<float>(0, pixelValCount++) = (float)curImagePointVal[0]/ weightFactor;
                        curPatch.at<float>(0, pixelValCount++) = (float)curImagePointVal[1]/ weightFactor;
                        curPatch.at<float>(0, pixelValCount++) = (float)curImagePointVal[2]/ weightFactor;
                    }
                }
            }

        double normFactor = getQuantRectFactor(imageROI.height, imageROI.width, 8);

        float normWidth = (curPoint.x - imageROI.x) * normFactor;
        float normHeight =(curPoint.y - imageROI.y) * normFactor;

        curPatch.at<float>(0, pixelValCount++) = normWidth;
        curPatch.at<float>(0, pixelValCount++) = normHeight;

        cv::Mat matchIndices(3, 1, CV_32S);  //CV_16S
        cv::Mat matchDists(3, 1, CV_32F);

        // search for the nearest patch
        if (curPosition == 2)
        {
            treeLeftWeight.knnSearch(curPatch, matchIndices, matchDists, 3, cv::flann::SearchParams(cvflann::FLANN_CHECKS_AUTOTUNED, 0, true));
            // cvflann::FLANN_CHECKS_AUTOTUNED
//            cout << " left side top/bottom/left/right " << bkgdCountTop << " " << bkgdCountBottom
//                 << " " << bkgdCountLeft << " " << bkgdCountRight << endl;
        }

        if (curPosition == 4)
        {
            treeRightWeight.knnSearch(curPatch, matchIndices, matchDists, 3, cv::flann::SearchParams(cvflann::FLANN_CHECKS_AUTOTUNED, 0, true));
            // cvflann::FLANN_CHECKS_AUTOTUNED
//            cout << "right side top/bottom/left/right " << bkgdCountTop << " " << bkgdCountBottom
//                 << " " << bkgdCountLeft << " " << bkgdCountRight << endl;
        }

        if (curPosition == 1)
        {
            treeTopWeight.knnSearch(curPatch, matchIndices, matchDists, 3, cv::flann::SearchParams(cvflann::FLANN_CHECKS_AUTOTUNED, 0, true));
            // cvflann::FLANN_CHECKS_AUTOTUNED
//            cout << "top side top/bottom/left/right " << bkgdCountTop << " " << bkgdCountBottom
//                 << " " << bkgdCountLeft << " " << bkgdCountRight << endl;
        }

        if (curPosition == 3)
        {
            treeBottomWeight.knnSearch(curPatch, matchIndices, matchDists, 3, cv::flann::SearchParams(cvflann::FLANN_CHECKS_AUTOTUNED, 0, true));
            // cvflann::FLANN_CHECKS_AUTOTUNED
//            cout << "bottom side top/bottom/left/right " << bkgdCountTop << " " << bkgdCountBottom
//                 << " " << bkgdCountLeft << " " << bkgdCountRight << endl;
        }

        float patchDist = matchDists.at<float>(0,0)/(patchSize * (patchSize) * 3 + 2);

        // for standard deviation of the bkgd patch
//        Mat patchDeviation1(inputImage, Rect(curPoint.x - halfPatchSize, curPoint.y - halfPatchSize, patchSize, halfPatchSize));
//        Mat patchDeviation2(inputImage, Rect(curPoint.x - halfPatchSize, curPoint.y - halfPatchSize, halfPatchSize, patchSize));
//        Mat patchDeviation3(inputImage, Rect(curPoint.x - halfPatchSize, curPoint.y + halfPatchSize, patchSize, halfPatchSize));
//        Mat patchDeviation4(inputImage, Rect(curPoint.x + halfPatchSize, curPoint.y - halfPatchSize, halfPatchSize, patchSize));

//        Mat tmpMean, tmpStdDev;
//        double devSum = 0.0;

//        switch (curPosition)
//        {
//        case 1:
//            meanStdDev(patchDeviation1, tmpMean, tmpStdDev);

//            devSum = tmpStdDev.at<double>(0, 0) + tmpStdDev.at<double>(0, 1) +  tmpStdDev.at<double>(0, 2);

//            break;

//        case 2:
//            meanStdDev(patchDeviation2, tmpMean, tmpStdDev);

//            devSum = tmpStdDev.at<double>(0, 0) + tmpStdDev.at<double>(0, 1) +  tmpStdDev.at<double>(0, 2);

//            break;

//        case 3:
//            meanStdDev(patchDeviation3, tmpMean, tmpStdDev);

//            devSum = tmpStdDev.at<double>(0, 0) + tmpStdDev.at<double>(0, 1) +  tmpStdDev.at<double>(0, 2);

//            break;

//        case 4:
//            meanStdDev(patchDeviation4, tmpMean, tmpStdDev);

//            devSum = tmpStdDev.at<double>(0, 0) + tmpStdDev.at<double>(0, 1) +  tmpStdDev.at<double>(0, 2);

//            break;

//        default:
//            ;
//        }

//        patchDist = devSum;

        int matchPointIndex = matchIndices.at<int>(0, 0);
        Point matchPoint = bkgdPatchPointsList[matchPointIndex];

//        matchPointIndex = matchIndices.at<int>(1, 0);
//        Point matchPoint2 = bkgdPatchPointsList[matchPointIndex];


//        cout << " point: " <<curPoint <<  "match dist: " << matchDists<< " match index: "<<matchIndices << " " << matchPointIndex<< endl;
//        cout << " bkgd point 1:  x/y: " << matchPoint.x << " " <<matchPoint.y  << endl;


        matchDist.push_back(PointMatchDist(curPoint, matchPoint, patchDist));
    }

//    cout << " matchDist.size() " << matchDist.size() << endl;

    CompareMatchDist compObj;

    std::sort(matchDist.begin(), matchDist.end(), compObj);

    // for sort by standard deviation
//    CompareStdVariance compVariance;
//    std::sort(matchDist.begin(), matchDist.end(), compVariance);

    for (int icount = 0; icount < (int)matchDist.size(); icount++)
    {
        PointMatchDist curMatchPoint = matchDist[icount];

//        cout << curMatchPoint.matchDist << " " << curMatchPoint.curPoint << " " << curMatchPoint.matchPoint << endl;

        // after the first point, if the point matchDist greater than (20*20), discard it.
        if ((icount > 0) && (curMatchPoint.matchDist > 400))
        {
            matchDist.erase(matchDist.begin() + icount, matchDist.end());
            break;
        }
    }

//    cout << " after truncate, matchDist.size() " << matchDist.size() << endl;
}



void TaoTextureAutoCompletion::copyPatch(Mat &inImage, Mat &inMask, vector<PointMatchDist> &points)
{
    int halfSize = patchSize / 2;

//    cout << "in copyPatch" << endl;

    for (int count = 0; count < (int)points.size(); count++)
    {
        PointMatchDist curMatchPoint = points[count];

        Point srcPoint = curMatchPoint.matchPoint;
        Point dstPoint = curMatchPoint.curPoint;

//        cout << "src point: " << srcPoint << endl;

//        cout << "dst point: " << dstPoint << endl;

//        int pixelVal = inMask.at<uchar>(dstPoint.y, dstPoint.x);
//        cout << " mask value: " << pixelVal << endl;

        bool close2EdgeDstPoint;

        close2EdgeDstPoint = (dstPoint.y - halfSize == 0) ||
                            (dstPoint.x - halfSize == 0) ||
                            (dstPoint.y + halfSize == inMask.rows - 1) ||
                            (dstPoint.x + halfSize == inMask.cols -1);

        // if the destination point is not updated, or the edge point is not updated and destination center is in the background
        // then copy patch and mask.
        if ((inMask.at<uchar>(dstPoint.y, dstPoint.x) > 0) || close2EdgeDstPoint)
        {
            //            cout << " src rect: " << srcPatchROI << endl;
            //            cout << " dst rect: " << dstPatchROI << endl;
            Rect srcPatchROI(srcPoint.x - halfSize, srcPoint.y - halfSize, patchSize, patchSize);
            Rect dstPatchROI(dstPoint.x - halfSize, dstPoint.y - halfSize, patchSize, patchSize);

            Mat srcImagePatch(inImage, srcPatchROI);
            Mat dstImagePatch(inImage, dstPatchROI);
            Mat srcMaskPatch(inMask, srcPatchROI);
            Mat dstMaskPatch(inMask, dstPatchROI);

//            srcImagePatch.copyTo(dstImagePatch);
            for (int irow = 0; irow < dstPatchROI.height; irow++)
                for (int icol = 0; icol < dstPatchROI.width; icol++)
                {
                    if (dstMaskPatch.at<uchar>(irow, icol) > 0)
                    {
                        dstImagePatch.at<Vec3b>(irow, icol) = srcImagePatch.at<Vec3b>(irow, icol);
                    }
                }

            srcMaskPatch.copyTo(dstMaskPatch);
        }
    }
}



// compute the position relative to center of imageROI
// left to right -1, 0 ,1
// top to bottom -1,, 0, 1
/*
void TaoTextureAutoCompletion::computeLine2CenterPos(vector<Point> &line2centerPos)
{
    if (bkgdBorderLineList.size() == 0)
        return;

    for (int icount = 0; icount < bkgdBorderLineList.size(); icount++)
    {
        struct CVLine curLine = bkgdBorderLineList[icount];
        Point curPoint1 = (Point)curLine.point1;
        Point curPoint2 = (Point)curLine.point2;

        // if the line segment is not in rectangle
        if (!cv::clipLine(imageROI, curPoint1, curPoint2))
        {
            bkgdBorderLineList.erase(bkgdBorderLineList.begin() + icount);
            icount--;

            continue;
        }

        Point curPos;
        Point centerPos;

        centerPos.x = maskROI.x + maskROI.width / 2;
        centerPos.y = maskROI.y + maskROI.height / 2;

        if (abs(curPoint1.x - curPoint2.x) < 1)
            curPos.x = 0;
        else {
            int yValue = (curPoint1.y - curPoint2.y) / (curPoint1.x - curPoint2.x)
                    * (centerPos.x - curPoint2.x) + curPoint2.y;

            if (yValue <= centerPos.y)
                curPos.y = -1;
            else
                curPos.y = 1;
        }


        if (abs(curPoint1.y - curPoint2.y) < 1)
            curPos.y = 0;
        else {
            int xValue = (curPoint1.x - curPoint2.x) / (curPoint1.y - curPoint2.y)
                    * (centerPos.y - curPoint2.y) + curPoint2.x;

            if (xValue <= centerPos.x)
                curPos.x = -1;
            else
                curPos.x = 1;
        }


        line2centerPos.push_back(curPos);

//        cout << curPoint1 << "  " << curPoint2 << endl;
//        cout << " imageROI center: " << centerPos << endl;
//        cout << " bound line position: " << curPos << endl;
    }
}
*/

// check if the input point is with in the line limit
/*
bool TaoTextureAutoCompletion::checkIsPointInBkgdBound(Point inputPoint, vector<Point> &lineBoundCenterPosList)
{
    for (int icount = 0; icount < bkgdBorderLineList.size(); icount++)
    {
        struct CVLine curLine = bkgdBorderLineList[icount];
        Point curPoint1 = (Point)curLine.point1;
        Point curPoint2 = (Point)curLine.point2;

        Point curPos;
        Point centerPos;

        if (abs(curPoint1.x - curPoint2.x) < 1)
            curPos.x = 0;
        else
        {
            int yValue = (curPoint1.y - curPoint2.y) / (curPoint1.x - curPoint2.x)
                    * (inputPoint.x - curPoint2.x) + curPoint2.y;

            if (yValue <= inputPoint.y)
                curPos.y = -1;
            else
                curPos.y = 1;
        }


        if (abs(curPoint1.y - curPoint2.y) < 1)
            curPos.y = 0;
        else
        {
            int xValue = (curPoint1.x - curPoint2.x) / (curPoint1.y - curPoint2.y)
                    * (inputPoint.y - curPoint2.y) + curPoint2.x;

            if (xValue <= inputPoint.x)
                curPos.x = -1;
            else
                curPos.x = 1;
        }

        Point lineBoundCenterPos = lineBoundCenterPosList[icount];

        if ((curPos.x != lineBoundCenterPos.x) || (curPos.y != lineBoundCenterPos.y))
            return false;
    }

    return true;
}
*/

void TaoTextureAutoCompletion::releaseMem()
{
    treeLeftWeight.~Index();
    treeRightWeight.~Index();
    treeTopWeight.~Index();
    treeBottomWeight.~Index();

    bkgdPatchPointsList.clear();
//    bkgdBorderLineList.clear();
//    line2CenterPos.clear();

    bkgdPatchMatLeft.release();
    bkgdPatchMatRight.release();
    bkgdPatchMatTop.release();
    bkgdPatchMatBottom.release();
}
