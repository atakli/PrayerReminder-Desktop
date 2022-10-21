#include "updatecontroller.h"
//#include "calcTimes.h"

#include <QJsonDocument>
#include <QMessageBox>
#include <QFile>

void UpdateController::setParameters(const QString &apiUrl, const QString &appName, const QString &downloadFileName)
{
    this->apiUrl = apiUrl;
    this->appName = appName;
    this->downloadFileName = downloadFileName;
    isParametersSet = true;
}

void UpdateController::downloadFile(QString fileName, QString urlSpec, const QString& downloadFileName)
{
    httpManager.downloadSynchronous(fileName, urlSpec, downloadFileName);
}

QString UpdateController::openFile(const QString& fileName, QIODevice::OpenModeFlag flag)
{
	QFile file(fileName);
    if (!file.open(flag | QIODevice::Text))
		return "";							// TODO: buraya girerse ne olcak?
    return file.readAll();
}
bool UpdateController::compareTagVersion(const QString& tagAtGithub, const QString& currentTag) // TODO: .'lar arasında birden fazla basamak olursa da çalışmalı
{
    osName = currentTag.contains("-") ? currentTag.split("-").last().trimmed() : "";
    auto parse = [](const QString& str){return str.mid(1).split("-").at(0).split(".");};
    auto makeIntVector = [parse](const QString& str)
    {
        std::vector<int> ivec;
        for (const auto& s : parse(str))
            ivec.emplace_back(s.toInt());
        return ivec;
    };

    return makeIntVector(tagAtGithub) > makeIntVector(currentTag);
}

void UpdateController::isNewVersionAvailable()
{
    if (!isParametersSet)
    {
        QMessageBox qmbox;
        qmbox.warning(nullptr, tr(appName.toStdString().c_str()), "Güncelleme Kontrolcüsüne parametreler geçilmemiş\nGüncelleme olup olmadığını kontrol edebilmek için gerekli parametreleri geçip tekrar deneyin");
        return;
    }
    httpManager.downloadSynchronous(apiPath, apiUrl, "");

    const QString saveData = openFile(apiPath);
//	QJsonDocument loadDoc = QJsonDocument::fromVariant(saveData);
    const QJsonDocument loadDoc = QJsonDocument::fromJson(QByteArray::fromStdString(saveData.toStdString()));

    const QString currentTag = openFile(versionFileName);
    if(const QString tag = loadDoc["tag_name"].toString(); compareTagVersion(tag, currentTag))
    {
        if (QMessageBox(QMessageBox::Question, appName, "Yeni sürüm bulundu\nİndirilelim mi?", QMessageBox::No | QMessageBox::Yes).exec() == QMessageBox::No)
		{
			return;
		}
//            https://github.com/atakli/EtkinlikKayit/releases/latest/download/EtkinlikKayit.zip
        QString url = loadDoc["assets"][0]["browser_download_url"].toString();
        if (!osName.isEmpty())
        {
            if (!url.endsWith(osName + ".zip", Qt::CaseInsensitive))
                url = loadDoc["assets"][1]["browser_download_url"].toString();
        }
        httpManager.downloadSynchronous("", url, downloadFileName); // ismi PrayerReminder.zip'dan başka bişey olursa diye // TODO: 0'da sıkıntı olabilir
//        QString url = "https://github.com/atakli/PrayerReminder-Desktop/releases/latest/download/PrayerReminder-" + osName + ".zip";
    }
	else
	{
		QMessageBox qmbox;
        qmbox.information(nullptr, tr(appName.toStdString().c_str()), "Program güncel");
    }
}
