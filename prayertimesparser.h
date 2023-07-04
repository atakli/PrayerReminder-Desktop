#ifndef PRAYERTIMESPARSER_H
#define PRAYERTIMESPARSER_H

#include <QJsonDocument>

enum JsonSuccess {EvkatFilesDoesNotExist, OnlineJsonFileIsOutOfDate};

class PrayerTimesParser : public QObject
{
    Q_OBJECT
public:
    PrayerTimesParser(QObject *parent = nullptr);
    std::variant<int, JsonSuccess> kalanVakit();
    int vakitleriCikar(const QJsonValue& value);
private:
    std::pair<int, bool> loopOverJson(const QJsonDocument& loadedJson);
    std::variant<QJsonDocument, JsonSuccess> loadJson();
    int kalan(QStringList&& list);
};

#endif // PRAYERTIMESPARSER_H
