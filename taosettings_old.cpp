#include "taosettings.h"

TaoSettings::TaoSettings()
{

}


int TaoSettings::getGUID()
{
    QUuid guid = QUuid::createUuid();

//    printf("%s\n", guid.toByteArray().toStdString().c_str());

    QJsonValue jValue = QJsonValue(guid.toByteArray().toStdString().c_str());
    QJsonObject jObject = QJsonObject();

    jObject.insert(QString(QObject::tr("GUID")), jValue);

    QJsonDocument jDoc = QJsonDocument(jObject);

    printf("%s\n", jDoc.toJson().toStdString().c_str());

    QFile configFile;

    configFile.setFileName(QObject::tr("config.txt"));

    if (!configFile.exists())
    {
        cout << " config file does not exist." << endl;

        configFile.open(QIODevice::WriteOnly |  QIODevice::Text | QIODevice::Unbuffered);


        configFile.write(jDoc.toJson().toStdString().c_str());

//        configFile.write("write only");

        configFile.close();
    }
    else
    {
        configFile.open(QIODevice::ReadOnly | QIODevice::Text);

        QByteArray fileData = configFile.readAll();

        cout << " file data: " << fileData.toStdString() << endl;

        configFile.close();

        QJsonDocument fileJDoc = QJsonDocument::fromJson(fileData);

        cout << " is empty: " << fileJDoc.isEmpty() << endl;

        cout << " is null: " << fileJDoc.isNull() << endl;

        cout << " is object: " << fileJDoc.isObject() << endl;

        cout << fileJDoc.toJson().toStdString() << endl;

        QJsonObject fileJObj  = fileJDoc.object();

        cout << " file JSon Array: " << fileJObj.contains(QString(QObject::tr("GUID"))) << endl;

        cout << " key value: " << fileJObj.value(QString(QObject::tr("GUID"))).toString().toStdString() << endl;

//        cout << fileValue.toString().toStdString()<< endl;

    }

    return 0;
}

int TaoSettings::makeGUID()
{
    QUuid guid = QUuid::createUuid();

//    printf("%s\n", guid.toByteArray().toStdString().c_str());

    QJsonValue jValue = QJsonValue(guid.toByteArray().toStdString().c_str());
    QJsonObject jObject = QJsonObject();

    jObject.insert(QString(QObject::tr("GUID")), jValue);

    QJsonDocument jDoc = QJsonDocument(jObject);

    printf("new GUID: %s\n", jDoc.toJson().toStdString().c_str());

    QFile configFile;

    configFile.setFileName(QObject::tr("config.txt"));

    if (!configFile.exists())
    {
        cout << " config file does not exist." << endl;

        configFile.open(QIODevice::WriteOnly |  QIODevice::Text | QIODevice::Unbuffered);


        configFile.write(jDoc.toJson().toStdString().c_str());

        configFile.close();
    }
    else
    {
        configFile.open(QIODevice::ReadOnly | QIODevice::Text);

        QByteArray fileData = configFile.readAll();

//        cout << " file data: " << fileData.toStdString() << endl;

        configFile.close();

        QJsonDocument fileJDoc = QJsonDocument::fromJson(fileData);

//        cout << fileJDoc.toJson().toStdString() << endl;

        QJsonObject fileJObj  = fileJDoc.object();

        cout << " file JSon Array: " << fileJObj.contains(QString(QObject::tr("GUID"))) << endl;

        cout << " key value: " << fileJObj.value(QString(QObject::tr("GUID"))).toString().toStdString() << endl;

//        cout << fileValue.toString().toStdString()<< endl;
    }

    return 0;
}


bool TaoSettings::checkDisplayHelp()
{
    QFile configFile;

    configFile.setFileName(QObject::tr("config.txt"));

    if (!configFile.exists())
    {
        return true;
    }
    else
    {
        configFile.open(QIODevice::ReadWrite | QIODevice::Text);

        QByteArray fileData = configFile.readAll();

//        cout << " file data: " << fileData.toStdString() << endl;
        configFile.close();

        QJsonDocument fileJDoc = QJsonDocument::fromJson(fileData);

        cout << fileJDoc.toJson().toStdString() << endl;

        QJsonObject fileJObj  = fileJDoc.object();

        cout << " file JSon Array: " << fileJObj.contains(QString(QObject::tr("DisplayHelp"))) << endl;

        if (fileJObj.contains(QString(QObject::tr("DisplayHelp"))))
        {
            cout << " key value: " << fileJObj.value(QString(QObject::tr("DisplayHelp"))).toString().toStdString() << endl;

//        cout << fileValue.toString().toStdString()<< endl;

            QString str1 = fileJObj.value(QString(QObject::tr("DisplayHelp"))).toString();

            return (str1.compare("yes", Qt::CaseInsensitive));
        }
    }

    return true;
}


bool TaoSettings::saveIsCheckDisplayHelp(int checked)
{
    QFile configFile;

    configFile.setFileName(QObject::tr("config.txt"));

    if (!configFile.exists())
    {
        return true;
    }
    else
    {
        configFile.open(QIODevice::ReadWrite | QIODevice::Text);

        QByteArray fileData = configFile.readAll();

//        cout << " file data: " << fileData.toStdString() << endl;


        QJsonDocument fileJDoc = QJsonDocument::fromJson(fileData);

        cout << fileJDoc.toJson().toStdString() << endl;

        QJsonObject fileJObj  = fileJDoc.object();

        cout << " file JSon Array: " << fileJObj.contains(QString(QObject::tr("DisplayHelpOnStart"))) << endl;

        if (checked != 1)
            fileJObj.insert(QString("DisplayHelpOnStart"), QJsonValue("yes"));
        else
            fileJObj.insert(QString("DisplayHelpOnStart"), QJsonValue("no"));

        QJsonDocument updatedJDoc = QJsonDocument(fileJObj);

        configFile.
        configFile.write(updatedJDoc.toJson().toStdString().c_str());

        configFile.close();
    }

    return true;
}
