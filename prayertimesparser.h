#ifndef PRAYERTIMESPARSER_H
#define PRAYERTIMESPARSER_H

#include <QJsonDocument>

class PrayerTimesParser
{
private:
	bool loadJson();
	QJsonDocument loadDoc;
	int Min(const QString& vakit);
	int kalan(QStringList&& list);
public:
	int nextDay();
    PrayerTimesParser();
	int vakitleriCikar(QJsonValue value);
};

#endif // PRAYERTIMESPARSER_H
