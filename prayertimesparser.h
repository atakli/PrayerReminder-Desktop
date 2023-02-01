#ifndef PRAYERTIMESPARSER_H
#define PRAYERTIMESPARSER_H

#include <QJsonDocument>

class PrayerTimesParser
{
private:
	bool loadJson();
	QJsonDocument loadDoc;
    bool evkatFilesExist = true;
	int Min(const QString& vakit);
    int kalan(QStringList list);
public:
    int vakitleriCikar(QJsonValue value);
	int nextDay();
};

#endif // PRAYERTIMESPARSER_H
