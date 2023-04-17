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

void myMessageHandler(QtMsgType type, const QMessageLogContext&, const QString &message)
{
    QFile file("log.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        return;
    QTextStream out(&file);
    out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ");
    switch (type) {
    case QtDebugMsg:
        out << message << "\n";
        out.flush();
        break;
    }
}

int main(int argc, char *argv[])
{
    exePath = QFileInfo{argv[0]}.absolutePath();
    SingleApplication app(argc, argv, true);
    if (app.isSecondary())
    {
        app.sendMessage(app.arguments().join(' ').toUtf8());
        return 0;
    }

//    app.setApplicationName(APPLICATION_NAME);
//    app.setApplicationVersion(APPLICATION_VERSION);
//    app.setOrganizationName(ORGANIZATION_NAME);

    const QString saveDir = QDir::home().filePath(".namazVaktiHatirlatici");

    QDir d;
    if(!d.exists(saveDir))
    {
        if(!d.mkpath(saveDir))
        {
            qDebug() << saveDir << "klasoru olusturulamadi";
            exit(EXIT_FAILURE);
        }
    }
    QDir::setCurrent(saveDir);
    qInstallMessageHandler(myMessageHandler);
    if (!QSystemTrayIcon::isSystemTrayAvailable())
	{
        QMessageBox::critical(nullptr, QObject::tr("Namaz Vakti Hatırlatıcı"), QObject::tr("I couldn't detect any system tray on this system."));
        return 1;
    }
    QApplication::setQuitOnLastWindowClosed(false);

    Window window;
    QObject::connect(&app, &SingleApplication::receivedMessage, &window, &Window::on_instanceOpen);
//    QObject::connect(&app, &QCoreApplication::aboutToQuit, []{file.close();});

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
