#ifndef PRAYERTIMESPARSER_H
#define PRAYERTIMESPARSER_H

#include <QCoreApplication>
#include <QJsonDocument>

const QString evkatOnlinePath = "/namazVakitFiles/evkatOnline.json";

class PrayerTimesParser
{
private:
	const QString applicationDirPath = QCoreApplication::applicationDirPath();
	bool loadJson();
	QJsonDocument loadDoc;
	int Min(QString vakit);
	int kalan(QStringList list);
public:
	int nextDay();
    PrayerTimesParser();
	int vakitleriCikar(QJsonValue value);
};

#endif // PRAYERTIMESPARSER_H
