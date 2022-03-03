#include "taopaywechat.h"
#include "ui_taopaywechat.h"

TaoPayWeChat::TaoPayWeChat(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TaoPayWeChat)
{
    ui->setupUi(this);

//    ui->lblPayQRCode->setText("<img src=\"./icons/wechat_cashier1yuan_small.png\">");

//    <font color=\"black\" size=\"14\"></font> <strong>发送红包给淘水印  分享给好友</strong>

    ui->lblPayQRCode->setText("<img alt=\"红包\" src=\"./pay/wechat_cashier1yuan_small.png\">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<img alt=\"分享\" src=\"./pay/ShareIcons.jpg\">");
}

TaoPayWeChat::~TaoPayWeChat()
{
    delete ui;
}


void TaoPayWeChat::on_btnPay_clicked()
{
    this->close();

    delete ui;
}


void TaoPayWeChat::on_btnShare_clicked()
{
    this->close();

    delete ui;
}
