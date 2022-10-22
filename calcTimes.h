#ifndef CALCTIMES_H
#define CALCTIMES_H

#include <QString>

class CalcTimes
{
private:
    enum class Temkin : int8_t
    {
        sunRise = -7, zuhr = 5, asr = 4, maghrib = 7
    };
    void calcPrayerTimes(const int year, const int month, const int day,
                         const double longitude, const double latitude, const int timeZone,
                         const double fajrTwilight, const double ishaTwilight, std::array<double, 6>& vakitler);
public:
    void offlineVakitleriHesapla(const double boylam = 29.43333330, const double enlem = 40.80000000);
};

#endif // CALCTIMES_H
