#include "taodownloadfile.h"

TaoDownloadFile::TaoDownloadFile(QMainWindow *inMainWindow)
{    
    manager = new QNetworkAccessManager;

    mainWindow = inMainWindow;
}


TaoDownloadFile::~TaoDownloadFile()
{

}


void TaoDownloadFile::download(QString url)
{
    QNetworkRequest *request = new QNetworkRequest(QUrl(url));

    request->setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));

    reply = manager->get(*request);

    connect(reply, &QNetworkReply::finished, this, &httpFinished);
    connect(reply, &QIODevice::readyRead, this, &httpReadyRead);

//    file.setFileName("./test.html");
//    file.open(QIODevice::WriteOnly);
}


void TaoDownloadFile::httpReadyRead()
{
//    file.write(reply->readAll());
//    cout << reply->readAll().toStdString().c_str() << endl;
//    QByteArray replyData = reply->readAll();

//    data = reply->readAll();
//    cout << data.toStdString().c_str()<< endl;
}


void TaoDownloadFile::httpFinished()
{
//    file.close();

    if (reply->error() != QNetworkReply::NoError)
        return;

    data = reply->readAll();
//    cout << data.toStdString().c_str() << endl;

    QFile filePtr;

    filePtr.setFileName("./tmp/curVersionFileMd5.txt");
    filePtr.open(QIODevice::WriteOnly);
    filePtr.write(data);
    filePtr.close();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(data, Q_NULLPTR);
    QJsonObject versionJsonObj  = jsonDoc.object();
    QString curVersion = versionJsonObj.value(QString(QObject::tr("Version"))).toString();

    QString localVersion = TaoSettings::getLocalVersion();

//    QMessageBox::about(NULL, "local version", localVersion);

//    QMessageBox::about(NULL, "current version", curVersion);

    // compare version number
    if (curVersion.compare(localVersion) > 0)
    {
        // enforce update
        if (versionJsonObj.value(QString(QObject::tr("EnforceUpdate"))).toBool())
        {
            switch(QMessageBox::warning(NULL, "发现最新版本", "当前你使用的不是最新版本, 需要立即更新版本才能继续使用!", "下载最新版本", "自动更新", 0, 1))
            {
            case 1:
                // run update program
                process = new QProcess();
                process->start("./update/TaoUpgrade.exe");

                mainWindow->close();
                exit(0);
                break;

            case 0:
            default:
                QDesktopServices::openUrl(QUrl(tr("www.taoshuiyin.com")));
                exit(0);
            }
        }
        else {
            // option update
            switch(QMessageBox::warning(NULL, "发现最新版本", "当前你使用的不是最新版本!", "不更新", "立即更新", 0, 1))
            {
            case 1:
                // run update program
                process = new QProcess();
                process->start("./update/TaoUpgrade.exe");
                mainWindow->close();
                exit(0);
                break;

            case 0:
            default:
                    ;
            }
        }
    }
}

