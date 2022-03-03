

#ifndef TAOPROCESSING_H
#define TAOPROCESSING_H

#include <QWidget>
#include <QDialog>
#include <QMovie>

namespace Ui {
class TaoProcessing;
}

class TaoProcessing : public QDialog
{
    Q_OBJECT

public:
    explicit TaoProcessing(QWidget *parent, QString msg);
    ~TaoProcessing();

private:
    Ui::TaoProcessing *ui;
};

#endif // TAOPROCESSING_H
