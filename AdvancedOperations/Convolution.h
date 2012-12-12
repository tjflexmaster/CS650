#ifndef CONVOLUTION_H
#define CONVOLUTION_H

#include <QMap>
#include <QPoint>
#include <QImage>
#include <QList>
#include <QPair>
#include "AdvancedOperations_global.h"

class ADVANCEDOPERATIONSSHARED_EXPORT Convolution
{
public:
    Convolution();

    static int sumConvolution(QImage *image, QList<QPair<QPoint, int > > *kernel, QPoint *coordinate);

    static QRgb colorSumConvolution(QImage *image, QList<QPair<QPoint, int > > *kernel, QPoint *coordinate, int denominator);

    static QRgb colorBilateralConvolution(QImage *image, QList<QPair<QPoint, int > > *kernel, QPoint *coordinate);

//    QMap<QPoint, int> convolveGrayscale(QImage *image, QMap<QPoint, int> *structure);
//    QMap<QPoint, int> getSquareStruct(int side = 1);
//    QMap<QPoint, int> getRectStruct(int height, int width);
//    QMap<QPoint, int> getCircleStruct(int radius);
};

#endif // CONVOLUTION_H
