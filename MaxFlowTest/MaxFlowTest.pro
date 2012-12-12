#-------------------------------------------------
#
# Project created by QtCreator 2012-10-02T13:26:33
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = MaxFlowTest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
        graph.cpp\
        maxflow.cpp

HEADERS += block.h\
        graph.h

OTHER_FILES += instances.inc
