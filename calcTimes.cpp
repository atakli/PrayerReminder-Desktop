#include <calcTimes.h>
#include <iostream>
#include <math.h>

#include <QDate>
#include <QFile>
#include <QVariant>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonDocument>

//extern const QString evkatOfflinePath;

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
QString CalcTimes::doubleToHrMin(double number, int hangiVakit)	//convert the double number to Hours and Minutes
{
	int hours = floor(moreLess24(number));
	int minutes = floor(moreLess24(number - hours) * 60);
	temkinEkle(&hours, &minutes, hangiVakit);
	return QString("0" + QString::number(hours)).right(2) + ":" + QString("0" + QString::number(minutes)).right(2);
}
void CalcTimes::temkinEkle(int* hours, int* minutes, int hangiVakit)
// TODO: her vakti ayrı bi fonksiyona koymak daha efficient olabilir. ama yeterli çünkü sürekli hesaplanmayacak. ayda bir. o da worst case :)
{															// ayrıca temkin ekleme işini asıl hesaplamanın yapıldığı yerde de yapabiliriz belki de
// https://vakithesaplama.diyanet.gov.tr/temkin.php
// imsak ve yatsıda temkin yok
	if(hangiVakit == 0)			// güneş doğuş
		*minutes -= 7;
	else if(hangiVakit == 2)	// öğle
		*minutes += 5;
	else if(hangiVakit == 3)	// ikindi
		*minutes += 4;
	else if(hangiVakit == 4)	// akşam
		*minutes += 7;

	if(*minutes > 60)
	{
		*minutes -= 60;
		*hours += 1;
		// if(hours > 24) // TODO: en azından türkiyede yatsı namazı 24'ü geçmeyeceği için şimdilik bunla uğraşmıyorum
	}
	else if(*minutes < 0)
	{
		*minutes += 60;
		*hours -= 1;
	}
}
void CalcTimes::calcPrayerTimes(int year, int month, int day,
                     double longitude, double latitude, int timeZone,
                     double fajrTwilight, double ishaTwilight,
                     double &fajrTime, double &sunRiseTime, double &zuhrTime,
                     double &asrTime, double &maghribTime, double &ishaTime)
{
    double D = (367 * year) - ((year + (int)((month + 9) / 12)) * 7 / 4) + (((int)(275 * month / 9)) + day - 730531.5);

    double L = 280.461 + 0.9856474 * D;
    L = moreLess360(L);

    double M = 357.528 + (0.9856003) * D;
    M = moreLess360(M);

    double Lambda = L + 1.915 * sin(degToRad(M)) + 0.02 * sin(degToRad(2 * M));
    Lambda = moreLess360(Lambda);

    double Obliquity = 23.439 - 0.0000004 * D;
    double Alpha = radToDeg(atan((cos(degToRad(Obliquity)) * tan(degToRad(Lambda)))));
    Alpha = moreLess360(Alpha);

    Alpha = Alpha - (360 * (int)(Alpha / 360));
    Alpha = Alpha + 90 * (floor(Lambda / 90) - floor(Alpha / 90));

    double ST = 100.46 + 0.985647352 * D;
    double Dec = radToDeg(asin(sin(degToRad(Obliquity)) * sin(degToRad(Lambda))));
    double Durinal_Arc = radToDeg(acos((sin(degToRad(-0.8333)) - sin(degToRad(Dec)) * sin(degToRad(latitude))) / (cos(degToRad(Dec)) * cos(degToRad(latitude)))));

    double Noon = Alpha - ST;
    Noon = moreLess360(Noon);


    double UT_Noon = Noon - longitude;


    ////////////////////////////////////////////
    // Calculating Prayer Times Arcs & Times //
    //////////////////////////////////////////

    // 2) Zuhr Time [Local noon]
    zuhrTime = UT_Noon / 15 + timeZone;

    // Asr Hanafi
    //double Asr_Alt =radToDeg(atan(2 + tan(degToRad(abs(latitude - Dec)))));

    // Asr Shafii
    double Asr_Alt = radToDeg(atan(1 + tan(degToRad(abs(latitude - Dec)))));
    double Asr_Arc = radToDeg(acos((sin(degToRad(90 - Asr_Alt)) - sin(degToRad(Dec)) * sin(degToRad(latitude))) / (cos(degToRad(Dec)) * cos(degToRad(latitude)))));
    Asr_Arc = Asr_Arc / 15;
    // 3) Asr Time
    asrTime = zuhrTime + Asr_Arc;

    // 1) Shorouq Time
    sunRiseTime = zuhrTime - (Durinal_Arc / 15);

    // 4) Maghrib Time
    maghribTime = zuhrTime + (Durinal_Arc / 15);


    double Esha_Arc = radToDeg(acos((sin(degToRad(ishaTwilight)) - sin(degToRad(Dec)) * sin(degToRad(latitude))) / (cos(degToRad(Dec)) * cos(degToRad(latitude)))));
    // 5) Isha Time
    ishaTime = zuhrTime + (Esha_Arc / 15);

    // 0) Fajr Time
    double Fajr_Arc = radToDeg(acos((sin(degToRad(fajrTwilight)) - sin(degToRad(Dec)) * sin(degToRad(latitude))) / (cos(degToRad(Dec)) * cos(degToRad(latitude)))));
    fajrTime = zuhrTime - (Fajr_Arc / 15);

    return;
}
void CalcTimes::offlineVakitleriHesapla()
{
	QDate dt = QDateTime::currentDateTime().date();

	double fajr, sunRise, zuhr, asr, maghrib, isha;

	QJsonObject vakitObject;
	QJsonArray vakitArray;

	for(int i=0; i<30; ++i)
	{
		int year = dt.year();
		int month = dt.month();
		int day = dt.day();

		dt = dt.addDays(1);

		calcPrayerTimes(year, month, day, 29.43333330, 40.80000000, 3, -18, -17, fajr, sunRise, zuhr, asr, maghrib, isha);

		QString dayWith0 = QString("0" + QString::number(day)).right(2);
		QString monthWith0 = QString("0" + QString::number(month)).right(2);

		QString toBeInserted = dayWith0 + "." + monthWith0 + "." + QString::number(year);
		vakitObject.insert("MiladiTarihKisa", QJsonValue::fromVariant(toBeInserted));
		toBeInserted = doubleToHrMin(fajr, 1);
		vakitObject.insert("Imsak", QJsonValue::fromVariant(toBeInserted));
		toBeInserted = doubleToHrMin(sunRise, 0);
		vakitObject.insert("Gunes", QJsonValue::fromVariant(toBeInserted));
		toBeInserted = doubleToHrMin(zuhr, 2);
		vakitObject.insert("Ogle", QJsonValue::fromVariant(toBeInserted));
		toBeInserted = doubleToHrMin(asr, 3);
		vakitObject.insert("Ikindi", QJsonValue::fromVariant(toBeInserted));
		toBeInserted = doubleToHrMin(maghrib, 4);
		vakitObject.insert("Aksam", QJsonValue::fromVariant(toBeInserted));
		toBeInserted = doubleToHrMin(isha, 5);
		vakitObject.insert("Yatsi", QJsonValue::fromVariant(toBeInserted));
		vakitArray.push_back(vakitObject);
	}

	QJsonDocument doc(vakitArray);
	QFile jsonFile(applicationDirPath + evkatOfflinePath);				// TODO: bütün qfile'lara bak close etmiş miyim
	jsonFile.open(QFile::WriteOnly);
	jsonFile.write(doc.toJson());
	jsonFile.close();
}
/*
 Prayers calculator end
*/


/*
 Personal code. Calculating for Cairo.

Date: 18-1-2012
Longitude: 30.2
Latitude: 30
Time Zone: +2
Fajr Twilight: -19.5
Esha Twilight: -17.5
*/
