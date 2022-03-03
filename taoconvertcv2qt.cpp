
#include "taoconvertcv2qt.h"

bool TaoConvertCV2Qt::convertMat2QImage(Mat &inputMat, Mat &inputMat4Buffer, QImage &outputQImage)
{
    if (inputMat.channels() == 3)
    {
        //convert Mat BGR to QImage RGB
        cvtColor(inputMat, inputMat4Buffer, CV_BGR2RGB);

        outputQImage = QImage((const unsigned char*)(inputMat4Buffer.data),
                            inputMat.cols,  inputMat.rows,
                            inputMat.cols *  inputMat.channels(),
                            QImage::Format_RGB888);

        return true;
    }

    return false;
}


bool TaoConvertCV2Qt::convertQImage2Mat(QImage &inputQImage, Mat &outputMat)
{
    switch (inputQImage.format())
    {
    case QImage::Format_RGB888:
//        cout << "inside RGB888 " << endl;
//        cout << " input h/w: " << inputQImage.height() << "  " << inputQImage.width()<<endl;

//         outputMat = Mat(inputQImage.height(), inputQImage.width(),
//                        CV_8UC3, (unsigned char*)inputQImage.bits());

//        return true;
        break;

    case QImage::Format_Grayscale8:
//        cout << "  inside grayscale8 " << endl;
        outputMat = Mat(inputQImage.height(), inputQImage.width(),
                        CV_8UC1, (void*)inputQImage.constBits(), inputQImage.bytesPerLine());

        return true;
        break;

    default:
        return false;
    }
}

void TaoConvertCV2Qt::convertQPointF2CVPoint2f(QPointF &qtPt, Point2f &cvPt)
{
    cvPt.x = qtPt.x();
    cvPt.y = qtPt.y();
}
