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
#include <QProcess>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>
#include <QStyleFactory>
#include <QCoreApplication>

extern QString exePath;

const char* appName = "Namaz Vakti Hatırlatıcı";

static QString readFile(const QString& fileName, QIODevice::OpenModeFlag flag = QIODevice::ReadOnly)
{
    QFile file(fileName);
    if (!file.open(flag | QIODevice::Text))
    {
        qDebug() << "cannot read " << fileName;
        return "";							// TODO: buraya girerse ne olcak?
    }
    QString text = file.readAll();
    file.close();
    return text;
}

QString evkatOnlinePath = "evkatOnline.json";
extern QString evkatOfflinePath;

Window::Window(QWidget* parent) : QWidget(parent), ui(std::make_shared<Ui::Window>())
{
	ui->setupUi(this);

#ifdef linux
    this->setMaximumHeight(200);
#endif
//    setWindowIcon(QIcon{"icon.png"});
    QFile::remove("version.txt");
    QFile::copy(exePath + "/version.txt", "version.txt");

    ulkeKodu = "2";		// Türkiye
    sehirKodu = "551";	// Kocaeli
    executeFileNames();

    createActions();
    createTrayIcon();

    if (!QFileInfo::exists(evkatOnlinePath) && !QFileInfo::exists(evkatOfflinePath))
        bolgeSec();

    update.setParameters("https://api.github.com/repos/atakli/PrayerReminder-Desktop/releases/latest", appName, "NamazVaktiHatirlatici.exe");
    update.isNewVersionAvailable();

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Window::showTime);
    timer->start(1000);

    ui->koordinatGroupBox->setChecked(false);
	connect(ui->hesaplaButton, &QAbstractButton::clicked, this, &Window::downloadEvkat);
	connect(ui->ulke, SIGNAL(currentIndexChanged(int)), SLOT(fillCities(int)));
	connect(ui->sehir, SIGNAL(currentIndexChanged(int)), SLOT(fillTown(int)));
    connect(ui->ilce, SIGNAL(currentIndexChanged(int)), SLOT(executeIlceKodu(int)));
    connect(ui->quitButton, &QAbstractButton::clicked, []{exit(EXIT_SUCCESS);});
    connect(ui->updateButton, &QAbstractButton::clicked, [this]{update.isNewVersionAvailable(true);});
#ifdef win32
    connect(ui->infoButton, &QAbstractButton::clicked, this, &Window::on_infoButtonClicked);
#elif linux
    ui->infoButton->setHidden(true);
#endif

	setWindowTitle(tr("Şehir Seçimi"));
	resize(400, 300);
}

void Window::on_infoButtonClicked()
{
    QMessageBox msgBox(QMessageBox::Question, appName, "Namaz Vakti Hatırlatıcısının simgesinin sağ alt köşedeki bildirim alanında görünür olmasını isterseniz Tamam'a basıp açılan pencereden \"Görev çubuğunda hangi simgelerin görüneceğini seçin.\" (\"Select which icons appear on the taskbar.\") yazısına tıklayıp prayerReminder.exe'yi etkinleştirin.", QMessageBox::Ok | QMessageBox::Abort);
    msgBox.setButtonText(QMessageBox::Ok, "Tamam");
    msgBox.setButtonText(QMessageBox::Abort, "Kapat");
    if (msgBox.exec() == QMessageBox::Ok)
    {
        std::system("start ms-settings:taskbar");
    }
}

void Window::on_instanceOpen(quint32 /*instanceId*/, QByteArray /*message*/)
{
    bolgeSec();
}
void Window::executeFileNames()
{
    ulkeFile =  exePath + "/ulkeler.txt";
    sehirFile = exePath + "/sehirler/" + ulkeKodu + ".txt";   // rastgele azerbaycani secmistim, cannot read sehirler/11639.txt falan dedi.
    ilceFile =  exePath + "/sehirler/" + sehirKodu + ".txt";
    // ayrica bi yeri sectikten sonra pencereyi kapatip tekrar acinca soyle oluyor, mesela: bermuda -> kocaeli -> gebze. yani ilki ayni kaliyor, diger ikisi degisiyor.
}

void Window::setIcon(std::variant<int, VakitStatus> kalanVakit) // bu fonsiyonun bir kisminin her saniye set edilmesi gereksiz sanki. o kisimlari static omurlu olabilir.
{
#ifdef linux                // TODO: hoş olmadı. zaten sıkıntı başka yerde sanırım, ubuntuyla ilgili bişey. bazen de (çoğu zaman) ikon ve message box'lar küçük çıkıyo
    QPixmap pixmap(25,25);
#else
    QPixmap pixmap(16,16);
#endif
    pixmap.fill(Qt::yellow);
    QPainter painter(&pixmap);
//    const QString string =  kalanVakit ? QString::number(kalanVakit.value()) : "X";
    const QString string = [&kalanVakit]
    {
        try
        {
            return QString::number(std::get<int>(kalanVakit));
        }
        catch (const std::bad_variant_access&)
        {
            return QString("X");
        }
    }();
    painter.drawText(pixmap.rect(), Qt::TextDontClip | Qt::AlignCenter, string);
    trayIcon->setIcon(pixmap);  // QIcon(pixmap) desen de oluyor
}

void Window::showMessage()
{
    static QMessageBox qmbox;
    qmbox.setWindowTitle("حي على الصلاة");
    qmbox.setText("5 dk'dan az kaldı!");
    qmbox.setWindowModality(Qt::NonModal);
    qmbox.show();
}
void Window::createActions()
{
	quitAction = new QAction(tr("&Çıkış"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);    // exit(0)'den farkli mi, hangisi daha iyi bak.
	sehirSecimiAction = new QAction(tr("&Şehir seç"), this);
	updateAction = new QAction(tr("&Yeni sürüm var mı?"), this);
    emailAction = new QAction(tr("&Gelistiriciye Yaz"), this);
//	connect(sehirSecimiAction, &QAction::triggered, this, &Window::bolgeSec);
	connect(sehirSecimiAction, SIGNAL(triggered()), this, SLOT(bolgeSec()));	// sanırım this'i kaldırınca da aynı mana
    connect(updateAction, &QAction::triggered, this, [this]{update.isNewVersionAvailable(true);});
	connect(this, &Window::son5Dk, this, &Window::showMessage);
}
void Window::ilkBolgeSecimi()
{
    fillCities(0);
    ui->sehir->setCurrentText("KOCAELİ");
    fillTown(51);
    ui->ilce->setCurrentText("GEBZE");
    executeIlceKodu(4);
}
void Window::fillCities(int ulkeIndex)
{
	executeFileNames();
    ulkeKodu = readFile(ulkeFile).split('\n').at(ulkeIndex).split("_").last();
	executeFileNames();
    const QStringList sehirler = readFile(sehirFile).split('\n');

	ui->sehir->clear();
    for (const QString& sehir : sehirler)
    {
        ui->sehir->addItem(sehir.split('_').at(0));
    }
}
void Window::fillTown(int sehirIndex)
{
    if(sehirIndex == -1)    // bu ne zaman -1 oluyo ya yazmaliydim bak hatirlamiyorum simdi. asagidaki fonksiyondaki hakeza
		return;
	executeFileNames();
    sehirKodu = readFile(sehirFile).split('\n').at(sehirIndex).split("_").last(); // 551
	executeFileNames();
    const QStringList ilceler = readFile(ilceFile).split('\n');

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
    ilceKodu = readFile(ilceFile).split('\n').at(ilceIndex).split("_").last(); // 551
}
void Window::downloadEvkat()
{
    const QString urlSpec = "https://ezanvakti.herokuapp.com/vakitler/" + ilceKodu;	// note that times in this site are not updated everyday
    QString hasOfflineDownloaded = "";
    if (ui->koordinatGroupBox->isChecked() && !ui->boylamLineEdit->text().isEmpty() && !ui->enlemLineEdit->text().isEmpty())    // girilen sayiların gecerli olup olmadigi kontrolu
    {
        const double boylam = ui->boylamLineEdit->text().toDouble();
        const double enlem = ui->enlemLineEdit->text().toDouble();
        CalcTimes{}.offlineVakitleriHesapla(boylam, enlem);
        hasOfflineDownloaded = " ve offline vakitler hesaplandı";
    }
//	fetchTimes.downloadSynchronous(fileName, urlSpec);
    update.downloadFile(evkatOnlinePath, urlSpec);
    ui->textLabel->setText(ui->ilce->currentText() + " için bir aylık vakitler indirildi" + hasOfflineDownloaded);
}
void Window::bolgeSec()
{
    QStringList ulkeler = readFile(ulkeFile).split('\n');
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
//	trayIconMenu->addAction(aboutAction);   // su yaziyi koy uygun bi yere: Icon by dDara on freeicons.io (credits: diyerek)

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
}

void Window::showTime()
{
    const auto result = ptp.kalanVakit();   // TODO: ayni isler her saniye yapiliyor! duzelt

    try
    {
        const auto kalanVakit = std::get<int>(result);
        if(kalanVakit <= 60)		// 60 dk'dan az kalmadıysa gösterme. bi de zaten ikiden fazla basamak göstermeye uygun değil ve gerek de yok
        {
            trayIcon->setVisible(true);
            setIcon(kalanVakit);
            if((kalanVakit <= 5) && (!kalanVakitBesOldu))
            {
                kalanVakitBesOldu = true;
                emit son5Dk();
            }
        }
        else
            trayIcon->setVisible(false);
        if(kalanVakit > 5)
            kalanVakitBesOldu = false;
    }
    catch (const std::bad_variant_access&)
    {
        if (std::get<JsonSuccess>(result) == EvkatFilesDoesNotExist)
        {
//            QMessageBox::critical(nullptr, appName, QString("Hem online hem offline namaz vakti dosyasi yok!\n"));
            setIcon(VakitError);
            trayIcon->setVisible(true);
            bolgeSec();
//            return;		// TODO: iki dosyanin da olmamasi soz konusu olmamali. burda o durumu kurtaralim. edit: niye burda kurtariyoruz? yerinde kurtaralim.
        }
        else if (std::get<JsonSuccess>(result) == OnlineJsonFileIsOutOfDate && !isInformationDialogClosed)
        {
            // TODO: offline'a yonlendir. ama yine de uyari cikar. o da yoksa X isareti koy. edit: bunu program ilk acildiginda da yapmaliyim.
            /*const auto res = */QMessageBox::information(nullptr, appName, QString("Online namaz vakit dosyası eski veya yok. Offline'dan devam edilecek.\n"));
//            if (res == QMessageBox::Ok || res == QMessageBox::Escape || res == QMessageBox::Close) // edit: nedense buraya girdi
            isInformationDialogClosed = true;
            setIcon(VakitError);
            trayIcon->setVisible(true);
            bolgeSec();
        }
    }
}

#endif
