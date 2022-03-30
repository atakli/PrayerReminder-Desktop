//#include "prayertimesparser.h"
#include "fetchtimes.h"

#include <QtWidgets>
#include <QtNetwork>

#include <algorithm>

HttpWindow::HttpWindow(QWidget *parent) : QDialog(parent)
{
	if(!QFileInfo::exists(applicationDirPath + evkatOnlinePath))
		downloadFile();
}
HttpWindow::~HttpWindow() = default;

void HttpWindow::startRequest(const QUrl &requestedUrl)
{
	reply.reset(qnam.get(QNetworkRequest(requestedUrl)));
	connect(reply.get(), &QNetworkReply::finished, this, &HttpWindow::httpFinished);
	connect(reply.get(), &QIODevice::readyRead, this, &HttpWindow::httpReadyRead);
#if QT_CONFIG(ssl)
	connect(reply.get(), &QNetworkReply::sslErrors, this, &HttpWindow::sslErrors);
#endif
}

void HttpWindow::downloadFile(QString ilceKodu)
{
	const char defaultUrl[] = "https://ezanvakti.herokuapp.com/vakitler/"; // note that times in this site are not updated everyday
	const QString urlSpec = defaultUrl + ilceKodu;

	const QUrl newUrl = QUrl::fromUserInput(urlSpec);

	QString fileName = applicationDirPath + evkatOnlinePath;

	if (QFile::exists(fileName))
	{
        if (QMessageBox::question(this, tr("Overwrite Existing File"), tr("There already exists a file called %1%2. Overwrite?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::No)
		{
			return;
		}
//		QFile::remove(fileName);
	}
	file = openFileForWrite(fileName);
	if (!file)
	{
		return;
	}
	startRequest(newUrl);	// schedule the request
}

std::unique_ptr<QFile> HttpWindow::openFileForWrite(const QString &fileName)
{
	std::unique_ptr<QFile> file = std::make_unique<QFile>(fileName);
	if (!file->open(QIODevice::WriteOnly))
	{
		QMessageBox::information(this, tr("Error"), tr("Unable to save the file %1: %2.").arg(QDir::toNativeSeparators(fileName), file->errorString()));
		return nullptr;
	}
	return file;
}

void HttpWindow::httpFinished()
{
	QFileInfo fi;
	if (file)
	{
		fi.setFile(file->fileName());
		file->close();
		file.reset();
	}

	QNetworkReply::NetworkError error = reply->error();
	reply.reset();
	if (error != QNetworkReply::NoError)
	{
		QFile::remove(fi.absoluteFilePath());
		return;
	}
}

void HttpWindow::httpReadyRead()
{
	// This slot gets called every time the QNetworkReply has new data.
	// We read all of its new data and write it into the file.
	// That way we use less RAM than when reading it at the finished()
	// signal of the QNetworkReply
	if (file)
		file->write(reply->readAll());
}

#if QT_CONFIG(ssl)
void HttpWindow::sslErrors(const QList<QSslError> &errors)
{
	QString errorString;
	for (const QSslError &error : errors)
	{
		if (!errorString.isEmpty())
			errorString += '\n';
		errorString += error.errorString();
	}

	if (QMessageBox::warning(this, tr("SSL Errors"), tr("One or more SSL errors has occurred:\n%1").arg(errorString), QMessageBox::Ignore | QMessageBox::Abort) == QMessageBox::Ignore)
	{
		reply->ignoreSslErrors();
	}
}
#endif
