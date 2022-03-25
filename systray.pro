CONFIG += c++17 console

HEADERS       = window.h \
                calcTimes.h \
                fetchtimes.h \
                prayertimesparser.h \
                update.h
SOURCES       = main.cpp \
                calcTimes.cpp \
                fetchtimes.cpp \
                prayertimesparser.cpp \
    update.cpp \
                window.cpp \


RESOURCES     = systray.qrc

QT += network widgets
requires(qtConfig(combobox))

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/desktop/systray
INSTALLS += target

FORMS += \
    sehirSecwindow.ui
