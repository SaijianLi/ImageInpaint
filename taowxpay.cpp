/*
 **
 **
 **
 **/

#include "taowxpay.h"
#include "ui_taowxpay.h"

TaoWxPay::TaoWxPay() : ui(new Ui::TaoWxPay)
{
    ui->setupUi(this);

    ui->lblWxPayLogo->setText("<img alt=\"微信支付\" src=\"./pay/WxPayLogoW260.png\">");
    ui->lblWxPayExplainText->setText("<img alt=\"微信支付\" src=\"./pay/WxPayExplainText.png\">");

    manager = new QNetworkAccessManager;

    this->setWindowTitle(QString("微信支付"));

    QDesktopWidget *desktopWidget = QApplication::desktop();

    QRect clientRect = desktopWidget->availableGeometry();

    this->setGeometry(clientRect.center().x() - this->width()/2 , clientRect.center().y() - this->height()/2,
                      this->width(), this->height());

    timer = new QTimer();
}


TaoWxPay::~TaoWxPay()
{
    timer->stop();

    delete timer;
    delete manager;
}


void TaoWxPay::httpSaveRequest(QString url, QByteArray &requestInfo)
{
//    reply = manager->get(QNetworkRequest(QUrl(url)));

    QNetworkRequest *request = new QNetworkRequest(QUrl(url));

//    request->setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
//    reply = manager->post(*request, requestInfo);
    reply = manager->get(*request);

    connect(reply, &QNetworkReply::finished, this, &httpSaveFinished, Qt::UniqueConnection);
    connect(reply, &QIODevice::readyRead, this, &httpSaveReadyRead, Qt::UniqueConnection);
}


void TaoWxPay::getOrderStatusRequest()
{
//    reply = manager->get(QNetworkRequest(QUrl("http://www.taoshuiyin.com:8082/demo/index/getOrderStatus")));
    QString url = QString("http://www.taoshuiyin.com/api/order/process/find/order_sn/") + orderNumber;
    reply = manager->get(QNetworkRequest(QUrl(url)));

    connect(reply, &QNetworkReply::finished, this, &getOrderStatusFinished, Qt::UniqueConnection);
}


void TaoWxPay::httpSaveReadyRead()
{
//    QByteArray replyData = reply->readAll();

//    data = reply->readAll();
//    cout << data.toStdString().c_str()<< endl;
}


void TaoWxPay::httpSaveFinished()
{
    data = reply->readAll();

//    cout << data.toStdString().c_str() << endl;
//    QMessageBox::about(NULL, "pay", QString(data));

    QJsonDocument jsonDoc = QJsonDocument::fromJson(data, Q_NULLPTR);

    QJsonObject payResultJObj  = jsonDoc.object();

    if (!payResultJObj.value(QString(QObject::tr("result"))).toBool())
        return;

    if (payResultJObj.value(QString(QObject::tr("result"))).toBool() &&
            !payResultJObj.value(QString(QObject::tr("need_pay"))).toBool())
    {
        emit saveImage();
        return;
    }

//    if (payResultJObj.value(QString(QObject::tr("result"))).toBool())
//    {
//        payResultJObj.value(QString(QObject::tr("code_url"))).toString().toStdString().c_str();
//    }

//    if (payResultJObj.value(QString(QObject::tr("pay_result"))).toString().compare("SUCCESS"))
//    {
//        payResultJObj.value(QString(QObject::tr("url"))).toString().toStdString().c_str();
//    }

//    orderNumber = payResultJObj.value(QString(QObject::tr("order_no"))).toString();
    orderNumber = payResultJObj.value(QString(QObject::tr("order_sn"))).toString();

//    QString filename("d://qrcode.svg");

//    QSvgWidget svgWidget(filename, this);  //QString("d://qrcode.svg"));

    int payFee = payResultJObj.value(QString(QObject::tr("order_fee"))).toInt();
    ui->lblMsg->setText(QString("本次支付 %1 元保存后, 可免费保存3次。").arg(payFee/100.0, 0, 'f', 2));

    QSvgWidget *svgWidget = new QSvgWidget();

    //this->setCentralWidget(svgWidget);
    ui->vLayOut->addWidget(svgWidget);

    const QrCode::Ecc &errCorLvl = QrCode::Ecc::LOW;  // Error correction level

    // Make and print the QR Code symbol
//    const QrCode qr = QrCode::encodeText(payResultJObj.value(QString(QObject::tr("url"))).toString().toStdString().c_str(), errCorLvl);  // text
    const QrCode qr = QrCode::encodeText(payResultJObj.value(QString(QObject::tr("code_url"))).toString().toStdString().c_str(), errCorLvl);


    std::string qrStr = qr.toSvgString(4);
    QByteArray byteQr(qrStr.c_str());

    svgWidget->load(byteQr);

    this->show();

    connect(timer, SIGNAL(timeout()), this, SLOT(getOrderStatusRequest()), Qt::UniqueConnection);

    timer->start(3000);
}


void TaoWxPay::getOrderStatusFinished()
{
    data = reply->readAll();

//    cout << "get order number: " << data.toStdString().c_str() << endl;
//    cout << " order nubmer: " << orderNumber.toStdString().c_str() << endl;

    QJsonDocument jsonDoc = QJsonDocument::fromJson(data, Q_NULLPTR);

    QJsonObject payResultJObj  = jsonDoc.object();

    bool payResult = payResultJObj.value(QString(QObject::tr("result"))).toBool();

    if (!payResult)
        return;

    QJsonArray payMsg = QJsonArray(payResultJObj.value(QString(QObject::tr("msg"))).toArray());

    QJsonObject orderObject = payMsg.at(0).toObject();

    QString queryOrderNumber = orderObject.value("order_sn").toString();
    QString payStatusValue = orderObject.value("pay_status").toString();

    orderFee = orderObject.value("order_fee").toInt();

//    QMessageBox::about(NULL, "order", queryOrderNumber);
//    QMessageBox::about(NULL, "pay status", payStatusValue);

    if ((orderNumber.compare(queryOrderNumber) == 0) && (payStatusValue.compare(QString("paid")) == 0))
    {
        timer->stop();

        this->close();

        QMessageBox::about(this, QString("支付成功"), QString("你已支付 %1 元！请保存图片！").arg(orderFee/100.0, 0, 'f', 2));

        emit saveImage();
    }
}

void TaoWxPay::closeEvent(QCloseEvent *event)
{
    timer->stop();

    event->accept();
}
