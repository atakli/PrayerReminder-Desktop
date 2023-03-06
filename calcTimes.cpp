#include "calcTimes.h"

#include <iostream>
#include <cmath>

#include <QDate>
#include <QFile>
#include <QDebug>
#include <QVariant>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonObject>
#include <QMessageBox>
#include <QJsonDocument>
//#include <QGeoCoordinate>

const static int number_of_days_to_calculate = 30;
QString evkatOfflinePath = "evkatOnline.json";

const static int timeZone = 3;
const static double fajrTwilight = -18;
const static double ishaTwilight = -17;

std::array<double, 6> CalcTimes::calcPrayerTimes(const QDate& date, const double longitude, const double latitude)
{
	const int year = date.year();
	const int month = date.month();
	const int day = date.day();

    auto degToRad = [](double degree)
    {
		return ((M_PI / 180) * degree);
    };
    auto radToDeg = [](double radian)
    {
		return (radian * (180 / M_PI));
    };
	auto moreLess360 = [](double value)	// make sure a value is between 0 and 360
    {
		return (value < 0 ? (fmod(value, 360) + 360) : fmod(value, 360));
    };
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

	double zuhrTime = UT_Noon / 15 + timeZone;	// 2) Zuhr Time [Local noon]

	//double Asr_Alt = radToDeg(atan(2 + tan(degToRad(abs(latitude - Dec)))));	// Asr Hanafi
	double Asr_Alt = radToDeg(atan(1 + tan(degToRad(abs(latitude - Dec)))));	// Asr Shafii
	const double Asr_Arc = radToDeg(acos((sin(degToRad(90 - Asr_Alt)) - sin(degToRad(Dec)) * sin(degToRad(latitude))) / (cos(degToRad(Dec)) * cos(degToRad(latitude))))) / 15;

	std::array<double, 6> vakitler;

    // 3) Asr Time
    vakitler[3] = zuhrTime + Asr_Arc;
	vakitler[1] = zuhrTime - (Durinal_Arc / 15);	// 1) Shorouq Time
	vakitler[4] = zuhrTime + (Durinal_Arc / 15);	// 4) Maghrib Time

	const double Esha_Arc = radToDeg(acos((sin(degToRad(ishaTwilight)) - sin(degToRad(Dec)) * sin(degToRad(latitude))) / (cos(degToRad(Dec)) * cos(degToRad(latitude)))));
	vakitler[5] = zuhrTime + (Esha_Arc / 15);		// 5) Isha Time

	const double Fajr_Arc = radToDeg(acos((sin(degToRad(fajrTwilight)) - sin(degToRad(Dec)) * sin(degToRad(latitude))) / (cos(degToRad(Dec)) * cos(degToRad(latitude)))));	// 0) Fajr Time
    vakitler[0] = zuhrTime - (Fajr_Arc / 15);

	// https://vakithesaplama.diyanet.gov.tr/temkin.php // imsak ve yatsıda temkin yok
	vakitler[1] += Temkin::sunRise / 60.0;
	vakitler[2] = Temkin::zuhr / 60.0 + zuhrTime;
	vakitler[3] += Temkin::asr / 60.0;
	vakitler[4] += Temkin::maghrib / 60.0;
	return vakitler;
}
void CalcTimes::offlineVakitleriHesapla(const double boylam, const double enlem)
{
    auto moreLess24 = [](double value)	// make sure a value is between 0 and 24
    {
		return (value < 0 ? (fmod(value, 24) + 24) : fmod(value, 24));
    };
    auto doubleToHrMin = [&moreLess24](double number)
    {
        const int hours = floor(moreLess24(number));
        const int minutes = floor(moreLess24(number - hours) * 60);
        return QString("0" + QString::number(hours)).right(2) + ":" + QString("0" + QString::number(minutes)).right(2);
    };

	QDate date = QDateTime::currentDateTime().date();

	QJsonObject vakitObject;
	QJsonArray vakitArray;

    for(int i = 0; i < number_of_days_to_calculate; ++i)
	{
		date = date.addDays(1);
		const auto vakitler = calcPrayerTimes(date, boylam, enlem);

		const QString dayWith0 = QString("0" + QString::number(date.day())).right(2);
		const QString monthWith0 = QString("0" + QString::number(date.month())).right(2);

		const QString toBeInserted = dayWith0 + "." + monthWith0 + "." + QString::number(date.year());
		vakitObject.insert("MiladiTarihKisa", QJsonValue::fromVariant(toBeInserted));

		const std::array<QString, 6> vakitNames = {"Imsak", "Gunes", "Ogle", "Ikindi", "Aksam", "Yatsi"};
		for (size_t i = 0; i < vakitler.size(); ++i)
        {
			const QString toBeInserted = doubleToHrMin(vakitler[i]);
            vakitObject.insert(vakitNames[i], QJsonValue::fromVariant(toBeInserted));
        }
		vakitArray.push_back(vakitObject);
	}
	QJsonDocument doc(vakitArray);
    QFile jsonFile(evkatOfflinePath);				// TODO: bütün qfile'lara bak close etmiş miyim
    if (!jsonFile.open(QFile::WriteOnly))
    {
        std::cout << "cannot write " << evkatOfflinePath.toStdString() << std::endl;
        QMessageBox::critical(nullptr, "", "cannot write " + evkatOfflinePath);		// TODO: buraya programın ismi gelsin
    }
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
