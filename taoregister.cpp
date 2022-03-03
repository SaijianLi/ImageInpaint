#include "taoregister.h"


TaoRegister::TaoRegister()
{
    manager = new QNetworkAccessManager(this);

    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(httpManagerFinished(QNetworkReply*)));
}


TaoRegister::~TaoRegister()
{

}


void TaoRegister::registerUser(QString guid)
{
//    QByteArray data = "{\"machine_id\" : \"4C94685E68EF6A9BD0D9865E\", \"download_channel\": \"taoshuiyin\"}";

    QCryptographicHash cryptoHash(QCryptographicHash::Md5);

    QByteArray byteArrayKey;
    QString strMd5;
    QString channel = QString("taoshuiyin");

    channel = TaoSettings::readKeyValue("Channel");

    strMd5 = QString("machine_id=") + guid
                    + QString("&download_channel=") + channel
                    + QString("&key=") + md5Key;

    byteArrayKey = strMd5.toLocal8Bit();

    cryptoHash.reset();
    cryptoHash.addData(byteArrayKey);

    QString signature = QString(cryptoHash.result().toHex());

//    QMessageBox::about(NULL, "channel", strMd5);

//    QMessageBox::about(NULL, "signature", signature);

    QString url = QString("http://www.taoshuiyin.com/api/user/process/register/machine_id/") + guid
                    + QString("/download_channel/") + channel + QString("/signature/") + signature;

    QNetworkRequest request = QNetworkRequest(QUrl(url));

//    QMessageBox::about(NULL, "url", url);

//    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));

//    reply = manager->post(request, data);
    reply = manager->get(request);
}


void TaoRegister::httpManagerFinished(QNetworkReply *curReply)
{
//    QMessageBox::about(NULL, "Http Manager Finished", "http manager finished");

    if(curReply->error() == QNetworkReply::NoError)
    {
        QByteArray data = curReply->readAll();  //获取字节
//        QString result(data);  //转化为字符串
//        QMessageBox::about(NULL, "register result", data);

        QJsonDocument jsonDoc = QJsonDocument::fromJson(data, Q_NULLPTR);

        QJsonObject regResultJObj  = jsonDoc.object();

        if (!regResultJObj.value(QString(QObject::tr("result"))).toBool())
            QMessageBox::warning(NULL, "自动登录失败", "请联系淘水印开发商");
    }

    curReply->deleteLater();
}


void TaoRegister::httpReplyFinished()
{
//    QMessageBox::about(NULL, "Http Finished", "http finished");
    if(reply->error() == QNetworkReply::NoError)
    {
        QByteArray data = reply->readAll();  //获取字节
        QString result(data);  //转化为字符串
        qDebug()<<result;
   }

    reply->deleteLater();
}
