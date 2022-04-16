#include "prayertimesparser.h"
#include "calcTimes.h"

#include <QJsonObject>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QDir>

PrayerTimesParser::PrayerTimesParser() {}

bool PrayerTimesParser::loadJson()
{
    QFile loadFile(applicationDirPath + evkatOnlinePath);

    if(!loadFile.exists())
        loadFile.setFileName(applicationDirPath + evkatOfflinePath);

    if (!loadFile.open(QIODevice::ReadOnly))
    {
		const QString warningStr = "Couldn't open " + loadFile.fileName() + " save file.";
        qWarning(warningStr.toStdString().data());
        return false;
    }

	const QByteArray saveData = loadFile.readAll();
    loadFile.close();

	loadDoc = QJsonDocument::fromJson(saveData);

	return true;
}

int PrayerTimesParser::Min(const QString& vakit)
{
	return vakit.mid(0,2).toInt() * 60 + vakit.mid(3).toInt();
}

int PrayerTimesParser::kalan(QStringList&& list)				// TODO: doğru mu
{
	const QTime time = QDateTime::currentDateTime().time();
	const int now = time.hour() * 60 + time.minute();
	const QStringList listCopy = list;
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
	const QString fajr = value["Imsak"].toString();
	const QString sunRise = value["Gunes"].toString();
	const QString zuhr = value["Ogle"].toString();
	const QString asr = value["Ikindi"].toString();
	const QString maghrib = value["Aksam"].toString();
	const QString isha = value["Yatsi"].toString();

	return kalan({fajr, sunRise, zuhr, asr, maghrib, isha});
}
int PrayerTimesParser::nextDay()
{
	loadJson();

	const QDate dt = QDateTime::currentDateTime().date();

	const int year = dt.year();
	const int month = dt.month();
	const int day = dt.day();

	const QString dayWith0 = QString("0" + QString::number(day)).right(2);
	const QString monthWith0 = QString("0" + QString::number(month)).right(2);

	const QString bugun = dayWith0 + "." + monthWith0 + "." + QString::number(year);

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
