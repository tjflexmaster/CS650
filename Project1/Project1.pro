#-------------------------------------------------
#
# Project created by QtCreator 2012-08-29T14:13:56
#
#-------------------------------------------------

QT       += core gui

TARGET = Project1
TEMPLATE = app


SOURCES += main.cpp \
    ImageViewer.cpp \
    ImageDisplay.cpp \
    ImageItem.cpp

HEADERS  += \
    ImageViewer.h \
    ImageDisplay.h \
    ImageItem.h

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

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../AdvancedOperations/release/ -lAdvancedOperations
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../AdvancedOperations/debug/ -lAdvancedOperations
else:symbian: LIBS += -lAdvancedOperations
else:unix: LIBS += -L$$OUT_PWD/../AdvancedOperations/ -lAdvancedOperations

INCLUDEPATH += $$PWD/../AdvancedOperations
DEPENDPATH += $$PWD/../AdvancedOperations
