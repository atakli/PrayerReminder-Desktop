#ifndef WINDOW_H
#define WINDOW_H

#include <QSystemTrayIcon>

#ifndef QT_NO_SYSTEMTRAYICON

#include <QDialog>
#include <QWidget>

#include <QElapsedTimer>

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
    void evkatCalculated();
	void onClickedOK5Dk();
    void bolgeSec();
    void fillCities(int ulkeIndex);
	void fillTown(int sehirIndex);
public slots:
	void showMessage();

	void showTime();
signals:
    void son5Dk();
    void sehirSec(QString sehirKodu);
private:
	QString ulkeFile, sehirFile, ilceFile;
	QString ulkeKodu, sehirKodu, ilceKodu;
	std::shared_ptr<Ui::Window> ui;
	bool kalanVakitBesOldu = false;
	void executeFileNames();
	void offlineVakitleriHesapla();
	void ilkBolgeSecimi();
	void setIcon(uint8_t number);    // önceden slot'tu
//    void createIconGroupBox();
//    void createMessageGroupBox();
    void createActions();
    void createTrayIcon();

    QElapsedTimer timer;

    QGroupBox *iconGroupBox;
    QLabel *iconLabel;
    QComboBox *iconComboBox;
    QCheckBox *showIconCheckBox;

    QGroupBox *messageGroupBox;
    QLabel *typeLabel;
    QLabel *durationLabel;
//    QLabel *durationWarningLabel;
    QLabel *titleLabel;
    QLabel *bodyLabel;
//    QComboBox *typeComboBox;
//    QSpinBox *durationSpinBox;
//    QLineEdit *titleEdit;
//    QTextEdit *bodyEdit;
//    QPushButton *showMessageButton;

//    QAction *minimizeAction;
//    QAction *maximizeAction;
//    QAction *restoreAction;
    QAction *quitAction;
	QAction *sehirSecimiAction;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
};
//! [0]

#endif // QT_NO_SYSTEMTRAYICON

#endif
