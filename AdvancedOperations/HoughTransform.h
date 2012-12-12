#ifndef HOUGHTRANSFORM_H
#define HOUGHTRANSFORM_H

#include <QImage>
#include <QList>
#include <QPoint>
#include "AdvancedOperations_global.h"

class ADVANCEDOPERATIONSSHARED_EXPORT HoughTransform
{
public:
    HoughTransform();

    QImage transform(QImage *image);
    QImage transform2(QImage *image);
    QList<QPoint> getCircleTable(int radius);

private:

};

#endif // HOUGHTRANSFORM_H
