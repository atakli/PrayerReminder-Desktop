#include "calcTimes.h"
#include "util.h"

#include <iostream>
#include <math.h>

#include <QDate>
#include <QFile>
#include <QDebug>
#include <QVariant>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonDocument>
//#include <QGeoCoordinate>

double CalcTimes::degToRad(double degree)
{
    return ((3.1415926 / 180) * degree);
}

double CalcTimes::radToDeg(double radian)
{
    return (radian * (180/3.1415926));
}
double CalcTimes::moreLess360(double value)	//make sure a value is between 0 and 360
{
    while(value > 360 || value < 0)
    {
        if(value > 360)
            value -= 360;

        else if (value <0)
            value += 360;
    }
    return value;
}
double CalcTimes::moreLess24(double value)	//make sure a value is between 0 and 24
{
    while(value > 24 || value < 0)
    {
        if(value > 24)
            value -= 24;

        else if (value <0)
            value += 24;
    }
    return value;
}
QString CalcTimes::doubleToHrMin(double number)
{
	const int hours = floor(moreLess24(number));
	const int minutes = floor(moreLess24(number - hours) * 60);
	return QString("0" + QString::number(hours)).right(2) + ":" + QString("0" + QString::number(minutes)).right(2);
}

void CalcTimes::calcPrayerTimes(const int year, const int month, const int day,
					 const double longitude, const double latitude, const int timeZone,
					 const double fajrTwilight, const double ishaTwilight,
                     double &fajrTime, double &sunRiseTime, double &zuhrTime,
                     double &asrTime, double &maghribTime, double &ishaTime)
{
	const double D = (367 * year) - ((year + (int)((month + 9) / 12)) * 7 / 4) + (((int)(275 * month / 9)) + day - 730531.5);

	const double L = moreLess360(280.461 + 0.9856474 * D);

	const double M = moreLess360(357.528 + (0.9856003) * D);

	const double Lambda = moreLess360(L + 1.915 * sin(degToRad(M)) + 0.02 * sin(degToRad(2 * M)));

	const double Obliquity = 23.439 - 0.0000004 * D;
	double Alpha = moreLess360(radToDeg(atan((cos(degToRad(Obliquity)) * tan(degToRad(Lambda))))));

	Alpha = Alpha - (360 * static_cast<int>(Alpha / 360));
    Alpha = Alpha + 90 * (floor(Lambda / 90) - floor(Alpha / 90));

	const double ST = 100.46 + 0.985647352 * D;
	const double Dec = radToDeg(asin(sin(degToRad(Obliquity)) * sin(degToRad(Lambda))));
	const double Durinal_Arc = radToDeg(acos((sin(degToRad(-0.8333)) - sin(degToRad(Dec)) * sin(degToRad(latitude))) / (cos(degToRad(Dec)) * cos(degToRad(latitude)))));

	const double Noon = moreLess360(Alpha - ST);

	const double UT_Noon = Noon - longitude;

    ////////////////////////////////////////////
    // Calculating Prayer Times Arcs & Times //
    //////////////////////////////////////////

    // 2) Zuhr Time [Local noon]
    zuhrTime = UT_Noon / 15 + timeZone;

    // Asr Hanafi
	//double Asr_Alt = radToDeg(atan(2 + tan(degToRad(abs(latitude - Dec)))));

    // Asr Shafii
    double Asr_Alt = radToDeg(atan(1 + tan(degToRad(abs(latitude - Dec)))));
	const double Asr_Arc = radToDeg(acos((sin(degToRad(90 - Asr_Alt)) - sin(degToRad(Dec)) * sin(degToRad(latitude))) / (cos(degToRad(Dec)) * cos(degToRad(latitude))))) / 15;
    // 3) Asr Time
    asrTime = zuhrTime + Asr_Arc;

    // 1) Shorouq Time
    sunRiseTime = zuhrTime - (Durinal_Arc / 15);

    // 4) Maghrib Time
    maghribTime = zuhrTime + (Durinal_Arc / 15);

	const double Esha_Arc = radToDeg(acos((sin(degToRad(ishaTwilight)) - sin(degToRad(Dec)) * sin(degToRad(latitude))) / (cos(degToRad(Dec)) * cos(degToRad(latitude)))));
    // 5) Isha Time
    ishaTime = zuhrTime + (Esha_Arc / 15);

    // 0) Fajr Time
	const double Fajr_Arc = radToDeg(acos((sin(degToRad(fajrTwilight)) - sin(degToRad(Dec)) * sin(degToRad(latitude))) / (cos(degToRad(Dec)) * cos(degToRad(latitude)))));
    fajrTime = zuhrTime - (Fajr_Arc / 15);

	// https://vakithesaplama.diyanet.gov.tr/temkin.php // imsak ve yatsıda temkin yok
	sunRiseTime+= static_cast<double>(Temkin::sunRise) / 60;	// böyle uzun uzun yazmayayım dedim ama o zaman da readability azalır. hem zaten şimdi compiler
	zuhrTime += static_cast<double>(Temkin::zuhr) / 60;			// kardeş bunu optimize ediyodur muhtemelen. çünkü her zaman aynı sonucu veriyor
	asrTime += static_cast<double>(Temkin::asr) / 60;
	maghribTime += static_cast<double>(Temkin::maghrib) / 60;
}
void CalcTimes::offlineVakitleriHesapla(const double boylam, const double enlem)
{
	QDate dt = QDateTime::currentDateTime().date();

	double fajr, sunRise, zuhr, asr, maghrib, isha;

	QJsonObject vakitObject;
	QJsonArray vakitArray;

	for(int i=0; i<30; ++i)
	{
		const int year = dt.year();
		const int month = dt.month();
		const int day = dt.day();

		dt = dt.addDays(1);

        calcPrayerTimes(year, month, day, boylam, enlem, 3, -18, -17, fajr, sunRise, zuhr, asr, maghrib, isha);

		const QString dayWith0 = QString("0" + QString::number(day)).right(2);
		const QString monthWith0 = QString("0" + QString::number(month)).right(2);

		QString toBeInserted = dayWith0 + "." + monthWith0 + "." + QString::number(year);
		vakitObject.insert("MiladiTarihKisa", QJsonValue::fromVariant(toBeInserted));
		toBeInserted = doubleToHrMin(fajr);
		vakitObject.insert("Imsak", QJsonValue::fromVariant(toBeInserted));
		toBeInserted = doubleToHrMin(sunRise);
		vakitObject.insert("Gunes", QJsonValue::fromVariant(toBeInserted));
		toBeInserted = doubleToHrMin(zuhr);
		vakitObject.insert("Ogle", QJsonValue::fromVariant(toBeInserted));
		toBeInserted = doubleToHrMin(asr);
		vakitObject.insert("Ikindi", QJsonValue::fromVariant(toBeInserted));
		toBeInserted = doubleToHrMin(maghrib);
		vakitObject.insert("Aksam", QJsonValue::fromVariant(toBeInserted));
		toBeInserted = doubleToHrMin(isha);
		vakitObject.insert("Yatsi", QJsonValue::fromVariant(toBeInserted));
		vakitArray.push_back(vakitObject);
	}

	QJsonDocument doc(vakitArray);
    QFile jsonFile(Paths::applicationDirPath + Paths::evkatOfflinePath);				// TODO: bütün qfile'lara bak close etmiş miyim
	jsonFile.open(QFile::WriteOnly);
	jsonFile.write(doc.toJson());
	jsonFile.close();
}
/*
 Personal code. Calculating for Cairo.

Date: 18-1-2012
Longitude: 30.2
Latitude: 30
Time Zone: +2
Fajr Twilight: -19.5
Esha Twilight: -17.5
*/
