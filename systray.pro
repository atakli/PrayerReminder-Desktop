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
#requires(qtConfig(combobox))

## install
#target.path = $$[QT_INSTALL_EXAMPLES]/widgets/desktop/systray
#INSTALLS += target

FORMS += \
    sehirSecwindow.ui

# --no-plugins deyince çalışmıyor. ama --no-plugins demeyince üretilen klasörlerden bi tek platforms klasörünü silince sıkıntı oluyor (içinde de sadece qwindows.dll var)
# windeployqt ..\systray.exe --libdir . --plugindir . -no-translations --no-opengl-sw --no-system-d3d-compiler

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
