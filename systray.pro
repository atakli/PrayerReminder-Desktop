CONFIG += c++17 console

HEADERS       = window.h \
    calcTimes.h \
    fetchtimes.h \
    prayertimesparser.h
SOURCES       = main.cpp \
                calcTimes.cpp \
                fetchtimes.cpp \
                prayertimesparser.cpp \
                window.cpp \


RESOURCES     = systray.qrc

QT += network widgets
requires(qtConfig(combobox))

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/desktop/systray
INSTALLS += target

#FORMS += \
#    authenticationdialog.ui
