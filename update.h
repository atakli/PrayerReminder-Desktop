#ifndef UPDATE_H
#define UPDATE_H

#include <QWidget>

class Update : public QObject
{
	Q_OBJECT
public:
	Update();
//	void checkNewVersionNow();
	bool isChecked;
//public slots:
	void checkForUpdates();
};

#endif // UPDATE_H
