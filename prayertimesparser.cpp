#include "prayertimesparser.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>
#include <QFile>


PrayerTimesParser::PrayerTimesParser()
{

}

bool PrayerTimesParser::loadJson()
{
	QFile loadFile(QStringLiteral("/home/b720/evkatOnline.json"));

	if (!loadFile.open(QIODevice::ReadOnly))
	{
		qWarning("Couldn't open save file.");
		return false;
	}

	QByteArray saveData = loadFile.readAll();

	loadDoc = QJsonDocument::fromJson(saveData);

//	QJsonObject object = loadDoc.object();

	return true;
}
void PrayerTimesParser::vakitleriCikar(QJsonValue value)
{

}
void PrayerTimesParser::nextDay()
{
	QDate dt = QDateTime::currentDateTime().date();

	int year = dt.year();
	int month = dt.month();
	int day = dt.day();

	QString dayWith0 = QString("0" + QString::number(day)).rightRef(2).toString();
	QString monthWith0 = QString("0" + QString::number(month)).rightRef(2).toString();

	QString bugun = dayWith0 + "." + monthWith0 + "." + QString::number(year);

	uint8_t index = 0;
	QJsonValue next;
	while(!next.isUndefined())
	{
		next = loadDoc[index];
		if(bugun == next["MiladiTarihKisa"].toString())
		{
			vakitleriCikar(next);
			break;
		}
		++index;
	}

}
void PrayerTimesParser::nextPrayer()
{

}

