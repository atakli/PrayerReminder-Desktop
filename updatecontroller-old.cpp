#include "updatecontroller.h"
//#include "calcTimes.h"
#include "util.h"

#include <QJsonDocument>
#include <QMessageBox>
#include <QFile>

UpdateController::UpdateController(const QString& versionFileName) : versionFileName(versionFileName) {}
//UpdateController::~UpdateController()	{}
QString dosyayiAc(const QString& fileName, QIODevice::OpenModeFlag flag=QIODevice::ReadOnly)
{
	QFile file(fileName);
	if (!file.open(flag | QIODevice::Text))
		return "";							// TODO: buraya girerse ne olcak?
	QString text = file.readAll();
	file.close();
	return text;
}
bool UpdateController::compareTagVersion(const QString& tag, QString currentTag)
{
	const QString tag1 = tag.split("-").at(0).mid(1);

	const uint8_t ilkTagCurrent = currentTag.split(".").first().toUInt();
	const uint8_t ortaTagCurrent = currentTag.split(".").at(1).toUInt();
	const uint8_t sonTagCurrent = currentTag.split(".").last().toUInt();

	osName = currentTag.split(".").last().split("-").last().trimmed();

	const uint8_t ilkTag = tag1.split(".").first().toUInt();
	const uint8_t ortaTag = tag1.split(".").at(1).toUInt();
	const uint8_t sonTag = tag1.split(".").last().toUInt();

	if(ilkTag < ilkTagCurrent)
		return false;
	else if(ilkTag > ilkTagCurrent)
		return true;
	else
	{
		if(ortaTag < ortaTagCurrent)
			return false;
		else if(ortaTag > ortaTagCurrent)
			return true;
		else
		{
			if(sonTag > sonTagCurrent)
				return true;
			else
				return false;
		}
	}
	return false;
}

void UpdateController::isNewVersionAvailable()
{
    const QString apiPath = Paths::applicationDirPath + "/api.json";
	const QString newUrl = "https://api.github.com/repos/atakli/PrayerReminder-Desktop/releases/latest";
	fetchTimes.downloadSynchronous(apiPath, newUrl);

	const QString saveData = dosyayiAc(apiPath);
//	QJsonDocument loadDoc = QJsonDocument::fromVariant(saveData);
	const QJsonDocument loadDoc = QJsonDocument::fromJson(QByteArray::fromStdString(saveData.toStdString()));

//    const QString currentTag = dosyayiAc(Paths::applicationDirPath + "/namazVakitFiles/version.txt");
    const QString currentTag = dosyayiAc(versionFileName);
	if(const QString tag = loadDoc["tag_name"].toString(); compareTagVersion(tag, currentTag))
	{
		if (QMessageBox(QMessageBox::Question, "Namaz Vakti Hatırlatıcı", "Yeni sürüm bulundu\nİndirilelim mi?", QMessageBox::No|QMessageBox::Yes).exec() == QMessageBox::No)
		{
			return;
		}
//			fetchTimes.downloadSynchronous("", loadDoc["assets"][0]["browser_download_url"].toString()); // ismi PrayerReminder.zip'dan başka bişey olursa diye
		fetchTimes.downloadSynchronous("", "https://github.com/atakli/PrayerReminder-Desktop/releases/latest/download/PrayerReminder-" + osName + ".zip");
	}
	else
	{
		QMessageBox qmbox;
		qmbox.information(nullptr, tr("Namaz Vakti Hatırlatıcı"), QString("Program güncel"));
	}
}
