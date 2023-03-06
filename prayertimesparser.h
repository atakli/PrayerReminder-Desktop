#ifndef PRAYERTIMESPARSER_H
#define PRAYERTIMESPARSER_H

#include <QJsonDocument>

enum LoadJsonSuccess {GoodJson = 0, EvkatFilesDoesNotExist = -1, FileOpeningError = -2};

class PrayerTimesParser
{
public:
    int vakitleriCikar(QJsonValue value);
	int nextDay();
private:
	std::pair<QJsonDocument, LoadJsonSuccess> loadJson();
	int kalan(QStringList list);
};

#endif // PRAYERTIMESPARSER_H
