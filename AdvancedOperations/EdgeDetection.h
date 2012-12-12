#ifndef EDGEDETECTION_H
#define EDGEDETECTION_H

#include <QImage>
#include <QVector>
#include <QPoint>
#include <QPair>
#include <QHash>
#include "AdvancedOperations_global.h"

class ADVANCEDOPERATIONSSHARED_EXPORT EdgeDetection
{
public:
    EdgeDetection();

    QImage gradientMagnitudeImage(QImage *image);
    QImage gradientOrientationImage(QImage *image);
    QHash<QPoint, QPair<qreal, qreal> > gradientValues(QImage *image);
    QList<QPoint> getZeroCrossings(QImage *image);
    QImage blurImage(QImage *image);
    QImage blurColorImage(QImage *image);
    QImage bilateralColorImage(QImage *image);
    QImage combinationEdgeDetection(QImage *image);
};


 inline uint qHash(const QPoint &key)
 {
     QString val = QString("%1,%2").arg(key.x(), key.y());
     return qHash(val);
 }

#endif // EDGEDETECTION_H
