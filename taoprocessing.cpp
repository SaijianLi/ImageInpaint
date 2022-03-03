#include "taoprocessing.h"
#include "ui_taoprocessing.h"

TaoProcessing::TaoProcessing(QWidget *parent, QString msg) :
    QDialog(parent),
    ui(new Ui::TaoProcessing)
{
    ui->setupUi(this);

//    ui->lblProcessing->setText("正在处理中...");
    ui->lblProcessing->setText(msg);

//    QMovie *movie = new QMovie("d://processing.gif");
//    ui->lblProcessing->setMovie(movie);
//    movie->start();
}

TaoProcessing::~TaoProcessing()
{
    delete ui;
}
