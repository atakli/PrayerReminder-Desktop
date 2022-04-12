#ifndef CALCTIMES_H
#define CALCTIMES_H
#include <QString>
#include <QCoreApplication>

const QString evkatOfflinePath = "/namazVakitFiles/evkatOffline.json";

class CalcTimes
{
private:
	double degToRad(double degree);
	double radToDeg(double radian);
	double moreLess360(double value);
	double moreLess24(double value);
	const QString applicationDirPath = QCoreApplication::applicationDirPath();
public:
    void offlineVakitleriHesapla(double boylam=29.43333330, double enlem=40.80000000);
	QString doubleToHrMin(double number);
	void calcPrayerTimes(int year, int month, int day,
						 double longitude, double latitude, int timeZone,
						 double fajrTwilight, double ishaTwilight,
						 double &fajrTime, double &sunRiseTime, double &zuhrTime,
						 double &asrTime, double &maghribTime, double &ishaTime);
	void hesapla();
};

#endif // CALCTIMES_H
