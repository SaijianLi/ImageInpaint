

#ifndef TaoCanvasArea_H
#define TaoCanvasArea_H

#include <QMessageBox>
#include <QWidget>
#include <QPainter>
#include <QRect>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QPoint>
#include <QPolygon>
#include <QPainterPath>
#include <QScrollArea>
#include <QTime>
#include <QTextCodec>
#include <QLibrary>

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/photo/photo.hpp>
#include <opencv2/core.hpp>

#include <opencv2/stitching.hpp>
#include <opencv2/stitching/detail/blenders.hpp>

#include <opencv2/xphoto/inpainting.hpp>

#include <cstdio>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <deque>


#include "taoconvertcv2qt.h"
#include "taopropagateedge.h"
#include "taotextureautocompletion.h"
#include "taotexturemanualcompletion.h"

#include "xinpaint/xinpaint.h"


#define STRETCH_RECT_WIDTH 8
#define STRETCH_RECT_HEIGHT 8

using namespace cv;
using namespace std;

// current mouse point strect rect state for bkgd selection
enum StretchRectState{
    StretchNotSelect = 0,
    StretchTopLeft,
    StretchTopRight,
    StretchBottomLeft,
    StretchBottomRight,
    StretchLeftCenter,
    StretchTopCenter,
    StretchRightCenter,
    StretchBottomCenter
};


struct Node
{
    Point curPt;
    Node* next;
};


struct BrushPainterPath
{
    float fBrushSize;
    QPainterPath ppathBrush;
};


struct BrushPointsPath
{
    float fBrushSize;
    vector<QPointF> brushPoints;
};


enum CompletedMethod { PATCH_COMPLETION = 1, DRIFT_COMPLETION, LINE_COMPLETION } ;

struct CompletionNode
{
    enum CompletedMethod method;
    Mat image;
};


class TaoCanvasArea : public QWidget
{
    Q_OBJECT
public:
    explicit TaoCanvasArea(QWidget *parent = 0);

    ~TaoCanvasArea();

    int openImage(const QString &fileName);
    bool reloadImage();
    bool viewImage();
    bool saveImage(const QString &fileName); //, const char *fileFormat);

    bool loadCurQImage(); // load current QImage, without any drawings

    bool driftClearImage();
    bool patchClearImage();
    bool lineClearImage();

    int textureCompleteImageAuto();
    int textureCompleteImageManual();
    int shiftMapCompleteImage();

    int driftCompleteImage();

    int patchCompleteImage(bool blnBlendBkgd);

    int patchCompleteImageBlend();
    int patchCompleteImageCopy();
    int patchCompleteRun2Algos();

    int lineCompleteImage();

    void zoomImage(int direction);
    void zoomRatioImage(int ratio);

    QSize getDisplayImageSize();
    float getCurScale();

    bool isDrawnTextureBkgd();
    bool isDrawnTexture();
    bool isDrawnPatch();
    bool isDrawnEdge();
    bool isPatchCompleted();

    int dequeClearCompleteHist();
    void releaseMem();
    void cleanVariables();

    void getOrigImage(QImage &inImage);
    int getScalePos();

    int undo();
    int redo();

    void setPatchCircleWatermarkSelected(bool isChecked);
    void setPatchCompleted(bool isCompleted);
    void setPatchToolsUnSelected();
    void patchClearDrawingVars();

    void setTextureCircleBkgdSelected(bool isChecked);

    void setSmallBrushSelected(bool isChecked);
    void setMediumBrushSelected(bool isChecked);
    void setBigBrushSelected(bool isChecked);
    void setDriftToolsUnSelected();
    void driftClearDrawingVars();

    void setLineDrawSelected(bool isChecked);
    void setLineToolsUnSelected();
    void lineClearDrawingVars();

protected:
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
//    void mouseDoubleClickEvent(QMouseEvent *event)  Q_DECL_OVERRIDE;

private:
    int makeScaleList(vector<float> &list);

    void driftDrawBrushPaths(QImage *input, bool isDrawRect, bool isCircleBkgd);

    bool makeMaskMat(Mat &outMaskMat, vector<struct BrushPointsPath> &brushPaths);

    int getMaskROI(Rect &maskRect, Mat &inMaskQImage);

    void getImageROI(Rect &imageROI, int icols, int irows,  Rect &watermark);

    int getDisplayImageROI(QRectF &imageROI);

    int getPatchSize(int watermarkHeight, int watermarkWidth);

    int getBkgdPixSpacing(int watermarkHeight, int watermarkWidth);

    Rect2f getROIBoundRect(int imageWidth, int imageHeight, QRectF &boundingRect, int extension);

    void convertVecQLine2CVLine(vector<QLineF> &inVecLine, vector<CVLine> &outVecLine);

    void resizeVecCVLine(vector<struct CVLine> &outVecLine, vector<struct CVLine> &inVecLine, float factor);

    void propagateEdges(Mat &inputImage, Mat &inputMask, vector<QLineF>  &lineList);

    float getPixelDiffThresholdPct(Mat &src1, Mat &src2, Mat &mask, int limit);
    int getCircleWatermarkMask(Mat &mask);

    void resetPointInImage(QPointF *inPoint, int width , int height);

    Mat inputMatBGR;  // original Mat
    Mat inputMatRGB;  // convert to RGB format from original Mat for QImage, always keep valid

    Mat curMatBGR;
    Mat curMatRGB;

    QImage inputQImage;  // original image
    QImage curQImage;  // current QImage with the original size, not scaled, might restore watermark
    QImage curScaledQImage;   // for store the scaled current QImage
    QImage curDisplayQImage;   // for display to the users, with scaled, user drawn

    Mat completedImageRGB;
    Mat completedImageBGR;

    vector<float> scaleList;  // the scale list for zoom in/out/one

    int curScalePos = 5;
    // 0.10, 0.15, 0.25, 0.5, 0.75, 1.0(position is 4), 1.5, 2.0, 3.0, 4.0

    QPointF preQPointDisplay;
    QPointF preQPointImage;
    QPointF endQPointDisplay;
    QPointF endQPointImage;

    QPointF firstCircleQPointDisplay;
    QPointF firstCircleQPointImage;


    // **************************************************
    // for Texture completion
    TaoTextureAutoCompletion *imageCompletion = NULL;
    TaoTextureManualCompletion *imageManualCompletion = NULL;

    Mat curMatMaskTexture;   // mask of the current QImage, make from the user's drawing
    // bkgd --- 0, frgd(user drawing) --- 255

    int iBrushSize = 10;  // small , medium , big

    bool blnTextureDrawn = false;
    bool blnBrushing = false;
    bool blnSmallBrushSelected = false;
    bool blnMediumBrushSelected = false;
    bool blnBigBrushSelected = false;

    QPainterPath *ppathBrushDisplay  = NULL;  // for display
    QPainterPath *ppathBrushImage = NULL;   // for the unscaled image

    // in order to reproduce the user drawing, we have to record the points of the path.
    // Because the path is for scaled QImage, it could not be used to reproduce

    // vector of the brush path wiht brush size, (for unscaled current QImage)
    vector<struct BrushPointsPath>  brushPointsPathList;

    // vector of the brush path with brush size, (for unscaled current QImage)
    vector<struct BrushPainterPath>  brushPathList;

    vector<QPointF> brushQPointsCurPath;
    vector<QPointF> textureCircleBkgdPoints;

    // circle bkgd -- for texture completion:
    bool blnTextureCircleBkgdSelected = false;
    bool blnTextureCircleBkgdDrawn = false;
    QRectF textureBkgdBoundingRect;

    QPainterPath *ppathTextureCircleBkgdDisplay = NULL;
    QPainterPath *ppathTextureCircleBkgdImage = NULL;

    // *************************************************
    // for patch completion: circle watermark
    bool blnPatchCircleWatermarkSelected = false;
    bool blnPatchCircleWatermarkDrawn = false;
    bool blnPatchCompleted = false;

    QPainterPath *ppathCircleWatermarkDisplay = NULL;
    QPainterPath *ppathCircleWatermarkImage = NULL;
    vector<QPointF> patchCircleWatermarkPoints;

    QPointF patchSrcCenter;
    QPointF patchDstCenter;

    QRectF patchSrcBoundingRect;
    QRectF patchDstBoundingRect;

    Mat completedImagePatchBlend;
    Mat completedImagePatchCopy;

    // *************************************************
    // for edge completion
    bool blnLineDrawSelected = false;
    bool blnLineDrawn = false;
    QLineF lineEdgeDrawn;

    // for undo/redo
    deque<CompletionNode> dqCompleteHist;
    int completeHistPos = -1;
    const int histSizeLimit = 20;

    int displayCompletedImage(Mat &inMat);

    void patchDrawCircleWatermark(QImage *input);
    void lineDrawEdge(QImage *input);

    // adjust with the center offset
    float paintOffsetX;
    float paintOffsetY = 10;

    // current mouse at Bkgd rect state
    StretchRectState m_stretchRectState;

    StretchRectState getStretchRectState(QPointF curPoint, QRect &curRect);
    void setStretchCursorStyle(StretchRectState stretchRectState);
    int getDrawingRect(QRect &drawingRect, Mat &inMaskMat);
    void getMinMaxInitStretchRect(QRect &minRect, QRect &maxRect, QRect &initRect,
                              QRect &drawingRect, int width, int height);

    bool blnBeginMoveStretchRect = false;

    void driftDrawSelection(QImage *input, QRect &selectionRect);

    QRect m_curStretchRect;
    QRect m_maskBoundRect;
    QRect m_maxStretchRect;
    QRect m_minStretchRect;

    void updateStretchRect(QRect &curRect, StretchRectState stretchState,
                        QPointF &curPoint, QRect &minRect, QRect &maxRect);
};

#endif // TaoCanvasArea_H
