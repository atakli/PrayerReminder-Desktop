#include "prayertimesparser.h"

#include <QJsonObject>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QDir>

extern QString evkatOnlinePath;
extern QString evkatOfflinePath;

std::pair<QJsonDocument, LoadJsonSuccess> PrayerTimesParser::loadJson()
{
    QFile loadFile(evkatOnlinePath);

    if (!QFileInfo::exists(evkatOnlinePath) && !QFileInfo::exists(evkatOfflinePath))
	{
		return {{}, EvkatFilesDoesNotExist};
	}
	else if (!QFileInfo::exists(evkatOnlinePath))
	{
		loadFile.setFileName(evkatOfflinePath);
	}

    if (!loadFile.open(QIODevice::ReadOnly))
    {
		qWarning() << "Couldn't open " + loadFile.fileName() + " save file.";
		return {{}, FileOpeningError};
    }

	const QByteArray saveData = loadFile.readAll();
    loadFile.close();

	return {QJsonDocument::fromJson(saveData), GoodJson};
}

int PrayerTimesParser::kalan(QStringList list)
{
	auto Min = [](const QString& vakit)
	{
		return vakit.mid(0,2).toInt() * 60 + vakit.mid(3).toInt();
	};
	const QTime time = QDateTime::currentDateTime().time();
	const int now_as_minutes = time.hour() * 60 + time.minute();
	const QStringList listCopy = list;
    for(const QString& l : list)
	{
		if((Min(l) - now_as_minutes) < 0)
			list.removeOne(l);
	}
	QVector<int> listInt;
    for(const QString& l : list)
	{
		listInt.push_back(Min(l));
	}
	std::transform(list.begin(), list.end(), std::back_inserter(listInt), [Min](const QString& l){return Min(l);});

    const int enUfagi = listInt.isEmpty() ? Min(listCopy.first()) : *std::min_element(listInt.begin(), listInt.end());
	int sonuc = enUfagi - now_as_minutes;
    return sonuc < 0 ? sonuc + 60 * 24 : sonuc;
}

int PrayerTimesParser::vakitleriCikar(QJsonValue value)
{
	const QString fajr = value["Imsak"].toString();
	const QString sunRise = value["Gunes"].toString();
	const QString zuhr = value["Ogle"].toString();
	const QString asr = value["Ikindi"].toString();
	const QString maghrib = value["Aksam"].toString();
	const QString isha = value["Yatsi"].toString();

    return kalan({fajr, sunRise, zuhr, asr, maghrib, isha});    // yuh. bunu && ile almisim. cahillik kotu bisey.
}
int PrayerTimesParser::nextDay()
{
	const auto [loadedJson, jsonSuccess] = loadJson();
	if (jsonSuccess != GoodJson)
		return jsonSuccess;

	const QDate date = QDateTime::currentDateTime().date();

	const int year = date.year();
	const int month = date.month();
	const int day = date.day();

	const QString dayWith0 = QString("0" + QString::number(day)).right(2);
	const QString monthWith0 = QString("0" + QString::number(month)).right(2);
	const QString bugun = dayWith0 + "." + monthWith0 + "." + QString::number(year);

	uint8_t index = 0;
	QJsonValue next;
	int kalanVakit = 0;
	while(!next.isUndefined())
	{
		next = loadedJson[index];
		if(bugun == next["MiladiTarihKisa"].toString())
		{
			kalanVakit = vakitleriCikar(next);
			break;
		}
		++index;
	}
	return kalanVakit;
}
