
#include "taosettings.h"

TaoSettings::TaoSettings()
{

}


QString TaoSettings::getLocalVersion()
{
    QString filename = QString("config.ini");
    QSettings settings(filename, QSettings::IniFormat);
    QString strVersion("");

    if (settings.contains("Version"))
    {
//        cout << settings.value("Version").toString().toStdString().c_str() << endl;

        strVersion = settings.value("Version").toString();
    }

    return strVersion;
}


// true  ---  (show)
// false ---  (not show)

bool TaoSettings::isDisplayToolHelp(QString toolName)
{
    QString filename = QString("config.ini");
    QSettings settings(filename, QSettings::IniFormat);

    if (settings.contains(toolName))
    {
        bool displayValue = settings.value(toolName).toBool();

        return displayValue;
    }

    return true;
}


bool TaoSettings::saveDisplayToolHelpOption(QString toolName, bool checked)
{
    QString filename = QString("config.ini");
    QSettings settings(filename, QSettings::IniFormat);

    if (checked)
        settings.setValue(toolName, false);
    else
        settings.setValue(toolName, true);

    settings.sync();

    return true;
}


bool TaoSettings::writeKeyValue(QString key, QString value)
{
    QString filename = QString("config.ini");
    QSettings settings(filename, QSettings::IniFormat);

    settings.setValue(key, value);

    settings.sync();

    return true;
}


QString TaoSettings::readKeyValue(QString key)
{
    QString filename = QString("config.ini");
    QSettings settings(filename, QSettings::IniFormat);

    if (settings.contains(key))
    {
        return settings.value(key).toString();
    }

    return QString("");
}


QString TaoSettings::makeGUID()
{
    QString macID = getMACAddress();
    QString cpuID = getCPUId();
    QString guid;

    macID.remove(QChar(':'), Qt::CaseInsensitive);

//    cout << cpuID.toStdString() << endl;
//    cout << macID.toStdString() << endl;

    // one char of CPU id  and MAC id
    for (int icount = 0; icount < (int)cpuID.size(); icount++)
    {
        guid.append(cpuID.at(icount));
        guid.append(macID.at(icount));
    }

    // one char of MAC id begin from above and one char of CPU id begin from next last
    for (int icount = 0; icount < (int)macID.size() - (int)cpuID.size(); icount++)
    {
        guid.append(macID.at(cpuID.size() + icount));
        guid.append(cpuID.at(cpuID.size() -2 - icount));
    }

    return guid;
}


QString TaoSettings::getMACAddress()
{
    QStringList addressList;

    QString strMac;
    QString minZeroMac;

    QList<QNetworkInterface> interfaceList = QNetworkInterface::allInterfaces();

    int minZeroCount = 99;

    for (int i=0; i<interfaceList.count(); i++)
    {
        QNetworkInterface curInterface = interfaceList.at(i);

        //filter loop-back and not running address
        if (curInterface.flags().testFlag(QNetworkInterface::IsUp)
                && curInterface.flags().testFlag(QNetworkInterface::IsRunning)
                && !curInterface.flags().testFlag(QNetworkInterface::IsLoopBack))
        {
            //filter virtual adresses
//            cout << curInterface.humanReadableName().toStdString()<< endl;

            if (!(curInterface.humanReadableName().contains("VMware",Qt::CaseInsensitive)) &&
                !(curInterface.humanReadableName().contains("Pseudo-Interface",Qt::CaseInsensitive)))
            {
                strMac = curInterface.hardwareAddress();
                addressList.append(strMac);

//                cout << strMac.toStdString().c_str()<< endl;

                // get the MAC address with minimum 0s
                if (strMac.count(QChar('0'), Qt::CaseInsensitive) < minZeroCount)
                {
                    minZeroMac = strMac;
                    minZeroCount = strMac.count(QChar('0'), Qt::CaseInsensitive);
                }
            }
        }

    }

    return minZeroMac;
}


QString TaoSettings::getCPUId()
{
    QString cpuId = "";

    unsigned int cpuInfo[4];

    unsigned long long shiftInfo;
    unsigned int infoType;

    __cpuid(infoType, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);

    shiftInfo = cpuInfo[3];

    shiftInfo = shiftInfo << 32;

    cpuId = QString::number(cpuInfo[3], 16).toUpper();

    cpuId = cpuId + QString::number(cpuInfo[0], 16).toUpper();

    return cpuId;
}
