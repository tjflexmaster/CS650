#-------------------------------------------------
#
# Project created by QtCreator 2012-11-28T15:39:36
#
#-------------------------------------------------

 QT += core gui multimedia

 contains(QT_CONFIG, opengl): QT += opengl

TARGET = VideoPlayerExample
TEMPLATE = app


SOURCES += main.cpp\
        videoplayer.cpp \
    videoitem.cpp

HEADERS  += videoplayer.h \
    videoitem.h
