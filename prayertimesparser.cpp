#include "prayertimesparser.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QDir>


PrayerTimesParser::PrayerTimesParser()
{
	loadJson();
}

bool PrayerTimesParser::loadJson()
{
//	QFile loadFile(QStringLiteral("/home/b720/evkatOnline.json"));
	QFile loadFile(QDir::homePath() + "/evkatOnline.json");

	if (!loadFile.open(QIODevice::ReadOnly))
	{
		qWarning("Couldn't open save file.");
		return false;
	}

	QByteArray saveData = loadFile.readAll();

	loadDoc = QJsonDocument::fromJson(saveData);

//	QJsonObject object = loadDoc.object();

	return true;
}

int PrayerTimesParser::Min(QString vakit)
{
	return vakit.mid(0,2).toInt() * 60 + vakit.mid(3).toInt();
}

int PrayerTimesParser::kalan(QStringList list)
{
	QTime time = QDateTime::currentDateTime().time();
	int now = time.hour() * 60 + time.minute();
	QStringList listCopy = list;
	for(QString l : list)
	{
		if((Min(l) - now) < 0)
			list.removeOne(l);
	}
	QVector<int> listInt;
	for(QString l : list)
	{
		listInt.push_back(Min(l));
	}
	int enUfagi = *std::min_element(listInt.begin(), listInt.end());
	if(listInt.isEmpty())
		enUfagi = Min(listCopy.first());
	int sonuc = enUfagi - now;
	if(sonuc < 0)
		sonuc += 60 * 24;
	return sonuc;
}

int PrayerTimesParser::vakitleriCikar(QJsonValue value)
{
	QString fajr = value["Imsak"].toString();
	QString sunRise = value["Gunes"].toString();
	QString zuhr = value["Ogle"].toString();
	QString asr = value["Ikindi"].toString();
	QString maghrib = value["Aksam"].toString();
	QString isha = value["Yatsi"].toString();

	return kalan({fajr, sunRise, zuhr, asr, maghrib, isha});
}
int PrayerTimesParser::nextDay()
{
	QDate dt = QDateTime::currentDateTime().date();

	int year = dt.year();
	int month = dt.month();
	int day = dt.day();

	QString dayWith0 = QString("0" + QString::number(day)).right(2);
	QString monthWith0 = QString("0" + QString::number(month)).right(2);

	QString bugun = dayWith0 + "." + monthWith0 + "." + QString::number(year);

	uint8_t index = 0;
	QJsonValue next;
	int kalanVakit = 0;
	while(!next.isUndefined())
	{
		next = loadDoc[index];
		if(bugun == next["MiladiTarihKisa"].toString())
		{
			kalanVakit = vakitleriCikar(next);
			break;
		}
		++index;
	}
	return kalanVakit;
}
