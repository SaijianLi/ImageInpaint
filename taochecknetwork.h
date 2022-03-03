#ifndef TAOCHECKNETWORK_H
#define TAOCHECKNETWORK_H

#include <QObject>
#include <QtNetwork/QHostInfo>
#include <QMessageBox>

#include "taomain.h"

class TaoCheckNetwork : public QObject
{
    Q_OBJECT
public:
    explicit TaoCheckNetwork(QObject *parent, TaoMain *main);

private:
    TaoMain *mainWindow;

    QString qstrUrlFileMd5 = QString("http://www.taoshuiyin.com/download/update/"
                                     "curVersionFileMd5.txt");

signals:

public slots:
    void onLookupHost(QHostInfo host);
};

#endif // TAOCHECKNETWORK_H
