#include "window.h"
#include "calcTimes.h"
#include "ui_sehirSecwindow.h"

#ifndef QT_NO_SYSTEMTRAYICON

#include <QDir>
#include <QFile>
#include <QMenu>
#include <QTimer>
#include <QDebug>
#include <QAction>
#include <QPainter>
#include <QFuture>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>
#include <QStyleFactory>
#include <QCoreApplication>
#include <QtConcurrent/QtConcurrent>

QString dosyayiAc(const QString& fileName, QIODevice::OpenModeFlag flag=QIODevice::ReadOnly);

Window::Window(QWidget* parent) : QWidget(parent), ui(std::make_shared<Ui::Window>())
{
	ui->setupUi(this);

//    QApplication::setStyle(QStyleFactory::create("Fusion"));
#ifdef linux
    this->setMaximumHeight(200);
#endif

	ulkeKodu = "2";		// Türkiye
	sehirKodu = "551";	// Kocaeli
	executeFileNames();

    createActions();
	createTrayIcon();

//    if(!QFileInfo::exists(applicationDirPath + evkatOfflinePath))
//    {
//		CalcTimes calc;
//		calc.offlineVakitleriHesapla();
//    }

    // offlinevakitler yok uyarısı verilebilir

//	connect(trayIcon, &QSystemTrayIcon::messageClicked, this, &Window::onClickedOK);
//    connect(trayIcon, &QWidget::closeEvent, this, &Window::addClockToThread);
//	addClockToThread();
//	QFuture<void> futureClock = QtConcurrent::run(this, &Window::repeatClockRequest);

	QTimer *timer = new QTimer(this);
//	std::unique_ptr<QTimer> timer = std::make_unique<QTimer>(this);
	connect(timer, &QTimer::timeout, this, &Window::showTime);
    timer->start(1000);

	update.isNewVersionAvailable();

//	QTimer *timer1 = new QTimer(this);
//	connect(timer1, &QTimer::timeout, update, &UpdateController::isNewVersionAvailable);
//	timer1->start(1000 * 60 * 60 * 24); // günde bir yeni versiyon kontrolü

    ui->koordinatGroupBox->setChecked(false);
	connect(ui->hesaplaButton, &QAbstractButton::clicked, this, &Window::downloadEvkat);
//	connect(ui->koordinatGroupBox, &QGroupBox::toggled, this, &Window::downloadEvkat);
//	connect(this, &QWidget::close, ui->textLabel, &QLabel::clear);	// close fonksiyonu signal değil, slot
//    connect(ui->ulke, SIGNAL(currentTextChanged(QString)), [this](QString ulke) {fillCities(ulke);});
	connect(ui->ulke, SIGNAL(currentIndexChanged(int)), SLOT(fillCities(int)));
	connect(ui->sehir, SIGNAL(currentIndexChanged(int)), SLOT(fillTown(int)));
	connect(ui->ilce, SIGNAL(currentIndexChanged(int)), SLOT(executeIlceKodu(int)));

	controlOnlineEvkatFileExistOtherwiseRequestDownload();

	setWindowTitle(tr("Şehir Seçimi"));
	resize(400, 300);

	trayIcon->show();
}
void Window::executeFileNames()
{
	ulkeFile = applicationDirPath + "/namazVakitFiles/ulkeler.txt";
	sehirFile = applicationDirPath + "/namazVakitFiles/sehirler/" + ulkeKodu + ".txt";
	ilceFile = applicationDirPath + "/namazVakitFiles/sehirler/" + sehirKodu + ".txt";
}

void Window::setIcon(uint8_t number)
{
#ifdef linux                // TODO: hoş olmadı
	QPixmap pixmap(35,35);
#else
    QPixmap pixmap(16,16);
#endif
    pixmap.fill(Qt::yellow);
    QPainter painter(&pixmap);
//    QPainter painter;
//	QFont font = painter.font();
//	font.setPixelSize(28);
//	painter.setFont(font);
	const QString string = QString::number(number);
//	const QRect rectangle = QRect(0, 0, 100, 50);
//	QRect boundingRect;
//    painter.drawText(0,0,16,16, Qt::TextDontClip, string);
    painter.drawText(pixmap.rect(), Qt::TextDontClip | Qt::AlignCenter, string);
//    painter.drawText(pixmap.rect(), Qt::AlignCenter, string); // olmadı
//	painter.drawText(string); // olmadı

//	QPen pen = painter.pen();
//	pen.setStyle(Qt::DotLine);
//	painter.setPen(pen);
//	painter.drawRect(boundingRect.adjusted(0, 0, -pen.width(), -pen.width()));

//	pen.setStyle(Qt::DashLine);
//	painter.setPen(pen);
//	painter.drawRect(rectangle.adjusted(0, 0, -pen.width(), -pen.width()));

    trayIcon->setIcon(pixmap);  // QIcon(pixmap) desen de oluyor
}

void Window::showMessage()
{
//    qmbox.setText("حي على الصلاة");
    qmbox.setWindowTitle("حي على الصلاة");
    qmbox.setText("5 dk'dan az kaldı!");
//    qmbox.resize(1200,12);
//    qmbox.setInformativeText("5 dk'dan az kaldı!");
//    qmbox.setWindowFlag(Qt::WindowStaysOnTopHint);
//	QMessageBox::StandardButton out =
    qmbox.setWindowModality(Qt::NonModal);
//    qmbox.information(nullptr, tr("حي على الصلاة"), QString("5 dk'dan az kaldı!"));
//    msgBox.setWindowFlags(Qt::WindowStaysOnTopHint);
//    msgBox->setAttribute(Qt::WA_DeleteOnClose);
//    qmbox.exec();
//    while (1)
//    {
//        QCoreApplication::processEvents();
        qmbox.show();
//        QCoreApplication::processEvents();
//    }

}
void Window::createActions()
{
	quitAction = new QAction(tr("&Çıkış"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
	sehirSecimiAction = new QAction(tr("&Şehir seç"), this);
	updateAction = new QAction(tr("&Yeni sürüm var mı?"), this);
	emailAction = new QAction(tr("&Önerini Yaz"), this);
//	connect(sehirSecimiAction, &QAction::triggered, this, &Window::bolgeSec);
	connect(sehirSecimiAction, SIGNAL(triggered()), this, SLOT(bolgeSec()));	// sanırım this'i kaldırınca da aynı mana
	connect(updateAction, SIGNAL(triggered()), this, SLOT(controlUpdate()));
	connect(this, &Window::son5Dk, this, &Window::showMessage);
}
void Window::controlUpdate()
{
	update.isNewVersionAvailable();
}
void Window::ilkBolgeSecimi()
{
	ui->sehir->setCurrentText("KOCAELİ");
	ui->ilce->setCurrentText("GEBZE");
}
void Window::fillCities(int ulkeIndex)
{
	executeFileNames();
	ulkeKodu = dosyayiAc(ulkeFile).split('\n').at(ulkeIndex).split("_").last();
	executeFileNames();
	QStringList sehirler = dosyayiAc(sehirFile).split('\n');

	ui->sehir->clear();
	for(const QString& sehir : sehirler)
    {
        ui->sehir->addItem(sehir.split('_').at(0));
    }
}
void Window::fillTown(int sehirIndex)
{
	if(sehirIndex == -1)
		return;
	executeFileNames();
	sehirKodu = dosyayiAc(sehirFile).split('\n').at(sehirIndex).split("_").last(); // 551
	executeFileNames();
	QStringList ilceler = dosyayiAc(ilceFile).split('\n');

	ui->ilce->clear();
	for(const QString& ilce : ilceler)
	{
		ui->ilce->addItem(ilce.split('_').at(0));
	}
}
void Window::executeIlceKodu(int ilceIndex)
{
	if(ilceIndex == -1)
		return;
	executeFileNames();
	ilceKodu = dosyayiAc(ilceFile).split('\n').at(ilceIndex).split("_").last(); // 551
}
void Window::downloadEvkat()
{
	QString urlSpec = "https://ezanvakti.herokuapp.com/vakitler/" + ilceKodu;	// note that times in this site are not updated everyday
    double boylam = ui->boylamLineEdit->text().toDouble();
    double enlem = ui->enlemLineEdit->text().toDouble();
    QString hasOfflineDownloaded = "";
    if(ui->koordinatGroupBox->isChecked() & (boylam != 0.0) & (enlem != 0.0))
    {
        CalcTimes calc;
        calc.offlineVakitleriHesapla(boylam, enlem);                // TODO: CalcTimes{}.offlineVakitleriHesapla(boylam, enlem); daha mantıklı olabilir
        hasOfflineDownloaded = " ve offline vakitler hesaplandı";
    }
	QString fileName = applicationDirPath + evkatOnlinePath;
	fetchTimes.downloadSynchronous(fileName, urlSpec);
    ui->textLabel->setText(ui->ilce->currentText() + " için bir aylık vakitler indirildi" + hasOfflineDownloaded);
}

void Window::controlOnlineEvkatFileExistOtherwiseRequestDownload()
{
	if(!QFileInfo::exists(applicationDirPath + evkatOnlinePath))
		bolgeSec();
}

void Window::bolgeSec()
{
	QStringList ulkeler = dosyayiAc(ulkeFile).split('\n');
	for(const QString& ulke : ulkeler)
	{
		ui->ulke->addItem(ulke.split('_').at(0));
	}
	ilkBolgeSecimi();
    show();
}

void Window::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
	trayIconMenu->addAction(sehirSecimiAction);
	trayIconMenu->addSeparator();
	trayIconMenu->addAction(updateAction);
//	trayIconMenu->addAction(emailAction);
	trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
}

void Window::showTime()
{
	int kalanVakit = ptp.nextDay();
	if(kalanVakit <= 60)			// 60 dk'dan az kalmadıysa gösterme. bi de zaten ikiden fazla basamak göstermeye uygun değil ve gerek de yok
	{
		trayIcon->setVisible(true);
		setIcon(kalanVakit);
		if((kalanVakit <= 5) & (!kalanVakitBesOldu))
		{
			kalanVakitBesOldu = true;
//            QFuture<void> future = QtConcurrent::run(this, &Window::showMessage);	// 5 dk uyarısı çıktığında ok'a basmadığım sürece sayaç akmaya devam etmiyor. o yüzden
//            while(future.isFinished())												// yapmıştım ama olmadı
//                QCoreApplication::processEvents();
//            showMessage();
            emit son5Dk();
		}
	}
	else
		trayIcon->setVisible(false);
	if(kalanVakit > 5)
		kalanVakitBesOldu = false;
}

#endif
