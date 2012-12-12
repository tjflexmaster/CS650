#-------------------------------------------------
#
# Project created by QtCreator 2012-10-02T12:53:21
#
#-------------------------------------------------

QT       -= gui

TARGET = GraphCut
TEMPLATE = lib

DEFINES += GRAPHCUT_LIBRARY

SOURCES += graph.cpp\
        maxflow.cpp

HEADERS += block.h\
        graph.h\
        GraphCut_global.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE90389F4
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = GraphCut.dll
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
