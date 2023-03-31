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

std::variant<QJsonDocument, JsonSuccess> PrayerTimesParser::loadJson()
{
    QFile loadFile(evkatOnlinePath);

    if (!QFileInfo::exists(evkatOnlinePath) && !QFileInfo::exists(evkatOfflinePath))
	{
        return EvkatFilesDoesNotExist;
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
template<typename InputIt, typename OutputIt, typename UnaryOperation, typename UnaryPredicate>
OutputIt transform_if(InputIt first, InputIt last, OutputIt dest, UnaryOperation op, UnaryPredicate pred)
{
    while (first != last)
    {
        const auto& val = *first;                                            // TODO: && olmasi dogru mu tam anlamadim ???
        if (pred(val))
            *dest++ = op(val);
        ++first;
    }
    return dest;
}
int PrayerTimesParser::kalan(QStringList list)
{
    auto toMinutes = [](const QString& vakit)
	{
//		return vakit.mid(0,2).toInt() * 60 + vakit.mid(3).toInt();
        const auto time = QTime::fromString(vakit, "HH:mm");
        return time.hour() * 60 + time.minute();
	};
    const QTime time = QTime::currentTime();
	const int now_as_minutes = time.hour() * 60 + time.minute();
//	const QStringList listCopy = list;

    QVector<int> listInt;
//    bool isIcerde = false;
//    for(const QString& l : list)
//	{
//        const int min = toMinutes(l);
//		if(min >= now_as_minutes)
//			listInt.emplaceBack(std::move(min));
//        else
//	}

    transform_if(list.begin(), list.end(), std::back_inserter(listInt), toMinutes, [&toMinutes, &now_as_minutes](const QString& l){return toMinutes(l) >= now_as_minutes;});

    const int enUfagi = listInt.isEmpty() ? toMinutes(list.first()) : *std::min_element(listInt.begin(), listInt.end());
    const int sonuc = enUfagi - now_as_minutes;
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

    const QString date = QDate::currentDate().toString("dd.MM.yyyy");

    while(!next.isUndefined())
    {
        next = loadedJson[index];
        if(date == next["MiladiTarihKisa"].toString()) // TODO: dongu yerine direk o girdiye de gidilebilir belki
        {
            kalanVakit = vakitleriCikar(next);
            isJsonUpToDate = true;
            break;
        }
        ++index;
    }
    return {kalanVakit, isJsonUpToDate};
}
std::variant<int, JsonSuccess> PrayerTimesParser::kalanVakit()
{
    QJsonDocument loadedJsonResult;
    try {
        loadedJsonResult = std::get<QJsonDocument>(loadJson());
    } catch (const std::bad_variant_access&) {
        return std::get<JsonSuccess>(loadJson());
    }

    const auto [kalanVakit, isJsonUpToDate] = loopOverJson(loadedJsonResult);

    if (!isJsonUpToDate)
        return OnlineJsonFileIsOutOfDate;
	return kalanVakit;
}
