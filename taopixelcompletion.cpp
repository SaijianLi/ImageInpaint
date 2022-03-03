#include "taopixelcompletion.h"

TaoPixelCompletion::TaoPixelCompletion(Mat &inImage, Mat &inMask, Rect inMaskROI, int bkgdPixelRadius)
{
    inputImage = inImage;
    inputMask = inMask;

    maskROI = inMaskROI;

    iBkgdPixelRadius = bkgdPixelRadius;
}


TaoPixelCompletion::~TaoPixelCompletion()
{

}

void TaoPixelCompletion::completeImage()
{
    flann::Index tree;
    Mat bkgdFeatures;

    buildKDTree(tree, bkgdFeatures);

    vector<Point> edgePoints;


    findMaskEdgePoints(edgePoints, inputMask, maskROI);

    for (int icount = 0; icount < (int)edgePoints.size(); icount++)
    {
        ;//cout << edgePoints[icount] << endl;
    }


//    imshow("pixel inpaint0", inputImage);

//    imshow("mask inpaint0", inputMask);

//    while (edgePoints.size() >0)
    for (int icount = 0; icount < 2; icount++)
    {
        cout << "edge points: " << edgePoints.size() << endl;

        // for the edge points of mask, inpaint it
//        completeEdge(tree, edgePoints, bkgdFeatures);
//        bkgdROI.x = 368;
//        bkgdROI.y = 373;
//        bkgdROI.width = 20;
//        bkgdROI.height = 13;

        Mat patch(inputImage, Rect(368, 373, 10, 13));

        for (int icount = 0; icount < (int)edgePoints.size(); icount++)
        {
            Mat dstPatch(inputImage, Rect(edgePoints[icount].x - 5, edgePoints[icount].y - 6, 10, 13));

            patch.copyTo(dstPatch);

        }

        edgePoints.clear();

        findMaskEdgePoints(edgePoints, inputMask, maskROI);

//        imshow(" inpaint edge", inputImage);
    }

//    imshow("pixel inpaint", inputImage);

//    imshow("mask inpaint", inputMask);
}


void TaoPixelCompletion::completeEdge(flann::Index &tree, vector<Point> &edgePoints, Mat &bkgdFeatures)
{
    cout << " in complete edge: " << edgePoints.size() << endl;

    for (int icount = 0; icount < (int)edgePoints.size(); icount++)
    {
        Vec3b pointVal[8];
        float pixelDist[8];
        int featureRowNum[8];

        Point curPoint = edgePoints[icount];
        int curRow = curPoint.y;
        int curCol = curPoint.x;

        Vec3b pixelMean;

//        getAdjacentPixelMean(curPoint, pixelMean);

        // check if current point in the tree

        cout << pixelMean << endl;

        int rowNum;
        float curDist = getSearchDist(inputImage.at<Vec3b>(curRow, curCol), tree, &rowNum);
        //curPoint,
//        float curDist = getSearchDist(curPoint, pixelMean, tree, &rowNum);


//        cout << " row/col: " << curRow << " " << curCol << " " << curDist << endl;

//        if (curDist < 12.01)
        {
            inputMask.at<uchar>(curRow, curCol) = (uchar)0;

//            cout << inputImage.at<Vec3b>(curRow, curCol) << endl;
//            cout << bkgdFeatures.at<float>(rowNum, 0) << " "<< bkgdFeatures.at<float>(rowNum, 1) << " " <<bkgdFeatures.at<float>(rowNum, 2) <<endl;

            inputImage.at<Vec3b>(curRow, curCol)[0] = (uchar)bkgdFeatures.at<float>(rowNum, 0);
            inputImage.at<Vec3b>(curRow, curCol)[1] = (uchar)bkgdFeatures.at<float>(rowNum, 1);
            inputImage.at<Vec3b>(curRow, curCol)[2] = (uchar)bkgdFeatures.at<float>(rowNum, 2);

            cout << inputImage.at<Vec3b>(curRow, curCol) << endl;

            cout << " row/col: " << curRow << " " << curCol << " dist: " << curDist << " rowNum: " << rowNum << endl;
//            cout << " row/col: " << curRow << " " << curCol << " is bkgd point." << endl;

            continue;
        }

        pointVal[0] = inputImage.at<Vec3b>(curRow -1, curCol-1);
        pointVal[1] = inputImage.at<Vec3b>(curRow -1, curCol);
        pointVal[2] = inputImage.at<Vec3b>(curRow -1, curCol+1);
        pointVal[3] = inputImage.at<Vec3b>(curRow, curCol-1);
        pointVal[4] = inputImage.at<Vec3b>(curRow, curCol+1);
        pointVal[5] = inputImage.at<Vec3b>(curRow +1, curCol-1);
        pointVal[6] = inputImage.at<Vec3b>(curRow +1, curCol);
        pointVal[7] = inputImage.at<Vec3b>(curRow +1, curCol+1);

        for (int icount = 0; icount < 8; icount++)
            pixelDist[icount] = getSearchDist(pointVal[icount], tree, &featureRowNum[icount]);
        //Point(curCol, curRow),


        // sort the edgePoints of distance
        float pixelVal0 = 0;
        float pixelVal1 = 0;
        float pixelVal2 = 0;

        int minDistCount = 0;
        for (int icount = 0; icount < 8; icount++)
        {
            if (pixelDist[icount] < 24.1)
            {
                minDistCount++;

                pixelVal0 += bkgdFeatures.at<float>(featureRowNum[icount], 0);
                pixelVal1 += bkgdFeatures.at<float>(featureRowNum[icount], 1);
                pixelVal2 += bkgdFeatures.at<float>(featureRowNum[icount], 2);

                inputImage.at<Vec3b>(curRow, curCol)[0] = (uchar)(pixelVal0 / minDistCount);
                inputImage.at<Vec3b>(curRow, curCol)[1] = (uchar)(pixelVal1 / minDistCount);
                inputImage.at<Vec3b>(curRow, curCol)[2] = (uchar)(pixelVal2 / minDistCount);

                inputMask.at<uchar>(curRow, curCol) = (uchar)0;
            }
        }

//        if (minDistCount >= 2)
//        {
//            inputImage.at<Vec3b>(curRow, curCol)[0] = (uchar)(pixelVal0 / minDistCount);
//            inputImage.at<Vec3b>(curRow, curCol)[1] = (uchar)(pixelVal1 / minDistCount);
//            inputImage.at<Vec3b>(curRow, curCol)[2] = (uchar)(pixelVal2 / minDistCount);

//            inputMask.at<uchar>(curRow, curCol) = (uchar)0;

//            cout << "row/col: " << curRow << " " << curCol << endl;
//        }
    }
}


void TaoPixelCompletion::getAdjacentPixelMean(Point curPoint, Vec3b &pixelVal)
{
    int icount = 0;
    int val1, val2, val3;
    int curRow = curPoint.y;
    int curCol = curPoint.x;

    val1 = val2 = val3 =0;

    if (checkIsImagePoint(curCol-1, curRow-1, inputMask.cols, inputMask.rows)
            && (inputMask.at<uchar>(curRow-1, curCol-1) == 0))
    {
        val1 += inputImage.at<Vec3b>(curRow -1, curCol-1)[0];
        val2 += inputImage.at<Vec3b>(curRow -1, curCol-1)[1];
        val3 += inputImage.at<Vec3b>(curRow -1, curCol-1)[2];

        icount++;
    }

    if (checkIsImagePoint(curCol, curRow -1, inputMask.cols, inputMask.rows)
            && (inputMask.at<uchar>(curRow-1, curCol) == 0))
    {
        val1 += inputImage.at<Vec3b>(curRow -1, curCol)[0];
        val2 += inputImage.at<Vec3b>(curRow -1, curCol)[1];
        val3 += inputImage.at<Vec3b>(curRow -1, curCol)[2];

        icount++;
    }

    if (checkIsImagePoint(curCol+1, curRow -1, inputMask.cols, inputMask.rows)
            && (inputMask.at<uchar>(curRow-1, curCol+1) == 0))
    {
        val1 += inputImage.at<Vec3b>(curRow -1, curCol+1)[0];
        val2 += inputImage.at<Vec3b>(curRow -1, curCol+1)[1];
        val3 += inputImage.at<Vec3b>(curRow -1, curCol+1)[2];

        icount++;
    }

    // second row
    if (checkIsImagePoint(curCol-1, curRow, inputMask.cols, inputMask.rows)
            && (inputMask.at<uchar>(curRow, curCol-1) == 0))
    {
        val1 += inputImage.at<Vec3b>(curRow, curCol-1)[0];
        val2 += inputImage.at<Vec3b>(curRow, curCol-1)[1];
        val3 += inputImage.at<Vec3b>(curRow, curCol-1)[2];

        icount++;
    }

    if (checkIsImagePoint(curCol+1, curRow, inputMask.cols, inputMask.rows)
            && (inputMask.at<uchar>(curRow, curCol+1) == 0))
    {
        val1 += inputImage.at<Vec3b>(curRow, curCol+1)[0];
        val2 += inputImage.at<Vec3b>(curRow, curCol+1)[1];
        val3 += inputImage.at<Vec3b>(curRow, curCol+1)[2];

        icount++;
    }

    // third row
    if (checkIsImagePoint(curCol-1, curRow+1, inputMask.cols, inputMask.rows)
            && (inputMask.at<uchar>(curRow+1, curCol-1) == 0))
    {
        val1 += inputImage.at<Vec3b>(curRow +1, curCol-1)[0];
        val2 += inputImage.at<Vec3b>(curRow +1, curCol-1)[1];
        val3 += inputImage.at<Vec3b>(curRow +1, curCol-1)[2];

        icount++;
    }

    if (checkIsImagePoint(curCol, curRow + 1, inputMask.cols, inputMask.rows)
            && (inputMask.at<uchar>(curRow + 1, curCol) == 0))
    {
        val1 += inputImage.at<Vec3b>(curRow +1, curCol)[0];
        val2 += inputImage.at<Vec3b>(curRow +1, curCol)[1];
        val3 += inputImage.at<Vec3b>(curRow +1, curCol)[2];

        icount++;
    }

    if (checkIsImagePoint(curCol+1, curRow+1, inputMask.cols, inputMask.rows)
            && (inputMask.at<uchar>(curRow+1, curCol+1) == 0))
    {
        val1 += inputImage.at<Vec3b>(curRow +1, curCol+1)[0];
        val2 += inputImage.at<Vec3b>(curRow +1, curCol+1)[1];
        val3 += inputImage.at<Vec3b>(curRow +1, curCol+1)[2];

        icount++;
    }

    pixelVal[0] = (uchar)val1/icount;
    pixelVal[1] = (uchar)val2/icount;
    pixelVal[2] = (uchar)val3/icount;

    cout << " adjacent pixel count: " << icount << endl;
    cout << pixelVal << endl;

}


float TaoPixelCompletion::getSearchDist(Vec3b &pointVal, flann::Index &tree, int *featureRowNum)
//Point curPoint,
{
    cv::Mat matchIndices(3, 1, CV_32S);  //CV_16S
    cv::Mat matchDists(3, 1, CV_32F);

    Mat query(1, 3, CV_32F);

    query.at<float>(0, 0) = (float)pointVal[0];
    query.at<float>(0, 1) = (float)pointVal[1];
    query.at<float>(0, 2) = (float)pointVal[2];

    tree.knnSearch(query, matchIndices, matchDists, 3, cv::flann::SearchParams(cvflann::FLANN_CHECKS_AUTOTUNED, 0, true));

    float pixelDist = matchDists.at<float>(0,0);
    int pixelIndex = matchIndices.at<int>(0, 0);

    *featureRowNum = pixelIndex;

    return pixelDist;
}


void TaoPixelCompletion::buildKDTree(flann::Index &tree, Mat &bkgdFeatures)
{
    // get bkgd ROI
    Rect bkgdROI;

    getBkgdROI(&bkgdROI);

    cout << "maskROI: " << maskROI << endl;

    cout << "bkgdROI: " << bkgdROI << endl;

    // make features

    //get number of bkgd points
    int numBkgdPoints = 0;
    vector<Point> bkgdPoints;

    bkgdROI.x = 368;
    bkgdROI.y = 373;
    bkgdROI.width = 20;
    bkgdROI.height = 13;

    for (int irow = bkgdROI.y; irow < bkgdROI.y + bkgdROI.height; irow++)
    {
        for (int icol = bkgdROI.x; icol < bkgdROI.x +bkgdROI.width; icol++)
        {
            if (inputMask.at<uchar>(irow, icol) == (uchar)0)
            {
                numBkgdPoints++;
                bkgdPoints.push_back(Point(icol, irow));
            }
        }
    }

//    imshow(" input mask", inputMask);

    cout << " number of bkgd ponts: " << numBkgdPoints << endl;

    cout << " bkgd points: " << bkgdPoints.size() << endl;

    bkgdFeatures.create(numBkgdPoints, 3, CV_32F);

    bkgdFeatures = Scalar::all(0);

    for (int icount = 0; icount < (int)bkgdPoints.size(); icount++)
    {
        Point curPoint = bkgdPoints[icount];
        Vec3b curPointVal = inputImage.at<Vec3b>(curPoint.y, curPoint.x);

        bkgdFeatures.at<float>(icount, 0) = (float)curPointVal[0];
        bkgdFeatures.at<float>(icount, 1) = (float)curPointVal[1];
        bkgdFeatures.at<float>(icount, 2) = (float)curPointVal[2];

//        cout << curPoint << " " << curPointVal << endl;
    }

//    cout << bkgdFeatures << endl;
    // build KDTree
    flann::KDTreeIndexParams indexParams(4);

    tree.build(bkgdFeatures, indexParams);
}


void TaoPixelCompletion::getBkgdROI(Rect *bkgdROI)
{
    bkgdROI->x = maskROI.tl().x - iBkgdPixelRadius >= 0 ? maskROI.tl().x - iBkgdPixelRadius : 0;
    bkgdROI->y = maskROI.tl().y - iBkgdPixelRadius >= 0 ? maskROI.tl().y - iBkgdPixelRadius : 0;

    int iBottomRightX, iBottomRightY;

    iBottomRightX = maskROI.br().x + iBkgdPixelRadius <= inputMask.cols -1 ? maskROI.br().x + iBkgdPixelRadius : inputMask.cols -1;
    bkgdROI->width = iBottomRightX - bkgdROI->x;

    iBottomRightY = maskROI.br().y + iBkgdPixelRadius <= inputMask.rows -1 ? maskROI.br().y + iBkgdPixelRadius : inputMask.rows -1;
    bkgdROI->height = iBottomRightY - bkgdROI->y;
}


bool TaoPixelCompletion::checkIsMaskEdgePoint(const Point &inPoint, Mat &mask)
{
    // if it is a background point
    if (mask.at<uchar>(inPoint.y, inPoint.x) == 0)
        return false;

    // first row
    if (checkIsImagePoint(inPoint.x-1, inPoint.y-1, mask.cols, mask.rows)
            && (mask.at<uchar>(inPoint.y-1, inPoint.x-1) == 0))
    {
        return true;
    }

    if (checkIsImagePoint(inPoint.x, inPoint.y -1, mask.cols, mask.rows)
            && (mask.at<uchar>(inPoint.y-1, inPoint.x) == 0))
    {
        return true;
    }

    if (checkIsImagePoint(inPoint.x+1, inPoint.y -1, mask.cols, mask.rows)
            && (mask.at<uchar>(inPoint.y-1, inPoint.x+1) == 0))
    {
        return true;
    }

    // second row
    if (checkIsImagePoint(inPoint.x-1, inPoint.y, mask.cols, mask.rows)
            && (mask.at<uchar>(inPoint.y, inPoint.x-1) == 0))
    {
        return true;
    }

    if (checkIsImagePoint(inPoint.x+1, inPoint.y, mask.cols, mask.rows)
            && (mask.at<uchar>(inPoint.y, inPoint.x+1) == 0))
    {
        return true;
    }

    // third row
    if (checkIsImagePoint(inPoint.x-1, inPoint.y+1, mask.cols, mask.rows)
            && (mask.at<uchar>(inPoint.y+1, inPoint.x-1) == 0))
    {
        return true;
    }

    if (checkIsImagePoint(inPoint.x, inPoint.y + 1, mask.cols, mask.rows)
            && (mask.at<uchar>(inPoint.y + 1, inPoint.x) == 0))
    {
        return true;
    }

    if (checkIsImagePoint(inPoint.x+1, inPoint.y+1, mask.cols, mask.rows)
            && (mask.at<uchar>(inPoint.y+1, inPoint.x+1) == 0))
    {
        return true;
    }

    return false;
}


bool TaoPixelCompletion::checkIsImagePoint(int pointx, int pointy, int cols, int rows)
{
    if ((pointx >=0) && (pointx < cols)  && (pointy >= 0) && (pointy < rows))
        return true;
    else
        return false;
}


void TaoPixelCompletion::findMaskEdgePoints(vector<Point> &edgePoints, Mat &inMask, Rect &inMaskROI)
{
    int startx = inMaskROI.x;
    int starty = inMaskROI.y;
    int endx = inMaskROI.x + inMaskROI.width;
    int endy = inMaskROI.y + inMaskROI.height;

    Point curPoint;

    for (int irow = starty; irow < endy; irow++)
        for (int icol = startx; icol < endx; icol++)
        {
            curPoint.x = icol;
            curPoint.y = irow;

            // if it is a foreground point adjacent to background, add to the list.
            if (checkIsMaskEdgePoint(curPoint, inMask))
            {
                edgePoints.push_back(curPoint);
            }
        }
}
