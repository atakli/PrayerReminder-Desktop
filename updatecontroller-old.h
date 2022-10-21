#ifndef UPDATECONTROLLER_H
#define UPDATECONTROLLER_H

#include "fetchtimes.h"

#include <QWidget>
#include <QObject>
#include <QDialog>

class UpdateController : public QObject
{
    Q_OBJECT
public:
    UpdateController(const QString &versionFileName);
//	~UpdateController();
	void isNewVersionAvailable();
//	void checkNewVersionNow();
    bool isChecked;
    void checkForUpdates();
private:
	QString osName;
    QString versionFileName;
	HttpWindow fetchTimes;
	bool compareTagVersion(const QString& tag, QString currentTag);
};

#endif // UPDATECONTROLLER_H
