#-------------------------------------------------
#
# Project created by QtCreator 2012-11-28T15:07:14
#
#-------------------------------------------------

QT       += core gui

TARGET = AnomolyDetection
TEMPLATE = app

INCLUDEPATH += C:\\armadillo-3.4.4/include
INCLUDEPATH += C:\\boost_1_52_0


SOURCES += main.cpp\
        MainWindow.cpp \
    RxAlgorithm.cpp \
    ArmadilloExamples.cpp

HEADERS  += MainWindow.h \
    RxAlgorithm.h \
    ArmadilloExamples.h

FORMS    += MainWindow.ui


win32: LIBS += -L$$PWD/../../../../../armadillo-3.4.4/examples/lib_win32/ -lblas_win32_MT
win32: LIBS += -L$$PWD/../../../../../armadillo-3.4.4/examples/lib_win32/ -llapack_win32_MT

INCLUDEPATH += $$PWD/../../../../../armadillo-3.4.4/lapack
DEPENDPATH += $$PWD/../../../../../armadillo-3.4.4/lapack

win32: LIBS += -L$$PWD/../../../../../fftw_32/ -llibfftw3-3

INCLUDEPATH += $$PWD/../../../../../fftw
DEPENDPATH += $$PWD/../../../../../fftw
