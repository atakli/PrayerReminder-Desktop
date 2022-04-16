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
	void offlineVakitleriHesapla(const double boylam=29.43333330, const double enlem=40.80000000);
	QString doubleToHrMin(double number);
	void calcPrayerTimes(const int year, const int month, const int day,
						 const double longitude, const double latitude, const int timeZone,
						 const double fajrTwilight, const double ishaTwilight,
						 double &fajrTime, double &sunRiseTime, double &zuhrTime,
						 double &asrTime, double &maghribTime, double &ishaTime);
};

#endif // CALCTIMES_H
