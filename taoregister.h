#ifndef TAOREGISTER_H
#define TAOREGISTER_H

#include <QObject>

#include <QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QUrl>
#include <QByteArray>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>

#include <QMessageBox>
#include <QFile>

#include "taosettings.h"

class TaoRegister : public QObject
{
    Q_OBJECT
public:
    TaoRegister();
    ~TaoRegister();

    void registerUser(QString guid);

private:
    QNetworkAccessManager *manager;
    QNetworkReply *reply;

    QString md5Key = QString("ab_de12_45");


private slots:
    void httpManagerFinished(QNetworkReply *curReply);

    void httpReplyFinished();
};

#endif // TAOREGISTER_H
