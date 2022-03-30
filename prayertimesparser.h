#ifndef PRAYERTIMESPARSER_H
#define PRAYERTIMESPARSER_H

#include <QCoreApplication>
#include <QJsonDocument>

const QString evkatOnlinePath = "/namazVakitFiles/evkatOnline.json";
const QString evkatOfflinePath = "/namazVakitFiles/evkatOffline.json";

class PrayerTimesParser
{
private:
	const QString applicationDirPath = QCoreApplication::applicationDirPath();
	QJsonDocument loadDoc;
	int Min(QString vakit);
	bool loadJson();
	int kalan(QStringList list);
public:
	int nextDay();
	int vakitleriCikar(QJsonValue value);
    PrayerTimesParser();
};

#endif // PRAYERTIMESPARSER_H
