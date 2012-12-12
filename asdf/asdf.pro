#-------------------------------------------------
#
# Project created by QtCreator 2012-10-02T13:34:23
#
#-------------------------------------------------

QT       -= gui

TARGET = asdf
TEMPLATE = lib

DEFINES += ASDF_LIBRARY

SOURCES += asdf.cpp

HEADERS += asdf.h\
        asdf_global.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xEA34747F
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = asdf.dll
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
