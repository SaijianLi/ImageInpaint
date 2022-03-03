#include "taocheckexe.h"

TaoCheckExe::TaoCheckExe(QObject *parent, QString filename)
{
    exeFilename = filename;
}


// check if the exe file has been modified by hacker
bool TaoCheckExe::intact()
{
    QFile filePtr(exeFilename);

    //QMessageBox::information(this, " file size : ", QString("%1").arg((int)filePtr.size()));

    QCryptographicHash cryptoHash(QCryptographicHash::Md5);

    QByteArray byteArrayExeMd5;

    filePtr.open(QIODevice::ReadOnly);

    QByteArray fileData = filePtr.read(blockSize);

    cryptoHash.reset();

    while (!fileData.isEmpty())
    {
        cryptoHash.addData(fileData);
        fileData = filePtr.read(blockSize);
    }

    // add filename to be hashed
    cryptoHash.addData(QByteArray(exeFilename.toUtf8()));

    filePtr.close();

    byteArrayExeMd5 = cryptoHash.result();

//    QMessageBox::information(this, " MD5" , QString(byteArrayExeMd5.toHex()));

    QFile localVersionMd5(localMd5Filename);  //"curVersionFileMd5.txt"

    localVersionMd5.open(QIODevice::ReadOnly);

    QByteArray localFileMd5Data = localVersionMd5.readAll();

    QJsonDocument jsonDocLocalMd5 = QJsonDocument::fromJson(localFileMd5Data, Q_NULLPTR);

    QJsonObject jsonObjLocalMd5  = jsonDocLocalMd5.object();

    QJsonObject jsonObjFiles = jsonObjLocalMd5.value(QString(QObject::tr("Files"))).toObject();

    QString localExeMd5 = jsonObjFiles.value(exeFilename).toString();  //QObject::tr("TaoShuiYin.exe")

//    QMessageBox::about(NULL, QString("local md5 version"),jsonObjFiles.value(QObject::tr("TaoShuiYin.exe")).toString());

    if (localExeMd5.compare(QString(byteArrayExeMd5.toHex())) != 0)
        return false;
    else
        return true;
}
