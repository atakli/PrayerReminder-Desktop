#ifndef CALCTIMES_H
#define CALCTIMES_H

#include <QString>

class QDate;

class CalcTimes
{
private:
	enum Temkin : int8_t
    {
        sunRise = -7, zuhr = 5, asr = 4, maghrib = 7
	};
    static std::array<double, 6> calcPrayerTimes(const QDate& date, const double longitude, const double latitude);
public:
    static void offlineVakitleriHesapla(const double boylam = 29.43333330, const double enlem = 40.80000000);
};

#endif // CALCTIMES_H
