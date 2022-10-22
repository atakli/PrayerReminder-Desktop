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
    QString apiPath = "namazVakitFiles/api.json";
    QString versionFileName = "namazVakitFiles/version.txt";
    bool compareTagVersion(const QString& tag, const QString& currentTag);
    QString openFile(const QString& fileName, QIODevice::OpenModeFlag flag=QIODevice::ReadOnly);
};

#endif // UPDATECONTROLLER_H
