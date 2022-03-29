#ifndef UPDATECONTROLLER_H
#define UPDATECONTROLLER_H

#include <QWidget>

class UpdateController : public QObject
{
    Q_OBJECT
public:
    UpdateController();
//	void checkNewVersionNow();
    bool isChecked;
//public slots:
    void checkForUpdates();
};

#endif // UPDATECONTROLLER_H
