#ifndef WINDOW_H
#define WINDOW_H

#include <QCoreApplication>
#include <QSystemTrayIcon>

#ifndef QT_NO_SYSTEMTRAYICON

#include <QElapsedTimer>
#include <QDialog>
#include <QWidget>

#include "fetchtimes.h"

#include <memory>

QT_BEGIN_NAMESPACE
class QAction;
class QCheckBox;
class QComboBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QMenu;
class QPushButton;
class QSpinBox;
class QTextEdit;
QT_END_NAMESPACE

//! [0]

namespace Ui {
class Window;
}

class Window : public QWidget
{
    Q_OBJECT

public:
	Window(QWidget* parent=0);

//    void setVisible(bool visible) override;
//protected:
//    void closeEvent(QCloseEvent *event) override;

private slots:
//    void iconActivated(QSystemTrayIcon::ActivationReason reason);
	void downloadEvkat();
	void onClickedOK5Dk();
    void bolgeSec();
    void fillCities(int ulkeIndex);
	void fillTown(int sehirIndex);
	void executeIlceKodu(int ilceIndex);
public slots:
	void showMessage();

	void showTime();
    void isNewVersionAvailable();
signals:
    void son5Dk();
    void sehirSec(QString sehirKodu);
private:
	const QString applicationDirPath = QCoreApplication::applicationDirPath();
	QString ulkeFile, sehirFile, ilceFile;
	QString ulkeKodu, sehirKodu, ilceKodu;
	void controlOnlineEvkatFileExistOtherwiseRequestDownload();
    bool compareTagVersion(QString tag, QString& currentTag);
	HttpWindow fetchTimes;	// TODO: önceki hali gibi olup da signal slot ile filan HttpWindow'dan Window'a sinyal gönderip indirme işlemi bitmeden fetchTimes objesinin ömrünü doldurmasını
	std::shared_ptr<Ui::Window> ui;	// men edebilirim
	bool kalanVakitBesOldu = false;
	void executeFileNames();
	void offlineVakitleriHesapla();
	void ilkBolgeSecimi();
	void setIcon(uint8_t number);    // önceden slot'tu
    void createActions();
    void createTrayIcon();

    QElapsedTimer timer;

    QAction *quitAction;
	QAction *sehirSecimiAction;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
};
//! [0]

#endif // QT_NO_SYSTEMTRAYICON

#endif
