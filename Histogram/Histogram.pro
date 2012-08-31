#-------------------------------------------------
#
# Project created by QtCreator 2012-08-30T18:16:13
#
#-------------------------------------------------

QT       += core opengl gui

TARGET = Histogram
TEMPLATE = lib

DEFINES += HISTOGRAM_LIBRARY

SOURCES += Histogram.cpp \
    HistogramWidget.cpp

HEADERS += Histogram.h\
        Histogram_global.h \
    HistogramWidget.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xEAE34C7F
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = Histogram.dll
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
