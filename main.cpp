#include <QtWidgets/QApplication>
//#include <Windows.h>
#ifndef QT_NO_SYSTEMTRAYICON

#include <iostream>

#include <QMessageBox>
#include <QFileInfo>
#include <QDebug>
#include <QDir>

#include "window.h"

//#include <QApplication>
#include "singleapplication.h"

//#ifdef _WIN32
//  #pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")   // TODO: bu en iyisi mi bakmak lazım. bi de bu sadece msvc için heralde. edit: pro'daki console bu ise yariyomus zaten :(
////  For linux I know how to create a .desktop file so I can launch the app that does not show a terminal when it is launched.
//#endif

QString exePath;

//mincore\com\oleaut32\dispatch\ups.cpp(2122)\OLEAUT32.dll!00007FFCBE4C9DD6: (caller: 00007FFCBE4C91E9) ReturnHr(1) tid(1278) 8002801D Library not registered.
//QSystemTrayIcon::setVisible: No Icon set
//onecore\windows\directx\database\helperlibrary\lib\directxdatabasehelper.cpp(652)\d3d9.dll!00007FFCA954BF45: (caller: 00007FFCA954BA4C) ReturnHr(1) tid(1278) 80004002 No such interface supported

//static const QString APPLICATION_NAME = "Namaz Vakti Hatirlatici";
//static const QString ORGANIZATION_NAME = "ATAKLI";
//static const QString APPLICATION_VERSION = "1.5.0";

int main(int argc, char *argv[])
{
    SingleApplication app(argc, argv, true);
    if (app.isSecondary())
    {
        app.sendMessage(app.arguments().join(' ').toUtf8());
        return 0;
    }

//    app.setApplicationName(APPLICATION_NAME);
//    app.setApplicationVersion(APPLICATION_VERSION);
//    app.setOrganizationName(ORGANIZATION_NAME);

    const QString userName = QDir::home().dirName();
    const QString saveDir = "C:/Users/" + userName + "/.namazVaktiHatirlatici";

    QDir d;
    if(!d.exists(saveDir))
    {
        if(!d.mkpath(saveDir))
        {
            qDebug() << saveDir << "klasoru olusturulamadi";
            exit(EXIT_FAILURE);
        }
        else
        {
//            std::vector<TCHAR> Buffer(saveDir.length() + 1);
//            saveDir.toWCharArray(Buffer.data());
//            Buffer[saveDir.length()] = '\0';
//            SetCurrentDirectory(Buffer.data());
        }
    }
    QDir::setCurrent(saveDir);

    exePath = QFileInfo{argv[0]}.absolutePath();

	if (!QSystemTrayIcon::isSystemTrayAvailable())
	{
		QMessageBox::critical(nullptr, QObject::tr("Systray"), QObject::tr("I couldn't detect any system tray on this system."));
        return 1;
    }
    QApplication::setQuitOnLastWindowClosed(false);

    Window window;
    QObject::connect(&app, &SingleApplication::receivedMessage, &window, &Window::on_instanceOpen);

    return app.exec();
}

#else

#include <QLabel>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QString text("QSystemTrayIcon is not supported on this platform");

    QLabel *label = new QLabel(text);
    label->setWordWrap(true);

    label->show();
    qDebug() << text;

    app.exec();
}

#endif
