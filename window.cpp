#include "window.h"
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
//    createIconGroupBox();
//    createMessageGroupBox();

//    iconLabel->setMinimumWidth(durationLabel->sizeHint().width());

	ulkeKodu = "2";
	sehirKodu = "551";
	executeFileNames();

    createActions();
	createTrayIcon();

	QString fileName = QDir::homePath() + "/evkatOffline.json";
	if(!QFileInfo(fileName).exists())
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
//	connect(this, &QWidget::close, ui->textLabel, &QLabel::clear);
//    connect(ui->ulke, SIGNAL(currentTextChanged(QString)), [this](QString ulke) {fillCities(ulke);});
	connect(ui->ulke, SIGNAL(currentIndexChanged(int)), SLOT(fillCities(int)));
	connect(ui->sehir, SIGNAL(currentIndexChanged(int)), SLOT(fillTown(int)));
//	showTime();

	/*QVBoxLayout *mainLayout = new QVBoxLayout;
//    mainLayout->addWidget(iconGroupBox);
//    mainLayout->addWidget(messageGroupBox);
	setLayout(mainLayout);*/

//    iconComboBox->setCurrentIndex(1);

	setWindowTitle(tr("Şehir Seçimi"));
	resize(400, 300);

//    titleEdit = new QLineEdit(tr("Saati değiştir kardeş. Başın ağrımasın"));    // sonradan koydum

	trayIcon->show();
}
void Window::executeFileNames()
{
	ulkeFile = QDir::homePath() + "/.namazVakitSehirKodlari/ulkeler.txt";
	sehirFile = QDir::homePath() + "/.namazVakitSehirKodlari/sehirler/" + ulkeKodu + ".txt";
	ilceFile = QDir::homePath() + "/.namazVakitSehirKodlari/sehirler/" + sehirKodu + ".txt";
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
	QFile jsonFile(QDir::homePath() + "/evkatOffline.json");
	jsonFile.open(QFile::WriteOnly);
	jsonFile.write(doc.toJson());
}
//! [0]

//! [1]
/*void Window::setVisible(bool visible)
{
    minimizeAction->setEnabled(visible);
    maximizeAction->setEnabled(!isMaximized());
    restoreAction->setEnabled(isMaximized() || !visible);
    QDialog::setVisible(visible);
}*/
//! [1]

//! [2]
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

	QPixmap pixmap(30,30);
	pixmap.fill(Qt::yellow);
	QPainter painter(&pixmap);
//	QFont font = painter.font();
//	font.setPixelSize(28);
//	painter.setFont(font);
	QString string = QString::number(number);
//	const QRect rectangle = QRect(0, 0, 100, 50);
//	QRect boundingRect;
	painter.drawText(0,0,16,16, Qt::TextDontClip, string);
//	painter.drawText(0,0,16,16, Qt::AlignCenter, string); // olmadı
//	painter.drawText(string); // olmadı

//	QPen pen = painter.pen();
//	pen.setStyle(Qt::DotLine);
//	painter.setPen(pen);
//	painter.drawRect(boundingRect.adjusted(0, 0, -pen.width(), -pen.width()));

//	pen.setStyle(Qt::DashLine);
//	painter.setPen(pen);
//	painter.drawRect(rectangle.adjusted(0, 0, -pen.width(), -pen.width()));

	// Hala aynı gibi ve güzel

	trayIcon->setIcon(QIcon(pixmap));
//	trayIcon->show();
}
//! [3]

//! [4]
/*void Window::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    qDebug() << "burda";
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
//! [5]

//! [6]
//!
void Window::onClickedOK5Dk()
{
//	zamaniHesapla();		// TODO: burda ne alaka lan.
    QMessageBox qmbox;
	qmbox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	qmbox.information(nullptr, tr("اَلصَّلَاةُ خَيْرٌ"), tr("Kıldın mı?"));
    QCoreApplication::instance()->quit();   //
}

//! [6]
/*void Window::createIconGroupBox()
{
    iconGroupBox = new QGroupBox(tr("Tray Icon"));

    iconLabel = new QLabel("Icon:");

    iconComboBox = new QComboBox;
    iconComboBox->addItem(QIcon(":/images/bad.png"), tr("Bad"));
    iconComboBox->addItem(QIcon(":/images/heart.png"), tr("Heart"));
    iconComboBox->addItem(QIcon(":/images/trash.png"), tr("Trash"));

//    showIconCheckBox = new QCheckBox(tr("Show icon"));
//    showIconCheckBox->setChecked(true);

    QHBoxLayout *iconLayout = new QHBoxLayout;
    iconLayout->addWidget(iconLabel);
    iconLayout->addWidget(iconComboBox);
    iconLayout->addStretch();
//    iconLayout->addWidget(showIconCheckBox);
    iconGroupBox->setLayout(iconLayout);
}*/

/*void Window::createMessageGroupBox()
{
    messageGroupBox = new QGroupBox(tr("Balloon Message"));

    typeLabel = new QLabel(tr("Type:"));

//    typeComboBox = new QComboBox;
//    typeComboBox->addItem(tr("None"), QSystemTrayIcon::NoIcon);
//    typeComboBox->addItem(style()->standardIcon(QStyle::SP_MessageBoxCritical), tr("Critical"), QSystemTrayIcon::Critical);
//    typeComboBox->addItem(style()->standardIcon(QStyle::SP_MessageBoxWarning), tr("Warning"), QSystemTrayIcon::Warning);
//    typeComboBox->addItem(style()->standardIcon(QStyle::SP_MessageBoxInformation), tr("Information"), QSystemTrayIcon::Information); // QSystemTrayIcon::NoIcon);
//    typeComboBox->addItem(QIcon(), tr("Custom icon"), -1);
//    typeComboBox->setCurrentIndex(1);

    durationLabel = new QLabel(tr("Duration:"));

//    durationSpinBox = new QSpinBox;
//    durationSpinBox->setRange(5, 60);
//    durationSpinBox->setSuffix(" s");
//    durationSpinBox->setValue(15); // critical message box, message box gibi çıkmadığı için (nedense) bunun anlamı yok. kaybolmayacak olan küçük bi pencere açılıyo zaten

//    durationWarningLabel = new QLabel(tr("(some systems might ignore this " "hint)"));
//    durationWarningLabel->setIndent(10);

    titleLabel = new QLabel(tr("Title:"));

    titleEdit = new QLineEdit(tr("Saati değiştir kardeş. Başın ağrımasın"));

    bodyLabel = new QLabel(tr("Body:"));

    bodyEdit = new QTextEdit;
//    bodyEdit->setPlainText(tr("Saati değiştir kardeş. Başın ağrımasın"));

    showMessageButton = new QPushButton(tr("Show Message"));
    showMessageButton->setDefault(true);

    QGridLayout *messageLayout = new QGridLayout;
    messageLayout->addWidget(typeLabel, 0, 0);
//    messageLayout->addWidget(typeComboBox, 0, 1, 1, 2);
    messageLayout->addWidget(durationLabel, 1, 0);
//    messageLayout->addWidget(durationSpinBox, 1, 1);
//    messageLayout->addWidget(durationWarningLabel, 1, 2, 1, 3);
    messageLayout->addWidget(titleLabel, 2, 0);
    messageLayout->addWidget(titleEdit, 2, 1, 1, 4);
    messageLayout->addWidget(bodyLabel, 3, 0);
    messageLayout->addWidget(bodyEdit, 3, 1, 2, 4);
    messageLayout->addWidget(showMessageButton, 5, 4);
    messageLayout->setColumnStretch(3, 1);
    messageLayout->setRowStretch(4, 1);
    messageGroupBox->setLayout(messageLayout);
}*/

void Window::createActions()
{
    /*minimizeAction = new QAction(tr("Hide"), this);
    connect(minimizeAction, &QAction::triggered, this, &QWidget::hide);

    maximizeAction = new QAction(tr("Ma&ximize"), this);
    connect(maximizeAction, &QAction::triggered, this, &QWidget::showMaximized);

    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction, &QAction::triggered, this, &QWidget::showNormal);*/

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
	sehirSecimiAction = new QAction(tr("&Şehir seç"), this);
//	connect(sehirSecimiAction, &QAction::triggered, this, &Window::bolgeSec);
	connect(sehirSecimiAction, SIGNAL(triggered()), this, SLOT(bolgeSec()));
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
void Window::evkatCalculated()
{
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
    /*trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();*/
    trayIconMenu->addAction(quitAction);
	trayIconMenu->addAction(sehirSecimiAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
//    connect(trayIcon, &QAbstractButton::clicked, this, &Window::showMessage);
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
