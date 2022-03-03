#ifndef TAODOWNLOADFILE_H
#define TAODOWNLOADFILE_H


#include <QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QUrl>
#include <QByteArray>
#include <QNetworkReply>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#include <QObject>
#include <QFile>
#include <QMessageBox>
#include <QDesktopServices>
#include <QMainWindow>
#include <QProcess>

#include <iostream>
#include <stdio.h>

#include "taosettings.h"


using namespace std;

class TaoDownloadFile: public QObject
{
public:
    TaoDownloadFile(QMainWindow *inMainWindow);
    ~TaoDownloadFile();

    void download(QString url);

    void httpFinished();

    void httpReadyRead();

private:
    QNetworkAccessManager *manager;
    QNetworkReply *reply;

    QFile file;
    QByteArray data;

    QProcess *process;

    QMainWindow *mainWindow;
};

#endif // TAODOWNLOADFILE_H
