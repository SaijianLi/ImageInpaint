

#include "taotexturemanualcompletion.h"

#include <QTime>


TaoTextureManualCompletion::TaoTextureManualCompletion(Mat &inImage, Mat &inMask,
                                                 Rect inBkgdROI, Rect inImageROI, Rect inMaskROI, int pSize, int pixSpacing)
{
    inputImage = inImage;
    inputMask = inMask;
    bkgdROI = inBkgdROI;
    imageROI = inImageROI;
    maskROI = inMaskROI;
    patchSize = pSize;
    bkgdPixSpacing = pixSpacing;

//    cout << " in TaoTextureManualCompletion" << endl;
}


TaoTextureManualCompletion::~TaoTextureManualCompletion()
{

}


int TaoTextureManualCompletion::completeImage()
{
    int iResultBuildBkgdTree;

    QTime curTime;

    curTime.start();

    iResultBuildBkgdTree = buildBkgdTree4SideWt();

    if (iResultBuildBkgdTree != 0)
        return 1;

//    cout << " build tree time: " << curTime.elapsed() <<endl;

    vector<struct BoundaryPointPosition> boundaryPoints;

    curTime.restart();

//    cout << " imageROI: " << imageROI.x << " " << imageROI.y << " " << imageROI.width << " " << imageROI.height << endl;

    getFrgdBoundPoints(boundaryPoints,  inputMask, imageROI);

//    cout << " boundary points: " << boundaryPoints.size() << endl;
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
    }

    releaseMem();

    return 0;
}


int TaoTextureManualCompletion::buildBkgdTree4SideWt()
{
    int halfPatchSize = patchSize / 2;
    int startx = bkgdROI.x;
    int starty = bkgdROI.y;
    int endx = bkgdROI.x + bkgdROI.width - 1;
    int endy = bkgdROI.y + bkgdROI.height - 1;

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
        double quantFactor = getQuantRectFactor(bkgdROI.height, bkgdROI.width, 8);

        float quantWidth = (curPoint.x - bkgdROI.x) * quantFactor;
        float quantHeight = (curPoint.y - bkgdROI.y) * quantFactor;

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


double TaoTextureManualCompletion::getQuantRectFactor(int height, int width, int stdSize)
{
    return (height > width? (stdSize* 1.0 / height) : ( stdSize* 1.0/ width));
}


void TaoTextureManualCompletion::getFrgdBoundPoints(vector<struct BoundaryPointPosition> &boundaryPoints, Mat &inputMask, Rect &imgROI)
{
    int startx = imgROI.x;
    int starty = imgROI.y;
    int endx = imgROI.x + imgROI.width;
    int endy = imgROI.y + imgROI.height;
    Point inPoint, outPoint;
    int position;

//    cout << " image ROI: " << startx <<  " " << starty << " " << endx << " " << endy << endl;

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
//                cout << " boundary point: " << outPoint.x << " " << outPoint.y << endl;
            }
        }

//    cout <<" patch size: " << patchSize << "  " << patchSize / 2 << endl;

//    cout<< "boundary points count: " << boundaryPoints.size() << endl;
}


bool TaoTextureManualCompletion::checkIsFrgdBoundPoint(Point *outPoint, int *position, const Point &inPoint, Mat &mask)
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


bool TaoTextureManualCompletion::checkIsImagePoint(int pointx, int pointy, int cols, int rows)
{
    if ((pointx >= 0) && (pointx < cols) && (pointy >= 0) && (pointy < rows))
        return true;
    else
        return false;
}


void TaoTextureManualCompletion::searchNsortMatchPointDist(vector<BoundaryPointPosition> &points, vector<PointMatchDist> &matchDist, Mat &inputImage)
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

        double normFactor = getQuantRectFactor(bkgdROI.height, bkgdROI.width, 8);

        float normWidth = (curPoint.x - bkgdROI.x) * normFactor;
        float normHeight =(curPoint.y - bkgdROI.y) * normFactor;

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

void TaoTextureManualCompletion::searchNsortBkgdStdVariance(vector<BoundaryPointPosition> &points, vector<PointMatchDist> &matchDist, Mat &inputImage)
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

        double normFactor = getQuantRectFactor(bkgdROI.height, bkgdROI.width, 8);

        float normWidth = (curPoint.x - bkgdROI.x) * normFactor;
        float normHeight =(curPoint.y - bkgdROI.y) * normFactor;

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



void TaoTextureManualCompletion::copyPatch(Mat &inImage, Mat &inMask, vector<PointMatchDist> &points)
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


void TaoTextureManualCompletion::releaseMem()
{
    treeLeftWeight.~Index();
    treeRightWeight.~Index();
    treeTopWeight.~Index();
    treeBottomWeight.~Index();

    bkgdPatchPointsList.clear();

    bkgdPatchMatLeft.release();
    bkgdPatchMatRight.release();
    bkgdPatchMatTop.release();
    bkgdPatchMatBottom.release();
}

