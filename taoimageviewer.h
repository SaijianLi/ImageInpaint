

#ifndef TAOIMAGEVIEWER_H
#define TAOIMAGEVIEWER_H

#include <QMainWindow>

#include <QLabel>
#include <QPixmap>
#include <QImageReader>
#include <QScrollArea>
#include <QToolBar>
#include <QToolButton>
#include <vector>

using namespace std;

namespace Ui {
class TaoImageViewer;
}

class TaoImageViewer : public QMainWindow
{
    Q_OBJECT

public:
    explicit TaoImageViewer(QWidget *parent = 0);
    ~TaoImageViewer();

    void setImage(QImage &inImage);
    void setScalePos(int pos);

private slots:
    void zoomInClicked();
    void zoomOutClicked();
    void zoomOneClicked();

private:
    Ui::TaoImageViewer *ui;

    QToolBar *toolBar;

    QToolButton *btnZoomIn;
    QToolButton *btnZoomOut;
    QToolButton *btnZoomOne;

    QLabel lblScale;

    QImage displayImage;
    QImage curImage;
    QLabel *imageLabel;
    QScrollArea *scrollArea;

    std::vector<float> scaleList;  // the scale list for zoom in/out/one
    int curScalePos = 5;
    // 0.10, 0.15, 0.25, 0.5, 0.75, 1.0(position is 4), 1.5, 2.0, 3.0, 4.0

    int assignScaleList(vector<float> &list);

    void createToolBar();
};

#endif // TAOIMAGEVIEWER_H
