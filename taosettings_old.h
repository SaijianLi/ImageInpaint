#ifndef TAOSETTINGS_H
#define TAOSETTINGS_H

#include <QUuid>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonDocument>
#include <QVariant>

#include <QFile>
#include <QByteArray>
#include <QJsonArray>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;


class TaoSettings
{
public:
    TaoSettings();

    static int getGUID();

    static int makeGUID();

    static bool checkDisplayHelp();

    static bool saveIsCheckDisplayHelp(int checked);

};

#endif // TAOSETTINGS_H
