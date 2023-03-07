#ifndef PRAYERTIMESPARSER_H
#define PRAYERTIMESPARSER_H

#include <QJsonDocument>

#include <expected>

enum JsonSuccess {EvkatFilesDoesNotExist, OnlineJsonFileIsOutOfDate};

class PrayerTimesParser : public QObject
{
    Q_OBJECT
public:
    PrayerTimesParser(QObject *parent = nullptr);
    std::expected<int, JsonSuccess> kalanVakit();
    int vakitleriCikar(QJsonValue value);
private:
    std::pair<int, bool> loopOverJson(const QJsonDocument& loadedJson);
    std::expected<QJsonDocument, JsonSuccess> loadJson();
	int kalan(QStringList list);
};

#endif // PRAYERTIMESPARSER_H
