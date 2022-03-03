#include "taohelp.h"
#include "ui_taohelp.h"

#include <QMessageBox>

TaoHelp::TaoHelp(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TaoHelp)
{
    ui->setupUi(this);

/*
    ui->lblHelpContent->setText("<img src=\"d://projects/Qt/icons/TaoShuiYin/Brush_32px.ico\">&nbsp;--- 选择不同的刷子大小去涂刷水印的区域。 <br> \
                                <img src=\"d://projects/Qt/icons/TaoShuiYin/BackgroundBorderMark_32px.ico\">&nbsp;---  在水印区域附件的背景旁画一条黄线， <br> 以确定黄线外的区域没有用来修复水印。 <br> \
                                <img src=\"d://projects/Qt/icons/TaoShuiYin/CurveMark_32px.ico\">&nbsp;---  画直线和曲线延续背景的边缘到水印区域内。 <br>");
*/
    ui->lblHelpContent->setText("<img src=\"./icons/helpManual.jpg\">");
}

TaoHelp::~TaoHelp()
{
    delete ui;
}


void TaoHelp::on_btnGo2WebSite_clicked()
{
    QDesktopServices::openUrl(QUrl(tr("www.baidu.com")));
}


void TaoHelp::on_chkBoxStartShowHelp_stateChanged(int curState)
{
    if (curState == Qt::Checked)
    {
//        QMessageBox::about(this, "title", "checked1");
          TaoSettings::saveIsCheckDisplayHelp(1);
    }
    else
    {
//         QMessageBox::about(this, "title", "checked0");
            TaoSettings::saveIsCheckDisplayHelp(0);
    }
}


void TaoHelp::on_btnClose_clicked()
{
    this->close();
    delete ui;
}
