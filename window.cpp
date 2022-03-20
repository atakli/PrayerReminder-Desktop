#include "window.h"
#include "calcTimes.h"
#include "prayertimesparser.h"

#ifndef QT_NO_SYSTEMTRAYICON

#include <QAction>
#include <QPainter>
#include <QCheckBox>
#include <QComboBox>
#include <QCoreApplication>
#include <QCloseEvent>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QPushButton>
#include <QSpinBox>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QMessageBox>

#include <QDebug>
#include <QtConcurrent/QtConcurrent>
//! [0]
Window::Window()
{
//    createIconGroupBox();
//    createMessageGroupBox();

//    iconLabel->setMinimumWidth(durationLabel->sizeHint().width());

    createActions();
	createTrayIcon();

//    connect(showMessageButton, &QAbstractButton::clicked, this, &Window::showMessage);
//    connect(showIconCheckBox, &QAbstractButton::toggled, trayIcon, &QSystemTrayIcon::setVisible);
//    connect(iconComboBox, &QComboBox::currentIndexChanged, this, &Window::setIcon);   // hata veriyor. msvc'de sıkıntı çıkmamıştı
	connect(trayIcon, &QSystemTrayIcon::messageClicked, this, &Window::onClickedOK);
//    connect(trayIcon, &QSystemTrayIcon::activated, this, &Window::iconActivated);
//    connect(trayIcon, &QWidget::closeEvent, this, &Window::addClockToThread);
//	addClockToThread();
	timer.start();
	QFuture<void> futureClock = QtConcurrent::run(this, &Window::repeatClockRequest);

	/*QVBoxLayout *mainLayout = new QVBoxLayout;
//    mainLayout->addWidget(iconGroupBox);
//    mainLayout->addWidget(messageGroupBox);
	setLayout(mainLayout);*/

//    iconComboBox->setCurrentIndex(1);

	setWindowTitle(tr("Change Computer Clock"));
	resize(400, 300);

//    titleEdit = new QLineEdit(tr("Saati değiştir kardeş. Başın ağrımasın"));    // sonradan koydum

	trayIcon->show();
}
void Window::kalanVakitDongusu()
{
	bool kalanVakitBesOldu = false;
	PrayerTimesParser ptp;
	qDebug() << "1";
	while(1)
	{
		int kalanVakit = ptp.nextDay();
		qDebug() << "kalan:" << kalanVakit;
		if(kalanVakit <= 60)			// 60 dk'dan az kalmadıysa gösterme. bi de zaten ikiden fazla basamak göstermeye uygun değil şuan
		{
			setIcon(kalanVakit);
			qDebug() << "2";
			if((kalanVakit <= 5) & (!kalanVakitBesOldu))
			{
				qDebug() << "3";
				kalanVakitBesOldu = true;
				showMessage();
			}
		}
		qDebug() << "4";
		if(kalanVakit > 5)
			kalanVakitBesOldu = false;
		qDebug() << "5";
		QThread::sleep(60);				// 60 saniyede bir kontrol et. ama bunu ayrı bit threde koysam iyi olur yoksa kitlenir
		qDebug() << "6";
	}
	qDebug() << "7";
}
void Window::degistir()
{
//	QJson
	for(uint8_t i=3; i>0; --i)
	{
		QThread::sleep(1);
		setIcon(i);
		trayIcon->show();
	}
}
void Window::zamaniHesapla()
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

		int hours, minutes;

		QString toBeInserted = QString::number(day) + "/" + QString::number(month) + "/" + QString::number(year);
		vakitObject.insert("Date", QJsonValue::fromVariant(toBeInserted));
		QJsonObject recordObject;
		toBeInserted = ct.doubleToHrMin(fajr, hours, minutes, 1);
		recordObject.insert("Fajr", QJsonValue::fromVariant(toBeInserted));
		toBeInserted = ct.doubleToHrMin(sunRise, hours, minutes, 0);
		recordObject.insert("SunRise", QJsonValue::fromVariant(toBeInserted));
		toBeInserted = ct.doubleToHrMin(zuhr, hours, minutes, 2);
		recordObject.insert("Zuhr", QJsonValue::fromVariant(toBeInserted));
		toBeInserted = ct.doubleToHrMin(asr, hours, minutes, 3);
		recordObject.insert("Asr", QJsonValue::fromVariant(toBeInserted));
		toBeInserted = ct.doubleToHrMin(maghrib, hours, minutes, 4);
		recordObject.insert("Maghrib", QJsonValue::fromVariant(toBeInserted));
		toBeInserted = ct.doubleToHrMin(isha, hours, minutes, 5);
		recordObject.insert("Isha", QJsonValue::fromVariant(toBeInserted));
		vakitObject.insert("Evkat",recordObject);
		vakitArray.push_back(vakitObject);
	}

	QJsonDocument doc(vakitArray);
	QFile jsonFile("/home/b720/evkatOffline.json");
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
//    QIcon icon = iconComboBox->itemIcon(index);

/*	QIcon icon(QIcon(":/images/heart.png"));
	trayIcon->setIcon(icon);
//	setWindowIcon(icon);

	trayIcon->setToolTip("Heart");*/

	QPixmap pixmap(30,30);
	pixmap.fill(Qt::yellow);
	QPainter painter(&pixmap);
	QString string = QString::number(number);
	painter.drawText(0,0,16,16, Qt::TextDontClip, string);

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
	qmbox.information(nullptr, tr("Hayye alessaleh"), tr("5 dk'dan az kaldı!"));
	QCoreApplication::instance()->quit();   //
}
//! [5]

//! [6]
void Window::onClickedOK()
{
//	zamaniHesapla();		// TODO: burda ne alaka lan.
    QMessageBox qmbox;
    qmbox.information(nullptr, tr("Clock"), tr("Saati değiştirdin değil mi?"));
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
    downloadTimesAction = new QAction(tr("&Namaz Vakitlerini İndir"), this);
    connect(downloadTimesAction, &QAction::triggered, this, &Window::downloadTimes);
}
void Window::downloadTimes()
{
    // ...
    QMessageBox qmbox;
//    QIcon icon(QIcon(":/images/heart.png"));
//    qmbox.setIcon(QMessageBox::Information);
    qmbox.information(nullptr, tr("İşlem Başarılı"), tr("30 Günlük Vakitler İndirildi"));
}

void Window::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    /*trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();*/
    trayIconMenu->addAction(quitAction);
    trayIconMenu->addAction(downloadTimesAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
//    connect(trayIcon, &QAbstractButton::clicked, this, &Window::showMessage);
}

void Window::repeatClockRequest()
{
	PrayerTimesParser ptp;
	bool kalanVakitBesOldu = false;
		qDebug() << "1";
		while(1)
		{
//			if(timer.elapsed() > 60000)
//			{
				qDebug() << "elapsed:" << timer.elapsed();
				int kalanVakit = ptp.nextDay();
				qDebug() << "kalan:" << kalanVakit;
				if(kalanVakit <= 60)			// 60 dk'dan az kalmadıysa gösterme. bi de zaten ikiden fazla basamak göstermeye uygun değil şuan
				{
					setIcon(kalanVakit);
					qDebug() << "2";
					if((kalanVakit <= 5) & (!kalanVakitBesOldu))
					{
						qDebug() << "3";
						kalanVakitBesOldu = true;
						showMessage();
					}
				}
				qDebug() << "4";
				if(kalanVakit > 5)
					kalanVakitBesOldu = false;
				qDebug() << "5";
//				QThread::sleep(60);				// 60 saniyede bir kontrol et. ama bunu ayrı bi threde koysam iyi olur yoksa kitlenir
				if_oncesi:
				if(timer.elapsed() < 60000)
				{
					QCoreApplication::processEvents();
					goto if_oncesi;
				}
				else
				{
					timer.restart();
					continue;
				}
				qDebug() << "6";
//				showMessage();
//				timer.restart();
//			}
//			else
//				continue;
		}
		qDebug() << "7";

}

void Window::addClockToThread()
{
	timer.start();
	qDebug() << "8";

	QFuture<void> futureClock = QtConcurrent::run(this, &Window::repeatClockRequest);
	qDebug() << "9";

	while(!futureClock.isFinished())
	{
		QCoreApplication::processEvents();
	}
	qDebug() << "10";
}

#endif
