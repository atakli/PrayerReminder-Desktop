#ifndef CALCTIMES_H
#define CALCTIMES_H
#include <QString>
class CalcTimes
{
private:
	void temkinEkle(int* hours, int* minutes, int hangiVakit);
	double degToRad(double degree);
	double radToDeg(double radian);
	double moreLess360(double value);
	double moreLess24(double value);
public:
	QString doubleToHrMin(double number, int hangiVakit);
	void calcPrayerTimes(int year, int month, int day,
						 double longitude, double latitude, int timeZone,
						 double fajrTwilight, double ishaTwilight,
						 double &fajrTime, double &sunRiseTime, double &zuhrTime,
						 double &asrTime, double &maghribTime, double &ishaTime);
};

#endif // CALCTIMES_H
