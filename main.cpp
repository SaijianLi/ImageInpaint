/*  filename: main.cpp
 **
 **
 */


#include <QApplication>
#include <QString>
#include <QObject>
#include <QTextCodec>

#include <QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QUrl>
#include <QByteArray>

#include <QDesktopWidget>
#include <QNetworkInterface>

#include "taomain.h"
#include "taosettings.h"
#include "taochecknetwork.h"
#include "taocheckexe.h"
#include "taodownloadfile.h"

QString qstrUrlFileMd5 = QString("http://www.taoshuiyin.com/download/update/"
                                 "curVersionFileMd5.txt");
QString qstrExeFilename = QString("淘水印");


TaoMain *mainWindow;

void slotDetail()
{
    QString detail = "";
    QList<QNetworkInterface> list = QNetworkInterface::allInterfaces();

    for(int i = list.count()/3; i < 2*list.count()/3; i++)
    {
        QNetworkInterface interface = list.at(i);
        detail = detail + QObject::tr("设备：") + interface.name()+"\n";
        detail = detail + QObject::tr("硬件地址：") + interface.hardwareAddress()+"\n";
        QList<QNetworkAddressEntry> entryList = interface.addressEntries();

        for(int j = 0; j < entryList.count(); j++)
        {
            QNetworkAddressEntry entry = entryList.at(j);
            detail = detail + "\t" + QObject::tr("IP 地址：") + entry.ip().toString() + "\n";
            detail = detail + "\t" + QObject::tr("子网掩码：") + entry.netmask().toString() + "\n";
            detail = detail + "\t" + QObject::tr("广播地址：") + entry.broadcast().toString() + "\n";
        }
    }

    QMessageBox::information(0, QObject::tr("Detail"), detail);
}


int main(int argc, char *argv[])
{
    QApplication::addLibraryPath(QObject::tr(".//imageformats"));
    QApplication app(argc, argv);

    app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

    // check if the exe file has been modified.

    // commented on Sep. 29, 2017 for demo
    //TaoCheckExe checkExe(NULL, QString("TaoShuiYin.exe"));

    //    if (!checkExe.intact())
    //    {
    //        QMessageBox::about(NULL, QString("请重新下载淘水印"), QString("你的淘水印不是最新版，请去360软件管家\n或官网www.taoshuiyin.com重新下载！"));

    //        app.closeAllWindows();

    //        return 1;
    //    }

    QRect deskRect = QApplication::desktop()->availableGeometry();

    mainWindow = new TaoMain();
    mainWindow->move((deskRect.width() - mainWindow->width())/2, 0);


    // check if the network is connected.
    TaoCheckNetwork lookupHost(NULL, mainWindow);

    QHostInfo::lookupHost("www.taoshuiyin.com", &lookupHost, SLOT(onLookupHost(QHostInfo)));


    // download the server Version file md5, check version number.
    // compare with the local version, if not latest, remind to update
//    TaoDownloadFile *updateVersion;
//    updateVersion = new TaoDownloadFile(mainWindow);

//    updateVersion->download(qstrUrlFileMd5);

    mainWindow->show();

    // make GUID, save it, register to server.
    QString guidMade = TaoSettings::makeGUID();

    TaoSettings::writeKeyValue(QString("GUID"), guidMade);

    TaoRegister registerUser;

    registerUser.registerUser(guidMade);

    return app.exec();
}
