
#include "taopushpullapprox.h"

void TaoPushPullApprox::pushPullApprox(Mat &outputImage, Mat &inputImage, Mat &inputMask, int levels)
{
    Mat preRoundImage = inputImage.clone();
    Mat curRoundImage = preRoundImage.clone();
    Mat diffImage = inputImage.clone();

    if (levels <= 1)
        return;

    for (int iLevel = levels; iLevel > 1; iLevel /= 2)
    {
        double minVal, maxVal;

        do
        {
            diffImage = Scalar::all(0);

            // downsample
            Mat preDownsample = preRoundImage.clone();

            for (int idown = 1; idown < iLevel; idown *=2)
            {
                Mat curDownsample(preDownsample.rows / 2, preDownsample.cols / 2, CV_8UC3);

                pyrDown(preDownsample, curDownsample);

                preDownsample.release();
                preDownsample = curDownsample.clone();
            }

            // upsample
            Mat preUpsample = preDownsample;
            for (int iup = 1; iup < iLevel; iup *=2)
            {
                Mat curUpsample(inputImage.rows * 2* iup/ iLevel, inputImage.cols * 2* iup / iLevel, CV_8UC3);

                pyrUp(preUpsample, curUpsample);

                preUpsample.release();
                preUpsample = curUpsample.clone();
            }

            // replace background
            for (int irow = 0; irow < curRoundImage.rows; irow++)
                for (int icol = 0; icol < curRoundImage.cols; icol++)
                {
                    if (inputMask.at<uchar>(irow, icol) > 0 )
                        curRoundImage.at<Vec3b>(irow, icol) = preUpsample.at<Vec3b>(irow, icol);
                }

            // compare with current round and pre-round
            maxVal = 0.0;

            absdiff(preRoundImage, curRoundImage, diffImage);
            minMaxLoc(diffImage, &minVal, &maxVal);

            curRoundImage.copyTo(preRoundImage);
        } while ((int)maxVal > 0);
    }

    curRoundImage.copyTo(outputImage);
}
