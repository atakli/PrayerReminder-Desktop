#include "fetchtimes.h"
#include "prayertimesparser.h"

#include <QtWidgets>
#include <QtNetwork>
#include <QUrl>

#include <algorithm>
#include <memory>

///usr/share/applications

#if QT_CONFIG(ssl)
const char defaultUrl[] = "https://ezanvakti.herokuapp.com/vakitler/"; // note that times in this site are not updated everyday
#else
const char defaultUrl[] = "http://www.qt.io/";
#endif
//const char defaultFileName[] = "index.html";

/*ProgressDialog::ProgressDialog(const QUrl &url, QWidget *parent) : QProgressDialog(parent)
{
	setWindowTitle(tr("Download Progress"));
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	setLabelText(tr("Downloading %1.").arg(url.toDisplayString()));
	setMinimum(0);
	setValue(0);
	setMinimumDuration(0);
	setMinimumSize(QSize(400, 75));
}

void ProgressDialog::networkReplyProgress(qint64 bytesRead, qint64 totalBytes)
{
	setMaximum(totalBytes);
	setValue(bytesRead);
}*/

HttpWindow::HttpWindow(QWidget *parent) : QDialog(parent)
{
	controlEvkatFile();
}
HttpWindow::~HttpWindow() = default;

void HttpWindow::startRequest(const QUrl &requestedUrl)
{
	url = requestedUrl;
	httpRequestAborted = false;

	//! [qnam-download]
	reply.reset(qnam.get(QNetworkRequest(url)));
	//! [qnam-download]
	//! [connecting-reply-to-slots]
	connect(reply.get(), &QNetworkReply::finished, this, &HttpWindow::httpFinished);
	//! [networkreply-readyread-1]
	connect(reply.get(), &QIODevice::readyRead, this, &HttpWindow::httpReadyRead);
	//! [networkreply-readyread-1]
#if QT_CONFIG(ssl)
	//! [sslerrors-1]
//    connect(reply.get(), &QNetworkReply::sslErrors, this, &HttpWindow::sslErrors);
	//! [sslerrors-1]
#endif
}

void HttpWindow::controlEvkatFile()
{
    QString fileName = "namazVakitFiles/evkatOnline.json";
    if(!QFileInfo::exists(fileName))
        downloadFile();
}

void HttpWindow::downloadFile(QString ilceKodu)
{
	const QString urlSpec = defaultUrl + ilceKodu;
	if (urlSpec.isEmpty())
		return;

	const QUrl newUrl = QUrl::fromUserInput(urlSpec);
	if (!newUrl.isValid())
	{
		QMessageBox::information(this, tr("Error"), tr("Invalid URL: %1: %2").arg(urlSpec, newUrl.errorString()));
		return;
	}

    QString fileName = "namazVakitFiles/evkatOnline.json";

	if (QFile::exists(fileName))
	{
        if (QMessageBox::question(this, tr("Overwrite Existing File"), tr("There already exists a file called %1%2. Overwrite?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::No)
		{
			return;
		}
		QFile::remove(fileName);
	}
//	QFile::remove(fileName);

	file = openFileForWrite(fileName);
	if (!file)
		return;

	// schedule the request
	startRequest(newUrl);
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

/*void HttpWindow::cancelDownload()
{
	statusLabel->setText(tr("Download canceled."));
	httpRequestAborted = true;
	reply->abort();
	downloadButton->setEnabled(true);
}*/

void HttpWindow::httpFinished()
{
	QFileInfo fi;
	if (file) {
		fi.setFile(file->fileName());
		file->close();
		file.reset();
	}

	//! [networkreply-error-handling-1]
	QNetworkReply::NetworkError error = reply->error();
//	const QString &errorString = reply->errorString();
	//! [networkreply-error-handling-1]
	reply.reset();
	//! [networkreply-error-handling-2]
	if (error != QNetworkReply::NoError)
	{
		QFile::remove(fi.absoluteFilePath());
		// For "request aborted" we handle the label and button in cancelDownload()
		if (!httpRequestAborted)
		{
//            statusLabel->setText(tr("Download failed:\n%1.").arg(errorString));

			/*int selectedIcon = 3;																// TODO: olmuyor?
			QSystemTrayIcon::MessageIcon msgIcon = QSystemTrayIcon::MessageIcon(selectedIcon);
			QSystemTrayIcon trayIcon;
			trayIcon.showMessage(tr("Download failed:\n%1.").arg(errorString), "", msgIcon, 1 * 1000);*/

//            downloadButton->setEnabled(true);
		}
		return;
	}

	//! [networkreply-error-handling-2]

//    statusLabel->setText(tr("Downloaded %1 bytes to %2\nin\n%3").arg(fi.size()).arg(fi.fileName(), QDir::toNativeSeparators(fi.absolutePath())));
//    if (launchCheckBox->isChecked())
//        QDesktopServices::openUrl(QUrl::fromLocalFile(fi.absoluteFilePath()));
//    downloadButton->setEnabled(true);
}

//! [networkreply-readyread-2]
void HttpWindow::httpReadyRead()
{
	// This slot gets called every time the QNetworkReply has new data.
	// We read all of its new data and write it into the file.
	// That way we use less RAM than when reading it at the finished()
	// signal of the QNetworkReply
	if (file)
		file->write(reply->readAll());
}
//! [networkreply-readyread-2]

void HttpWindow::enableDownloadButton()
{
	downloadButton->setEnabled(!urlLineEdit->text().isEmpty());
}

//! [qnam-auth-required-2]
/*void HttpWindow::slotAuthenticationRequired(QNetworkReply *, QAuthenticator *authenticator)
{
	QDialog authenticationDialog;
	Ui::Dialog ui;
	ui.setupUi(&authenticationDialog);
	authenticationDialog.adjustSize();
	ui.siteDescription->setText(tr("%1 at %2").arg(authenticator->realm(), url.host()));

	// Did the URL have information? Fill the UI.
	// This is only relevant if the URL-supplied credentials were wrong
	ui.userEdit->setText(url.userName());
	ui.passwordEdit->setText(url.password());

	if (authenticationDialog.exec() == QDialog::Accepted) {
		authenticator->setUser(ui.userEdit->text());
		authenticator->setPassword(ui.passwordEdit->text());
	}
}*/
//! [qnam-auth-required-2]

#if QT_CONFIG(ssl)
//! [sslerrors-2]
/*void HttpWindow::sslErrors(const QList<QSslError> &errors)
{
	QString errorString;
	for (const QSslError &error : errors) {
		if (!errorString.isEmpty())
			errorString += '\n';
		errorString += error.errorString();
	}

	if (QMessageBox::warning(this, tr("SSL Errors"),
							 tr("One or more SSL errors has occurred:\n%1").arg(errorString),
							 QMessageBox::Ignore | QMessageBox::Abort) == QMessageBox::Ignore) {
		reply->ignoreSslErrors();
	}
}*/
//! [sslerrors-2]
#endif
