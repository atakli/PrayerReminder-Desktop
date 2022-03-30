#include "window.h"
#include "fetchtimes.h"
#include "calcTimes.h"
#include "prayertimesparser.h"
#include "ui_sehirSecwindow.h"

#ifndef QT_NO_SYSTEMTRAYICON

#include <QAction>
#include <QFile>
#include <QDir>
#include <QPainter>
//#include <QCheckBox>
//#include <QComboBox>
#include <QCoreApplication>
#include <QJsonObject>
#include <QJsonArray>
//#include <QCloseEvent>
//#include <QGroupBox>
//#include <QLabel>
//#include <QLineEdit>
#include <QMenu>
#include <QTimer>
#include <QDateTime>
//#include <QPushButton>
//#include <QSpinBox>
//#include <QTextEdit>
//#include <QVBoxLayout>
#include <QMessageBox>

#include <QDebug>
//#include <QtConcurrent/QtConcurrent>
//! [0]
Window::Window(QWidget* parent) : QWidget(parent), ui(std::make_shared<Ui::Window>())
{
	ui->setupUi(this);

	ulkeKodu = "2";
	sehirKodu = "551";
	executeFileNames();

    createActions();
	createTrayIcon();

	if(!QFileInfo(applicationDirPath + evkatOfflinePath).exists())
		offlineVakitleriHesapla();
//    connect(showIconCheckBox, &QAbstractButton::toggled, trayIcon, &QSystemTrayIcon::setVisible);
//    connect(iconComboBox, &QComboBox::currentIndexChanged, this, &Window::setIcon);   // hata veriyor. msvc'de sıkıntı çıkmamıştı
//	connect(trayIcon, &QSystemTrayIcon::messageClicked, this, &Window::onClickedOK);
//    connect(trayIcon, &QSystemTrayIcon::activated, this, &Window::iconActivated);
//    connect(trayIcon, &QWidget::closeEvent, this, &Window::addClockToThread);
//	addClockToThread();

//	timer.start();
//	QFuture<void> futureClock = QtConcurrent::run(this, &Window::repeatClockRequest);

	QTimer *timer = new QTimer(this);
//	std::unique_ptr<QTimer> timer = std::make_unique<QTimer>(this);
	connect(timer, &QTimer::timeout, this, &Window::showTime);
	timer->start(1000);

	connect(ui->hesaplaButton, &QAbstractButton::clicked, this, &Window::evkatCalculated);
//	connect(this, &QWidget::close, ui->textLabel, &QLabel::clear);	// close fonksiyonu signal değil, slot
//    connect(ui->ulke, SIGNAL(currentTextChanged(QString)), [this](QString ulke) {fillCities(ulke);});
	connect(ui->ulke, SIGNAL(currentIndexChanged(int)), SLOT(fillCities(int)));
	connect(ui->sehir, SIGNAL(currentIndexChanged(int)), SLOT(fillTown(int)));
	connect(ui->ilce, SIGNAL(currentIndexChanged(int)), SLOT(executeIlceKodu(int)));

	setWindowTitle(tr("Şehir Seçimi"));
	resize(400, 300);

//    titleEdit = new QLineEdit(tr("Saati değiştir kardeş. Başın ağrımasın"));    // sonradan koydum

	trayIcon->show();
}
void Window::executeFileNames()
{
	ulkeFile = applicationDirPath + "/namazVakitFiles/ulkeler.txt";
	sehirFile = applicationDirPath + "/namazVakitFiles/sehirler/" + ulkeKodu + ".txt";
	ilceFile = applicationDirPath + "/namazVakitFiles/sehirler/" + sehirKodu + ".txt";
}
void Window::offlineVakitleriHesapla()
{
	QDate dt = QDateTime::currentDateTime().date();
	CalcTimes ct;

	double fajr, sunRise, zuhr, asr, maghrib, isha;

	QJsonObject vakitObject;
	QJsonArray vakitArray;

	for(int i=0; i<30; ++i)
	{
		int year = dt.year();
		int month = dt.month();
		int day = dt.day();

		dt = dt.addDays(1);

		ct.calcPrayerTimes(year, month, day, 29.43333330, 40.80000000, 3, -18, -17, fajr, sunRise, zuhr, asr, maghrib, isha);

		QString dayWith0 = QString("0" + QString::number(day)).right(2);
		QString monthWith0 = QString("0" + QString::number(month)).right(2);

		QString toBeInserted = dayWith0 + "." + monthWith0 + "." + QString::number(year);
		vakitObject.insert("MiladiTarihKisa", QJsonValue::fromVariant(toBeInserted));
		toBeInserted = ct.doubleToHrMin(fajr, 1);
		vakitObject.insert("Imsak", QJsonValue::fromVariant(toBeInserted));
		toBeInserted = ct.doubleToHrMin(sunRise, 0);
		vakitObject.insert("Gunes", QJsonValue::fromVariant(toBeInserted));
		toBeInserted = ct.doubleToHrMin(zuhr, 2);
		vakitObject.insert("Ogle", QJsonValue::fromVariant(toBeInserted));
		toBeInserted = ct.doubleToHrMin(asr, 3);
		vakitObject.insert("Ikindi", QJsonValue::fromVariant(toBeInserted));
		toBeInserted = ct.doubleToHrMin(maghrib, 4);
		vakitObject.insert("Aksam", QJsonValue::fromVariant(toBeInserted));
		toBeInserted = ct.doubleToHrMin(isha, 5);
		vakitObject.insert("Yatsi", QJsonValue::fromVariant(toBeInserted));
		vakitArray.push_back(vakitObject);
	}

	QJsonDocument doc(vakitArray);
	QFile jsonFile(applicationDirPath + evkatOfflinePath);
	jsonFile.open(QFile::WriteOnly);
	jsonFile.write(doc.toJson());
}

/*void Window::closeEvent(QCloseEvent *event)
{
#ifdef Q_OS_MACOS
    if (!event->spontaneous() || !isVisible()) {
        return;
    }
#endif
    if (trayIcon->isVisible())
    {
        QMessageBox::information(this, tr("Systray"),
                                 tr("The program will keep running in the "
                                    "system tray. To terminate the program, "
                                    "choose <b>Quit</b> in the context menu "
                                    "of the system tray entry."));
        hide();
        event->ignore();
    }
}*/
//! [2]

//! [3]
void Window::setIcon(uint8_t number)
{
//	QSystemTrayIcon::NoIcon noo;
//	trayIcon->setVisible()

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
	QString string = QString::number(number);
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

	// Hala aynı gibi ve güzel

    trayIcon->setIcon(pixmap);  // QIcon(pixmap) desen de oluyor
//	trayIcon->show();
}
//! [3]

//! [4]
/*void Window::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        iconComboBox->setCurrentIndex((iconComboBox->currentIndex() + 1) % iconComboBox->count());
        break;
    case QSystemTrayIcon::MiddleClick:
        showMessage();
        break;
    default:
        ;
    }
}*/
//! [4]

//! [5]
void Window::showMessage()
{
//    showIconCheckBox->setChecked(true);
//    int selectedIcon = typeComboBox->itemData(typeComboBox->currentIndex()).toInt();
	/*int selectedIcon = 3;   // üstteki yorum satırındaki toInt()'in sonucu 3. QSystemTrayIcon::Critical 3'e karşılık geliyor sanırım
    QSystemTrayIcon::MessageIcon msgIcon = QSystemTrayIcon::MessageIcon(selectedIcon);

//    if (selectedIcon == -1) // custom icon
//    {
//        QIcon icon(iconComboBox->itemIcon(iconComboBox->currentIndex()));
//        trayIcon->showMessage(titleEdit->text(), bodyEdit->toPlainText(), icon, durationSpinBox->value() * 1000);
//    }
//    else
//    {
		trayIcon->showMessage("Saati değiştir kardeş. Başın ağrımasın", "", msgIcon, 1 * 1000);   // durationSpinBox->value() 1 diye salladım. ne olduğu önemli değil
//    }                                                                                         // normal message box açılmıyor çünkü şuan nedense
	*/QMessageBox qmbox;
//	qmbox.setWindowFlag(Qt::WindowStaysOnTopHint);
	/*QMessageBox::StandardButton out =*/ qmbox.information(nullptr, tr("حي على الصلاة"), QString("5 dk'dan az kaldı!"));
//	connect(qmbox, &QMessageBox::Ok, this, &Window::onClickedOK5Dk);
//	QCoreApplication::instance()->quit();   //
//	if(out == QMessageBox::Ok)
//		qmbox.close();
//		return true;
}

void Window::onClickedOK5Dk()
{
//	zamaniHesapla();		// TODO: burda ne alaka lan.
    QMessageBox qmbox;
	qmbox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	qmbox.information(nullptr, tr("اَلصَّلَاةُ خَيْرٌ"), tr("Kıldın mı?"));
    QCoreApplication::instance()->quit();   //
}

void Window::createActions()
{
    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
	sehirSecimiAction = new QAction(tr("&Şehir seç"), this);
//	connect(sehirSecimiAction, &QAction::triggered, this, &Window::bolgeSec);
	connect(sehirSecimiAction, SIGNAL(triggered()), this, SLOT(bolgeSec()));	// sanırım this'i kaldırınca da aynı mana
	connect(this, &Window::son5Dk, this, &Window::showMessage);
}
QString dosyayiAc(QString fileName, QIODevice::OpenModeFlag flag=QIODevice::ReadOnly)
{
	QFile file(fileName);
	if (!file.open(flag | QIODevice::Text))
		return "";							// TODO: buraya girerse ne olcak?
	QString text = file.readAll();
	file.close();
	return text;
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
void Window::evkatCalculated()
{
	HttpWindow fetchTimes;
	fetchTimes.downloadFile(ilceKodu);
	ui->textLabel->setText(ui->ilce->currentText() + " için bir aylık vakitler indirildi ve offline vakitler hesaplandı");
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
//    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);
	trayIconMenu->addAction(sehirSecimiAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
}

void Window::showTime()
{
	PrayerTimesParser ptp;			// TODO: her saniye burdaki her işi yapmasına gerek yok aslında, sanırım
	int kalanVakit = ptp.nextDay();
	if(kalanVakit <= 60)			// 60 dk'dan az kalmadıysa gösterme. bi de zaten ikiden fazla basamak göstermeye uygun değil ve gerek de yok
	{
		trayIcon->setVisible(true);
		setIcon(kalanVakit);
		if((kalanVakit <= 5) & (!kalanVakitBesOldu))
		{
			kalanVakitBesOldu = true;
//			QFuture<void> future = QtConcurrent::run(this, &Window::showMessage);	// 5 dk uyarısı çıktığında ok'a basmadığım sürece sayaç akmaya devam etmiyor. o yüzden
//			while(future.isFinished())												// yapmıştım ama olmadı
//				QCoreApplication::processEvents();
//			showMessage();
			emit son5Dk();
		}
	}
	else
		trayIcon->setVisible(false);
	if(kalanVakit > 5)
		kalanVakitBesOldu = false;
}

#endif
