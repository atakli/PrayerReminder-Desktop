#include "prayertimesparser.h"

#include <QMessageBox>
#include <QJsonObject>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QDir>

extern QString evkatOnlinePath;
extern QString evkatOfflinePath;

extern const char* appName;

PrayerTimesParser::PrayerTimesParser(QObject* parent) : QObject(parent) {}

std::expected<QJsonDocument, JsonSuccess> PrayerTimesParser::loadJson()
{
    QFile loadFile(evkatOnlinePath);

    if (!QFileInfo::exists(evkatOnlinePath) && !QFileInfo::exists(evkatOfflinePath))
	{
        return std::unexpected {EvkatFilesDoesNotExist};
	}
	else if (!QFileInfo::exists(evkatOnlinePath))
	{
		loadFile.setFileName(evkatOfflinePath);
	}

    if (!loadFile.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(nullptr, appName, QString("Dosya acilamadi -> %1\n").arg(loadFile.fileName()) + QDir::currentPath());
        exit(EXIT_FAILURE);
    }

	const QByteArray saveData = loadFile.readAll();
    loadFile.close();

    return QJsonDocument::fromJson(saveData);
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
std::pair<int, bool> PrayerTimesParser::loopOverJson(const QJsonDocument& loadedJson)
{
    uint8_t index = 0;
    QJsonValue next;
    int kalanVakit = 0;
    bool isJsonUpToDate = false;

    const QDate date = QDateTime::currentDateTime().date();
    const QString dayWith0 = QString("0" + QString::number(date.day())).right(2);
    const QString monthWith0 = QString("0" + QString::number(date.month())).right(2);
    const QString bugun = dayWith0 + "." + monthWith0 + "." + QString::number(date.year());

    while(!next.isUndefined())
    {
        next = loadedJson[index];
        if(bugun == next["MiladiTarihKisa"].toString()) // TODO: dongu yerine direk o girdiye de gidilebilir belki
        {
            kalanVakit = vakitleriCikar(next);
            isJsonUpToDate = true;
            break;
        }
        ++index;
    }
    return {kalanVakit, isJsonUpToDate};
}
std::expected<int, JsonSuccess> PrayerTimesParser::kalanVakit()
{
    const auto loadedJsonResult = loadJson();
    if (!loadedJsonResult.has_value())
        return std::unexpected {(JsonSuccess)loadedJsonResult.error()};

    const auto [kalanVakit, isJsonUpToDate] = loopOverJson(loadedJsonResult.value());

    if (!isJsonUpToDate)
        return std::unexpected {OnlineJsonFileIsOutOfDate};
	return kalanVakit;
}
