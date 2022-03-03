#include "taocanvasarea.h"


TaoCanvasArea::TaoCanvasArea(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_StaticContents);

    makeScaleList(scaleList);
}


TaoCanvasArea::~TaoCanvasArea()
{
    releaseMem();
}


int TaoCanvasArea::openImage(const QString &strQFileName)
{
//    string strInputFilename = (char *)strQFileName.toLatin1().data(); 不能处理中文文件名

    // first convert QString to string, especially for Chinese filenames
//  this one does not work on some machines.
//    inputMatBGR = imread(string((const char *)strQFileName.toLocal8Bit()), 1);

    QTextCodec *txtCodec = QTextCodec::codecForName("System");

    inputMatBGR = imread(string((const char *)txtCodec->fromUnicode(strQFileName)), IMREAD_COLOR);

//    cout << " channels: " <<  inputMatBGR.channels() << " depth:" << inputMatBGR. << endl;

    if ( inputMatBGR.empty() )
    {
        cout << "Failed: open color image."  << endl;
        return 1;
     }


    if (inputMatBGR.channels() != 3)
        return 2;

    TaoConvertCV2Qt::convertMat2QImage(inputMatBGR, inputMatRGB, inputQImage);

    curMatBGR = inputMatBGR.clone();
    curMatRGB = inputMatRGB.clone();

    curQImage = inputQImage.copy();

//    curDisplayQImage = curQImage.copy();

//    if ((this->width()  < curQImage.width()) ||  (this->height() < curQImage.height()))
//    {
//        curDisplayQImage = curQImage.scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
//    }
//    else
//    {
//        curDisplayQImage = curQImage.copy();
//    }

    // find the scale factor in order to display the full image

    curScalePos = 5;   // set the position at scale 1.0

    if ((this->width()  < curQImage.width()) ||  (this->height() < curQImage.height() + 2 * paintOffsetY))
    {
         while (curScalePos > 0)
         {
             curScalePos--;

             if ((this->width() > scaleList[curScalePos] * curQImage.width())
                    && (this->height() > scaleList[curScalePos] * curQImage.height()))
                    break;
         }

         // we don't want the initial zoom out two much, minimum initial 50%
         curScalePos = curScalePos >= 3 ? curScalePos : 3;
     }

    float scaleFactor = scaleList[curScalePos];

    curScaledQImage = curQImage.scaled(scaleFactor * curQImage.size(),
                                                Qt::KeepAspectRatio, Qt::SmoothTransformation);

    curDisplayQImage = curQImage.scaled(scaleFactor * curQImage.size(),
                                                Qt::KeepAspectRatio, Qt::SmoothTransformation);

    driftClearDrawingVars();
    patchClearDrawingVars();
    lineClearDrawingVars();

    setMouseTracking(true);

    dequeClearCompleteHist();

    if (ppathCircleWatermarkDisplay != NULL)
    {
        delete ppathCircleWatermarkDisplay;
        ppathCircleWatermarkDisplay = NULL;
    }

    patchCircleWatermarkPoints.clear();

    blnPatchCircleWatermarkDrawn = false;
    blnPatchCompleted = false;
    blnTextureDrawn = false;
    blnLineDrawn = false;

    update();

    // add to queue for undo/redo history
    CompletionNode completion;

    completion.method = PATCH_COMPLETION;
    completion.image = inputMatBGR.clone();

    dqCompleteHist.push_back(completion);

    completeHistPos = 0;

    return 0;
}


/****
 * load the current QImage for display
 ***/
bool TaoCanvasArea::loadCurQImage()
{
    float scaleFactor = scaleList[curScalePos];

    curScaledQImage = curQImage.scaled(scaleFactor * curQImage.size(),
                                        Qt::KeepAspectRatio, Qt::SmoothTransformation);

    curDisplayQImage = curQImage.scaled(scaleFactor * curQImage.size(),
                                        Qt::KeepAspectRatio, Qt::SmoothTransformation);

    patchClearDrawingVars();
    driftClearDrawingVars();
    lineClearDrawingVars();

    blnPatchCompleted = false;

    update();

    return true;
}


bool TaoCanvasArea::patchClearImage()
{
    float scaleFactor = scaleList[curScalePos];

    curScaledQImage = curQImage.scaled(scaleFactor * curQImage.size(),
                                        Qt::KeepAspectRatio, Qt::SmoothTransformation);

    curDisplayQImage = curQImage.scaled(scaleFactor * curQImage.size(),
                                        Qt::KeepAspectRatio, Qt::SmoothTransformation);

    update();

    patchClearDrawingVars();

    return true;
}


bool TaoCanvasArea::driftClearImage()
{
    float scaleFactor = scaleList[curScalePos];

    curScaledQImage = curQImage.scaled(scaleFactor * curQImage.size(),
                                        Qt::KeepAspectRatio, Qt::SmoothTransformation);

    curDisplayQImage = curQImage.scaled(scaleFactor * curQImage.size(),
                                        Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // if circle bkgd is not drawn
    if (!blnTextureCircleBkgdDrawn)
    {
        driftClearDrawingVars();
    }
    else
    {
        // if circle bkgd is drawn.
        // if brushed watermark is drawn
        if (blnTextureDrawn)
            driftDrawBrushPaths(&curDisplayQImage, true, false);

        blnTextureCircleBkgdDrawn = false;
    }

    update();

    return true;
}


bool TaoCanvasArea::lineClearImage()
{
    float scaleFactor = scaleList[curScalePos];

    curScaledQImage = curQImage.scaled(scaleFactor * curQImage.size(),
                                        Qt::KeepAspectRatio, Qt::SmoothTransformation);

    curDisplayQImage = curQImage.scaled(scaleFactor * curQImage.size(),
                                        Qt::KeepAspectRatio, Qt::SmoothTransformation);

    update();

    lineClearDrawingVars();

    return true;
}


void TaoCanvasArea::driftClearDrawingVars()
{
    // for texture completion
    blnTextureDrawn = false;

    blnTextureCircleBkgdDrawn = false;

    brushPointsPathList.clear();
    brushPathList.clear();
    brushQPointsCurPath.clear();
    textureCircleBkgdPoints.clear();

    if (ppathBrushDisplay  != NULL)
    {
        delete ppathBrushDisplay; // for display image
        ppathBrushDisplay = NULL;
    }

    if (ppathBrushImage != NULL)
    {
        delete ppathBrushImage;   // for the unscaled image
        ppathBrushImage = NULL;
    }

    if (ppathTextureCircleBkgdDisplay != NULL)
    {
        delete ppathTextureCircleBkgdDisplay;
        ppathTextureCircleBkgdDisplay = NULL;
    }

    if (ppathTextureCircleBkgdImage != NULL)
    {
        delete ppathTextureCircleBkgdImage;
        ppathTextureCircleBkgdImage = NULL;
    }
}


void TaoCanvasArea::patchClearDrawingVars()
{
    // for patch completion
//    blnPatchCircleWatermarkSelected = false;
//    blnPatchCompleted = false;
    blnPatchCircleWatermarkDrawn = false;

    patchCircleWatermarkPoints.clear();

    if (ppathCircleWatermarkDisplay != NULL)
    {
        delete ppathCircleWatermarkDisplay;
        ppathCircleWatermarkDisplay = NULL;
    }

    if (ppathCircleWatermarkImage != NULL)
    {
        delete ppathCircleWatermarkImage;
        ppathCircleWatermarkImage = NULL;
    }
}


void TaoCanvasArea::lineClearDrawingVars()
{
    blnLineDrawn = false;
    lineEdgeDrawn.setPoints(QPointF(0,0), QPointF(0,0));
}


bool TaoCanvasArea::reloadImage()
{
    curQImage = inputQImage.copy();

    float scaleFactor = scaleList[curScalePos];

    curScaledQImage = curQImage.scaled(scaleFactor * curQImage.size(),
                                        Qt::KeepAspectRatio, Qt::SmoothTransformation);

    curDisplayQImage = curQImage.scaled(scaleFactor * curQImage.size(),
                                        Qt::KeepAspectRatio, Qt::SmoothTransformation);

//    curScaledQImage = inputQImage.copy();
//    curDisplayQImage = inputQImage.copy();

    curMatBGR = inputMatBGR.clone();
    curMatRGB = inputMatRGB.clone();

    update();

    // clean variable for restart an image
    blnPatchCompleted = false;
    patchClearDrawingVars();
    driftClearDrawingVars();
    lineClearDrawingVars();
    dequeClearCompleteHist();

    // add to queue for undo/redo history
    CompletionNode completion;

    completion.method = PATCH_COMPLETION;
    completion.image = inputMatBGR.clone();

    dqCompleteHist.push_back(completion);

    completeHistPos = 0;

    return true;
}


bool TaoCanvasArea::saveImage(const QString &fileName) //, const char *fileFormat)
{
    // set output image parameters
    vector<int> params;
    params.push_back(IMWRITE_JPEG_QUALITY);
    params.push_back(97);

//  This one does not work on some machines.
//    imwrite(string((const char *)fileName.toLocal8Bit()), completedImageBGR, params);

    QTextCodec *txtCodec = QTextCodec::codecForName("System");
    imwrite(string((const char *)txtCodec->fromUnicode(fileName)), completedImageBGR, params);

    inputQImage = curQImage.copy();
    inputMatBGR = curMatBGR.clone();
    inputMatRGB = curMatRGB.clone();

    // clear completion history
    dequeClearCompleteHist();

    return true;
}


bool TaoCanvasArea::viewImage()
{
    //imshow("original image", inputMatBGR);

    return true;
}


void TaoCanvasArea::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QRect paintRect = event->rect();

//    QMessageBox::information(this, tr("inside paint event"), tr("before drawImage"), QMessageBox::Ok);
//    painter.drawImage(paintRect, curDisplayQImage, paintRect);
//    painter.drawImage(0, 0, curDisplayQImage);


//    float scaleFactor = scaleList[curScalePos];
//    painter.scale(scaleFactor, scaleFactor);

//    cout << " event rect: " << paintRect.x() << " " << paintRect.y() << " " << paintRect.width() << " " << paintRect.height() << endl;

    paintOffsetX = (int)((paintRect.width() - curDisplayQImage.width()) / 2);

//    float paintOffsetX = (paintRect.width() - curDisplayQImage.width()) / 2;
//    float paintOffsetY = 10;

    paintOffsetX = paintOffsetX > 0 ? paintOffsetX : 0;

    // translate the coordinate, move the image to the horizontal center of the main window
    painter.translate(paintOffsetX, paintOffsetY);

//    painter.drawImage((paintRect.width() - curDisplayQImage.width()) / 2, 0, curDisplayQImage);
    painter.drawImage(0, 0, curDisplayQImage);
}



void TaoCanvasArea::resizeEvent(QResizeEvent *event)
{
    update();
}


void TaoCanvasArea::mousePressEvent(QMouseEvent *event)
{
//    this->setCursor(QCursor(QPixmap(".//icons//Brush_24px.ico")));

    if (event->button() == Qt::LeftButton)
    {
        // get the current QPointF, adjust with the center offset
        preQPointDisplay = event->localPos() - QPointF(paintOffsetX, paintOffsetY);  // / scaleList[curScalePos];

        resetPointInImage(&preQPointDisplay, curScaledQImage.width(), curScaledQImage.height());

        preQPointImage = preQPointDisplay / scaleList[curScalePos];

        resetPointInImage(&preQPointImage, inputQImage.width(), inputQImage.height());

        // initialize end QPoint as first QPoint
        endQPointDisplay = preQPointDisplay;

        float scale = scaleList[curScalePos];
        QRect displayStretchRect((QPoint)(scale * m_curStretchRect.topLeft()), (QPoint)(scale * m_curStretchRect.bottomRight()));

        m_stretchRectState = getStretchRectState(preQPointDisplay, displayStretchRect);

        if (blnTextureDrawn && (m_stretchRectState != StretchNotSelect))
        {
            blnBeginMoveStretchRect = true;

            setStretchCursorStyle(m_stretchRectState);
            return;
        }

        // mouse press, for texture toolbar -- brush checked
        if (blnSmallBrushSelected || blnMediumBrushSelected || blnBigBrushSelected)
        {
            //icon size must be >= 16x16px
            if (blnMediumBrushSelected || blnBigBrushSelected)
                this->setCursor(QCursor(QPixmap(".//icons//circle_add_16px.ico"), 8, 8));

//            this->setCursor(Qt::PointingHandCursor);

            ppathBrushDisplay = new QPainterPath(preQPointDisplay);
            ppathBrushImage = new QPainterPath(preQPointDisplay / scaleList[curScalePos]);

            brushQPointsCurPath.clear();
            brushQPointsCurPath.push_back(preQPointDisplay / scaleList[curScalePos]);

            blnBrushing = true;

            return;
        }

        // mouse press, for texture toolbar -- circle bkgd checked, but not drawn
        if (blnTextureCircleBkgdSelected && !blnTextureCircleBkgdDrawn)
        {
            this->setCursor(Qt::PointingHandCursor);
            ppathTextureCircleBkgdDisplay = new QPainterPath(preQPointDisplay);

            ppathTextureCircleBkgdImage = new QPainterPath(preQPointImage);

            firstCircleQPointDisplay = preQPointDisplay;
            firstCircleQPointImage = preQPointImage;

//            cout << " first circle point x/y: " << firstCircleQPointDisplay.x() <<  " " << firstCircleQPointDisplay.y() << endl;

            textureCircleBkgdPoints.clear();
//            textureCircleBkgdPoints.push_back(preQPointDisplay / scaleList[curScalePos]);
            textureCircleBkgdPoints.push_back(preQPointImage);

            return;
        }

        // mouse press, for patch toolbar -- circle watermark checked, but not drawn
        if (blnPatchCircleWatermarkSelected && !blnPatchCircleWatermarkDrawn)
        {
            this->setCursor(Qt::PointingHandCursor);
            ppathCircleWatermarkDisplay = new QPainterPath(preQPointDisplay);
            ppathCircleWatermarkImage = new QPainterPath(preQPointImage);

            firstCircleQPointDisplay = preQPointDisplay;
            firstCircleQPointImage = preQPointImage;

            patchCircleWatermarkPoints.clear();
            patchCircleWatermarkPoints.push_back(preQPointImage);
            return;
        }
    }
}


void TaoCanvasArea::mouseMoveEvent(QMouseEvent *event)
{
    if (blnTextureDrawn && !blnBrushing)
    {
        // get the current QPointF, adjust with the center offset
        QPointF curPointDisplay = event->localPos() - QPointF(paintOffsetX, paintOffsetY);

        resetPointInImage(&curPointDisplay, curScaledQImage.width(), curScaledQImage.height());

        float scale = scaleList[curScalePos];

        QRect displayStretchRect((QPoint)(scale * m_curStretchRect.topLeft()), (QPoint)(scale * m_curStretchRect.bottomRight()));

        StretchRectState stretchState = getStretchRectState(curPointDisplay, displayStretchRect);

        if (!blnBeginMoveStretchRect)
        {
            setStretchCursorStyle(stretchState);
        }

        if ((event->buttons() & Qt::LeftButton) && blnTextureDrawn && blnBeginMoveStretchRect)
        {
            QPointF curPointImage = curPointDisplay / scaleList[curScalePos];

            resetPointInImage(&curPointImage, curQImage.width(), curQImage.height());
            updateStretchRect(m_curStretchRect, stretchState, curPointImage,
                           m_minStretchRect, m_maxStretchRect);

            curDisplayQImage = curScaledQImage.copy();

            driftDrawSelection(&curDisplayQImage, m_curStretchRect);
            update();

            return;
        }
    }


    // mouse move --- brush checked
    if ((event->buttons() & Qt::LeftButton) &&
            (blnSmallBrushSelected || blnMediumBrushSelected || blnBigBrushSelected))
    {
        QImage tmpImage = curScaledQImage.copy();

        QPainter painter(&tmpImage);
        painter.setPen(QPen(QBrush(QColor(255, 0, 0, 96)), iBrushSize, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));  //QPen(QBrush(QColor(192, 0, 0, 192)), 2.0)

        // get the current QPointF, adjust with the center offset
        endQPointDisplay = event->localPos() - QPointF(paintOffsetX, paintOffsetY);
        resetPointInImage(&endQPointDisplay, curScaledQImage.width(), curScaledQImage.height());

        endQPointImage = endQPointDisplay / scaleList[curScalePos];
        resetPointInImage(&endQPointImage, inputQImage.width(), inputQImage.height());

        ppathBrushDisplay->lineTo(endQPointDisplay);
        ppathBrushImage->lineTo(endQPointImage);

        brushQPointsCurPath.push_back(endQPointImage);

        painter.drawPath(*ppathBrushDisplay);

        curDisplayQImage = tmpImage.copy();

        driftDrawBrushPaths(&curDisplayQImage, false, blnTextureCircleBkgdDrawn);

        update();

        return;
    }


    // mouse move -- texture tool
    // for circling the bkgd
    if ((event->buttons() & Qt::LeftButton) &&
            (blnTextureCircleBkgdSelected) && (!blnTextureCircleBkgdDrawn))
    {
//        cout << " circle bkgd mouse move" << endl;

        QImage tmpImage = curScaledQImage.copy();

        QPainter painter(&tmpImage);
        painter.setPen(QPen(Qt::green, 2, Qt::DashLine, Qt::SquareCap, Qt::MiterJoin));  //QPen(QBrush(QColor(192, 0, 0, 192)), 2.0)

        // get the current QPointF, adjust with the center offset
        endQPointDisplay = event->localPos() - QPointF(paintOffsetX, paintOffsetY);
        resetPointInImage(&endQPointDisplay, curScaledQImage.width(), curScaledQImage.height());

        endQPointImage = endQPointDisplay / scaleList[curScalePos];
        resetPointInImage(&endQPointImage, inputQImage.width(), inputQImage.height());

        ppathTextureCircleBkgdDisplay->lineTo(endQPointDisplay);
        ppathTextureCircleBkgdImage->lineTo(endQPointImage);

        textureCircleBkgdPoints.push_back(endQPointImage);

        painter.drawPath(*ppathTextureCircleBkgdDisplay);

        curDisplayQImage = tmpImage.copy();

        driftDrawBrushPaths(&curDisplayQImage, false, blnTextureCircleBkgdDrawn);

        update();

        return;
    }

    // mouse move, patch tool
    // for circling the watermark
    if ((event->buttons() & Qt::LeftButton) &&
            (blnPatchCircleWatermarkSelected) && (!blnPatchCircleWatermarkDrawn))
    {
        QImage tmpImage = curScaledQImage.copy();

        QPainter painter(&tmpImage);

        painter.setPen(QPen(Qt::red, 2, Qt::DashLine, Qt::SquareCap, Qt::MiterJoin));  //QPen(QBrush(QColor(192, 0, 0, 192)), 2.0)

        // get the current QPointF, adjust with the center offset
        endQPointDisplay = event->localPos() - QPointF(paintOffsetX, paintOffsetY);
        resetPointInImage(&endQPointDisplay, curScaledQImage.width(), curScaledQImage.height());

        endQPointImage = endQPointDisplay / scaleList[curScalePos];
        resetPointInImage(&endQPointImage, inputQImage.width(), inputQImage.height());

        ppathCircleWatermarkDisplay->lineTo(endQPointDisplay);

        ppathCircleWatermarkImage->lineTo(endQPointImage);

        patchCircleWatermarkPoints.push_back(endQPointImage);

        painter.drawPath(*ppathCircleWatermarkDisplay);

        curDisplayQImage = tmpImage.copy();

        update();

        return;
    }

    // mouse move, patch tool
    // move the watermark replace patch, when watermark is circled,
    if ((event->buttons() & Qt::LeftButton) &&
            (blnPatchCircleWatermarkSelected) && (blnPatchCircleWatermarkDrawn))
    {
        QImage tmpImage = curScaledQImage.copy();

        QPainter painter(&tmpImage);
        painter.setPen(QPen(Qt::red, 2, Qt::DashLine, Qt::SquareCap, Qt::MiterJoin));  //QPen(QBrush(QColor(192, 0, 0, 192)), 2.0)

        QPainterPath ppathWatermarkPatchDisplay(patchCircleWatermarkPoints[0] * scaleList[curScalePos]);

        // make the scaled QPainterPath
        for (int icount = 1; icount < (int)patchCircleWatermarkPoints.size(); icount++)
        {
            ppathWatermarkPatchDisplay.lineTo(patchCircleWatermarkPoints[icount] * scaleList[curScalePos]);
        }

        // get the current QPointF, adjust with the center offset
        endQPointDisplay = event->localPos() - QPointF(paintOffsetX, paintOffsetY);

        QRectF displayDstBoundingRect = ppathWatermarkPatchDisplay.boundingRect();

        painter.drawPath(ppathWatermarkPatchDisplay);

        // make sure the source patch is inside the display image
        if ((int)endQPointDisplay.x() - (int)displayDstBoundingRect.width() / 2 < 0 )
            endQPointDisplay.setX((int)displayDstBoundingRect.width() / 2);

        if ((int)endQPointDisplay.y() - (int)displayDstBoundingRect.height() / 2 < 0 )
            endQPointDisplay.setY((int)displayDstBoundingRect.height() / 2);

        if ((int)endQPointDisplay.x() + (int)displayDstBoundingRect.width() / 2 > tmpImage.width() - 1)
            endQPointDisplay.setX(tmpImage.width() - 1 - (int)displayDstBoundingRect.width() / 2);

        if ((int)endQPointDisplay.y() + (int)displayDstBoundingRect.height() / 2 > tmpImage.height() - 1)
            endQPointDisplay.setY(tmpImage.height() - 1 - (int)displayDstBoundingRect.height() / 2);

        QPointF offset = endQPointDisplay - QPointF(displayDstBoundingRect.x() + displayDstBoundingRect.width() / 2,
                                                    displayDstBoundingRect.y() + displayDstBoundingRect.height() / 2);

        QPainterPath ppathWatermarkReplace = ppathWatermarkPatchDisplay.translated(offset);

        painter.setPen(QPen(Qt::green, 2, Qt::DashLine, Qt::SquareCap, Qt::MiterJoin));  //QPen(QBrush(QColor(192, 0, 0, 192)), 2.0)

        painter.drawPath(ppathWatermarkReplace);

        curDisplayQImage = tmpImage.copy();

        //for copy the selection to the red circle
        QRectF displaySrcBoundingRect = ppathWatermarkReplace.boundingRect();

        for (int irow = 0; irow < (int)displayDstBoundingRect.height(); irow++)
            for (int icol = 0; icol < (int)displayDstBoundingRect.width(); icol++)
            {
                if (ppathWatermarkPatchDisplay.contains(QPointF(icol + displayDstBoundingRect.x(), irow + displayDstBoundingRect.y())))
                {
                    QColor srcColor = curScaledQImage.pixelColor(icol + displaySrcBoundingRect.x(), irow+displaySrcBoundingRect.y());

                    curDisplayQImage.setPixelColor(icol + displayDstBoundingRect.x(), irow + displayDstBoundingRect.y(), srcColor);
                }
            }

        update();

        return;
    }

    // mouse move
    // edge tool -- draw edge
    if ((event->buttons() & Qt::LeftButton) && blnLineDrawSelected)
    {
        QImage tmpImage = curScaledQImage.copy();

        QPainter painter(&tmpImage);
        painter.setPen(QPen(Qt::green, 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

        // get the current QPointF, adjust with the center offset
        endQPointDisplay = event->localPos()  - QPointF(paintOffsetX, paintOffsetY);

        resetPointInImage(&endQPointDisplay, curScaledQImage.width(), curScaledQImage.height());

        painter.drawLine(preQPointDisplay, endQPointDisplay);

        curDisplayQImage = tmpImage.copy();

        update();

        return;
    }
}


void TaoCanvasArea::mouseReleaseEvent(QMouseEvent *event)
{
    this->unsetCursor();

    if ((event->button() == Qt::LeftButton) && blnTextureDrawn && blnBeginMoveStretchRect)
    {
//        QPointF curPointDisplay = event->localPos() - QPointF(paintOffsetX, paintOffsetY);

//        curDisplayQImage = curScaledQImage.copy();

//        driftDrawSelection(&curDisplayQImage, drawingRect);
//        update();

        blnBeginMoveStretchRect = false;

        return;
    }


    // mouse release, texture tool
    // brush checked
    if ((event->button() == Qt::LeftButton) &&
            (blnSmallBrushSelected || blnMediumBrushSelected || blnBigBrushSelected))
    {
        blnBrushing = false;

        if (brushQPointsCurPath.size() < 2)
            return;

        QPainter painter(&curScaledQImage);
        painter.setPen(QPen(QBrush(QColor(255, 0, 0, 96)), iBrushSize, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));  //QPen(QBrush(QColor(192, 0, 0, 192)), 2.0)

        // get the current QPointF, adjust with the center offset
        endQPointDisplay = event->localPos()  - QPointF(paintOffsetX, paintOffsetY);

        resetPointInImage(&endQPointDisplay, curScaledQImage.width(), curScaledQImage.height());

        endQPointImage = endQPointDisplay / scaleList[curScalePos];

        resetPointInImage(&endQPointImage, inputQImage.width(), inputQImage.height());

        ppathBrushDisplay->lineTo(endQPointDisplay);
        ppathBrushImage->lineTo(endQPointImage);

        struct BrushPainterPath pathNode = {iBrushSize/ scaleList[curScalePos], *ppathBrushImage};
        brushPathList.push_back(pathNode);

//        brushQPointsCurPath.push_back(endQPointDisplay / scaleList[curScalePos]);
        brushQPointsCurPath.push_back(endQPointImage);

        struct BrushPointsPath pointsPath = {iBrushSize/ scaleList[curScalePos], brushQPointsCurPath};
        brushPointsPathList.push_back(pointsPath);

        painter.drawPath(*ppathBrushDisplay);

        curDisplayQImage = curScaledQImage.copy();

        curMatMaskTexture = Mat(curQImage.height(), curQImage.width(), CV_8UC1);
        makeMaskMat(curMatMaskTexture, brushPointsPathList);

        // unscaled mask bounding rectangle
        getDrawingRect(m_maskBoundRect, curMatMaskTexture);

//        m_curStretchRect = drawingRect;
        getMinMaxInitStretchRect(m_minStretchRect, m_maxStretchRect, m_curStretchRect,
                                 m_maskBoundRect, curMatMaskTexture.cols, curMatMaskTexture.rows);

        if (blnTextureCircleBkgdDrawn)
            driftDrawBrushPaths(&curDisplayQImage, false, true);
        else
            driftDrawBrushPaths(&curDisplayQImage, true, false);

        m_stretchRectState = StretchNotSelect;

        // *** for testing
        // draw brushes on not scaled image
        /*
        QImage drawImage(curQImage.width(), curQImage.height(), QImage::Format_Grayscale8);
        drawImage.fill(0);

        QPainter drawPainter(&drawImage);

        struct BrushPainterPath curPath;

        for (int count = 0; count < (int)brushPathList.size(); count++)
        {
            curPath = brushPathList[count];
            drawPainter.setPen(QPen(Qt::white, curPath.fBrushSize, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            drawPainter.drawPath(curPath.ppathBrush);
        }

        drawImage.save(QString("d://drawImageBrushes.jpg"), "JPG");
        Mat drawMat;

        TaoConvertCV2Qt::convertQImage2Mat(drawImage, drawMat);

        imshow("brush mat", drawMat);
        drawMat.release();
        */

        update();

        blnTextureDrawn = true;

        return;
    }

    // mouse release, texture tool
    // circle bkgd, when circle bkgd checked, and not drawn yet.
    if (((event->button() == Qt::LeftButton) &&
            (blnTextureCircleBkgdSelected)) && (!blnTextureCircleBkgdDrawn))
    {
        if (textureCircleBkgdPoints.size() < 2)
            return;

        blnTextureCircleBkgdDrawn = true;

        QImage tmpImage = curScaledQImage.copy();

        QPainter painter(&tmpImage);

        ppathTextureCircleBkgdDisplay->lineTo(firstCircleQPointDisplay);
//        ppathTextureCircleBkgdImage->lineTo(firstCircleQPointDisplay / scaleList[curScalePos]);
        ppathTextureCircleBkgdImage->lineTo(firstCircleQPointImage);

//        textureCircleBkgdPoints.push_back(firstCircleQPointDisplay / scaleList[curScalePos]);
        textureCircleBkgdPoints.push_back(firstCircleQPointImage);

        textureBkgdBoundingRect = ppathTextureCircleBkgdImage->boundingRect();

        painter.setPen(QPen(Qt::green, 2, Qt::DashLine, Qt::SquareCap, Qt::MiterJoin));  //QPen(QBrush(QColor(192, 0, 0, 192)), 2.0)

        painter.drawPath(*ppathTextureCircleBkgdDisplay);

        curDisplayQImage = tmpImage.copy();

        driftDrawBrushPaths(&curDisplayQImage, false, false);

        update();

        return;
    }

    // mouse release, patch tool
    // for circle watermark, when circle watermark checked, and not drawn yet.
    if (((event->button() == Qt::LeftButton) &&
            (blnPatchCircleWatermarkSelected)) && (!blnPatchCircleWatermarkDrawn))
    {
        if (patchCircleWatermarkPoints.size() < 2)
            return;

        blnPatchCircleWatermarkDrawn = true;

        blnPatchCompleted = false;

        QImage tmpImage = curScaledQImage.copy();

        QPainter painter(&tmpImage);
        painter.setPen(QPen(Qt::red, 2, Qt::DashLine, Qt::SquareCap, Qt::MiterJoin));  //QPen(QBrush(QColor(192, 0, 0, 192)), 2.0)

        ppathCircleWatermarkDisplay->lineTo(firstCircleQPointDisplay);
//        ppathCircleWatermarkImage->lineTo(firstCircleQPointDisplay / scaleList[curScalePos]);
//        patchCircleWatermarkPoints.push_back(firstCircleQPointDisplay / scaleList[curScalePos]);

        ppathCircleWatermarkImage->lineTo(firstCircleQPointImage);

        patchCircleWatermarkPoints.push_back(firstCircleQPointImage);

        patchDstBoundingRect = ppathCircleWatermarkImage->boundingRect();

        painter.drawPath(*ppathCircleWatermarkDisplay);

        QPointF displayOffset(15, 15);
        QRectF  tmpBoundingRect = ppathCircleWatermarkDisplay->boundingRect();

        if (tmpBoundingRect.x() + tmpBoundingRect.width()/2 > tmpImage.width() / 2)
            displayOffset.setX(-15);

        if (tmpBoundingRect.y() + tmpBoundingRect.height()/2 > tmpImage.height() / 2)
            displayOffset.setY(-15);


        // get unscaled image patch centers: source, destination
        patchDstCenter = QPointF(patchDstBoundingRect.x() + patchDstBoundingRect.width() / 2, patchDstBoundingRect.y() + patchDstBoundingRect.height() / 2);
        patchSrcCenter = patchDstCenter + displayOffset / scaleList[curScalePos];

        QPointF imageOffset = displayOffset / scaleList[curScalePos];

        QPainterPath ppathWatermarkReplaceImage = ppathCircleWatermarkImage->translated(imageOffset);

        QPainterPath ppathWatermarkReplaceDisplay = ppathCircleWatermarkDisplay->translated(displayOffset);

        patchSrcBoundingRect = ppathWatermarkReplaceImage.boundingRect();

        painter.setPen(QPen(Qt::green, 2, Qt::DashLine, Qt::SquareCap, Qt::MiterJoin));  //QPen(QBrush(QColor(192, 0, 0, 192)), 2.0)

        painter.drawPath(ppathWatermarkReplaceDisplay);

        curDisplayQImage = tmpImage.copy();

        update();

        return;
    }

    // mouse release, patch tool
    // for movement of watermark replacement, when circle watermark checked, and drawn,
    if (((event->button() == Qt::LeftButton) &&
            (blnPatchCircleWatermarkSelected)) && (blnPatchCircleWatermarkDrawn))
    {
        QImage tmpImage = curScaledQImage.copy();

        QPainter painter(&tmpImage);
        painter.setPen(QPen(Qt::red, 2, Qt::DashLine, Qt::SquareCap, Qt::MiterJoin));  //QPen(QBrush(QColor(192, 0, 0, 192)), 2.0)

        QPainterPath ppathWatermarkPatchDisplay(patchCircleWatermarkPoints[0] * scaleList[curScalePos]);

        // make the scaled QPainterPath
        for (int icount = 1; icount < (int)patchCircleWatermarkPoints.size(); icount++)
        {
            ppathWatermarkPatchDisplay.lineTo(patchCircleWatermarkPoints[icount] * scaleList[curScalePos]);
        }

        // get the current QPointF, adjust with the center offset
        endQPointDisplay = event->localPos() - QPointF(paintOffsetX, paintOffsetY);
        endQPointImage = endQPointDisplay / scaleList[curScalePos];

        QRectF displayDstBoundingRect = ppathWatermarkPatchDisplay.boundingRect();

        // make sure the green(source) patch is inside the display image
        if ((int)endQPointDisplay.x() - (int)displayDstBoundingRect.width() / 2 < 0 )
            endQPointDisplay.setX((int)displayDstBoundingRect.width() / 2);

        if ((int)endQPointDisplay.y() - (int)displayDstBoundingRect.height() / 2 < 0 )
            endQPointDisplay.setY((int)displayDstBoundingRect.height() / 2);

        if ((int)endQPointDisplay.x() + (int)displayDstBoundingRect.width() / 2 > tmpImage.width() - 1)
            endQPointDisplay.setX(tmpImage.width() - 1 - (int)displayDstBoundingRect.width() / 2);

        if ((int)endQPointDisplay.y() + (int)displayDstBoundingRect.height() / 2 > tmpImage.height() - 1)
            endQPointDisplay.setY(tmpImage.height() - 1 - (int)displayDstBoundingRect.height() / 2);

        // get unscaled image patch centers: source, destination
        patchSrcCenter = endQPointDisplay / scaleList[curScalePos];

        patchDstBoundingRect = ppathCircleWatermarkImage->boundingRect();

        // make sure the green(source) patch is inside the actual image
        if ((int)patchSrcCenter.x() - (int)patchDstBoundingRect.width() / 2 < 0 )
            patchSrcCenter.setX((int)patchDstBoundingRect.width() / 2);

        if ((int)patchSrcCenter.y() - (int)patchDstBoundingRect.height() / 2 < 0 )
            patchSrcCenter.setY((int)patchDstBoundingRect.height() / 2);

        if ((int)patchSrcCenter.x() + (int)patchDstBoundingRect.width() / 2 > inputQImage.width() - 1)
            patchSrcCenter.setX(inputQImage.width() - 1 - (int)displayDstBoundingRect.width() / 2);

        if ((int)patchSrcCenter.y() + (int)patchDstBoundingRect.height() / 2 > inputQImage.height() - 1)
            patchSrcCenter.setY(inputQImage.height() - 1 - (int)patchDstBoundingRect.height() / 2);

        patchDstCenter = QPointF(patchDstBoundingRect.x() + patchDstBoundingRect.width() / 2, patchDstBoundingRect.y() + patchDstBoundingRect.height() / 2);

        displayDstBoundingRect = ppathWatermarkPatchDisplay.boundingRect();

        painter.drawPath(ppathWatermarkPatchDisplay);

        // for green (source) patch offset to the red (destination) patch
        QPointF displayOffset = endQPointDisplay - QPointF(displayDstBoundingRect.x() + displayDstBoundingRect.width() / 2, displayDstBoundingRect.y() + displayDstBoundingRect.height() / 2);
//        QPointF imageOffset = displayOffset / scaleList[curScalePos];
        QPointF imageOffset = patchSrcCenter - patchDstCenter;

        QPainterPath ppathWatermarkReplaceDisplay = ppathWatermarkPatchDisplay.translated(displayOffset);

        QPainterPath ppathWatermarkReplaceImage = ppathCircleWatermarkImage->translated(imageOffset);

        painter.setPen(QPen(Qt::green, 2, Qt::DashLine, Qt::SquareCap, Qt::MiterJoin));  //QPen(QBrush(QColor(192, 0, 0, 192)), 2.0)

        painter.drawPath(ppathWatermarkReplaceDisplay);

        patchSrcBoundingRect = ppathWatermarkReplaceImage.boundingRect();

        curDisplayQImage = tmpImage.copy();

        // for copy the selection to the red circle
        QRectF displaySrcBoundingRect = ppathWatermarkReplaceDisplay.boundingRect();

        for (int irow = 0; irow < (int)displayDstBoundingRect.height(); irow++)
            for (int icol = 0; icol < (int)displayDstBoundingRect.width(); icol++)
            {
                if (ppathWatermarkPatchDisplay.contains(QPointF(icol + displayDstBoundingRect.x(), irow + displayDstBoundingRect.y())))
                {
                    QColor srcColor = curScaledQImage.pixelColor(icol + displaySrcBoundingRect.x(), irow+displaySrcBoundingRect.y());

                    curDisplayQImage.setPixelColor(icol + displayDstBoundingRect.x(), irow + displayDstBoundingRect.y(), srcColor);
                }
            }

        // for compare src patch and dst patch similarity
//        Mat mask((int)patchSrcBoundingRect.height(), (int)patchSrcBoundingRect.width(), CV_8UC1);
//        Mat patchDst = Mat(curMatBGR, Rect((int)patchDstBoundingRect.x(), (int)patchDstBoundingRect.y(), (int)patchDstBoundingRect.width(), (int)patchDstBoundingRect.height()));
//        Mat patchSrc = Mat(curMatBGR, Rect((int)patchSrcBoundingRect.x(), (int)patchSrcBoundingRect.y(), (int)patchSrcBoundingRect.width(), (int)patchSrcBoundingRect.height()));

//        getCircleWatermarkMask(mask);
//        cout << " in limit percentage: " << getPixelDiffThresholdPct(patchDst, patchSrc, mask, 20) << endl;

        update();

        return;
    }

    // mouse release -- draw edge
    if ((event->button() == Qt::LeftButton) && blnLineDrawSelected)
    {
        if (preQPointDisplay == endQPointDisplay)
            return;

        QImage tmpImage = curScaledQImage.copy();

//        QPainter painter(&curScaledQImage);
        QPainter painter(&tmpImage);

        painter.setPen(QPen(Qt::green, 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

        // get the current QPointF, adjust with the center offset
        endQPointDisplay = event->localPos()  - QPointF(paintOffsetX, paintOffsetY);

        resetPointInImage(&endQPointDisplay, curScaledQImage.width(), curScaledQImage.height());

        endQPointImage = endQPointDisplay / scaleList[curScalePos];
        resetPointInImage(&endQPointImage, inputQImage.width(), inputQImage.height());

        painter.drawLine(preQPointDisplay, endQPointDisplay);

//        curDisplayQImage = curScaledQImage.copy();
        curDisplayQImage = tmpImage.copy();

        update();

        blnLineDrawn = true;

        lineEdgeDrawn.setPoints(preQPointImage, endQPointImage);

        return;
    }
}


int TaoCanvasArea::makeScaleList(vector<float> &list)
{
//    int count = 0;

//    for (count = 1; count <= 15; count++)
//    {
//        float factor = (float) (count * 0.2);
//        list.push_back(factor);
//    }

    list.push_back(0.10);
    list.push_back(0.15);
    list.push_back(0.25);
    list.push_back(0.5);
    list.push_back(0.75);
    list.push_back(1.0);
    list.push_back(1.5);
    list.push_back(2.0);
    list.push_back(3.0);
    list.push_back(4.0);

    return 0;
}


void TaoCanvasArea::zoomImage(int direction)
{
//    if (curDisplayQImage.isNull())
//        return;

    float scaleFactor;
    QImage tmpScaledImage;

    switch (direction)
    {
    case -1:
        if (curScalePos == 0)
            return;

        scaleFactor = scaleList[--curScalePos];
//        cout << "scale factor: " << scaleFactor << endl;

        curScaledQImage = curQImage.scaled(scaleFactor * curQImage.size(),
                                            Qt::KeepAspectRatio, Qt::SmoothTransformation);

        curDisplayQImage = curQImage.scaled(scaleFactor * curQImage.size(),
                                            Qt::KeepAspectRatio, Qt::SmoothTransformation);

//        tmpScaledImage = curQImage.scaled(scaleFactor * curQImage.size(),
//                                            Qt::KeepAspectRatio, Qt::SmoothTransformation);

//        curScaledQImage = tmpScaledImage.convertToFormat(QImage::Format_RGB888);

//        curDisplayQImage = curScaledQImage.copy();

        break;

    case 1:
        if (curScalePos == (int)scaleList.size()-1)
            return;

        scaleFactor = scaleList[++curScalePos];
//        cout << "scale factor: " << scaleFactor << endl;

        curScaledQImage = curQImage.scaled(scaleFactor * curQImage.size(),
                                            Qt::KeepAspectRatio, Qt::SmoothTransformation);

        curDisplayQImage = curQImage.scaled(scaleFactor * curQImage.size(),
                                            Qt::KeepAspectRatio, Qt::SmoothTransformation);

//        tmpScaledImage = curQImage.scaled(scaleFactor * curQImage.size(),
//                                            Qt::KeepAspectRatio, Qt::SmoothTransformation);

//        curScaledQImage = tmpScaledImage.convertToFormat(QImage::Format_RGB888);

//        curDisplayQImage = curScaledQImage.copy();

        break;

    case 0:
        curScalePos = 5; // scale = 1.0,

        curScaledQImage = curQImage.copy();
        curDisplayQImage = curQImage.copy();
        break;

    default:
        ;
    }

    if (blnTextureDrawn && blnTextureCircleBkgdDrawn)
        driftDrawBrushPaths(&curDisplayQImage, false, true);

    if (blnTextureDrawn && !blnTextureCircleBkgdDrawn)
        driftDrawBrushPaths(&curDisplayQImage, true, false);

    if (blnPatchCircleWatermarkDrawn)
        patchDrawCircleWatermark(&curDisplayQImage);

    if (blnLineDrawn)
        lineDrawEdge(&curDisplayQImage);

    update();
}


void TaoCanvasArea::zoomRatioImage(int ratio)
{
    //ScalePos = 5;  scale = 1.0,
    switch (ratio)
    {
    case 1:
        curScalePos = 5;
        break;

    case 2:
        curScalePos = 7;
        break;

    case 3:
        curScalePos = 8;
        break;

    case 4:
        curScalePos = 9;
        break;

    default:
        return;
    }

    float scaleFactor = scaleList[curScalePos];

    curScaledQImage = curQImage.scaled(scaleFactor * curQImage.size(),
                                        Qt::KeepAspectRatio, Qt::SmoothTransformation);

    curDisplayQImage = curQImage.scaled(scaleFactor * curQImage.size(),
                                        Qt::KeepAspectRatio, Qt::SmoothTransformation);

    if (blnTextureDrawn && blnTextureCircleBkgdDrawn)
        driftDrawBrushPaths(&curDisplayQImage, false, true);

    if (blnTextureDrawn && !blnTextureCircleBkgdDrawn)
        driftDrawBrushPaths(&curDisplayQImage, true, false);

    if (blnPatchCircleWatermarkDrawn)
        patchDrawCircleWatermark(&curDisplayQImage);

    if (blnLineDrawn)
        lineDrawEdge(&curDisplayQImage);

    update();
}




/***
 * when zoom the image, draw circle watermark and its replacement
 */
void TaoCanvasArea::patchDrawCircleWatermark(QImage *input)
{
    QPainter painter(input);

    float scaleFactor = scaleList[curScalePos];

    painter.setPen(QPen(Qt::red, 2, Qt::DashLine, Qt::SquareCap, Qt::MiterJoin));

    QPainterPath ppathDisplay(patchCircleWatermarkPoints[0] * scaleFactor);

    for (int icount = 1; icount < (int)patchCircleWatermarkPoints.size(); icount++)
    {
        ppathDisplay.lineTo(patchCircleWatermarkPoints[icount] * scaleFactor);
    }

    painter.drawPath(ppathDisplay);

    QRectF displayDstBoundingRect = ppathDisplay.boundingRect();

    QPointF displayOffset = patchSrcCenter * scaleFactor - QPointF(displayDstBoundingRect.x() + displayDstBoundingRect.width() / 2,
                                                       displayDstBoundingRect.y() + displayDstBoundingRect.height() / 2);

    QPainterPath ppathReplace = ppathDisplay.translated(displayOffset);

    painter.setPen(QPen(Qt::green, 2, Qt::DashLine, Qt::SquareCap, Qt::MiterJoin));

    painter.drawPath(ppathReplace);
}


/***
 * when zoom image, draw the edge whick has been drawn
 */
void TaoCanvasArea::lineDrawEdge(QImage *input)
{
    if (lineEdgeDrawn.p1() == lineEdgeDrawn.p2())
        return;

    QPainter painter(input);

    painter.setPen(QPen(Qt::green, 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    float scaleFactor = scaleList[curScalePos];

    QLineF lineDisplay(lineEdgeDrawn.p1() * scaleFactor, lineEdgeDrawn.p2() * scaleFactor);

    painter.drawLine(lineDisplay);
}


void TaoCanvasArea::driftDrawSelection(QImage *input, QRect &selectionRect)
{
//    QPainter drawPainter(&curDisplayQImage);
    QPainter drawPainter(input);

    float scaleFactor = scaleList[curScalePos];

    for (int icount = 0; icount < (int)brushPointsPathList.size(); icount++)
    {
        struct BrushPointsPath curPath = brushPointsPathList[icount];

        drawPainter.setPen(QPen(QBrush(QColor(255, 0, 0, 96)), curPath.fBrushSize * scaleFactor, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

        QPainterPath brushPath(scaleFactor * curPath.brushPoints[0]);

        for (int iPointCount = 1; iPointCount < (int)curPath.brushPoints.size(); iPointCount++)
        {
           brushPath.lineTo(scaleFactor * curPath.brushPoints[iPointCount]);
        }

        drawPainter.drawPath(brushPath);
    }


    drawPainter.setPen(QPen(Qt::green, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    QRectF selectionDisplay(scaleFactor * selectionRect.topLeft(), scaleFactor * selectionRect.bottomRight());
    drawPainter.drawRect(selectionDisplay);
}



/***
 * when zoom image, draw the texture brush and circle bkgd
 */
void TaoCanvasArea::driftDrawBrushPaths(QImage *input, bool isDrawRect, bool isDrawCircleBkgd)
{
//    QPainter drawPainter(&curDisplayQImage);
    QPainter drawPainter(input);

    float scaleFactor = scaleList[curScalePos];

    for (int icount = 0; icount < (int)brushPointsPathList.size(); icount++)
    {
        struct BrushPointsPath curPath = brushPointsPathList[icount];

        drawPainter.setPen(QPen(QBrush(QColor(255, 0, 0, 96)), curPath.fBrushSize * scaleFactor, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

        QPainterPath brushPath(scaleFactor * curPath.brushPoints[0]);

        for (int iPointCount = 1; iPointCount < (int)curPath.brushPoints.size(); iPointCount++)
        {
           brushPath.lineTo(scaleFactor * curPath.brushPoints[iPointCount]);
        }

        drawPainter.drawPath(brushPath);
    }

    if (isDrawRect)
    {
        QRectF imageROI;
        getDisplayImageROI(imageROI);

        drawPainter.setPen(QPen(Qt::green, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

//        drawPainter.drawRect(imageROI);
        float scale = scaleList[curScalePos];
        QRectF displayStretchRect(scale * m_curStretchRect.topLeft(), scale * m_curStretchRect.bottomRight());
        drawPainter.drawRect(displayStretchRect);
    }

    if (isDrawCircleBkgd)
    {
        QPainterPath ppathDisplay(textureCircleBkgdPoints[0] * scaleFactor);

        for (int icount = 1; icount < (int)textureCircleBkgdPoints.size(); icount++)
        {
            ppathDisplay.lineTo(textureCircleBkgdPoints[icount] * scaleFactor);
        }

        drawPainter.setPen(QPen(Qt::green, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

        drawPainter.drawPath(ppathDisplay);
    }
}


QSize TaoCanvasArea::getDisplayImageSize()
{
    return scaleList[curScalePos]* curQImage.size();
}


void TaoCanvasArea::setPatchCompleted(bool isCompleted)
{
    blnPatchCompleted = isCompleted;
}


void TaoCanvasArea::setPatchToolsUnSelected()
{
    blnPatchCircleWatermarkSelected = false;
    blnPatchCircleWatermarkDrawn = false;

    if (ppathCircleWatermarkDisplay != NULL)
    {
        delete ppathCircleWatermarkDisplay;

        ppathCircleWatermarkDisplay = NULL;
    }
}


void TaoCanvasArea::setDriftToolsUnSelected()
{
    blnSmallBrushSelected = false;
    blnMediumBrushSelected = false;
    blnBigBrushSelected = false;
    blnTextureCircleBkgdSelected = false;
    blnTextureDrawn = false;
}


void TaoCanvasArea::setLineToolsUnSelected()
{
    blnLineDrawSelected = false;
    blnLineDrawn = false;
}


void TaoCanvasArea::setSmallBrushSelected(bool isChecked)
{
    iBrushSize = 4;
    blnSmallBrushSelected = isChecked;

    blnMediumBrushSelected = !isChecked;
    blnBigBrushSelected = !isChecked;

    blnTextureCircleBkgdSelected = !isChecked;
}


void TaoCanvasArea::setMediumBrushSelected(bool isChecked)
{
    iBrushSize = 12;
    blnMediumBrushSelected = isChecked;

    blnBigBrushSelected = !isChecked;
    blnSmallBrushSelected = !isChecked;

    blnTextureCircleBkgdSelected = !isChecked;
}


void TaoCanvasArea::setBigBrushSelected(bool isChecked)
{
    iBrushSize = 20;
    blnBigBrushSelected = isChecked;

    blnSmallBrushSelected = !isChecked;
    blnMediumBrushSelected = !isChecked;

    blnTextureCircleBkgdSelected = !isChecked;
}


void TaoCanvasArea::setPatchCircleWatermarkSelected(bool isChecked)
{
    blnPatchCircleWatermarkSelected = isChecked;
}


void TaoCanvasArea::setTextureCircleBkgdSelected(bool isChecked)
{
    // first set the texture circle bkgd is not drawn
    // discard the previous drawing
    blnTextureCircleBkgdDrawn = false;

    blnTextureCircleBkgdSelected = isChecked;

    blnSmallBrushSelected = !isChecked;
    blnMediumBrushSelected = !isChecked;
    blnBigBrushSelected = !isChecked;
}


void TaoCanvasArea::setLineDrawSelected(bool isChecked)
{
    blnLineDrawSelected = isChecked;
}


float TaoCanvasArea::getCurScale()
{
    return scaleList[curScalePos];
}


int TaoCanvasArea::displayCompletedImage(Mat &inMat)
{
    completedImageBGR = inMat.clone();

    // for copy the completed image back to class variable and display
    TaoConvertCV2Qt::convertMat2QImage(completedImageBGR, completedImageRGB, curQImage);

    curMatBGR = completedImageBGR.clone();
    curMatRGB = completedImageRGB.clone();

    float scaleFactor = scaleList[curScalePos];

    curScaledQImage = curQImage.scaled(scaleFactor * curQImage.size(),
                                        Qt::KeepAspectRatio, Qt::SmoothTransformation);

    curDisplayQImage = curQImage.scaled(scaleFactor * curQImage.size(),
                                        Qt::KeepAspectRatio, Qt::SmoothTransformation);

    update();

    return 0;
}


int TaoCanvasArea::undo()
{
    CompletionNode completion;

    if ((dqCompleteHist.size() == 0) || completeHistPos <= 0)
    {
        displayCompletedImage(inputMatBGR);

        return 1;
    }

    completion = dqCompleteHist.at(completeHistPos -1);
    completeHistPos--;

//    cout << " queue pos: " << completeHistPos << endl;

/*  switch (completion.method)
    {
        case PATCH_COMPLETION:
            ;
        case DRIFT_COMPLETION:
            ;
        case LINE_COMPLETION:
            ;
    }
    */

    displayCompletedImage(completion.image);

    return 0;
}


int TaoCanvasArea::redo()
{
    CompletionNode completion;

    if (((int)dqCompleteHist.size() == 0) || completeHistPos >= (int)dqCompleteHist.size() -1)
        return 1;

    completion = dqCompleteHist.at(completeHistPos +1);
    completeHistPos++;

//    cout << " queue pos: " << completeHistPos << endl;

/*  switch (completion.method)
    {
        case PATCH_COMPLETION:
            ;
        case DRIFT_COMPLETION:
            ;
        case LINE_COMPLETION:
            ;
    }
    */

    displayCompletedImage(completion.image);

    return 0;
}


// this function is not used.
int TaoCanvasArea::patchCompleteImage(bool blnBlendBkgd)
{
    if (!blnPatchCircleWatermarkDrawn)
        return 1;


    Mat blendMask((int)patchSrcBoundingRect.height(), (int)patchSrcBoundingRect.width(), CV_8UC1);

    blendMask = Scalar::all(255);

    for (int irow = 0; irow < (int)patchDstBoundingRect.height(); irow++)
        for (int icol = 0; icol < (int)patchDstBoundingRect.width(); icol++)
        {
            if (!ppathCircleWatermarkImage->contains(QPointF(icol + patchDstBoundingRect.x(), irow+patchDstBoundingRect.y())))
                blendMask.at<uchar>(irow, icol) = (uchar)0;
        }

//    imshow(" mask ", blendMask);

    Mat srcPatch(curMatBGR, Rect((int)patchSrcBoundingRect.x(), (int)patchSrcBoundingRect.y(), (int)patchSrcBoundingRect.width(), (int)patchSrcBoundingRect.height()));
    Mat dstPatch(curMatBGR, Rect((int)patchDstBoundingRect.x(), (int)patchDstBoundingRect.y(), (int)patchDstBoundingRect.width(), (int)patchDstBoundingRect.height()));

    if (blnBlendBkgd)
    {
        Mat seamlessBlend;
        Mat edgePatchSrc, edgePatchDst;
        int edgeWidth = 0;

        /*
        // for the image border, copy the the small rectangle to it.
        if ((int)patchDstBoundingRect.x() < edgeWidth)
        {
            edgePatchDst = Mat(curMatBGR, Rect((int)patchDstBoundingRect.x(), (int)patchDstBoundingRect.y(), edgeWidth - (int)patchDstBoundingRect.x(), (int)patchDstBoundingRect.height()));
            edgePatchSrc = Mat(curMatBGR, Rect((int)patchSrcBoundingRect.x(), (int)patchSrcBoundingRect.y(), edgeWidth - (int)patchDstBoundingRect.x(), (int)patchSrcBoundingRect.height()));

            edgePatchSrc.copyTo(edgePatchDst);
        }

        if ((int)patchDstBoundingRect.y() < edgeWidth)
        {
            edgePatchDst = Mat(curMatBGR, Rect((int)patchDstBoundingRect.x(), (int)patchDstBoundingRect.y(), (int)patchDstBoundingRect.width(), edgeWidth - (int)patchDstBoundingRect.y()));
            edgePatchSrc = Mat(curMatBGR, Rect((int)patchSrcBoundingRect.x(), (int)patchSrcBoundingRect.y(), (int)patchSrcBoundingRect.width(), edgeWidth - (int)patchDstBoundingRect.y()));

            edgePatchSrc.copyTo(edgePatchDst);
        }

        if ((int)patchDstBoundingRect.x() + (int)patchDstBoundingRect.width()- 1 > curMatBGR.cols - edgeWidth)
        {
            edgePatchDst = Mat(curMatBGR, Rect((int)patchDstBoundingRect.x() + (int)patchDstBoundingRect.width()- edgeWidth, (int)patchDstBoundingRect.y(), edgeWidth, (int)patchDstBoundingRect.height()));
            edgePatchSrc = Mat(curMatBGR, Rect((int)patchSrcBoundingRect.x() + (int)patchSrcBoundingRect.width() - edgeWidth, (int)patchSrcBoundingRect.y(), edgeWidth, (int)patchSrcBoundingRect.height()));

            edgePatchSrc.copyTo(edgePatchDst);
        }

        if ((int)patchDstBoundingRect.y() + (int)patchDstBoundingRect.height() -1 > curMatBGR.rows - edgeWidth)
        {
            edgePatchDst = Mat(curMatBGR, Rect((int)patchDstBoundingRect.x(), (int)patchDstBoundingRect.y() + (int)patchDstBoundingRect.height() - edgeWidth, (int)patchDstBoundingRect.width(), edgeWidth));
            edgePatchSrc = Mat(curMatBGR, Rect((int)patchSrcBoundingRect.x(), (int)patchSrcBoundingRect.y() + (int)patchSrcBoundingRect.height() - edgeWidth, (int)patchSrcBoundingRect.width(), edgeWidth));

            edgePatchSrc.copyTo(edgePatchDst);
        }
        */

        QTime curTime;
        curTime.start();

//        seamlessClone(srcPatch, curMatBGR, blendMask,
//                      Point((int)patchDstCenter.x(), (int)patchDstCenter.y()),
//                      seamlessBlend, cv::NORMAL_CLONE);
        //MIXED_CLONE -- does not work,   NORMAL_CLONE

        Rect2f exBoundingRect = getROIBoundRect(curMatBGR.cols, curMatBGR.rows, patchDstBoundingRect, 20);

//        cout << " dst bounding Rect " << patchDstBoundingRect.x() << " " << patchDstBoundingRect.y() << " " << patchDstBoundingRect.width() << " " << patchDstBoundingRect.height() << endl;
//        cout << " exbounding Rect " << exBoundingRect.x << " " << exBoundingRect.y << " " << exBoundingRect.width << " " << exBoundingRect.height << endl;
//        cout << " image: width/height: " << curMatBGR.cols << "  " << curMatBGR.rows << endl;

        Mat workerMat(curMatBGR, Rect((int)exBoundingRect.x, (int)exBoundingRect.y,
                                      (int)exBoundingRect.width, (int)exBoundingRect.height));
        Mat workerBlendOut;

        seamlessClone(srcPatch, workerMat, blendMask,
                      Point((int)(patchDstCenter.x() - exBoundingRect.x),
                            (int)(patchDstCenter.y() -exBoundingRect.y)),
                      workerBlendOut, cv::NORMAL_CLONE);

//        imshow(" worker blend", workerBlendOut);

        workerBlendOut.copyTo(workerMat);


//        cout << " seamlessClone time: " << curTime.elapsed() << endl;
//        imshow(" seamless", seamlessBlend);
//        completedImageBGR = seamlessBlend.clone();
        completedImageBGR = curMatBGR.clone();
    }
    else  //for not blend bkgd
    {
        srcPatch.copyTo(dstPatch, blendMask);

        completedImageBGR = curMatBGR.clone();

//        imshow(" copy", curMatBGR);
//        imshow(" src patch", srcPatch);
    }


    blnPatchCircleWatermarkSelected = false;
    blnPatchCircleWatermarkDrawn = false;

    // for copy the completed image back to class variable and display
    TaoConvertCV2Qt::convertMat2QImage(completedImageBGR, completedImageRGB, curQImage);

    curMatBGR = completedImageBGR.clone();
    curMatRGB = completedImageRGB.clone();

    float scaleFactor = scaleList[curScalePos];

    curScaledQImage = curQImage.scaled(scaleFactor * curQImage.size(),
                                        Qt::KeepAspectRatio, Qt::SmoothTransformation);

    curDisplayQImage = curQImage.scaled(scaleFactor * curQImage.size(),
                                        Qt::KeepAspectRatio, Qt::SmoothTransformation);

    patchClearDrawingVars();

    update();

    return 0;
}



int TaoCanvasArea::patchCompleteRun2Algos()
{
    if (!blnPatchCircleWatermarkDrawn)
        return 1;

    if (blnPatchCompleted)
        return 2;

    // if the drawing patch is less than 1, copy the current Mat to the output.
    if ((int)patchDstBoundingRect.height() < 1 || (int)patchDstBoundingRect.width() < 1)
    {
        curMatBGR.copyTo(completedImagePatchBlend);
        curMatBGR.copyTo(completedImagePatchCopy);

        return 0;
    }

    // if src patch is the same as dst patch, do nothing and return
    if ((int)patchSrcBoundingRect.x() == (int)patchDstBoundingRect.x()
            && (int)patchSrcBoundingRect.y() == (int)patchDstBoundingRect.y())
        return 0;

//    cout << " src bounding rect: " << (int)patchSrcBoundingRect.height() << " " << (int)patchSrcBoundingRect.width()<< endl;

    Mat blendMask((int)patchSrcBoundingRect.height(), (int)patchSrcBoundingRect.width(), CV_8UC1);

    blendMask = Scalar::all(255);

    for (int irow = 0; irow < (int)patchDstBoundingRect.height(); irow++)
        for (int icol = 0; icol < (int)patchDstBoundingRect.width(); icol++)
        {
            if (!ppathCircleWatermarkImage->contains(QPointF(icol + patchDstBoundingRect.x(), irow + patchDstBoundingRect.y())))
                blendMask.at<uchar>(irow, icol) = (uchar)0;
        }

//    imshow(" mask ", blendMask);

//    cout << " patch src bounding rect  x: " << (int)patchSrcBoundingRect.x() << " " << (int)patchSrcBoundingRect.y() << endl;

    // *********** patch copy without blending  *************
    completedImagePatchCopy = curMatBGR.clone();

    Mat srcCopyPatch(completedImagePatchCopy, Rect((int)patchSrcBoundingRect.x(), (int)patchSrcBoundingRect.y(), (int)patchSrcBoundingRect.width(), (int)patchSrcBoundingRect.height()));
    Mat dstCopyPatch(completedImagePatchCopy, Rect((int)patchDstBoundingRect.x(), (int)patchDstBoundingRect.y(), (int)patchDstBoundingRect.width(), (int)patchDstBoundingRect.height()));

    srcCopyPatch.copyTo(dstCopyPatch, blendMask);

    // ************ patch copy with blending  ************************
    Mat srcPatch(curMatBGR, Rect((int)patchSrcBoundingRect.x(), (int)patchSrcBoundingRect.y(), (int)patchSrcBoundingRect.width(), (int)patchSrcBoundingRect.height()));
    Mat dstPatch(curMatBGR, Rect((int)patchDstBoundingRect.x(), (int)patchDstBoundingRect.y(), (int)patchDstBoundingRect.width(), (int)patchDstBoundingRect.height()));

    completedImagePatchBlend = curMatBGR.clone();

//    Mat edgePatchSrc, edgePatchDst;
//    int edgeWidth = 0;

//    if ((int)patchDstBoundingRect.x() < edgeWidth)
//    {
//        edgePatchDst = Mat(completedImagePatchBlend, Rect((int)patchDstBoundingRect.x(), (int)patchDstBoundingRect.y(), edgeWidth, (int)patchDstBoundingRect.height()));
//        edgePatchSrc = Mat(completedImagePatchBlend, Rect((int)patchSrcBoundingRect.x(), (int)patchSrcBoundingRect.y(), edgeWidth, (int)patchSrcBoundingRect.height()));

//        edgePatchSrc.copyTo(edgePatchDst);
//    }

//    if ((int)patchDstBoundingRect.y() < edgeWidth)
//    {
//        edgePatchDst = Mat(completedImagePatchBlend, Rect((int)patchDstBoundingRect.x(), (int)patchDstBoundingRect.y(), (int)patchDstBoundingRect.width(), edgeWidth));
//        edgePatchSrc = Mat(completedImagePatchBlend, Rect((int)patchSrcBoundingRect.x(), (int)patchSrcBoundingRect.y(), (int)patchSrcBoundingRect.width(), edgeWidth));

//        edgePatchSrc.copyTo(edgePatchDst);
//    }

//    if ((int)patchDstBoundingRect.x() + (int)patchDstBoundingRect.width()- 1 > completedImagePatchBlend.cols - edgeWidth)
//    {
//        edgePatchDst = Mat(completedImagePatchBlend, Rect((int)patchDstBoundingRect.x() + (int)patchDstBoundingRect.width()- edgeWidth, (int)patchDstBoundingRect.y(), edgeWidth, (int)patchDstBoundingRect.height()));
//        edgePatchSrc = Mat(completedImagePatchBlend, Rect((int)patchSrcBoundingRect.x() + (int)patchSrcBoundingRect.width() - edgeWidth, (int)patchSrcBoundingRect.y(), edgeWidth, (int)patchSrcBoundingRect.height()));

//        edgePatchSrc.copyTo(edgePatchDst);
//    }

//    if ((int)patchDstBoundingRect.y() + (int)patchDstBoundingRect.height() -1 > completedImagePatchBlend.rows - edgeWidth)
//    {
//        edgePatchDst = Mat(completedImagePatchBlend, Rect((int)patchDstBoundingRect.x(), (int)patchDstBoundingRect.y() + (int)patchDstBoundingRect.height() - edgeWidth, (int)patchDstBoundingRect.width(), edgeWidth));
//        edgePatchSrc = Mat(completedImagePatchBlend, Rect((int)patchSrcBoundingRect.x(), (int)patchSrcBoundingRect.y() + (int)patchSrcBoundingRect.height() - edgeWidth, (int)patchSrcBoundingRect.width(), edgeWidth));

//        edgePatchSrc.copyTo(edgePatchDst);
//    }

    QTime curTime;
    curTime.start();

    Rect2f exBoundingRect = getROIBoundRect(completedImagePatchBlend.cols, completedImagePatchBlend.rows, patchDstBoundingRect, 5);

//    cout << " dst bounding Rect " << patchDstBoundingRect.x() << " " << patchDstBoundingRect.y() << " "
//                  << patchDstBoundingRect.width() << " " << patchDstBoundingRect.height() << endl;
//    cout << " exbounding Rect " << exBoundingRect.x << " " << exBoundingRect.y << " " << exBoundingRect.width << " " << exBoundingRect.height << endl;
//    cout << " image: width/height: " << completedImagePatchBlend.cols << "  " << completedImagePatchBlend.rows << endl;
//    cout << " dst patch center: " << (int)patchDstCenter.x() << "  " << (int)patchDstCenter.y() << endl;
//    cout << " blend center: " << (int)(patchDstCenter.x() - exBoundingRect.x) <<  " " << (int)(patchDstCenter.y() -exBoundingRect.y) << endl;

    Mat workerMat(completedImagePatchBlend, Rect((int)exBoundingRect.x, (int)exBoundingRect.y, (int)exBoundingRect.width, (int)exBoundingRect.height));
    Mat workerBlendOut;

    seamlessClone(srcPatch, workerMat, blendMask,
                  Point((int)(patchDstCenter.x() - exBoundingRect.x),
                        (int)(patchDstCenter.y() -exBoundingRect.y)),
                  workerBlendOut, cv::NORMAL_CLONE);
        //MIXED_CLONE -- does not work,   NORMAL_CLONE

    workerBlendOut.copyTo(workerMat);

//        cout << " seamlessClone time: " << curTime.elapsed() << endl;

    blnPatchCircleWatermarkDrawn = false;

    blnPatchCompleted = true;

    return 0;
}


int TaoCanvasArea::patchCompleteImageBlend()
{
//    int algoResult = -1;

    if (!blnPatchCompleted)
        patchCompleteRun2Algos();

    displayCompletedImage(completedImagePatchBlend);

    patchClearDrawingVars();

    // add to queue for undo/redo history
    CompletionNode completion;

    completion.method = PATCH_COMPLETION;
    completion.image = completedImageBGR.clone();

    if ((int)dqCompleteHist.size() >= histSizeLimit)
    {
        dqCompleteHist.pop_front();
        completeHistPos--;
    }

    dqCompleteHist.push_back(completion);

    completeHistPos++;

    return 0;
}


int TaoCanvasArea::patchCompleteImageCopy()
{
//    int algoResult = -1;

    if (!blnPatchCompleted)
        patchCompleteRun2Algos();

    displayCompletedImage(completedImagePatchCopy);

    patchClearDrawingVars();

    // add to queue for undo/redo history
    CompletionNode completion;

    completion.method = PATCH_COMPLETION;
    completion.image = completedImageBGR.clone();

    if ((int)dqCompleteHist.size() >= histSizeLimit)
    {
        dqCompleteHist.pop_front();
        completeHistPos--;
    }

    dqCompleteHist.push_back(completion);

    completeHistPos++;

    return 0;
}


int TaoCanvasArea::textureCompleteImageAuto()
{
    if (!blnTextureDrawn)
        return 1;

    // make watermark mask
    curMatMaskTexture = Mat(curQImage.height(), curQImage.width(), CV_8UC1);
    makeMaskMat(curMatMaskTexture, brushPointsPathList);

//    imshow(" cur mat mask", curMatMaskTexture);

//    Mat tmpImage = inputMatBGR.clone();
    Mat tmpImage = curMatBGR.clone();
    Mat tmpMask = curMatMaskTexture.clone();

    // complete watermark
    Rect maskROI;
    Rect imageROI;

    getMaskROI(maskROI, curMatMaskTexture);
    getImageROI(imageROI, curMatMaskTexture.cols, curMatMaskTexture.rows, maskROI);

    // if the brush area is too small
    if (maskROI.height < 1 || maskROI.width < 1)
    {
        displayCompletedImage(tmpImage);

        driftClearDrawingVars();

        return 2;
    }

    int patchSize = getPatchSize(maskROI.width, maskROI.height);
    int pixSpacing = getBkgdPixSpacing(maskROI.width, maskROI.height);

    Mat halfInputImage;
    Mat halfInputMask;

    cv::resize(tmpImage, halfInputImage, Size(), 0.5, 0.5, cv::INTER_AREA);
    cv::resize(tmpMask, halfInputMask, Size(), 0.5, 0.5, cv::INTER_NEAREST);

    Mat halfPushPulledImage = halfInputImage.clone();

    QTime curTime;

    curTime.start();

    int pushPullFactor = 8;

    if ((maskROI.width >= 80) && (maskROI.height >= 80))
        pushPullFactor = 16;

    TaoPushPullApprox::pushPullApprox(halfPushPulledImage, halfInputImage, halfInputMask, pushPullFactor);

//    cout << " push-pull half image time: " << curTime.elapsed() << endl;

//    imshow("half push-pulled", halfPushPulledImage);

    Rect halfImageROI;
    halfImageROI.x = (imageROI.x) / 2;
    halfImageROI.y = (imageROI.y)/ 2;
    halfImageROI.width = (2 * (imageROI.width / 2) + 2* halfImageROI.x -1) / 2 - halfImageROI.x +1;
    halfImageROI.height = (2 * (imageROI.height/ 2) + 2 * halfImageROI.y -1) / 2 - halfImageROI.y + 1;

    Rect halfMaskROI;
    halfMaskROI.x = (maskROI.x) / 2;
    halfMaskROI.y = (maskROI.y) / 2;
    halfMaskROI.width = (maskROI.width + 2 * halfMaskROI.x -1)/ 2 - halfMaskROI.x + 1;
    halfMaskROI.height = (maskROI.height + 2* halfMaskROI.y -1) / 2 - halfMaskROI.y + 1;

//    cout << "image ROI" << imageROI << endl;
//    cout << "half image ROI" << halfImageROI << endl;

    curTime.restart();

    imageCompletion = new TaoTextureAutoCompletion(halfPushPulledImage, halfInputMask, halfImageROI,
                                                   halfMaskROI, patchSize, pixSpacing);

    imageCompletion->completeImage();

//    cout << " complete half image time: " << curTime.elapsed() << endl;

//    imshow("half complete image", halfPushPulledImage);

    Mat resizeHalfCompletedImage = tmpImage.clone();

    cv::resize(halfPushPulledImage, resizeHalfCompletedImage, resizeHalfCompletedImage.size(), 0, 0, cv::INTER_CUBIC);

//    imshow("resizeHalfCompletedImage", resizeHalfCompletedImage);

    // copy the mask area back to inputImage
    for (int irow = 0; irow < tmpImage.rows; irow++)
        for (int icol = 0; icol < tmpImage.cols; icol++)
        {
            if (tmpMask.at<uchar>(irow, icol) > 0)
            {
                Vec3b curImagePointVal = resizeHalfCompletedImage.at<Vec3b>(irow, icol);

                tmpImage.at<Vec3b>(irow, icol)[0] = (uchar)curImagePointVal[0];
                tmpImage.at<Vec3b>(irow, icol)[1] = (uchar)curImagePointVal[1];
                tmpImage.at<Vec3b>(irow, icol)[2] = (uchar)curImagePointVal[2];
            }
        }

//    imshow("back to inputImage", tmpImage);

    delete(imageCompletion);

    imageCompletion = NULL;

    curTime.restart();

    imageCompletion = new TaoTextureAutoCompletion(tmpImage, tmpMask, imageROI,
                                                   maskROI, patchSize, pixSpacing);

    imageCompletion->completeImage();

//    cout << " complete full image time: " << curTime.elapsed() << endl;

//    imshow("completed image", tmpImage);

    displayCompletedImage(tmpImage);

    driftClearDrawingVars();

    blnTextureDrawn = false;

    // add to queue for undo/redo history
    CompletionNode completion;

    completion.method = DRIFT_COMPLETION;
    completion.image = completedImageBGR.clone();

    dqCompleteHist.push_back(completion);

    completeHistPos++;

    return 0;
}


int TaoCanvasArea::textureCompleteImageManual()
{
    if (!blnTextureDrawn)
        return 1;

    // make watermark mask
    curMatMaskTexture = Mat(curQImage.height(), curQImage.width(), CV_8UC1);
    makeMaskMat(curMatMaskTexture, brushPointsPathList);

//    Mat tmpImage = inputMatBGR.clone();
    Mat tmpImage = curMatBGR.clone();
    Mat tmpMask = curMatMaskTexture.clone();

    // complete watermark
    Rect maskROI;
    Rect imageROI;
    Rect bkgdROI;

    getMaskROI(maskROI, curMatMaskTexture);
    getImageROI(imageROI, curMatMaskTexture.cols, curMatMaskTexture.rows, maskROI);

    if (maskROI.height < 1 || maskROI.width < 1)
    {
        displayCompletedImage(tmpImage);

        driftClearDrawingVars();

        return 2;
    }

    bkgdROI.x = (int)textureBkgdBoundingRect.x();
    bkgdROI.y = (int)textureBkgdBoundingRect.y();
    bkgdROI.height = (int)textureBkgdBoundingRect.height();
    bkgdROI.width = (int)textureBkgdBoundingRect.width();

    int patchSize = getPatchSize(maskROI.width, maskROI.height);
    int pixSpacing = getBkgdPixSpacing(maskROI.width, maskROI.height);

    // if the bkgd ROI is smaller than 2* patchSize, remind user
    if ((bkgdROI.height < 2* patchSize) || (bkgdROI.width < 2 * patchSize))
    {
        blnTextureCircleBkgdDrawn = false;

        textureCircleBkgdPoints.clear();

        return 3;
    }

    QTime curTime;

    curTime.start();

    imageManualCompletion = new TaoTextureManualCompletion(tmpImage, tmpMask, bkgdROI, imageROI,
                                                           maskROI, patchSize, pixSpacing);

    imageManualCompletion->completeImage();

//    cout << " complete full image time: " << curTime.elapsed() << endl;

//    imshow("completed image", tmpImage);

    displayCompletedImage(tmpImage);

    driftClearDrawingVars();

    blnTextureDrawn = false;

    // add to queue for undo/redo history
    CompletionNode completion;

    completion.method = DRIFT_COMPLETION;
    completion.image = completedImageBGR.clone();

    dqCompleteHist.push_back(completion);

    completeHistPos++;

    return 0;
}


int TaoCanvasArea::lineCompleteImage()
{
    if (!blnLineDrawn)
        return 1;

    Mat tmpImage = curMatBGR.clone();

    Point2f cvPoint1, cvPoint2;
    QPointF point1 = lineEdgeDrawn.p1();
    QPointF point2 = lineEdgeDrawn.p2();

    TaoConvertCV2Qt::convertQPointF2CVPoint2f(point1, cvPoint1);
    TaoConvertCV2Qt::convertQPointF2CVPoint2f(point2, cvPoint2);

    TaoPropagateEdge::propagateConstEdge(tmpImage, cvPoint1, cvPoint2, 7);

    displayCompletedImage(tmpImage);

    lineEdgeDrawn.setPoints(QPointF(0,0), QPointF(0,0));

    // add to queue for undo/redo history
    CompletionNode completion;

    completion.method = LINE_COMPLETION;
    completion.image = completedImageBGR.clone();

    dqCompleteHist.push_back(completion);

    completeHistPos++;

    return 0;
}


bool  TaoCanvasArea::isDrawnPatch()
{
    return blnPatchCircleWatermarkDrawn;
}


bool  TaoCanvasArea::isPatchCompleted()
{
    return blnPatchCompleted;
}



bool  TaoCanvasArea::isDrawnTexture()
{
    return blnTextureDrawn;
}


bool  TaoCanvasArea::isDrawnTextureBkgd()
{
    return blnTextureCircleBkgdDrawn;
}


bool  TaoCanvasArea::isDrawnEdge()
{
    return blnLineDrawn;
}


bool TaoCanvasArea::makeMaskMat(Mat &outMaskMat, vector<struct BrushPointsPath> &brushPaths)
{
    QImage brushQImage(outMaskMat.cols, outMaskMat.rows, QImage::Format_Grayscale8);

    brushQImage.fill(0);

    QPainter drawPainter(&brushQImage);

    for (int icount = 0; icount < (int)brushPaths.size(); icount++)
    {
        struct BrushPointsPath curPointsPath = brushPaths[icount];

        drawPainter.setPen(QPen(QBrush(Qt::white), curPointsPath.fBrushSize, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

        // make a brush path
        QPainterPath curPainterPath(curPointsPath.brushPoints[0]);

        for (int iPointCount = 1; iPointCount < (int)curPointsPath.brushPoints.size(); iPointCount++)
        {
           curPainterPath.lineTo(curPointsPath.brushPoints[iPointCount]);
        }

        drawPainter.drawPath(curPainterPath);
    }

//    brushQImage.save(QString("d://makeMatMaskQImage.jpg"), "JPG");

    Mat outMaskMat2;
    TaoConvertCV2Qt::convertQImage2Mat(brushQImage, outMaskMat2);

    outMaskMat = outMaskMat2.clone();

    outMaskMat2.release();

    return true;
}



int TaoCanvasArea::getDrawingRect(QRect &drawingRect, Mat &inMaskMat)
{
    int minMaskRow = inMaskMat.rows;
    int minMaskCol = inMaskMat.cols;
    int maxMaskRow = -1;
    int maxMaskCol = -1;

    for (int irow = 0; irow < inMaskMat.rows; irow++)
        for (int icol = 0; icol < inMaskMat.cols; icol++)
        {
            if (inMaskMat.at<uchar>(irow, icol) > 0)
            {
                minMaskRow = minMaskRow > irow ? irow : minMaskRow;
                minMaskCol = minMaskCol > icol ? icol : minMaskCol;

                maxMaskRow = maxMaskRow < irow ? irow : maxMaskRow;
                maxMaskCol = maxMaskCol < icol ? icol : maxMaskCol;
            }
        }

    drawingRect.setLeft(minMaskCol);
    drawingRect.setTop(minMaskRow);
    drawingRect.setRight(maxMaskCol);
    drawingRect.setBottom(maxMaskRow);

    if (maxMaskRow == -1)
        return 0;
    else
        return 1;
}


int TaoCanvasArea::getMaskROI(Rect &maskRect, Mat &inMaskMat)
{
    int minMaskRow = inMaskMat.rows;
    int minMaskCol = inMaskMat.cols;
    int maxMaskRow = -1;
    int maxMaskCol = -1;

    for (int irow = 0; irow < inMaskMat.rows; irow++)
        for (int icol = 0; icol < inMaskMat.cols; icol++)
        {
            if (inMaskMat.at<uchar>(irow, icol) > 0)
            {
                minMaskRow = minMaskRow > irow ? irow : minMaskRow;
                minMaskCol = minMaskCol > icol ? icol : minMaskCol;

                maxMaskRow = maxMaskRow < irow ? irow : maxMaskRow;
                maxMaskCol = maxMaskCol < icol ? icol : maxMaskCol;
            }
        }

    maskRect.x = minMaskCol;
    maskRect.y = minMaskRow;
    maskRect.width = maxMaskCol - minMaskCol + 1;
    maskRect.height = maxMaskRow - minMaskRow + 1;

    if (maxMaskRow == -1)
        return 0;
    else
        return 1;
}


void TaoCanvasArea::getImageROI(Rect &imageROI, int icols, int irows,  Rect &watermark)
{
    int markWidth = watermark.width;
    int markHeight = watermark.height;
//    int minSize = markHeight <= markWidth ? markHeight : markWidth;
//    int maxSize = markHeight >= markWidth ? markHeight : markWidth;


    // define the margin of the mask
    int shoulder;

/*
    float maxminRatio = maxSize * 1.0 / minSize;

    if (maxminRatio >= 3.0)
        shoulder = maxSize / 3;
    else if (maxminRatio >= 2.0)
        shoulder = (int)(maxSize / 2.5);
    else
        shoulder = maxSize / 2;

    // make sure the shoulder is wide enough to get patches
    shoulder = shoulder > 15? shoulder : 15;

    // in order to reduce processing time and useful patches
    shoulder = shoulder > 100? 100 : shoulder;

//    cout << "shoulder: " << shoulder << endl;
*/
    shoulder = (int)(3.0 * getPatchSize(markHeight, markWidth));

    imageROI.x = ((watermark.x - shoulder) > 0) ? (watermark.x - shoulder) : 0 ;

    imageROI.width = (watermark.x + markWidth -1 + shoulder > icols -1 ) ?
                (icols - 1 - imageROI.x + 1) : (watermark.x + markWidth -1 + shoulder - imageROI.x + 1);

    imageROI.y = ((watermark.y - shoulder) > 0) ? (watermark.y - shoulder) : 0 ;

    imageROI.height = ((watermark.y + markHeight + shoulder)> irows) ?
                (irows - 1 - imageROI.y + 1) : (watermark.y + markHeight - 1 +  shoulder - imageROI.y + 1);
}


int TaoCanvasArea::getPatchSize(int watermarkHeight, int watermarkWidth)
{
    int minSize = watermarkHeight > watermarkWidth ? watermarkWidth : watermarkHeight;
    int patchSize = 7;

    if (minSize < 25)
        patchSize = 5;
    else if (minSize < 55)
        patchSize = 7;
    else if (minSize < 80)
        patchSize = 9;
    else if (minSize < 105)
        patchSize = 11;
    else
        patchSize = 11;

    return patchSize;
}


int TaoCanvasArea::getScalePos()
{
    return curScalePos;
}


int TaoCanvasArea::getBkgdPixSpacing(int watermarkHeight, int watermarkWidth)
{
    int minSize = watermarkHeight > watermarkWidth ? watermarkWidth : watermarkHeight;
    int pixSpacing = 2;

    if (minSize < 25)
        pixSpacing = 1;
    else if (minSize < 100)
        pixSpacing = 2;
    else if (minSize < 180)
        pixSpacing = 3;
    else
        pixSpacing = 3;

    return pixSpacing;
}


Rect2f TaoCanvasArea::getROIBoundRect(int imageWidth, int imageHeight, QRectF &boundingRect, int extension)
{
    Rect2f exBound;
    float endx, endy;

    exBound.x = boundingRect.x() - extension > 0? boundingRect.x() - extension : 0;
    exBound.y = boundingRect.y() - extension > 0? boundingRect.y() - extension : 0;

    endx = boundingRect.x() + boundingRect.width() - 1 + extension > imageWidth - 1? imageWidth - 1: boundingRect.x() + boundingRect.width()-1 + extension;
    endy = boundingRect.y() + boundingRect.height() - 1 + extension > imageHeight - 1? imageHeight - 1: boundingRect.y() +boundingRect.height() -1+ extension;

    exBound.width = endx - exBound.x;
    exBound.height = endy - exBound.y;

    return exBound;
}


void TaoCanvasArea::convertVecQLine2CVLine(vector<QLineF> &inVecLine, vector<struct CVLine> &outVecLine)
{
    for (int icount = 0; icount < (int)inVecLine.size(); icount++)
    {
        QLineF curQLine = inVecLine[icount];
        struct CVLine curCVLine;

        curCVLine.point1.x = curQLine.x1();
        curCVLine.point1.y = curQLine.y1();
        curCVLine.point2.x = curQLine.x2();
        curCVLine.point2.y = curQLine.y2();

        outVecLine.push_back(curCVLine);
    }
}


void TaoCanvasArea::resizeVecCVLine(vector<struct CVLine> &outVecLine, vector<struct CVLine> &inVecLine, float factor)
{
    for (int icount = 0; icount < (int)inVecLine.size(); icount++)
    {
        struct CVLine inLine = inVecLine[icount];
        struct CVLine outLine;

        outLine.point1.x = inLine.point1.x / factor;
        outLine.point1.y = inLine.point1.y / factor;
        outLine.point2.x = inLine.point2.x / factor;
        outLine.point2.y = inLine.point2.y / factor;

        outVecLine.push_back(outLine);
    }
}


void TaoCanvasArea::propagateEdges(Mat &inputImage, Mat &inputMask,
                                   vector<QLineF>  &lineList)
{
    Mat pushPulledImage = inputImage.clone();

    TaoPushPullApprox::pushPullApprox(pushPulledImage, inputImage, inputMask, 8);

    // for line edge propagation
    for (int lineCount = 0; lineCount < (int)lineList.size(); lineCount++)
    {
        Point2f cvPoint1, cvPoint2;
        QLineF curLine = lineList[lineCount];
        QPointF point1 = curLine.p1();
        QPointF point2 = curLine.p2();

        TaoConvertCV2Qt::convertQPointF2CVPoint2f(point1, cvPoint1);
        TaoConvertCV2Qt::convertQPointF2CVPoint2f(point2, cvPoint2);

//        TaoPropagateEdge::propagateLineEdge(inputImage, inputMask, pushPulledImage, cvPoint1, cvPoint2, 5);
        TaoPropagateEdge::propagateConstLine(inputImage, inputMask, cvPoint1, cvPoint2, 7);
    }

//    cout << " end line edge in function propagateEdges" << endl;

    pushPulledImage.release();
}


void TaoCanvasArea::getOrigImage(QImage &inImage)
{
    inImage = inputQImage;
}


void TaoCanvasArea::releaseMem()
{
    inputMatBGR.release();
    inputMatRGB.release();

    curMatBGR.release();
    curMatRGB.release();

    completedImageRGB.release();
    completedImageBGR.release();

    completedImagePatchBlend.release();
    completedImagePatchCopy.release();

    curMatMaskTexture.release();   // mask of the current QImage, make from the user's drawing

    scaleList.clear();  // the scale list for zoom in/out/one

    brushPathList.clear(); // vector of the brush path wiht brush size

    brushQPointsCurPath.clear();

    brushPointsPathList.clear(); // (for unscaled current QImage)vector of the brush path wiht brush siz

    patchCircleWatermarkPoints.clear();

    textureCircleBkgdPoints.clear();

    if (ppathBrushDisplay != NULL)
    {
        delete ppathBrushDisplay;  // for display
        ppathBrushDisplay = NULL;
    }

    if (ppathBrushImage != NULL)
    {
        delete ppathBrushImage;   // for the unscaled image
        ppathBrushImage = NULL;
    }

    if (imageCompletion != NULL)
    {
        delete imageCompletion;
        imageCompletion = NULL;
    }

    inputQImage.~QImage();  // original image
    curQImage.~QImage();  // current QImage with the original size, not scaled, might restore watermark
    curScaledQImage.~QImage();   // for store the scaled current QImage
    curDisplayQImage.~QImage();   // for display to the users, with scaled

    dequeClearCompleteHist();
}


int TaoCanvasArea::getCircleWatermarkMask(Mat &mask)
{
    mask = Scalar::all(255);

    for (int irow = 0; irow < (int)patchDstBoundingRect.height(); irow++)
        for (int icol = 0; icol < (int)patchDstBoundingRect.width(); icol++)
        {
            if (!ppathCircleWatermarkDisplay->contains(QPointF(icol + patchDstBoundingRect.x(), irow+patchDstBoundingRect.y())))
                mask.at<uchar>(irow, icol) = (uchar)0;
        }

    return 0;
}


float TaoCanvasArea::getPixelDiffThresholdPct(Mat &src1, Mat &src2, Mat &mask, int limit)
{
    int numPixels = 0;
    int totalPixels = 0;

    for (int irow = 0; irow < 5; irow++) //src1.rows
        for (int icol = 0; icol < src1.cols; icol++)
        {
            if (mask.at<uchar>(irow, icol) > 0)
            {
                totalPixels++;

                Vec3b pixel1 = src1.at<Vec3b>(irow, icol);
                Vec3b pixel2 = src2.at<Vec3b>(irow, icol);
                if ((abs(pixel1[0]- pixel2[0]) < limit) &&
                    (abs(pixel1[1]- pixel2[1]) < limit) &&
                    (abs(pixel1[2]- pixel2[2]) < limit))
                {
                    numPixels++;
                }
            }
        }

    for (int irow = src1.rows - 5; irow < src1.rows; irow++) //src1.rows
        for (int icol = 0; icol < src1.cols; icol++)
        {
            if (mask.at<uchar>(irow, icol) > 0)
            {
                totalPixels++;

                Vec3b pixel1 = src1.at<Vec3b>(irow, icol);
                Vec3b pixel2 = src2.at<Vec3b>(irow, icol);
                if ((abs(pixel1[0]- pixel2[0]) < limit) &&
                    (abs(pixel1[1]- pixel2[1]) < limit) &&
                    (abs(pixel1[2]- pixel2[2]) < limit))
                {
                    numPixels++;
                }
            }
        }
    return (numPixels * 1.0 / totalPixels);
}


int TaoCanvasArea::getDisplayImageROI(QRectF &inImageROI)
{
    // make display watermark mask
    float scaleFactor = scaleList[curScalePos];
/*
    // get the current display image(scaled) painter path list
    vector<struct BrushPointsPath>  brushPointsDisplayPathList;  // for scaled current QImage

    for (int icount = 0; icount < (int)brushPointsPathList.size(); icount++)
    {
        struct BrushPointsPath curPath = brushPointsPathList[icount];

        vector<QPointF> brushPointsDisplay;

        for (int iPointCount = 0; iPointCount < (int)curPath.brushPoints.size(); iPointCount++)
        {
           brushPointsDisplay.push_back(scaleFactor * curPath.brushPoints[iPointCount]);
        }

        struct BrushPointsPath curDisplayPath = {curPath.fBrushSize * scaleFactor, brushPointsDisplay};

        brushPointsDisplayPathList.push_back(curDisplayPath);
    }


    // get the mask of the painter path
    Mat curScaledMatMask(curScaledQImage.height(), curScaledQImage.width(), CV_8UC1);
    makeMaskMat(curScaledMatMask, brushPointsDisplayPathList);
*/
    Mat curMatMaskTexture(curQImage.height(), curQImage.width(), CV_8UC1);

    Rect maskROI;
    Rect imageROI;

    makeMaskMat(curMatMaskTexture, brushPointsPathList);
    getMaskROI(maskROI, curMatMaskTexture);
    getImageROI(imageROI, curMatMaskTexture.cols, curMatMaskTexture.rows, maskROI);

    inImageROI.setX(imageROI.x * scaleFactor);
    inImageROI.setY(imageROI.y * scaleFactor);
    inImageROI.setWidth(imageROI.width * scaleFactor);
    inImageROI.setHeight(imageROI.height * scaleFactor);

//    cout << " display image ROI: " << imageROI.x << " " << imageROI.y << " " << imageROI.width << " " << imageROI.height << endl;

    return 0;
}


void TaoCanvasArea::cleanVariables()
{
    // for patch completion
    blnPatchCircleWatermarkDrawn = false;
    blnPatchCompleted = false;
    patchCircleWatermarkPoints.clear();

    if (ppathCircleWatermarkDisplay != NULL)
    {
        delete ppathCircleWatermarkDisplay;
        ppathCircleWatermarkDisplay = NULL;
    }

    if (ppathCircleWatermarkImage != NULL)
    {
        delete ppathCircleWatermarkImage;
        ppathCircleWatermarkImage = NULL;
    }

    // for texture completion
    blnTextureDrawn = false;

    brushPointsPathList.clear();
    brushPathList.clear();
    brushQPointsCurPath.clear();
    textureCircleBkgdPoints.clear();

    if (ppathBrushDisplay  != NULL)
    {
        delete ppathBrushDisplay; // forr display image
        ppathBrushDisplay = NULL;
    }

    if (ppathBrushImage != NULL)
    {
        delete ppathBrushImage;   // for the unscaled image
        ppathBrushImage = NULL;
    }

    if (ppathTextureCircleBkgdDisplay != NULL)
    {
        delete ppathTextureCircleBkgdDisplay;
        ppathTextureCircleBkgdDisplay = NULL;
    }

    if (ppathTextureCircleBkgdImage != NULL)
    {
        delete ppathTextureCircleBkgdImage;
        ppathTextureCircleBkgdImage = NULL;
    }


    // for edge completion
    blnLineDrawn = false;

    // for undo/redo
    dequeClearCompleteHist();
}


int TaoCanvasArea::dequeClearCompleteHist()
{
    if (dqCompleteHist.size() == 0)
        return 0;

    for (int icount = 0; icount < (int)dqCompleteHist.size(); icount++)
    {
        struct CompletionNode curNode = dqCompleteHist.at(icount);

        ~curNode.image;
    }

    dqCompleteHist.clear();

    return 0;
}


void TaoCanvasArea::resetPointInImage(QPointF *inPoint, int imageWidth , int imageHeight)
{
    if (inPoint->x() < 0)
        inPoint->setX(0);

    if (inPoint->y() < 0)
        inPoint->setY(0);

    if (inPoint->x() > imageWidth -1)
        inPoint->setX(imageWidth - 1);

    if (inPoint->y() > imageHeight -1)
        inPoint->setY(imageHeight -1);
}


int TaoCanvasArea::shiftMapCompleteImage()
{
    if (!blnTextureDrawn)
        return 1;

    // make watermark mask
    curMatMaskTexture = Mat(curQImage.height(), curQImage.width(), CV_8UC1);
    makeMaskMat(curMatMaskTexture, brushPointsPathList);

//    imshow(" cur mat mask", curMatMaskTexture);

    Mat tmpImage = curMatBGR.clone();
    Mat tmpMask = curMatMaskTexture.clone();

    // complete watermark
    Rect maskROI;
    Rect imageROI;

    getMaskROI(maskROI, curMatMaskTexture);
    getImageROI(imageROI, curMatMaskTexture.cols, curMatMaskTexture.rows, maskROI);

    // if the brush area is too small
    if (maskROI.height < 1 || maskROI.width < 1)
    {
        displayCompletedImage(tmpImage);

        driftClearDrawingVars();

        return 2;
    }


    QTime curTime;

    curTime.start();

//    cout << " complete full image time: " << curTime.elapsed() << endl;

//    imshow("completed image", tmpImage);

    // for xinpaint function: background 255, watermark 0
    Mat maskCVWatermark_invert(curMatBGR.size(), CV_8UC1);

    maskCVWatermark_invert = Scalar::all(0);

    // invert the colors of the mask and background
    bitwise_not(curMatMaskTexture, maskCVWatermark_invert);

    Mat maskInpaint(maskCVWatermark_invert, imageROI);
    Mat tmpImageInpaint(tmpImage, imageROI);
    Mat curMatBGRInpaint(curMatBGR, imageROI);

//    xphoto::inpaint(curMatBGRInpaint, maskInpaint, tmpImageInpaint, xphoto::INPAINT_SHIFTMAP);

    displayCompletedImage(tmpImage);

    driftClearDrawingVars();

    blnTextureDrawn = false;

    // add to queue for undo/redo history
    CompletionNode completion;

    completion.method = DRIFT_COMPLETION;
    completion.image = completedImageBGR.clone();

    dqCompleteHist.push_back(completion);

    completeHistPos++;

    return 0;
}


int TaoCanvasArea::driftCompleteImage()
{
    if (!blnTextureDrawn)
        return 1;

    // make watermark mask
    curMatMaskTexture = Mat(curQImage.height(), curQImage.width(), CV_8UC1);
    makeMaskMat(curMatMaskTexture, brushPointsPathList);

//    imshow(" cur mat mask", curMatMaskTexture);

    Mat tmpImage = curMatBGR.clone();
    Mat tmpMask = curMatMaskTexture.clone();

    // complete watermark
    Rect maskROI(m_maskBoundRect.x(), m_maskBoundRect.y(), m_maskBoundRect.width(), m_maskBoundRect.height());
    Rect imageROI(m_curStretchRect.x(), m_curStretchRect.y(), m_curStretchRect.width(), m_curStretchRect.height());

//    cout << "m_maskBoundRect: " << m_maskBoundRect.width() << "  "<< m_maskBoundRect.height() <<endl;
//    cout << "maskROI: " << maskROI.width << " " << maskROI.height << endl;

//    getMaskROI(maskROI, curMatMaskTexture);
//    getImageROI(imageROI, curMatMaskTexture.cols, curMatMaskTexture.rows, maskROI);

    // if the brush area is too small
    if (maskROI.height < 1 || maskROI.width < 1)
    {
        displayCompletedImage(tmpImage);

        driftClearDrawingVars();

        return 2;
    }


    QTime curTime;

    curTime.start();

//    cout << " complete full image time: " << curTime.elapsed() << endl;

//    imshow("completed image", tmpImage);
    // for xinpaint function: background 255, watermark 0

    Mat maskCVWatermark_invert(curMatBGR.size(), CV_8UC1);

    maskCVWatermark_invert = Scalar::all(0);

    // invert the colors of the mask and background
    bitwise_not(curMatMaskTexture, maskCVWatermark_invert);

    Mat maskInpaint(maskCVWatermark_invert, imageROI);
    Mat tmpImageInpaint(tmpImage, imageROI);
    Mat curMatBGRInpaint(curMatBGR, imageROI);

//    xphoto::inpaint(curMatBGRInpaint, maskInpaint, tmpImageInpaint, xphoto::INPAINT_SHIFTMAP);
    XInpaint inpainter(this);

    if (imageROI.height * imageROI.width < 20000)
    {
        inpainter.xinpaintA(curMatBGRInpaint, maskInpaint, tmpImageInpaint, 0);
    }
    else
    {
        float ratio = (200.0 * 200.0) / (imageROI.height * imageROI.width);

        ratio = ratio > 0.5 ? ratio : 0.5;

        Mat maskInpaintRatio;
        Mat tmpImageInpaintRatio;
        Mat curMatBGRInpaintRatio;

        cv::resize(curMatBGRInpaint, curMatBGRInpaintRatio, Size(), ratio, ratio, cv::INTER_AREA);
        cv::resize(maskInpaint, maskInpaintRatio, Size(), ratio, ratio, cv::INTER_NEAREST);

        inpainter.xinpaintA(curMatBGRInpaintRatio, maskInpaintRatio, tmpImageInpaintRatio, 0);

        cv::resize(tmpImageInpaintRatio, tmpImageInpaint, tmpImageInpaint.size(), 0, 0, cv::INTER_CUBIC);

//        imshow(" orig", curMatBGRInpaintRatio);
//        imshow(" result", tmpImageInpaintRatio);
//        imshow(" resize to orig size", tmpImageInpaint);
    }

    displayCompletedImage(tmpImage);

    driftClearDrawingVars();

    blnTextureDrawn = false;

    // add to queue for undo/redo history
    CompletionNode completion;

    completion.method = DRIFT_COMPLETION;
    completion.image = completedImageBGR.clone();

    dqCompleteHist.push_back(completion);

    completeHistPos++;

    return 0;
}


StretchRectState TaoCanvasArea::getStretchRectState(QPointF curPoint, QRect &curRect)
{
    // coordinate of the four corners
    QPoint topLeft = curRect.topLeft();
    QPoint topRight = curRect.topRight();
    QPoint bottomLeft = curRect.bottomLeft();
    QPoint bottomRight = curRect.bottomRight();

    // center of the four edges
    QPoint leftCenter = QPoint(topLeft.x(), (topLeft.y() + bottomLeft.y()) / 2);
    QPoint topCenter = QPoint((topLeft.x() + topRight.x()) / 2, topLeft.y());
    QPoint rightCenter = QPoint(topRight.x(), leftCenter.y());
    QPoint bottomCenter = QPoint(topCenter.x(), bottomLeft.y());

    int stretchWidthRadius = curRect.width() / 8 >= STRETCH_RECT_WIDTH ? curRect.width() / 8 : STRETCH_RECT_WIDTH;
    int stretchHeightRadius = curRect.height() / 8 >= STRETCH_RECT_HEIGHT ? curRect.height() / 8 : STRETCH_RECT_HEIGHT;

    if ((abs(curPoint.x() - topLeft.x()) < stretchWidthRadius) &&
            (abs(curPoint.y() - topLeft.y()) < stretchHeightRadius))
        return StretchTopLeft;

    if ((abs(curPoint.x() - topRight.x()) < stretchWidthRadius) &&
            (abs(curPoint.y() - topRight.y()) < stretchHeightRadius))
        return StretchTopRight;

    if ((abs(curPoint.x() - bottomLeft.x()) < stretchWidthRadius) &&
            (abs(curPoint.y() - bottomLeft.y()) < stretchHeightRadius))
        return StretchBottomLeft;

    if ((abs(curPoint.x() - bottomRight.x()) < stretchWidthRadius) &&
            (abs(curPoint.y() - bottomRight.y()) < stretchHeightRadius))
        return StretchBottomRight;

    if ((abs(curPoint.x() - leftCenter.x()) < stretchWidthRadius) &&
            (abs(curPoint.y() - leftCenter.y()) < stretchHeightRadius))
        return StretchLeftCenter;

    if ((abs(curPoint.x() - topCenter.x()) < stretchWidthRadius) &&
            (abs(curPoint.y() - topCenter.y()) < stretchHeightRadius))
        return StretchTopCenter;

    if ((abs(curPoint.x() - rightCenter.x()) < stretchWidthRadius) &&
            (abs(curPoint.y() - rightCenter.y()) < stretchHeightRadius))
        return StretchRightCenter;

    if ((abs(curPoint.x() - bottomCenter.x()) < stretchWidthRadius) &&
            (abs(curPoint.y() - bottomCenter.y()) < stretchHeightRadius))
        return StretchBottomCenter;

    return StretchNotSelect;
}


// set the cursor style when on the Strect Bkgd Rect
void TaoCanvasArea::setStretchCursorStyle(StretchRectState stretchRectState)
{
    switch (stretchRectState)
    {
    case StretchNotSelect:
        unsetCursor();
        break;

    case StretchTopLeft:
    case StretchBottomRight:
        setCursor(Qt::SizeFDiagCursor);
        break;

    case StretchTopRight:
    case StretchBottomLeft:
        setCursor(Qt::SizeBDiagCursor);
        break;

    case StretchLeftCenter:
    case StretchRightCenter:
        setCursor(Qt::SizeHorCursor);
        break;

    case StretchTopCenter:
    case StretchBottomCenter:
        setCursor(Qt::SizeVerCursor);
        break;

    default:
        break;
    }
}


void TaoCanvasArea::getMinMaxInitStretchRect(QRect &minRect, QRect &maxRect, QRect &initRect,
                          QRect &drawingRect, int imageWidth, int imageHeight)
{
    int maxMargin;

    int minMargin = 4;

    int drawingHeight = drawingRect.height();
    int drawingWidth = drawingRect.width();

    maxMargin = (int)(8.0 * getPatchSize(drawingHeight, drawingWidth));

    // set the min stretch rectangle
    minRect.setLeft(drawingRect.left() - minMargin >= 0 ? drawingRect.x() - minMargin : 0);

    minRect.setRight(drawingRect.right() + minMargin >= imageWidth -1 ?
                imageWidth - 1 : drawingRect.right() + minMargin);

    minRect.setTop(drawingRect.top() - minMargin >= 0 ? drawingRect.top() - minMargin : 0);

    minRect.setBottom(drawingRect.bottom() + minMargin >= imageHeight -1 ?
                imageHeight - 1 : drawingRect.bottom() + minMargin);

    // set the max stretch rectangle
    maxRect.setLeft(drawingRect.left() - maxMargin >= 0 ? drawingRect.x() - maxMargin : 0);

    maxRect.setRight(drawingRect.right() + maxMargin >= imageWidth -1 ?
                imageWidth - 1 : drawingRect.right() + maxMargin);

    maxRect.setTop(drawingRect.top() - maxMargin >= 0 ? drawingRect.top() - maxMargin : 0);

    maxRect.setBottom(drawingRect.bottom() + maxMargin >= imageHeight -1 ?
                imageHeight - 1 : drawingRect.bottom() + maxMargin);

    // set the initial stretch rectangle
    int initMargin = (int)(4.0 * getPatchSize(drawingHeight, drawingWidth));
    initRect.setLeft(drawingRect.left() - initMargin >= 0 ? drawingRect.x() - initMargin : 0);

    initRect.setRight(drawingRect.right() + initMargin >= imageWidth -1 ?
                imageWidth - 1 : drawingRect.right() + initMargin);

    initRect.setTop(drawingRect.top() - initMargin >= 0 ? drawingRect.top() - initMargin : 0);

    initRect.setBottom(drawingRect.bottom() + initMargin >= imageHeight -1 ?
                imageHeight - 1 : drawingRect.bottom() + initMargin);
}



// update the current display stretch retangle
void TaoCanvasArea::updateStretchRect(QRect &curRect, StretchRectState stretchState, QPointF &curPoint, QRect &minRect, QRect &maxRect)
{
    switch (stretchState)
    {
    case StretchLeftCenter:
        if ((int)curPoint.x() > minRect.left())
            curRect.setLeft(minRect.left());
        else if ((int)curPoint.x() < maxRect.left())
            curRect.setLeft(maxRect.left());
        else
            curRect.setLeft((int)curPoint.x());
        break;

    case StretchRightCenter:
        if ((int)curPoint.x() < minRect.right())
            curRect.setRight(minRect.right());
        else if ((int)curPoint.x() > maxRect.right())
            curRect.setRight(maxRect.right());
        else
            curRect.setRight((int)curPoint.x());
        break;

    case StretchTopCenter:
        if ((int)curPoint.y() < maxRect.top())
            curRect.setTop(maxRect.top());
        else if ((int)curPoint.y() > minRect.top())
            curRect.setTop(minRect.top());
        else
            curRect.setTop((int)curPoint.y());
        break;

    case StretchBottomCenter:
        if ((int)curPoint.y() < minRect.bottom())
            curRect.setBottom(minRect.bottom());
        else if ((int)curPoint.y() > maxRect.bottom())
            curRect.setBottom(maxRect.bottom());
        else
            curRect.setBottom((int)curPoint.y());
        break;

    case StretchTopLeft:
        if ((int)curPoint.y() < maxRect.top())
            curRect.setTop(maxRect.top());
        else if ((int)curPoint.y() > minRect.top())
            curRect.setTop(minRect.top());
        else
            curRect.setTop((int)curPoint.y());

        if ((int)curPoint.x() > minRect.left())
            curRect.setLeft(minRect.left());
        else if ((int)curPoint.x() < maxRect.left())
            curRect.setLeft(maxRect.left());
        else
            curRect.setLeft((int)curPoint.x());
        break;

    case StretchTopRight:
        if ((int)curPoint.y() < maxRect.top())
            curRect.setTop(maxRect.top());
        else if ((int)curPoint.y() > minRect.top())
            curRect.setTop(minRect.top());
        else
        curRect.setTop((int)curPoint.y());

        if ((int)curPoint.x() < minRect.right())
            curRect.setRight(minRect.right());
        else if ((int)curPoint.x() > maxRect.right())
            curRect.setRight(maxRect.right());
        else
            curRect.setRight((int)curPoint.x());
        break;

    case StretchBottomLeft:
        if ((int)curPoint.y() < minRect.bottom())
            curRect.setBottom(minRect.bottom());
        else if ((int)curPoint.y() > maxRect.bottom())
            curRect.setBottom(maxRect.bottom());
        else
            curRect.setBottom((int)curPoint.y());

        if ((int)curPoint.x() > minRect.left())
            curRect.setLeft(minRect.left());
        else if ((int)curPoint.x() < maxRect.left())
            curRect.setLeft(maxRect.left());
        else
            curRect.setLeft((int)curPoint.x());
        break;

    case StretchBottomRight:
        if ((int)curPoint.y() < minRect.bottom())
            curRect.setBottom(minRect.bottom());
        else if ((int)curPoint.y() > maxRect.bottom())
            curRect.setBottom(maxRect.bottom());
        else
            curRect.setBottom((int)curPoint.y());

        if ((int)curPoint.x() < minRect.right())
            curRect.setRight(minRect.right());
        else if ((int)curPoint.x() > maxRect.right())
            curRect.setRight(maxRect.right());
        else
            curRect.setRight((int)curPoint.x());
        break;

    default:
        ;
    }
}
