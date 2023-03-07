CONFIG += c++17
QT += network widgets           # network kullanmiyo olmaliyim sanki artik. o isleri updatemanager library'si halletmiyo mu

#QMAKE_CXXFLAGS_WARN_ON += -O3
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS += -O3

QMAKE_CXXFLAGS_WARN_ON += /std:c++latest

HEADERS       = window.h \
                calcTimes.h \
                prayertimesparser.h

SOURCES       = main.cpp \
                calcTimes.cpp \
                prayertimesparser.cpp \
                window.cpp

win32 { # vay be. bu curly brace burda olmaliymis muhakkak. asagida olmuyormus.
INCLUDEPATH += "C:\\Users\\Emre ATAKLI\\Documents\\qt\\UpdateController\\include"
CONFIG(release, debug|release): LIBS += -L"C:\\Users\\Emre ATAKLI\\Documents\\qt\\UpdateController\\build-UpdateController-Desktop_Qt_6_4_1_MSVC2019_64bit-Release\\release\\static" -lupdatecontroller
else:CONFIG(debug, debug|release): LIBS += -L"C:\\Users\\Emre ATAKLI\\Documents\\qt\\UpdateController\\build-UpdateController-Desktop_Qt_6_4_1_MSVC2019_64bit-Debug\\debug\\static" -lupdatecontroller
}
unix {
INCLUDEPATH += /home/b720/qt-projects/UpdateController/include
CONFIG(debug, debug|release): LIBS += -L/home/b720/qt-projects/UpdateController/build-UpdateController-Desktop_Qt_5_14_2_GCC_64bit-Debug/debug/static -lUpdateController
else:CONFIG(release, debug|release): LIBS += -L/home/b720/qt-projects/UpdateController/build-UpdateController-Desktop_Qt_5_14_2_GCC_64bit-Release/release/static -lUpdateController
}

win32 {
INCLUDEPATH += "C:\\Users\\Emre ATAKLI\\Documents\\qt\\SingleApplication\\include"
CONFIG(release, debug|release): LIBS += -L"C:\\Users\\Emre ATAKLI\\Documents\\qt\\SingleApplication\\build-SingleApplication-Desktop_Qt_6_4_1_MSVC2019_64bit-Release\\release\\static" -lSingleApplication
else:CONFIG(debug, debug|release): LIBS += -L"C:\\Users\\Emre ATAKLI\\Documents\\qt\\SingleApplication\\build-SingleApplication-Desktop_Qt_6_4_1_MSVC2019_64bit-Debug\\debug\\static" -lSingleApplication
}
unix {
INCLUDEPATH += /home/b720/qt-projects/UpdateController/include
CONFIG(debug, debug|release): LIBS += -L/home/b720/qt-projects/SingleApplication/build-SingleApplication-Desktop_Qt_5_14_2_GCC_64bit-Debug/debug/static -lSingleApplication
else:CONFIG(release, debug|release): LIBS += -L/home/b720/qt-projects/SingleApplication/build-SingleApplication-Desktop_Qt_5_14_2_GCC_64bit-Release/release/static -lSingleApplication
}

win32: LIBS += -ladvapi32 # -lUser32 -lpsapi -lsetupapi -lwinusb -lgdi32 -lkernel32

## install
#target.path = $$[QT_INSTALL_EXAMPLES]/widgets/desktop/systray
#INSTALLS += target

FORMS += \
    sehirSecwindow.ui

win32:RC_ICONS += icon.ico

RESOURCES += \
    infoIcon.qrc

#-O3'u eklemem gereken yerin QMAKE_CXXFLAGS degil QMAKE_CXXFLAGS_RELEASE olmasi gerektigini anladim hele sukur

#message("$$QMAKE_CXXFLAGS")                         # -pipe
#message("$$QMAKE_CXXFLAGS_WARN_ON")                 # -Wall -Wextra
#message("$$QMAKE_CXXFLAGS_WARN_OFF")                # -w
#message("$$QMAKE_CXXFLAGS_RELEASE_WITH_DEBUGINFO")  # -O2 -g
#message($$QMAKE_CXXFLAGS_RELEASE)                 # -O2
#message("$$QMAKE_CXXFLAGS_DEBUG")                   # -g
#message("$$QMAKE_CXXFLAGS_SHLIB")                   # -fPIC
#message("$$QMAKE_CXXFLAGS_THREAD")                  # -D_REENTRANT
#message($$QMAKE_LIBDIR)
# la olum tam benin aradigim sey: QMAKE_RPATHDIR. ama gel gor ki sadece unix'te gecerliymis

# --no-plugins deyince çalışmıyor. ama --no-plugins demeyince üretilen klasörlerden bi tek platforms klasörünü silince sıkıntı oluyor (içinde de sadece qwindows.dll var)
# C:\Qt-mingw\6.3.1\msvc2019_64\bin\windeployqt systray.exe --libdir . --plugindir . -no-translations --no-opengl-sw --no-system-d3d-compiler
# böyle bişey başına gelebilir; hatırla: Why does it miss these files? -> these files are related to the mingw runtime library, they do not belong to Qt and that's why windeployqt does not add them by default.

#contains(CONFIG, static)
#{
#    # Static compilation does not require deployment
#}
#else
#{
#    # Windows platform(only for x86 architecture)
#    win32
#    {
#        DEPLOY_TOOL = $${dirname(QMAKE_QMAKE)}/windeployqt.exe
#        DEPLOY_TOOL = $$replace(DEPLOY_TOOL, /, \\)

#        contains(CONFIG, debug, debug|release)
#        {
#            DEPLOY_TARGET = $${OUT_PWD}/debug/$${TARGET}.exe
#        }
#        else
#        {
#            DEPLOY_TARGET = $${OUT_PWD}/release/$${TARGET}.exe
#        }

#        DEPLOY_TARGET=$$replace(DEPLOY_TARGET, /, \\)
#        msvc
#        {
#            QMAKE_POST_LINK+=$${DEPLOY_TOOL} $${DEPLOY_TARGET} --force --no-translations $$escape_expand(\\n)
#        }
#        else
#        {
#            QMAKE_POST_LINK+='$$escape_expand("\\n\\t") $${DEPLOY_TOOL} $${DEPLOY_TARGET} --force --no-translations $$escape_expand("\\n\\t")'
#        }
#    }
#}
