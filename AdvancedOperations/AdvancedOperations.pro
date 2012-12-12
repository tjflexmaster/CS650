#-------------------------------------------------
#
# Project created by QtCreator 2012-09-07T07:39:15
#
#-------------------------------------------------

QT       += network opengl gui

TARGET = AdvancedOperations
TEMPLATE = lib

DEFINES += ADVANCEDOPERATIONS_LIBRARY

SOURCES += Morphology.cpp \
    Convolution.cpp \
    FFT.cpp \
    EdgeDetection.cpp \
    Utility.cpp \
    HoughTransform.cpp\
    maxflow/graph.cpp\
    maxflow/maxflow.cpp \
    GraphCut.cpp \
    GrabCut.cpp \
    GaussianCalc.cpp \
    ConnectedComponents.cpp \
    PatternRecognition.cpp \
    ImageStitching.cpp

HEADERS += Morphology.h\
        AdvancedOperations_global.h \
    Convolution.h \
    FFT.h \
    EdgeDetection.h \
    Utility.h \
    HoughTransform.h\
    maxflow/block.h\
    maxflow/graph.h \
    GraphCut.h \
    GrabCut.h \
    GaussianCalc.h \
    ConnectedComponents.h \
    PatternRecognition.h \
    tnt/tnt.h \
    tnt/tnt_array1d.h \
    tnt/tnt_array1d_utils.h \
    tnt/tnt_array2d.h \
    tnt/tnt_array2d_utils.h \
    tnt/tnt_array3d.h \
    tnt/tnt_array3d_utils.h \
    tnt/tnt_cmat.h \
    tnt/tnt_fortran_array1d.h \
    tnt/tnt_fortran_array1d_utils.h \
    tnt/tnt_fortran_array2d.h \
    tnt/tnt_fortran_array2d_utils.h \
    tnt/tnt_fortran_array3d.h \
    tnt/tnt_fortran_array3d_utils.h \
    tnt/tnt_i_refvec.h \
    tnt/tnt_math_utils.h \
    tnt/tnt_sparse_matrix_csr.h \
    tnt/tnt_stopwatch.h \
    tnt/tnt_subscript.h \
    tnt/tnt_vec.h \
    tnt/tnt_version.h \
    tnt/jama_cholesky.h \
    tnt/jama_eig.h \
    tnt/jama_lu.h \
    tnt/jama_qr.h \
    tnt/jama_svd.h \
    ImageStitching.h

OTHER_FILES += maxflow/instances.inc

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xEC81A750
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = AdvancedOperations.dll
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
