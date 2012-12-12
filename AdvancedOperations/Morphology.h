#ifndef MORPHOLOGY_H
#define MORPHOLOGY_H

#include <QImage>
#include <QList>
#include <QPoint>

#include "AdvancedOperations_global.h"

class ADVANCEDOPERATIONSSHARED_EXPORT Morphology {
public:
    Morphology();

    QImage dilation(QImage *image, QList<QPoint> *structure);
    QImage erosion(QImage *image, QList<QPoint> *structure);
    QImage majority(QImage *image, QList<QPoint> *structure);
    QImage opening(QImage *image, QList<QPoint> *structure);
    QImage closing(QImage *image, QList<QPoint> *structure);

    QList<QPoint> getSquareStruct(int side);
    QList<QPoint> getRectStruct(int width, int height);
    QList<QPoint> getPlusStruct(int length);
    QList<QPoint> getCustomStruct();

private:
    void convolve(QImage *new_image, QImage *image, QList<QPoint> *structure, int threshold);
};

#endif // MORPHOLOGY_H
