#-------------------------------------------------
#
# Project created by QtCreator 2012-08-31T14:41:24
#
#-------------------------------------------------

TARGET = Otsu
TEMPLATE = lib

DEFINES += OTSU_LIBRARY

SOURCES += Otsu.cpp

HEADERS += Otsu.h\
        Otsu_global.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xEA61914E
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = Otsu.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Histogram/release/ -lHistogram
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Histogram/debug/ -lHistogram
else:symbian: LIBS += -lHistogram
else:unix: LIBS += -L$$OUT_PWD/../Histogram/ -lHistogram

INCLUDEPATH += $$PWD/../Histogram
DEPENDPATH += $$PWD/../Histogram
