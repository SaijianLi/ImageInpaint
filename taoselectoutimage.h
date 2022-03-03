

#ifndef TAOSELECTOUTIMAGE_H
#define TAOSELECTOUTIMAGE_H

#include <QMainWindow>

#include <QLabel>
#include <QImage>
#include <QRadioButton>

namespace Ui {
class taoSelectOutImage;
}

class TaoSelectOutImage : public QMainWindow
{
    Q_OBJECT

public:
    explicit TaoSelectOutImage(QWidget *parent = 0);
    ~TaoSelectOutImage();

    void setImages(QImage &inImageBlend, QImage &inImageNotBlend);

private:
    Ui::taoSelectOutImage *ui;

    QImage displayImage;
    QImage curImageBlend, curImageNotBlend;
    QLabel *imageLabelBlend = NULL;
    QLabel *imageLabelNotBlend = NULL;

    QRadioButton *radioBtnSatisfyBlend;
    QRadioButton *radioBtnSatisfyNotBlend;
    QRadioButton *radioBtnNotSatisfy;
};

#endif // TAOSELECTOUTIMAGE_H
