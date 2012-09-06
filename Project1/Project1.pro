#-------------------------------------------------
#
# Project created by QtCreator 2012-08-29T14:13:56
#
#-------------------------------------------------

QT       += core gui

TARGET = Project1
TEMPLATE = app


SOURCES += main.cpp \
    ImageViewer.cpp

HEADERS  += \
    ImageViewer.h

FORMS    += \
    ImageViewer.ui

RESOURCES += \
    assets.qrc


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Histogram/release/ -lHistogram
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Histogram/debug/ -lHistogram
else:symbian: LIBS += -lHistogram
else:unix: LIBS += -L$$OUT_PWD/../Histogram/ -lHistogram

INCLUDEPATH += $$PWD/../Histogram
DEPENDPATH += $$PWD/../Histogram

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Otsu/release/ -lOtsu
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Otsu/debug/ -lOtsu
else:symbian: LIBS += -lOtsu
else:unix: LIBS += -L$$OUT_PWD/../Otsu/ -lOtsu

INCLUDEPATH += $$PWD/../Otsu
DEPENDPATH += $$PWD/../Otsu
