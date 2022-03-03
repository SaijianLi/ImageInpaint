#include "taochecknetwork.h"


TaoCheckNetwork::TaoCheckNetwork(QObject *parent, TaoMain *main)
{
    mainWindow = main;
}


void TaoCheckNetwork::onLookupHost(QHostInfo host)
{
    if (host.error() != QHostInfo::NoError)
    {
        QMessageBox::warning(NULL, "网络问题", "请检查您的网络是否连接？\n或联系淘水印开发商www.taoshuiyin.com!\n重试一次！");
        mainWindow->close();
    }
    else
    {
//        QMessageBox::about(NULL, "host info", "true");

        // download the server Version file md5, check version number.
        // compare with the local version, if not latest, remind to update
        TaoDownloadFile *updateVersion;

        updateVersion = new TaoDownloadFile(mainWindow);

        updateVersion->download(qstrUrlFileMd5);
    }
}
