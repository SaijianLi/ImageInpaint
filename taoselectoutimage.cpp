#include "taoselectoutimage.h"
#include "ui_taoselectoutimage.h"

TaoSelectOutImage::TaoSelectOutImage(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::taoSelectOutImage)
{
    ui->setupUi(this);

    imageLabelBlend = new QLabel(this);
    imageLabelNotBlend = new QLabel(this);

    radioBtnSatisfyBlend = new QRadioButton(this);
    radioBtnSatisfyNotBlend = new QRadioButton(this);
    radioBtnNotSatisfy = new QRadioButton(this);
}

TaoSelectOutImage::~TaoSelectOutImage()
{
    delete ui;

    if (imageLabelBlend != NULL)
    {
        delete imageLabelBlend;
        imageLabelBlend = NULL;
    }

    if (imageLabelNotBlend != NULL)
    {
        delete imageLabelNotBlend;
        imageLabelNotBlend = NULL;
    }
}


void TaoSelectOutImage::setImages(QImage &inImageBlend, QImage &inImageNotBlend)
{
    curImageBlend = inImageBlend;
    curImageNotBlend = inImageNotBlend;

    imageLabelBlend->setGeometry(0, 0, curImageBlend.width(), curImageBlend.height());
    imageLabelBlend->setPixmap(QPixmap::fromImage(curImageBlend));

    imageLabelNotBlend->setGeometry(curImageBlend.width() + 20, 0, curImageNotBlend.width(), curImageNotBlend.height());
    imageLabelNotBlend->setPixmap(QPixmap::fromImage(curImageNotBlend));

    radioBtnSatisfyBlend->setGeometry(curImageBlend.width()/2, curImageBlend.height() +10, 50, 50);
    radioBtnSatisfyBlend->setText("满意");

    radioBtnSatisfyNotBlend->setGeometry( 1.5 * curImageBlend.width() + 20 , curImageBlend.height() +10, 50, 50);
    radioBtnSatisfyNotBlend->setText("满意");

    radioBtnNotSatisfy->setGeometry(curImageBlend.width() + 10, curImageBlend.height()+30, 100, 100);
    radioBtnNotSatisfy->setText("都不满意");
}
