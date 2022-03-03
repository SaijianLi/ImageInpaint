

#ifndef TAOSETTINGS_H
#define TAOSETTINGS_H

#include <QSettings>
#include <QUuid>
#include <QString>
#include <QStringList>
#include <QtNetwork/QNetworkInterface>

#include <iostream>
#include <string>

#include <cpuid.h>

using namespace std;

class TaoSettings
{
public:
    TaoSettings();

    static QString makeGUID();

    static QString getLocalVersion();

    static bool writeKeyValue(QString key, QString value);

    static QString readKeyValue(QString key);

    static bool isDisplayToolHelp(QString toolName);

    static bool saveDisplayToolHelpOption(QString toolName, bool checked);

private:
    static QString getMACAddress();

    static QString getCPUId();

};

#endif // TAOSETTINGS_H
