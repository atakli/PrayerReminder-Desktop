#include "httpmanager.h"

#include <QtWidgets>
#include <QtNetwork>
#include <QtGlobal>

#include <algorithm>

HttpManager::HttpManager(QWidget *parent) : QDialog(parent)	{}

void HttpManager::startRequest(const QUrl &requestedUrl)
{
	QNetworkRequest req = QNetworkRequest(requestedUrl);
//#if QT_VERSION < QT_VERSION_CHECK(5,15,3)
//#if QT_VERSION < 393729                                                 // TODO: düzeltmem gerekli burayı. generic değil. sadece bana uygun
//#if QT_VERSION < 493729                                                 // TODO: düzeltmem gerekli burayı. generic değil. sadece bana uygun
//    req.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
//#endif	// 331266 (linux)
//	req.setHeader(QNetworkRequest::LocationHeader, "Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:98.0) Gecko/20100101 Firefox/98.0");

	reply.reset(qnam.get(req));
    connect(reply.get(), &QIODevice::readyRead, this, &HttpManager::httpReadyRead);
#if QT_CONFIG(ssl)
	connect(reply.get(), &QNetworkReply::sslErrors, this, &HttpManager::sslErrors);
#endif
	QEventLoop eventLoop;
	connect(reply.get(), SIGNAL(finished()), &eventLoop, SLOT(quit()));
	connect(reply.get(), QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error), this, &HttpManager::connectionControl);
	eventLoop.exec();
	httpFinished();
}
void HttpManager::downloadFile(const QString fileName, const QString urlSpec)	// TODO: ismini downloadAsynchronous yap
{
	QUrl url = QUrl::fromUserInput(urlSpec);

	file = openFileForWrite(fileName);
	if (!file)
		return;
	startRequest(url);	// schedule the request
}
void HttpManager::downloadSynchronous(QString fileName, QString urlSpec, const QString& downloadFileName)
{
	bool newVersion = false;
	QString directory;
	if(fileName == "")
	{
		newVersion = true;
		directory = QFileDialog::getExistingDirectory(this, tr("Yeni sürümü indireceğiniz klasörü seçin"));
		if(directory == "")
			return;
		fileName = directory + "/" + downloadFileName;  // TODO: burda seperator muydu neydi o hale getir
	}

	QUrl url = QUrl::fromUserInput(urlSpec);

	file = openFileForWrite(fileName);
	if (!file)
	{
		return;
	}
	startRequest(url);

	if(newVersion)
	{
        QMessageBox::information(nullptr, tr("Yeni versiyon"), QString(directory + " klasörüne indirildi."));
	}
}

std::unique_ptr<QFile> HttpManager::openFileForWrite(const QString &fileName, QIODevice::OpenModeFlag flag)
{
	std::unique_ptr<QFile> file = std::make_unique<QFile>(fileName);
	if (!file->open(flag))
	{
		QMessageBox::information(this, tr("Hata!"), tr("Dosyayı açamıyoruz: %1: %2.").arg(QDir::toNativeSeparators(fileName), file->errorString()));
		return nullptr;
	}
	return file;
}

void HttpManager::httpFinished()
{
	if (file)
	{
		file->close();
		file.reset();
	}
}

void HttpManager::httpReadyRead()
{
    // This slot gets called every time the QNetworkReply has new data. We read all of its new data and write it into the file. That way we use less RAM than when reading it at the finished() signal of the QNetworkReply
//	auto bytes = reply->bytesAvailable();
	if (file)
		file->write(reply->readAll());
//	qDebug() << "bytes: " << bytes;
}
void HttpManager::connectionControl(QNetworkReply::NetworkError)
{
	QMessageBox::warning(nullptr, tr("Hata!"), QString("İnternete bağlanamadık. İnternet bağlantınızı kontrol edin."));
	hasError = true;	// internet olmadığında HostNotFoundError oldu
//	QString logFileName = "logs.txt";
//	std::unique_ptr<QFile> logFile = openFileForWrite(logFileName, QIODevice::Append);
//	QString errorString = QDateTime::currentDateTime().toString() + " -> " + QVariant(error).toString();
//	logFile->write(errorString.toStdString().c_str());
}
#if QT_CONFIG(ssl)
void HttpManager::sslErrors(const QList<QSslError> &errors)
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
