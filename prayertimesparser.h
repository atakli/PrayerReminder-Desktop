#ifndef PRAYERTIMESPARSER_H
#define PRAYERTIMESPARSER_H

#include <QJsonDocument>

class PrayerTimesParser
{
private:
	QJsonDocument loadDoc;
	void nextDay();
	void nextPrayer();
	void vakitleriCikar(QJsonValue value);
public:
    PrayerTimesParser();
	bool loadJson();
};

#endif // PRAYERTIMESPARSER_H
