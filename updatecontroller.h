#ifndef UPDATECONTROLLER_H
#define UPDATECONTROLLER_H

#include "httpmanager.h"

#include <QWidget>
#include <QObject>
#include <QDialog>

class UpdateController : public QObject
{
    Q_OBJECT
public:
    void isNewVersionAvailable();
    void setParameters(const QString& apiUrl, const QString& appName, const QString& downloadFileName);
    void downloadFile(QString fileName, QString urlSpec, const QString& downloadFileName);
private:
    QString osName;
    QString apiUrl;
    QString appName;
    HttpManager httpManager;
    QString downloadFileName;
    bool isParametersSet = false;
#ifdef WIN32
    QString apiPath = "%userprofile%\\documents\\.namazVakitFiles\\api.json";
    QString versionFileName = "%userprofile%\\documents\\.namazVakitFiles\\version.txt";
#elif __linux__
    QString apiPath = "~/.namazVakitFiles/api.json";
    QString versionFileName = "~/.namazVakitFiles/version.txt";
#endif
    bool compareTagVersion(const QString& tag, const QString& currentTag);
    QString openFile(const QString& fileName, QIODevice::OpenModeFlag flag=QIODevice::ReadOnly);
};

#endif // UPDATECONTROLLER_H
