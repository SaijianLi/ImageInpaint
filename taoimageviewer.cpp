

#include "taoimageviewer.h"
#include "ui_taoimageviewer.h"


TaoImageViewer::TaoImageViewer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TaoImageViewer)
{
    ui->setupUi(this);

    createToolBar();

    assignScaleList(scaleList);

    imageLabel = new QLabel;

    scrollArea = new QScrollArea;

    scrollArea->setBackgroundRole(QPalette::Light);
    scrollArea->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    scrollArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustIgnored);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    scrollArea->setVisible(true);
    scrollArea->setWidgetResizable(true);

    scrollArea->setWidget(imageLabel);

    this->setCentralWidget(scrollArea);
}


TaoImageViewer::~TaoImageViewer()
{
    delete ui;
}

void TaoImageViewer::setImage(QImage &inImage)
{
    curImage = inImage;

//    displayImage = curImage.copy();

    float scaleFactor = scaleList[curScalePos];
    displayImage = curImage.scaled(scaleFactor * curImage.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    imageLabel->setGeometry(0, 0, displayImage.width(), displayImage.height());
    imageLabel->setPixmap(QPixmap::fromImage(displayImage));

    lblScale.setText(QString(" %1\% ").arg((int)(100 * (0.005 + scaleList[curScalePos]))));
}


void TaoImageViewer::createToolBar()
{
    toolBar = ui->toolBar;
//    addToolBar(Qt::TopToolBarArea, toolBar);
    toolBar->setFixedHeight(40);
    toolBar->setIconSize(QSize(32,32));

    // zoom out
    btnZoomOut = new QToolButton(this);
    QIcon iconZoomOut(tr(".//icons//ZoomOut_32px.ico"));

    btnZoomOut->setIcon(iconZoomOut);
    btnZoomOut->setToolTip(tr("缩小"));
    btnZoomOut->setAutoRaise(true);
    toolBar->addWidget(btnZoomOut);
    connect(btnZoomOut, SIGNAL(clicked(bool)), this, SLOT(zoomOutClicked()));

    btnZoomIn = new QToolButton(this);
    QIcon iconZoomIn(tr(".//icons//ZoomIn_32px.ico"));

    btnZoomIn->setIcon(iconZoomIn);
    btnZoomIn->setToolTip(tr("放大"));
    btnZoomIn->setAutoRaise(true);
    toolBar->addWidget(btnZoomIn);
    connect(btnZoomIn, SIGNAL(clicked(bool)), this, SLOT(zoomInClicked()));

    btnZoomOne = new QToolButton(this);
    QIcon iconZoomOne(tr(".//icons//ZoomOne_32px.ico"));

    btnZoomOne->setIcon(iconZoomOne);
    btnZoomOne->setToolTip(tr("原图大小"));
    btnZoomOne->setAutoRaise(true);
    toolBar->addWidget(btnZoomOne);
    connect(btnZoomOne, SIGNAL(clicked(bool)), this, SLOT(zoomOneClicked()));

    lblScale.setBaseSize(40, 32);
    lblScale.setFont(QFont(QGuiApplication::font().family(), 14));
    toolBar->addWidget(&lblScale);
}


int TaoImageViewer::assignScaleList(vector<float> &list)
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



void TaoImageViewer::setScalePos(int pos)
{
    curScalePos = pos;
}


void TaoImageViewer::zoomInClicked()
{
    if (curScalePos == (int)scaleList.size() - 1)
        return;

    float scaleFactor = scaleList[++curScalePos];
    displayImage = curImage.scaled(scaleFactor * curImage.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    imageLabel->setGeometry(0, 0, displayImage.width(), displayImage.height());
    imageLabel->setPixmap(QPixmap::fromImage(displayImage));

    lblScale.setText(QString(" %1\% ").arg((int)(100 * (0.005 + scaleList[curScalePos]))));
}


void TaoImageViewer::zoomOutClicked()
{
    if (curScalePos == 0)
        return;

    float scaleFactor = scaleList[--curScalePos];
    displayImage = curImage.scaled(scaleFactor * curImage.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    imageLabel->setGeometry(0, 0, displayImage.width(), displayImage.height());
    imageLabel->setPixmap(QPixmap::fromImage(displayImage));

    lblScale.setText(QString(" %1\% ").arg((int)(100 * (0.005 + scaleList[curScalePos]))));
}


void TaoImageViewer::zoomOneClicked()
{
    curScalePos = 5;

    displayImage = curImage;

    imageLabel->setGeometry(0, 0, displayImage.width(), displayImage.height());
    imageLabel->setPixmap(QPixmap::fromImage(displayImage));

    lblScale.setText(QString(" %1\% ").arg(100));
}
