#ifndef TAOCHECKEXE_H
#define TAOCHECKEXE_H

#include <QObject>
#include <QMessageBox>
#include <QCryptographicHash>
#include <QByteArray>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QString>
#include <QFile>

class TaoCheckExe : public QObject
{
    Q_OBJECT
public:
    explicit TaoCheckExe(QObject *parent, QString filename);

    bool intact();

private:
    QString exeFilename;

    const int blockSize = 10240;   // 10K
    QString localMd5Filename = QString("curVersionFileMd5.txt");

signals:

public slots:


};

#endif // TAOCHECKEXE_H
