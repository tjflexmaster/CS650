#ifndef UTILITY_H
#define UTILITY_H

#include <QImage>
#include "AdvancedOperations_global.h"
#include <qmath.h>

class ADVANCEDOPERATIONSSHARED_EXPORT Utility
{
public:
    Utility();
    static qreal rangeConvert(qreal old_max, qreal old_min, qreal new_max, qreal new_min, qreal value);
    static QImage threshold(QImage *image, int threshold);
    static QImage thresholdLightness(QImage *image, int threshold);
    static QImage blackImage(QSize size);
    static QImage binaryImage(QSize size);
    static QImage grayScaleImage(QImage *image);
    static qreal colorSimilarity(QRgb seed, QRgb target, qreal variance);
    static qreal colorSimilarity(QRgb seed, QRgb target);
    static QString pointToString(QPoint point);

    static const double PI;
};

#endif // UTILITY_H
