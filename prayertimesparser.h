#ifndef PRAYERTIMESPARSER_H
#define PRAYERTIMESPARSER_H

#include <QJsonDocument>

class PrayerTimesParser
{
private:
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
