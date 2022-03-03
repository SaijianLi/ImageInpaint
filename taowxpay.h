/*
 **
 **
 **
 */

#ifndef TAOWXPAY_H
#define TAOWXPAY_H

#include <QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QUrl>
#include <QByteArray>
#include <QBuffer>
#include <QNetworkReply>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

#include <QFile>
#include <QMessageBox>
#include <QDesktopServices>
#include <QMainWindow>
#include <QSvgWidget>
#include <QTimer>
#include <QCloseEvent>
#include <QDesktopWidget>
#include <QSizePolicy>

#include <iostream>
#include <stdio.h>

#include "taosettings.h"
//#include "taomain.h"

#include "qrencode/QrCode.hpp"

using std::uint8_t;
using qrcodegen::QrCode;
using qrcodegen::QrSegment;

using namespace std;


namespace Ui {
class TaoWxPay;
}

class TaoWxPay: public QMainWindow
{
    Q_OBJECT
public:
    explicit TaoWxPay();
    ~TaoWxPay();

    void httpSaveRequest(QString url, QByteArray &requestInfo);

    void httpSaveFinished();

    void httpSaveReadyRead();

//    void setSlotObject(TaoMain *mainObj);

    void getOrderStatusFinished();

public slots:
    void getOrderStatusRequest();

signals:
    void saveImage();

protected:
    void closeEvent(QCloseEvent *event);

private:
    QNetworkAccessManager *manager;
    QNetworkReply *reply;

    QByteArray data;

    QString orderNumber;
    int orderFee = 0;

    QTimer *timer;

    Ui::TaoWxPay *ui;
};


#endif // TAOWXPAY_H

